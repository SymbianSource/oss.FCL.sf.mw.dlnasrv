/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CCmShrinker class in the Memory manager component
*
*/


#include <e32std.h>
#include <imageconversion.h> // CImageDecoder, CImageEncoder
#include <bitmaptransforms.h> // CBitmapScaler
#include <fbs.h> // CFbsBitmap
#include <w32std.h> // RWsSession, CWsScreenDevice

#include "cmdmmain.h"
#include "msdebug.h"
#include "cmmmimagemetadataresolver.h"
#include "cmmmobserver.h"
#include "cmmmshrinker.h"

// CONSTANTS
const TInt KScreenWidth = 128;
const TInt KScreenHeight = 128;


// ---------------------------------------------------------------------------
// CCmMmShrinker::NewL
// ---------------------------------------------------------------------------
//
CCmMmShrinker* CCmMmShrinker::NewL( CCmDmMain& aDbManager )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::NewL() start"));
    CCmMmShrinker* self = CCmMmShrinker::NewLC( aDbManager );
    CleanupStack::Pop( self );
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::NewLC
// ---------------------------------------------------------------------------
//
CCmMmShrinker* CCmMmShrinker::NewLC( CCmDmMain& aDbManager )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::NewLC() start"));
    CCmMmShrinker* self = new ( ELeave ) CCmMmShrinker( aDbManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::NewLC() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCmMmShrinker::CCmMmShrinker( CCmDmMain& aDbManager ) :
    CActive( EPriorityIdle ),
    iShrinkIndex( 0 ),
    iState( EIdle ),
    iDbManager( aDbManager )
    {
    
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CCmMmShrinker::ConstructL
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::ConstructL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::ConstructL() start"));

    User::LeaveIfError( iFileSession.Connect() );
    User::LeaveIfError( RFbsSession::Connect() );

    // Get the screen size
    iScreenSize = ScreenSizeL();
    
    iImageMetadataResolver = CCmMmImageMetadataResolver::NewL( iFileSession );

    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::ConstructL() end"));
    }


// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CCmMmShrinker::~CCmMmShrinker()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::~CCmMmShrinker() start"));
    Cancel();
    delete iBitmap;
    iFileSession.Close();
    
    delete iBitmapScaler;
    delete iFiles;
    delete iImageDecoder;
    delete iImageEncoder;
    RFbsSession::Disconnect();
    delete iImageMetadataResolver;
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::~CCmMmShrinker() end"));
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::DoCancel
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::DoCancel()
    {
    ClearShrinker();
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::RunError
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CCmMmShrinker::RunError( TInt aError )
#else //_DEBUG
TInt CCmMmShrinker::RunError( TInt /*aError*/ )
#endif // _DEBÚG
    {
    TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker::RunError error = %d"),
                                                             aError ));
                                                                    
    // NOTE!!!
    // Should we continue from the next file, if there's error in the 
    // middle of the shrinking operation
    iShrinkIndex++;
    iState = EIdle;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::ShrinkImagesL
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::ShrinkImagesL( CDesCArray& aFiles )
    {
    // Cancel 1st
    Cancel();
    
    // Add processed files 
    if ( &aFiles )
        {
        delete iFiles; 
        iFiles = NULL;
        iFiles = new ( ELeave ) CDesC16ArrayFlat( aFiles.Count() );
        for (TInt i = 0; i < aFiles.Count(); i++)
            {
            iFiles->AppendL( aFiles[i] );
            }
        }
    // Start the action
    iShrinkIndex = 0;
    iState = EIdle;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone);    
    }
    
// ---------------------------------------------------------------------------
// CCmMmShrinker::SetObserver
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::SetObserver( MCmMmObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::RunL
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::RunL()
    {
    TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker::RunL status = %d"),
                                                             iStatus.Int() ));

    // If all files have been processed, notify the observer.
    if ( iShrinkIndex >= iFiles->Count() )
        {
        if ( iObserver )
            {
            iObserver->ShrinkCompleteL( KErrNone );
            }
        }
    else 
        {
        if ( iState == EIdle )
            {
            const TDesC& origFilename = (*iFiles)[iShrinkIndex];
            iImageMetadataResolver->CaptureOrginalMetadataL( origFilename );
            // Check that file exists (entry is not actually used)
            TEntry entry;
            User::LeaveIfError( iFileSession.Entry( origFilename, entry ));
            iStartTime.HomeTime();
            CImageDecoder* imageDecoder = NULL;
            TRAPD( error, 
                   imageDecoder = CImageDecoder::FileNewL( 
                        iFileSession,
                        origFilename,
                        CImageDecoder::EOptionNone ) );
            if ( error )
                {
                if ( iObserver )
                    {
                    iObserver->ShrinkCompleteL( error );
                    }
                }
            else
            	{
            	TUid imageType = KNullUid;
	            TUid imageSubType = KNullUid;
	            imageDecoder->ImageType( 0, imageType, imageSubType );

	            if (  imageType == KImageTypeBMPUid )
	                {
	                iState = EScale;
	                }
	            else if ( imageType == KImageTypeGIFUid ||
	                      imageType == KImageTypePNGUid ||
	                      imageType == KImageTypeJPGUid )
	                {
	                iState = EDecode;
	                }
	            else
	                {
	                if ( iObserver )
	                    {
	                    iObserver->ShrinkCompleteL( KErrNone );
	                    }
	                }

	            delete imageDecoder;
	            imageDecoder = NULL;
            	}
            }

        TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker: status = %d"),
            iStatus.Int() ));

        switch ( iState )
            {
            case EDecode:
                {
                const TDesC& origFilename = (*iFiles)[iShrinkIndex];
                TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker: Decoding file \
                    %S"), &origFilename ));

                delete iImageDecoder;
                iImageDecoder = NULL;
                delete iBitmap;
                iBitmap = NULL;

                iImageDecoder = CImageDecoder::FileNewL( iFileSession,
                                                         origFilename, CImageDecoder::EOptionNone );
                iBitmap = new (ELeave) CFbsBitmap();
                TInt error = iBitmap->Create(
                    iImageDecoder->FrameInfo().iOverallSizeInPixels,
                    iImageDecoder->FrameInfo().iFrameDisplayMode );
                if ( error != KErrNone )
                    {
                    TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker::RunL \
                        error: %d"), error ));
                    if ( iObserver )
                        {
                        TRACE(Print(_L("[MEMORY MNGR]\t Clearing shrinker" )));                        
                        ClearShrinker();
                        iObserver->ShrinkCompleteL( error );
                        }
                    Cancel();
                    }
                else 
                    {
                    iImageDecoder->Convert( &iStatus, *iBitmap );

                    iState = EScale;
                    SetActive();
                    }
                break;
                }
            case EScale:
                {
                const TDesC& origFilename = (*iFiles)[iShrinkIndex];
                TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker: Scaling file \
                    %S"), &origFilename ));

                // If converting is still ongoing we should continue it
                if ( iStatus == KErrUnderflow )
                    {
                    TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker: \
    Still decoding file %S"), &origFilename ));
                    iImageDecoder->ContinueConvert( &iStatus );
                    SetActive();
                    }
                else 
                    {
                    delete iBitmapScaler;
                    iBitmapScaler = NULL;
                    iBitmapScaler = CBitmapScaler::NewL();

                    iBitmapScaler->Scale( &iStatus, *iBitmap, iScreenSize );
                    iState = EEncode;
                    SetActive();

                    delete iImageDecoder;
                    iImageDecoder = NULL;
                    }

                break;
                }
            case EEncode:
                {
                const TDesC& origFilename = (*iFiles)[iShrinkIndex];
                TRACE(Print(_L("[MEMORY MNGR]\t CCmMmShrinker: Encoding file \
                    %S"), &origFilename ));

                delete iImageEncoder;
                iImageEncoder = NULL;
                
                // Shrink into private directory
                PrivatePath( iFileSession, iTempFilename, origFilename );
                
                // Check that if the file already exists somehow...
                if ( iTempFilename == origFilename )
                    {
                    // Delete the original
                    iFileSession.Delete( origFilename );
                    }
                iImageEncoder = CImageEncoder::FileNewL(
                    iFileSession,
                    iTempFilename,
                    CImageEncoder::EOptionNone,
                    KImageTypeJPGUid );

                iImageEncoder->Convert( &iStatus, *iBitmap );
                iState = EReplace;

                SetActive();
                break;
                }
            case EReplace:
                {
                const TDesC& origFilename = (*iFiles)[iShrinkIndex];
                TRACE( Print(
                    _L("[MEMORY MNGR]\t CCmMmShrinker: Replacing file %S"),
                    &origFilename) );                                                 
                                                        
                TInt error = iFileSession.Replace( 
                    iTempFilename, 
                    origFilename );                                       
                    
                TRACE( Print(
                    _L("[MEMORY MNGR]\t CCmMmShrinker: Replace done err: %d"),
                    error ) );
                    
                // Resolve orginal image metadata!!!
                TRAPD( mdError, iImageMetadataResolver->ResolveMetadataL(
                        origFilename ) );
                if( mdError )
                    {
                    TRACE( Print(
                        _L("[MEMORY MNGR]\t Metadata resolving error : %d"),
                        mdError ) );                    
                    }
                    
                iStopTime.HomeTime();

                TTimeIntervalMicroSeconds t =
                    iStartTime.MicroSecondsFrom( iStopTime );
                error =  iDbManager.IncrementShrinkTimeL(
                    iFiles->Count(),
                    iStartTime.MicroSecondsFrom( iStopTime ).Int64() / 1000 );
                    
                iState = EIdle;
                iShrinkIndex++;
                SetActive();
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, KErrNone );
                break;
                }
            case EIdle:
            	{
            	TRACE( Print(_L("[MEMORY MNGR]\t CCmMmShrinker::RunL() \
                Idle state")) );
                break;
                }
            default:
                {
                TRACE( Print(_L("[MEMORY MNGR]\t CCmMmShrinker::RunL() \
                Incorrect state")) );
                if ( iObserver )
                    {
                    iObserver->ShrinkCompleteL( iStatus.Int() );
                    }
                Cancel();
                break;
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CCmMmShrinker::ScreenSizeL
// ---------------------------------------------------------------------------
//
TSize CCmMmShrinker::ScreenSizeL()
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::ScreenSizeL() start"));

    TSize screenSize( KScreenWidth, KScreenHeight );
    RWsSession session;

    TInt error = session.Connect() ;
    CleanupClosePushL( session );
    if ( !error )
        {
        CWsScreenDevice* screenDevice = 
            new ( ELeave ) CWsScreenDevice( session );
        if ( screenDevice && !screenDevice->Construct() )
            {
            TSize temp( KScreenWidth, KScreenHeight );
            temp = screenDevice->SizeInPixels();
            // Use landscape mode in shrinking
            TRACE(Print(_L("[MEMORY MNGR]\t Image height = %d"),temp.iWidth));
            TRACE(Print(_L("[MEMORY MNGR]\t Image width = %d"),temp.iHeight)); 
            screenSize.iHeight = temp.iWidth;
            screenSize.iWidth = temp.iHeight;
            }
        delete screenDevice;
        screenDevice = NULL;
        }

    CleanupStack::PopAndDestroy( &session );

    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::ScreenSizeL() end"));
    return screenSize;
    }

// ---------------------------------------------------------------------------
// CCmMmShrinker::PrivatePath
// ---------------------------------------------------------------------------
//   
void CCmMmShrinker::PrivatePath( RFs& aFs, 
                                 TFileName& aPrivatePath, 
                                 const TFileName& aOriginal )
    {
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::PrivatePath() start"));
    
    aPrivatePath.Zero();
    
    TParse nameParse;
    nameParse.Set( aOriginal, NULL, NULL );
        
    aPrivatePath.Append( nameParse.Drive() );
    TFileName privatePath;
    TInt err = aFs.PrivatePath( privatePath );
    if ( !err )
        {
        aPrivatePath.Append( privatePath );
        
        // Now the path contains everything but filename and extension
        // => check that the directory exists. If not, it will be created.
        // Possible error is ignored at the moment 
        // (normal case is KErrAlreadyExists)
        err = aFs.MkDirAll( aPrivatePath );
        
        
        aPrivatePath.Append( nameParse.NameAndExt() );
        }
    LOG(_L("[MEMORY MNGR]\t CCmMmShrinker::PrivatePath() end"));
    }
    
// ---------------------------------------------------------------------------
// CCmMmShrinker::ClearShrinker
// ---------------------------------------------------------------------------
//
void CCmMmShrinker::ClearShrinker()
    {
    if ( iImageDecoder )
        {
        iImageDecoder->Cancel();
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    if ( iBitmapScaler )
        {
        iBitmapScaler->Cancel();
        delete iBitmapScaler;
        iBitmapScaler = NULL;        
        }
    if ( iImageEncoder )
        {
        iImageEncoder->Cancel();
        delete iImageEncoder;
        iImageEncoder = NULL;        
        }
    if( iBitmap )
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    }
    
// End of file

