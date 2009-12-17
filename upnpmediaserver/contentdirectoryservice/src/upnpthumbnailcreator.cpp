/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Thumbnail Creator.
*
*/
 

#include <e32svr.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include <eikfutil.h>
#include <gulicon.h> 

#include "upnpthumbnailcreator.h"
#include "upnpthumbnailcreator.h"
#include "upnpcontentdirectoryglobals.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPMediaServer.txt")
#endif
#include "upnpcustomlog.h"

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpThumbnailCreator* CUpnpThumbnailCreator::NewLC(MUpnpThumbnailCreatorObserver* aObserver)
{
    CUpnpThumbnailCreator* self = new (ELeave) CUpnpThumbnailCreator(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::ConstructL()
{
    User::LeaveIfError(iFs.Connect());
    iScaler = CBitmapScaler::NewL();
    iSize.SetSize( KThumbnailWidth, KThumbnailHeight );
    iPhase = ESleep;
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::CUpnpThumbnailCreator
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpThumbnailCreator::CUpnpThumbnailCreator( MUpnpThumbnailCreatorObserver* aObserver )
    :CActive(EPriorityStandard), iOldSize(0,0)
{
    iObserver = aObserver;
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::~CUpnpThumbnailCreator
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpThumbnailCreator::~CUpnpThumbnailCreator()
{
    Cancel();
    delete iScaler;
    delete iEncoder;
    delete iDecoder;
    delete iBitmap;
    iFs.Close();
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::RunError
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpThumbnailCreator::RunError(TInt aErr)
{
	LOGS1("CUpnpThumbnailCreator::RunError(%d)", aErr);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::RunL()
{
    if( iStatus == KErrNone )
    {
        if( EDecodeObject == iPhase )
        {
            iScaler->Scale(&iStatus, *iBitmap, iSize, ETrue);
            iPhase = EDecode;
            SetActive();
        }
        else if( EDecode == iPhase )
        {
            EncodeAndConvertL();
            iPhase = EEncode;
            SetActive();
        }
        else if( EEncode == iPhase )
        {
            iPhase = ESleep;
            iObserver->NotifyThumbnailResultL(this, iStatus.Int());
        }
    }
    else
    {
        iObserver->NotifyThumbnailResultL(this, iStatus.Int());
    }
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::DoCancel
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::DoCancel()
{
    if( EDecode == iPhase )
    {
        iDecoder->Cancel();
    }
    if( EEncode == iPhase )
    {
        iEncoder->Cancel();
    }
    if( EDecodeObject == iPhase )
    {
        iDecoder->Cancel();
    }
}



// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::RunError
// Requester function
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::EncodeAndConvertL()
{
    if( IsActive() )
    {
        Cancel();
    }

    delete iEncoder;
    iEncoder = 0;

    iPath.Append(iFileName);


    iEncoder = CImageEncoder::FileNewL( iFs,
                                        iPath,
                                        KThumbMimeType,
                                        CImageEncoder::EOptionAlwaysThread);

    iEncoder->Convert( &iStatus, *iBitmap, 0 );
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::CreateL
// Requester function
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::CreateL(const TDesC& aSourceFile, const TDesC& aThumbName)
{


    if( IsActive() )
    {
        Cancel();
    }

    iParse.Set( aSourceFile, NULL, NULL);
    TParse fp;
    User::LeaveIfError( fp.Set(aThumbName, 0 , 0) );
    iFileName = fp.NameAndExt();
    iExtension = fp.Ext();
    iPath = fp.DriveAndPath();

    TUint temp;
    if( iFs.Att( iPath, temp ) != KErrNone )
    {
        User::LeaveIfError( iFs.MkDirAll( iPath ) );
    }

    delete iDecoder;
    iDecoder = NULL;
    
    delete iBitmap;
    iBitmap = NULL;
    iBitmap = new (ELeave) CFbsBitmap();

    TSize size = GetSize(aSourceFile);
    size.SetSize( size.iWidth, size.iHeight );
    iOldSize = size; 
    CalculateSize( size );
    User::LeaveIfError(iBitmap->Create( size, iInfo.iFrameDisplayMode ));

    //Decode to the bitmap
    //RunL will be called when decoding is complete
    iPhase = EDecodeObject;
    iDecoder->Convert( &iStatus, *iBitmap );
    SetActive();
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::CalculateSize
// Requester function
// -----------------------------------------------------------------------------
//
void CUpnpThumbnailCreator::CalculateSize( TSize& aLoadSize )
{
    if( ( aLoadSize.iWidth <= KThumbnailWidth ) ||
        ( aLoadSize.iHeight <= KThumbnailHeight ) )
    {
        return;
    }

    TUint i( 8 ); // for 8 bits

    for( ; i > 0; i >>= 1 )
    {
        if( DivisionCeil( aLoadSize.iWidth,  i ) > KThumbnailWidth &&
        DivisionCeil( aLoadSize.iHeight, i ) > KThumbnailHeight )
        {
            break;
        }
    }

    aLoadSize.SetSize( DivisionCeil( aLoadSize.iWidth, i ),
                       DivisionCeil( aLoadSize.iHeight, i ) );
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::DivisionCeil
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpThumbnailCreator::DivisionCeil( const TInt aVal, const TInt aDiv )
{
    if( ( aVal % aDiv ) > 0 )
    {
        return (TInt)( ( aVal / aDiv ) + 1 );
    }
    else
    {
        return (TInt)( aVal / aDiv );
    }
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::GetFileName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TFileName CUpnpThumbnailCreator::GetFileName()
{
    return iFileName;
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::GetFileExt
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TPtrC CUpnpThumbnailCreator::GetFileExt()
{
    return iExtension;
}
// CUpnpThumbnailCreator::GetPath
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TFileName CUpnpThumbnailCreator::GetPath()
{
    return iPath;
}

// -----------------------------------------------------------------------------
// CUpnpThumbnailCreator::GetSize
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TSize CUpnpThumbnailCreator::GetSize(const TDesC& aSourceFile)
{
    TRAPD(sizeErr, iDecoder = CImageDecoder::FileNewL( iFs, aSourceFile ));
    if (sizeErr)
    {
        return TSize();
    }
    else
    {
        iInfo = iDecoder->FrameInfo();
        TSize size = iInfo.iOverallSizeInPixels;
        return size;    
    }
}

