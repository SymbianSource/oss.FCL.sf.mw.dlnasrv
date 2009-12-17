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
* Description:  Plays remote images and videos locally on the device
*
*/


// INCLUDE FILES
// System
#include <AiwGenericParam.h>
#include <AiwGenericParam.hrh>
#include <AiwCommon.hrh>

#include <aknnotewrappers.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <upnpdlnaprotocolinfo.h>
#include <upnpcommonui.rsg>
#include <utf.h>
#include <bautils.h>

// upnp specific MACRO definition
#include "upnpconstantdefs.h"
// upnp stack api
#include <upnpitem.h>
#include <upnpobject.h>
#include <upnpstring.h>

// upnp framework / avcontroller api
#include "upnpavcontroller.h"                   // MUPnPAVController
#include "upnpfiledownloadsession.h"            // MUPnPFileDownloadSession
#include "upnpavbrowsingsession.h"

// upnp framework / avcontroller helper api
#include "upnpdlnautility.h"
#include "upnpitemutility.h"
#include "upnpfileutility.h"

// upnp framework / internal api's
#include "upnpcommonutils.h"
#include "upnpsettingsengine.h" // get selected download location

// USER INCLUDE FILES
#include "upnpcommonui.h"
#include "upnplocalplayer.h"
#include "upnpdeviceobserver.h"

// DEBUG
_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// CONSTANT DEFINITIONS
_LIT8( KProtocolInfo,   "protocolInfo" );
_LIT8( KHttpDes,        "http://" );
_LIT8( KHttpGetDes, "http-get" );
_LIT8( KHttpEqual, "=" );

const TInt KDownloadPosition = 0;

// Video mimetypes that can be played on device


// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::CUPnPLocalPlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPLocalPlayer::CUPnPLocalPlayer( MUPnPAVController& aAVController,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI  ):
                                    iCommonUI(aCommonUI)
    {
    __LOG( "CUPnPLocalPlayer::CUPnPLocalPlayer" );
    iAVController = &aAVController;
    iExitReason = KErrNone;
    iBrowseSession = &aBrowseSession;
    __LOG( "CUPnPLocalPlayer::CUPnPLocalPlayer-END" );
    }

    
    
// --------------------------------------------------------------------------
// CUPnPLocalPlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPLocalPlayer::ConstructL()
    {
    __LOG( "CUPnPLocalPlayer::ConstructL" ); 
    iDocumentHandler = CDocumentHandler::NewL();
    iDocumentHandler->SetExitObserver( this );
    User::LeaveIfError( iFs.Connect() );
    iDownloadSession = &iAVController->StartDownloadSessionL(
                                            iBrowseSession->Device() );
    __LOG( "CUPnPLocalPlayer::ConstructL-END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPLocalPlayer* CUPnPLocalPlayer::NewL(
                                    MUPnPAVController& aAVController,
                                    MUPnPAVBrowsingSession& aBrowseSession,
                                    CUPnPCommonUI& aCommonUI  )
    {
    CUPnPLocalPlayer* self = new( ELeave )CUPnPLocalPlayer( aAVController,
                                                            aBrowseSession,
                                                            aCommonUI );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    __LOG( "CUPnPLocalPlayer::NewL-END" );
    return self;
    }    

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::~CUPnPLocalPlayer
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPLocalPlayer::~CUPnPLocalPlayer()
    {   
    __LOG( "CUPnPLocalPlayer::~CUPnPLocalPlayer" );
    
    // If download session is running, stop it
    if( iAVController &&
        iDownloadSession )
        {
        iAVController->StopDownloadSession( *iDownloadSession );
        }

    delete iDocumentHandler;
    
    delete iWaitNoteDialog;
    
    delete iItem;
    
    if( iFilePath )
        {
        iFs.Delete( *iFilePath );
        delete iFilePath;
        }
    iFs.Close();

    __LOG( "CUPnPLocalPlayer::~CUPnPLocalPlayer-END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::PlayL
// Play a selected item.
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPLocalPlayer::PlayL( const CUpnpObject& aItem )
    {
    __LOG( "CUPnPLocalPlayer::PlayL" );
    
    // recreate iItem
    delete iItem; iItem = NULL;
    iItem = CUpnpItem::NewL();
    iItem->CopyL( aItem );

    if ( !IsLocallySupportedL( *iItem ) ) 
        {
        User::Leave( KErrNotSupported );
        }
    
    //in order not to get two callbacks when MS is lost, 
    // in CUPnPLocalPlayer and CUPnPBrowseDialog
    iBrowseSessionObserver = iBrowseSession->Observer();
    iBrowseSession->RemoveObserver(); 
    iFs.Close();
    
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    delete iFilePath; iFilePath = NULL;
    iExitReason = KErrNone;
    iDownloadSession->SetObserver( *this );
    
    /**
     * All the temporiarily downloaded files should go to the
     * hidden folder \data\download\media\temp
     * fix for ETLU-7LKCJB
     */
    //create a file path which should contain the absolute file path
    //e.g c:\data\download\media\temp\image.jpg
    
    iFilePath = HBufC::NewL( KMaxFileName );
    HBufC* copyLocation = HBufC::NewLC( KMaxFileName );
    CUPnPSettingsEngine* settingsEngine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( settingsEngine );
    TBool copyLocationIsPhoneMemory = 0; // not used in this case
    TPtr copyLocationPtr( copyLocation->Des() );
    settingsEngine->GetCopyLocationL( copyLocationPtr,
        copyLocationIsPhoneMemory );
        
    CleanupStack::PopAndDestroy( settingsEngine );    
        
    iFilePath->Des().Append( *copyLocation );
    CleanupStack::PopAndDestroy( copyLocation );
    
    _LIT( KTempFolder, "temp\\");
    iFilePath->Des().Append( KTempFolder() );
    
    //check the existence of the target folder
    if( !BaflUtils::FolderExists( iFs, *iFilePath ) )
        {
        User::LeaveIfError( iFs.MkDirAll( *iFilePath ) );
        }
        
    User::LeaveIfError( iFs.SetAtt( *iFilePath, 
                                    KEntryAttHidden, 
                                    KEntryAttNormal ) );     
    
    //Get the title of the given item
    HBufC* title16 = UpnpString::ToUnicodeL( aItem.Title() );
    CleanupStack::PushL( title16 );
    HBufC* title16checked =
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( *title16 );
    CleanupStack::PopAndDestroy( title16 );
    
    iFilePath->Des().Append( *title16checked );
    delete title16checked; title16checked = NULL;
    //Get the extension of the given item
    
    const CUpnpItem* item = (CUpnpItem*)(&aItem);
    const CUpnpElement* tmpEl = &( UPnPItemUtility::ResourceFromItemL( 
        *item ) );
    
    if( !UPnPFileUtility::FitsInMemory( *tmpEl) )
        {
        User::Leave( KErrDiskFull );
        }  
            
    const CUpnpAttribute* tmpAttInfo = UPnPItemUtility::FindAttributeByName(
        *tmpEl, KAttributeProtocolInfo );
    
    User::LeaveIfNull( const_cast<CUpnpAttribute*>(tmpAttInfo ) );
    
    CUpnpDlnaProtocolInfo* tmpProtocolInfo = CUpnpDlnaProtocolInfo::NewL(
        tmpAttInfo->Value() );
   
    CleanupStack::PushL( tmpProtocolInfo );
        
    HBufC* fileExt = NULL;
    
    fileExt = UPnPCommonUtils::FileExtensionByMimeTypeL(
        tmpProtocolInfo->ThirdField() );
    
    User::LeaveIfNull( fileExt );
    
    iFilePath->Des().Append( *fileExt );
    
    delete fileExt; fileExt = NULL;
    
    CleanupStack::PopAndDestroy( tmpProtocolInfo );
        
    TInt err = KErrNone;
    
    RFile rfile;
    err = rfile.Create(iFs, *iFilePath, EFileWrite );
    
    CleanupClosePushL( rfile );
    if( KErrAlreadyExists == err )
        {
        __LOG( "Already exists -> Delete old and create new" );
        User::LeaveIfError( iFs.Delete( *iFilePath ) );
        User::LeaveIfError( rfile.Create(iFs, *iFilePath, EFileWrite ) );
        }
    
    iDownloadSession->StartDownloadL( *tmpEl, 
                                      ( CUpnpItem& )aItem, 
                                      rfile,
                                      KDownloadPosition );
    CleanupStack::PopAndDestroy(&rfile);
    
    iWaitingNote = EFalse;    
    if( !iWaitingNote ) //if ReserveLocalMSServicesCompleted is not called
        {               //immediately
        iWaitingNote = ETrue;
        StartWaitingNoteL();
        }
    
    __LOG1( "CUPnPLocalPlayer::PlayL-END %d", iExitReason );
    if( iExitReason != KErrNone )
        {    
        User::Leave( iExitReason );
        }
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::TransferStarted.
// --------------------------------------------------------------------------
//
void CUPnPLocalPlayer::TransferStarted( TInt aKey, TInt aStatus )
    {
    __LOG( "CUPnPLocalPlayer::TransferStarted" );
    if( aStatus != KErrNone)
        {
        iExitReason = aStatus;    
        }
    else if( aKey != KDownloadPosition )
        {
        iExitReason = KErrGeneral;
        }
    
    if( iExitReason != KErrNone )
        {
        FinishNote();
        }
    __LOG( "CUPnPLocalPlayer::TransferStarted-END" );
    }
    
// --------------------------------------------------------------------------
// CUPnPLocalPlayer::TransferCompleted.
// --------------------------------------------------------------------------
//        
void CUPnPLocalPlayer::TransferCompleted( TInt aKey,
                                          TInt aStatus,
                                          const TDesC& aFilePath )
    {
    __LOG( "CUPnPLocalPlayer::TransferCompleted" );
    
    if( aKey != KDownloadPosition )
        {
        iExitReason = KErrGeneral;
        FinishNote();
        }
    else
        {
        TRAP_IGNORE( CopyCompleteL( aStatus, aFilePath ) );    
        }    
    
    __LOG( "CUPnPLocalPlayer::TransferCompleted-END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::CopyCompleteL
// Returns from UPnP AV control point when a copy operation has been finished
// --------------------------------------------------------------------------
void CUPnPLocalPlayer::CopyCompleteL( TInt aError,
                                     const TDesC& /*aFilePath*/ )
    {
    __LOG1( "CopyCompleteL %d", aError );

    FinishNote();
    // If copying was successful, play the copied item
    if( KErrNone == aError )
        {
        // iFilePath Contains UTF8 content,we need change
        // to TDesC8 first ,Using Copy don't lost any data,because the 
        //low byte is NULL       
        TBuf8<KMaxFileName> filename;
        filename.Copy( *iFilePath );
        
        HBufC* temp = iFilePath;
        iFilePath = NULL;      
        
        // try transform UTF8 to UniCode
        HBufC* unicodename = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            filename );       
        CleanupStack::PushL( unicodename );

        // Rename the file
        iFilePath = UPnPCommonUtils::RenameFileL( *unicodename ) ;
        if( iFilePath )
            {
            delete temp; temp = NULL;
            }
        else
            {
            iFilePath = temp;
            }    
        CleanupStack::PopAndDestroy( unicodename );
        unicodename = NULL;
        
        if( iFilePath )
            {
            
            // fix for TSW: ESLX-7L3DMX
            // OpenFileEmbeddedL( aSharableFile,aDataType, aParamList)
            // leaves with KErrInUse which results into immediate close
            // of image viewer.
            
            RFile sharableFile;
            TRAPD( err, iDocumentHandler->OpenTempFileL( 
                    *iFilePath, sharableFile ) );
                    
            if ( err == KErrNone )
                {
                CleanupClosePushL( sharableFile );

                // Create a param list to remove 
                // the "Use image as" sub menu item
                CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC();
                paramList->Reset();
                
//       Append a param into the list to restrict the viewer application from
//       showing the "use image as" sub menu item. Currently there is no
//       AiwGenericParam for this. A CR has been created for AIW to add this
//       constant to "AiwGenericParam.hrh".
//          Example: This is how the "Save" menu item is allowed.
//          paramList->AppendL( EGenericParamAllowSave );
                
                TDataType dataType = TDataType();
                
                __LOG( "Open document now... " );
                            
                TRAP( err, err = iDocumentHandler->OpenFileEmbeddedL(
                            sharableFile, dataType, *paramList ) );

                __LOG1( "err, err = iDocumentHandler->OpenFileEmbeddedL %d",
                        err );
                
                // Cleanup
                CleanupStack::PopAndDestroy( paramList );
                CleanupStack::PopAndDestroy( &sharableFile );
                }
                                                    
            if( KErrNone != err)
                {
                iFs.Delete( *iFilePath );
                iExitReason = err;
                }
            }
        else
            {
            iExitReason = KErrNoMemory;
            }
        }
    else
        {
        iExitReason = aError;
        }

  
    iCommonUI.HandleCommonErrorL( iExitReason, 0 );

    __LOG( "CUPnPLocalPlayer::CopyCompleteL end" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::MediaServerDisappeared
// Returns from UPnP AV control point when a media server disppears
// --------------------------------------------------------------------------
void CUPnPLocalPlayer::MediaServerDisappeared( 
                                  TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "CUPnPLocalPlayer::MediaServerDisappeared" );
    TInt error = KErrNone;
    if( aReason == EDisconnected )
        {
        error = KErrSessionClosed;
        }
    else if( aReason == EWLANLost)
        {
        error = KErrDisconnected;
        }
    else
        {
        __PANICD( __FILE__, __LINE__);
        }
    iExitReason = error;
    FinishNote(); 
    __LOG1("CUPnPLocalPlayer::MediaServerDisappeared %d END",error );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::HandleServerAppExit
// Returns from application server after quitting a application
// here is either image player or video player or music player
// --------------------------------------------------------------------------
void CUPnPLocalPlayer::HandleServerAppExit( TInt aReason )
    {
    __LOG1( "CUPnPLocalPlayer::HandleServerAppExit %d" , aReason );

    if( iFilePath )
        {
        iFs.Delete( *iFilePath );
        }

    __LOG( "CUPnPLocalPlayer::HandleServerAppExit" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::DialogDismissedL
// Returns from dialog server after cancelling a dialog
// here is the wait note
// --------------------------------------------------------------------------
void CUPnPLocalPlayer::DialogDismissedL( TInt aButtonId )
    {
    __LOG1( "CUPnPLocalPlayer::DialogDismissedL %d", aButtonId );
    if( aButtonId == EEikBidCancel )
        {
        iDownloadSession->CancelAllTransfers();
        iDownloadSession->RemoveObserver();
        if( iBrowseSessionObserver )
            {
            iBrowseSession->SetObserver( *iBrowseSessionObserver );
            }
        iBrowseSessionObserver = NULL;
        iExitReason = KErrCancel;
        __LOG( "CUPnPLocalPlayer::DialogDismissedL Cancel " );   

        }
   __LOG( "CUPnPLocalPlayer::DialogDismissedL" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::FinishNote
// Finish the current waiting note and ready to quit the local playback
// --------------------------------------------------------------------------
void CUPnPLocalPlayer::FinishNote()
    {
    __LOG( "CUPnPLocalPlayer::FinishNote" );
    iDownloadSession->CancelAllTransfers();
    iDownloadSession->RemoveObserver();
    if( iBrowseSessionObserver )
        {
        iBrowseSession->SetObserver( *iBrowseSessionObserver );
        }
    iBrowseSessionObserver = NULL;
    if( iWaitNoteDialog )
        {
        TRAP_IGNORE( iWaitNoteDialog->ProcessFinishedL() );
        delete iWaitNoteDialog;
        iWaitNoteDialog = NULL;
        }
    __LOG( "CUPnPLocalPlayer::FinishNote-END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::StartWaitingNoteL
// Start the current waiting note
// -------------------------------------------------------------------------- 
void CUPnPLocalPlayer::StartWaitingNoteL()
    {
    iWaitNoteDialog = new ( ELeave )CAknWaitDialog(
                    ( REINTERPRET_CAST ( CEikDialog**, &iWaitNoteDialog ) ),
                                                            ETrue);
    iWaitNoteDialog->SetCallback( this );
    iWaitNoteDialog->ExecuteLD( 
                R_UPNPCOMMONUI_VIDEO_PLAYBACK_WAIT_NOTE_DIALOG );
    }

// --------------------------------------------------------------------------
// CUPnPLocalPlayer::IsLocallySupportedL
// Checks if the item can be played locally.
// -------------------------------------------------------------------------- 
TBool CUPnPLocalPlayer::IsLocallySupportedL( CUpnpObject& aItem )
    {
    TBool retval = EFalse; // return value

    // Get all res elements into array
    RUPnPElementsArray elms;
    CleanupClosePushL( elms );
    UPnPItemUtility::GetResElements( aItem, elms );
    TInt count = elms.Count();
    
    CUpnpDlnaProtocolInfo* pInfo = NULL;

    // Determine which resources are usable:
    //  1. Filter out other than HTTP GET resources (internal uri's, RTP)
    //  2. Filter out such resources for which DLNA profile is not supported.
    //  3. Filter out such resources for which mime type is not supported.
    for( TInt i = count-1 ; i >= 0; i-- )
        {
        // Make sure that it is a HTTP GET resource. Otherwise remove it and
        // continue with the next one.
        if( elms[ i ]->Value().Left( 
                KHttpDes.iTypeLength ).Compare( KHttpDes() ) != 0 )
            {
            CUpnpElement* destroyable = elms[i];
            aItem.RemoveElementL( destroyable ); 
            delete destroyable;

            continue;
            }

        // Obtain protocolInfo of the res element.
        const CUpnpAttribute* attr = NULL;
        TRAPD( nosuchattribute, 
               attr = &UPnPItemUtility::FindAttributeByNameL(
                                                        *elms[ i ],
                                                        KProtocolInfo() ) );

        if ( nosuchattribute ) 
            {
            // No mandatory protocolinfo attribute. Remove this and continue.
            CUpnpElement* destroyable = elms[i];
            aItem.RemoveElementL( destroyable ); 
            delete destroyable;

            continue;
            }

        // parse protocol info
        pInfo = CUpnpDlnaProtocolInfo::NewL( attr->Value() );
        CleanupStack::PushL( pInfo );

        // Check that DLNA profile is among the supported ones.            
        if ( pInfo->PnParameter() != KNullDesC8() 
             && !UPnPDlnaUtility::IsSupportedDlnaProfile( 
                                                pInfo->PnParameter() ) )
            {
            // DLNA profile not supported. Remove this and continue.
            CUpnpElement* destroyable = elms[i];
            aItem.RemoveElementL( destroyable ); 
            delete destroyable;
            CleanupStack::PopAndDestroy( pInfo );
            pInfo = NULL;

            continue;
            }

        // check that mime type is among the supported ones
        TPtrC8 mime = pInfo->ThirdField();
        if ( !UPnPDlnaUtility::IsSupportedMimeType( mime ) ) 
            {
            // mime type not supported.
            TPtrC8 httpget = pInfo->FirstField();
            TPtrC8 httpdlnatem = pInfo->FourthField();
            HBufC8* tem = NULL;
            tem = httpdlnatem.Right( httpdlnatem.Length() - httpdlnatem.Find( 
                                            KHttpEqual ) - 1 ).AllocLC();
            tem->Des().Trim();
            TPtrC8 httpdlna = *tem;
            CleanupStack::PopAndDestroy( tem );
            if ( httpget.Compare( KHttpGetDes ) != 0 ||
                    !UPnPDlnaUtility::IsSupportedDlnaProfile
                    ( httpdlna ) )
                {
                // mime type not supported and DLNA profile not supported. 
                // Remove this and continue
                CUpnpElement* destroyable = elms[i];
                aItem.RemoveElementL( destroyable ); 
                delete destroyable;
                CleanupStack::PopAndDestroy( pInfo );
                pInfo = NULL;
                
                continue;
                }
            }

        CleanupStack::PopAndDestroy( pInfo );
        pInfo = NULL;
        }

    // All res elements have been processed and removed if they are not 
    // supported. Clean up and return ETrue if there are res elements 
    // left in the item and EFalse if there are no res elements left.        
    CleanupStack::PopAndDestroy( &elms ); 

    UPnPItemUtility::GetResElements( aItem, elms );
    retval = elms.Count();
    elms.Close();    

    return retval;
    }


// End of File
