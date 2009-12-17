/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for UPnP Video player
*
*/


// INCLUDE FILES
// System
#include <aknlists.h>
#include <AknIconArray.h>
#include <aknnavilabel.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <aknnotewrappers.h> 
#include <layoutmetadata.cdl.h> 
#include <AknWaitDialog.h>                      // CAknWaitDialog
#include <aknnotewrappers.h>
#include <AknVolumePopup.h>
#include<remconinterfaceselector.h>             //for CRemConInterfaceSelector
#include<remconcoreapitarget.h>                 //for CRemConCoreApiTarget

// upnp stack api
#include <upnpitem.h>
#include <upnpstring.h>
#include <upnpdevice.h>

// upnpframework / avcontroller api
#include "upnpavrenderingsession.h"
#include "upnpavdevice.h"

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"                    // ResourceFromItemL

// upnpframework / internal api's
#include "upnpcommonutils.h"
#include "upnpcdsreselementutility.h"

// commonui internal
#include <upnpcommonui.rsg>
#include <upnpcommonui.mbg>
#include "upnpcommonui.h"
#include "upnpcommonui.hrh"
#include "upnpvideoplayerdialog.h"
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "commonui.txt" );
#include "upnplog.h"

// CONSTANTS
const TInt KMPOneSecond(1000000);
const TInt KOneHourInSeconds(3600);
const TInt KMaxVolume(100);
const TInt KMiniumVolume(0);
const TInt KGetPosInfoPlaybackInterval( 10 );
const TInt KGetPosInfoBeforePlaybackInterval( 1 );
const TInt KLength = 100;
const TInt KDotLength = 3;
const TInt KVolumeStep = 1;
const TInt KVolumeInterval = 10;

_LIT( KAknMultiViewsMbmFileName,    "\\resource\\apps\\upnpcommonui.mbm" );
_LIT( KDot,                         "..." );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::NewL
// Alternate two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPVideoPlayerDlg* CUPnPVideoPlayerDlg::NewL(
                                MUPnPAVRenderingSession& aRenderingSession,
                                const CUpnpObject& aObject,
                                CUPnPCommonUI& aCommonUI )
    {
    __LOG( "CUPnPVideoPlayerDlg::NewL" );

    CUPnPVideoPlayerDlg* self = NULL;
    self = new ( ELeave ) CUPnPVideoPlayerDlg( aRenderingSession,
                                             aObject,
                                             aCommonUI );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    __LOG( "CUPnPVideoPlayerDlg::NewL End" );
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::ConstructL( )
    {
    __LOG( "CUPnPVideoPlayerDlg::ConstructL" );
    
    iAdjustingVolume = EFalse;
    
    // Resolve the target device capabilities
    ResolveTargetDeviceCapabilitiesL();

    iMinSecFormatString =
        iEikonEnv->AllocReadResourceL( R_QTN_TIME_DURAT_MIN_SEC );

    iHourMinSecFormatString = 
        iEikonEnv->AllocReadResourceL( R_QTN_TIME_DURAT_LONG );

    iNaviPane = static_cast<CAknNavigationControlContainer*>(
                CEikStatusPaneBase::Current()->
                ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iNaviPane->Pop();            

    iOriginalTitleText = HBufC16::NewL( KMaxFileName );
        
    iTimer = CUPnPPeriodic::NewL( CActive::EPriorityStandard );
    
    iCurrentVolume = KMiniumVolume; //0
    
    //create a vertical volume controller
    iVolumePopup = CAknVolumePopup::NewL( NULL, ETrue );
    
    //iVolumePopup->SetObserver( this );
    
    //set value to the volume popup    
    iVolumePopup->SetRange( KMiniumVolume , KMaxVolume );
    iVolumePopup->SetStepSize( KVolumeStep );     
    iVolumePopup->SetDefaultValue( KMiniumVolume );
    
     // Remote control server command repeat timer.
    iVolumeTimer = CUPnPPeriodic::NewL( EPriorityNormal );
            
    // Open a connection to receive Volume Key events.
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    
    // owned by CRemConInterfaceSelector instance
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector,*this );
    TRAPD( err, iInterfaceSelector->OpenTargetL() );
    if( err != KErrNone )
        {
        __LOG1("Leave occured in OpenTargetL %d", err);
        }
    
    __LOG( "CUPnPVideoPlayerDlg::ConstructL End" );                  
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::CUPnPVideoPlayerDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPVideoPlayerDlg::CUPnPVideoPlayerDlg(
                                MUPnPAVRenderingSession& aRenderingSession,
                                const CUpnpObject& aObject,
                                CUPnPCommonUI& aCommonUI ):
                                iRendSession(aRenderingSession),
                                iCommonUI(aCommonUI)
                                
                                
    {
    __LOG( "CUPnPVideoPlayerDlg::CUPnPVideoPlayerDlg" );
    iTargetDevice = &( iRendSession.Device() );
    iItem = &aObject;
    iAction = CUPnPCommonUI::EUPnPNone;
    
    __LOG( "CUPnPVideoPlayerDlg::CUPnPVideoPlayerDlg End" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUPnPVideoPlayerDlg::~CUPnPVideoPlayerDlg()
    {
    __LOG( "CUPnPVideoPlayerDlg::~CUPnPVideoPlayerDlg" );
    
    if( iVolumeTimer )
        {
        iVolumeTimer->Cancel();
        delete iVolumeTimer;
        }
    
    if( iVolumePopup )
        {
        iVolumePopup->CloseVolumePopup();
        }
    
    delete iVolumePopup;        
    delete iInterfaceSelector;
    if( iVideoPlaybackWaitNote )
        {
        TRAP_IGNORE( iVideoPlaybackWaitNote->ProcessFinishedL() );
        delete iVideoPlaybackWaitNote;
        iVideoPlaybackWaitNote = NULL;
        }
    
    if( iTimer )
        {
        iTimer->Cancel();
        }
    
    delete iTimer;
    
    
    delete iMinSecFormatString;
    delete iHourMinSecFormatString;
    delete iTimerNavi;

    delete iNaviDecorator;

    delete iOriginalTitleText;
    TRAP_IGNORE( iRendSession.StopL() );
    iRendSession.RemoveObserver();

    __LOG( "CUPnPVideoPlayerDlg::~CUPnPVideoPlayerDlg End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::PreLayoutDynInitL()
    {
    __LOG( "CUPnPVideoPlayerDlg::PreLayoutDynInitL" );


    // Browse dialog title text
    TUid titlePaneUid;
    titlePaneUid.iUid = EEikStatusPaneUidTitle;
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CEikStatusPaneBase::TPaneCapabilities titlesubPane =
                        statusPane->PaneCapabilities( titlePaneUid );

    
    if( titlesubPane.IsPresent() &&
        titlesubPane.IsAppOwned() )
        {
        CAknTitlePane* titlePane =
                (CAknTitlePane*) statusPane->ControlL( titlePaneUid );
        // Take backup of title text
        iOriginalTitleText->Des().Copy( *titlePane->Text() );

        HBufC *tmpbuf = UpnpString::ToUnicodeL( iItem->Title() );
        CleanupStack::PushL( tmpbuf );
        titlePane->SetTextL( *tmpbuf );
        CleanupStack::PopAndDestroy();
        }
    
    
    // Start to observe the rendering session
    iRendSession.RemoveObserver();
    iRendSession.SetObserver( *this );
    
    PreparePlayBackL();
    
    StartWaitNoteL();

/*
// Temporally commented out. Will be uncommented when UpnpAvController 
// provides cancel functionaly fot SetUri action (Upnp Fw 2.5)
    if( iExitCode == EEikBidCancel )
        {
        // Cancel the SetUri
        //iRendSession.Cancel();
        // Send Stop
        TRAP_IGNORE( iRendSession.StopL() );
        ClearTitleL();
        iExitCode = KErrCancel;
        }
*/
    User::LeaveIfError( iExitCode );

    if ( !Layout_Meta_Data::IsLandscapeOrientation() )
        {
        iEikonEnv->AppUiFactory()->StatusPane()->SwitchLayoutL(
                                 R_AVKON_STATUS_PANE_LAYOUT_USUAL );
        }
    // Volume level to 0
    iCurrentVolume = KMiniumVolume;
    
    // try to get volume level from renderer
    TRAPD( err, iRendSession.GetVolumeL() );
    if( !err )
        {
        iAdjustingVolume = ETrue;
        }
    
    __LOG( "CUPnPVideoPlayerDlg::PreLayoutDynInitL End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::OkToExitL(TInt aButtonId)
// called by framework when the softkey is pressed
// may leave without trap(System error)
// --------------------------------------------------------------------------
//
TBool CUPnPVideoPlayerDlg::OkToExitL( TInt aButtonId )
    {
    __LOG( "CUPnPVideoPlayerDlg::OkToExitL" );

    TBool retval = EFalse;
    TInt error = KErrNone;

    switch ( aButtonId )
        {
        case KErrSessionClosed: //either MS or MR disappears
        case KErrDisconnected: //fall through
            {
            // Connection failed so we have to close player
            TRAP_IGNORE( ClearTitleL() );
            return ETrue;
            }
        case EUPnPPauseCmd:
            {
            iAction = CUPnPCommonUI::EUPnPShow;
            TRAP( error, iRendSession.PauseL() );
            if ( KErrNone != error )
                {

                __LOG1( "CUPnPVideoPlayerDialog::OkToExitL:Pause %d",
                         error );
                }
            break;
            }
        case EUPnPReplayCmd:
            {
            iAction = CUPnPCommonUI::EUPnPShow;

            PreparePlayBackL();
            iExitCode = KErrNone;
            StartWaitNoteL();
/*
// Temporally commented out. Will be uncommented when UpnpAvController 
// provides cancel functionaly fot SetUri action (Upnp Fw 2.5)

            if( iExitCode == EEikBidCancel ||
                iExitCode == KErrCancel )
                {
                // Cancel the SetUri
                //iRendSession.Cancel();
                // Send Stop
                TRAP_IGNORE( iRendSession.StopL() );
                }
*/
            if( iExitCode == KErrSessionClosed ||
                iExitCode == KErrDisconnected )
                {
                TryExitL( iExitCode );
                }
            break;
            }
        case EUPnPContinueCmd:
            {
            iAction = CUPnPCommonUI::EUPnPShow;
            TRAP( error, iRendSession.PlayL() );
            if ( KErrNone != error )
                {
                __LOG1( "CUPnPVideoPlayerDialog::\
                                            OkToExitL: Continue %d", error );
                }
            break;
            }
        case EUPnPBackCmd: //quit the video player dialog
            {
            TRAP_IGNORE( ClearTitleL() ); //avoid system error
            return ETrue;            
            }
        case EUPnPStopCmd:
            {
            iAction = CUPnPCommonUI::EUPnPShow;
            TRAP( error, iRendSession.StopL() );
            if ( KErrNone != error )
                {

                __LOG1( "CUPnPVideoPlayerDialog::\
                                    OkToExitL: Stop %d", error );
                }
            break;
            }
        case EAknSoftkeyOk:
            {
            break;
            }
        case EAknSoftkeyCancel: 
        case EAknSoftkeyExit: 
        case EAknCmdExit: // fall through
        case EEikCmdExit:
            {
            TRAP_IGNORE( ClearTitleL() );
            return ETrue; //back to previous dialog
            }
        default:
            {
            // ignore
            break;
            }
        }
    
    __LOG( "CUPnPVideoPlayerDlg::OkToExitL End" );    
    return retval;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::UpdateSoftkeysL
// --------------------------------------------------------------------------
void CUPnPVideoPlayerDlg::UpdateSoftkeysL( TInt aState )
    {
    __LOG( "CUPnPVideoPlayerDlg::UpdateSoftkeysL" );

    
    if ( aState == EUPnPVideoPlay &&
         iPauseCapability )
        {
        ButtonGroupContainer().SetCommandSetL( 
                                    R_UPNPCOMMONUI_SOFTKEYS_PAUSE_STOP );
        ButtonGroupContainer().DrawDeferred();
        }
    else if ( aState == EUPnPVideoPlay &&
              !iPauseCapability )
        {
        ButtonGroupContainer().SetCommandSetL( 
                                    R_UPNPCOMMONUI_SOFTKEYS_EMPTY_STOP );
        ButtonGroupContainer().DrawDeferred();
        }
    else if ( aState == EUPnPVideoPause )
        {
        ButtonGroupContainer().SetCommandSetL(
                                    R_UPNPCOMMONUI_SOFTKEYS_CONTINUE_STOP );
        ButtonGroupContainer().DrawDeferred();
        }
    else if ( aState == EUPnPVideoStop )
        {
        ButtonGroupContainer().SetCommandSetL( 
                                    R_UPNPCOMMONUI_SOFTKEYS_REPLAY_BACK );
        ButtonGroupContainer().DrawDeferred();
        }
    __LOG( "CUPnPVideoPlayerDlg::UpdateSoftkeysL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CUPnPVideoPlayerDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    __LOG( "CUPnPVideoPlayerDlg::OfferKeyEventL" );
    
    TKeyResponse response = EKeyWasConsumed;
    TInt error = KErrNone;
    if ( aType == EEventKeyUp )
        {
        if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
            {
            // Adjust volume if the target rendering device supports the
            // feature, and we are not waiting for a response for a previous
            // set volume action.
            
            iVolumeUp = EFalse;
            ChangeVolume( this );
            }
        else if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            // Adjust volume if the target rendering device supports the
            // feature, and we are not waiting for a response for a previous
            // set volume action.
            iVolumeUp = ETrue;    
            ChangeVolume( this );
            }

        if( KErrNotReady == error ||
            KErrDisconnected == error )
            {
            TryExitL( error );
            }
        }
    else if( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        CAknDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    __LOG( "CUPnPVideoPlayerDlg::OfferKeyEventL End" ); 
    return response;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ShowVolumeIndicatorL
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::ShowVolumeIndicatorL()
    {
    __LOG( "CUPnPVideoPlayerDlg::ShowVolumeIndicatorL" );

    if ( iCurrentVolume >= KMiniumVolume && iCurrentVolume <= KMaxVolume )
        {
        __LOG( " - display volume popup indicator" );
        if ( iVolumePopup )
            {
            iVolumePopup->SetValue( iCurrentVolume );
            iVolumePopup->ShowVolumePopupL(); // will close after 1s
            }                
        }
    else
        {
        HideVolumeIdicator( this );
        }
    __LOG( "CUPnPVideoPlayerDlg::ShowVolumeIndicatorL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::HideVolumeIdicator
// --------------------------------------------------------------------------
//
TInt CUPnPVideoPlayerDlg::HideVolumeIdicator( TAny* aPtr )
    {
    __LOG( "CUPnPVideoPlayerDlg::HideVolumeIdicator" );

    CUPnPVideoPlayerDlg* self = 
        static_cast<CUPnPVideoPlayerDlg*>( aPtr );

    if ( self->iVolumePopup )
        {
        self->iVolumePopup->CloseVolumePopup();
        }
        
    __LOG( "CUPnPVideoPlayerDlg::HideVolumeIdicator End" );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::TimerIndicatorL
// --------------------------------------------------------------------------
//
TInt CUPnPVideoPlayerDlg::TimerIndicatorL( TAny* aPtr )
    {
    __LOG( "CUPnPVideoPlayerDlg::TimerIndicatorL" );
    CUPnPVideoPlayerDlg* self = 
        static_cast<CUPnPVideoPlayerDlg*>(aPtr);
    
    if( self->iTotalLengthInSeconds > 0)
        {
        if( self->iPlaybackPosInSeconds >= self->iTotalLengthInSeconds )
            {
            self->iTimer->Cancel();
            self->iRendererPlaying = EFalse;            
            self->iPlaybackPosInSeconds = 0;
            self->iTotalLengthInSeconds = 0;
            return KErrNone;
            }
        }
        
    // Increase the count of timer expirations
    self->iTimerRefresh++;
    
    // Increase the elapsed time by one (this timer elapses once a second).
    if ( self->iRendererPlaying && !self->iPause )
        {
        self->iPlaybackPosInSeconds++;
        }
    
    self->UpdateNaviPaneL();
        
    // Refresh the position after every KGetPosInfoPlaybackInterval
    // secs from the beginning of play if necessary 
    // (total length not set set at that moment) 
    if( self->iTimerRefresh >= self->iGetPositionInfoInterval )
        {
        self->iTimerRefresh = 0;
        TRAPD( err, self->iRendSession.GetPositionInfoL() );
        if ( err )
            {
            __LOG1( "TimerIndicatorL GetPositionInfoL err: %d", err );
            }
        }   
        
    __LOG( "CUPnPVideoPlayerDlg::TimerIndicatorL End" );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ClearTitleL
// Sets title back to default.
// --------------------------------------------------------------------------
void CUPnPVideoPlayerDlg::ClearTitleL()
    {
    __LOG( "CUPnPVideoPlayerDlg::ClearTitleL" );

    TUid titlePaneUid;
    titlePaneUid.iUid = EEikStatusPaneUidTitle;
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CEikStatusPaneBase::TPaneCapabilities titlesubPane =
                        statusPane->PaneCapabilities( titlePaneUid );

    CAknTitlePane* titlePane =
        ( CAknTitlePane* ) statusPane->ControlL( titlePaneUid );
    // Set original text
    titlePane->SetTextL( *iOriginalTitleText );
    iNaviPane->Pop();
    __LOG( "CUPnPVideoPlayerDlg::ClearTitleL End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerView::UpdateNaviPaneL()
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::UpdateNaviPaneL()
    {
    __LOG( "CUPnPVideoPlayerDlg::UpdateNaviPaneL" );
    
    TTime posTime = TTime( ( iPlaybackPosInSeconds )*KMPOneSecond );
    TTime durTime = TTime( iTotalLengthInSeconds*KMPOneSecond );

    TBuf<16> pos;
    TBuf<16> dur;

    if ( iTotalLengthInSeconds > 0 &&
         iTotalLengthInSeconds < KOneHourInSeconds )
       {
       // Format time to user readable format. (min:sec)
       posTime.FormatL( pos, *iMinSecFormatString );
       durTime.FormatL( dur, *iMinSecFormatString );
       }
    else
       {
       // Format time to user readable format. (hour:min:sec)
       posTime.FormatL( pos, *iHourMinSecFormatString );
       durTime.FormatL( dur, *iHourMinSecFormatString );
       }
    if ( AknTextUtils::DigitModeQuery( AknTextUtils::EDigitModeShownToUser ) )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( pos );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( dur );
        }
    // if duration greated than 0, show postion in 00:00/00:00 format
    if ( iTotalLengthInSeconds > 0 )
        {
        CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat(2);
        CleanupStack::PushL( strings );
        strings->AppendL( pos ); //First string (position)
        strings->AppendL( dur ); //Second string (duration)
        delete iTimerNavi; iTimerNavi = NULL;
        iTimerNavi = StringLoader::LoadL( R_UPNPCOMMONUI_VIDEO_NAVI_TIME,
                                          *strings,
                                          iEikonEnv );

        SetNaviLabelL( *iTimerNavi );

        CleanupStack::PopAndDestroy( strings ); // strings & stringholder
        }
    else // show position in 00:00:00 format
        {
        SetNaviLabelL( pos );
        }

    __LOG( "CUPnPVideoPlayerDlg::UpdateNaviPaneL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerView::SetNaviLabelL
// Set navigation label from descriptor.
// (other items were commented in a header).
// --------------------------------------------------------------------------
void CUPnPVideoPlayerDlg::SetNaviLabelL( const TDesC& aText )
    {
    __LOG( "CUPnPVideoPlayerDlg::SetNaviLabelL" );
    
    TUid naviPaneUid;
    naviPaneUid.iUid = EEikStatusPaneUidNavi;

    // check if we can access the navi pane
    if ( CheckStatusPaneCapabilities( naviPaneUid ) )
        {
        CAknNavigationControlContainer* naviPane = 
            static_cast<CAknNavigationControlContainer*>
                ( iEikonEnv->AppUiFactory()->StatusPane()->ControlL( 
                                                            naviPaneUid ) );

        if ( iNaviDecorator &&
             ( iNaviDecorator->ControlType() ==
                CAknNavigationDecorator::ENaviLabel ) )
            {
            // Set the navi text
            CAknNaviLabel* naviLabel = static_cast<CAknNaviLabel*>
                                ( iNaviDecorator->DecoratedControl() );
            naviLabel->SetTextL( aText );
            }
        else
            {
            // create the navigation pane label
            iNaviDecorator = naviPane->CreateNavigationLabelL( aText );
            }
        naviPane->PushL( *iNaviDecorator );
        }
        
    __LOG( "CUPnPVideoPlayerDlg::SetNaviLabelL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::CheckStatusPaneCapabilities
// Check the status pane capabilities.
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
TBool CUPnPVideoPlayerDlg::CheckStatusPaneCapabilities(
                                            const TPaneId& aPaneId )
    {
    __LOG( "CUPnPVideoPlayerDlg::CheckStatusPaneCapabilities" );

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();

    CEikStatusPaneBase::TPaneCapabilities subPane =
                                statusPane->PaneCapabilities( aPaneId );
    __LOG( "CUPnPVideoPlayerDlg::CheckStatusPaneCapabilities End" );
    // check if we can access the status pane
    return ( subPane.IsPresent() && 
             subPane.IsAppOwned() );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::CreateCustomControlL() const
// Draws the display
// --------------------------------------------------------------------------
//
SEikControlInfo CUPnPVideoPlayerDlg::CreateCustomControlL( TInt aControlType )
    {   
    __LOG( "CUPnPVideoPlayerDlg::CreateCustomControlL" );

    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;
    switch ( aControlType )
        {
        case ECustomCtrlDlgCtCustomControl:
            {
            iContainer = new (ELeave) CCustomCtrlDlgCustomControl();
            controlInfo.iControl = iContainer;
            ( (CCustomCtrlDlgCustomControl*)controlInfo.iControl )->
                        SetRenderingSession( iRendSession );
            break;
            }
        default:
            {
            break;
            }
        }
    __LOG( "CUPnPVideoPlayerDlg::CreateCustomControlL End" );    
    return controlInfo;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::VolumeResult()
// Video volume callback
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::VolumeResult( TInt aError, TInt aVolumeLevel,
                                        TBool /*aActionResponse*/ )
    {
    __LOG2( "CUPnPVideoPlayerDlg::VolumeResult err=%d vol=%d",
        aError, aVolumeLevel );

    iAdjustingVolume = EFalse;
    
    if( aError == KErrNone )
        {
        iCurrentVolume = aVolumeLevel;
        }
    else 
        {
        // ignore error
        }
    TRAP_IGNORE( ShowVolumeIndicatorL() );
    __LOG( "CUPnPVideoPlayerDlg::VolumeResult End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::InteractOperationComplete() const
// Video operation callback
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::InteractOperationComplete(
                                        TInt aError,
                                        TUPnPAVInteractOperation aOperation )
    {
    
    __LOG1( "CUPnPVideoPlayerDlg::InteractOperationComplete %d", aError );
    TInt error = KErrNone;
    TRAP( error, InteractOperationCompleteL( aError, aOperation ) )
    __LOG( "CUPnPVideoPlayerDlg::InteractOperationComplete" );    
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::InteractOperationCompleteL() const
// leave function to handle callback
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::InteractOperationCompleteL(
                                        TInt aError,
                                        TUPnPAVInteractOperation aOperation )
    {
    
    __LOG1( "CUPnPVideoPlayerDlg::InteractOperationCompleteL %d", aError );
    // Show error note if there was an error

    if ( aError != KErrNone )
        {
        FinishWaitNoteL();
        //stop timer
        HideVolumeIdicator( this );
        iTimer->Cancel();
        iRendererPlaying = EFalse;        
        UpdateSoftkeysL( EUPnPVideoStop );
        iAction = CUPnPCommonUI::EUPnPShow;
        HandleErrorL( aError );
        return;
        }
    
    if( iExitCode == EEikBidCancel ) //if video playing is cancelled
        {
        iRendSession.StopL();
        iExitCode = KErrNone;
        }
    switch(aOperation)
        {
        case EUPnPAVPlay:
            {
            FinishWaitNoteL();
            ContinueAfterPauseL();     
            break;
            }
        case EUPnPAVPlayUser: // fall through
            {
            //display timer
            StartDisplayTimer();
            // Query the position info (track duration, position, etc.)
            TRAP_IGNORE( iRendSession.GetPositionInfoL() );
            ContinueAfterPauseL();
            break;
            }
        case EUPnPAVPause: // fall through
        case EUPnPAVPauseUser:
            {
            iPause = ETrue;
            iRendererPlaying = EFalse;
            iTimer->Cancel();
            UpdateSoftkeysL( EUPnPVideoPause );
            break;
            }
        case EUPnPAVStop: // fall through
        case EUPnPAVStopUser:
            {        
            HideVolumeIdicator( this );
            iTimer->Cancel();
            iRendererPlaying = EFalse;            
            iPlaybackPosInSeconds = 0;
            iTotalLengthInSeconds = 0;
            UpdateSoftkeysL( EUPnPVideoStop );    
            }
            break;
        default:
            {
            // unknown operation
            __PANICD( __FILE__, __LINE__ );
            break;
            }
        }
    __LOG( "CUPnPVideoPlayerDlg::InteractOperationCompleteL" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::InteractOperationComplete() const
// Video operation callback
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::MuteResult( TInt aError, TBool aMute,
                                      TBool /*aActionResponse*/ )
    {
    __LOG( "CUPnPVideoPlayerDlg::MuteResult" );
    __LOG3( "MuteResult - aError %d aMute %d iVolume %d ",
        aError, aMute, iCurrentVolume );
    
    if( KErrNone == aError )
        {
        if( aMute )
            {
            iCurrentVolume = KMiniumVolume;
            }
        if ( iVolumePopup)
            {               
            iVolumePopup->SetValue( iCurrentVolume );
            // will close after 1s
            TRAP_IGNORE( iVolumePopup->ShowVolumePopupL() );
            }
        }
    else
        {
        HideVolumeIdicator( this );
        }
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::PositionInfoResult
// UPnP AV Controller calls this method as a response to Position 
// Info action. The current position and the total length of the
// track that is currently playing is returned. The results are in
// (hh:mm:ss) format.
// --------------------------------------------------------------------------
void CUPnPVideoPlayerDlg::PositionInfoResult(  TInt aError,
                                               const TDesC8& aTrackPosition,
                                               const TDesC8& aTrackLength )
    {
    
    __LOG1( "CUPnPVideoPlayerDlg::PositionInfoResult %d", aError );
    if( KErrNone == aError )
        {
        if( aTrackPosition.Length() )
            {
            TLex8 lex( aTrackPosition );
            TInt tmpint;
            TChar ch;
            if ( lex.Val( tmpint ) == KErrNone )
                {
                iPlaybackPosInSeconds = tmpint * 3600;
                }
               ch = lex.Get();
            if ( lex.Val( tmpint ) == KErrNone )
                {
                iPlaybackPosInSeconds = iPlaybackPosInSeconds + tmpint * 60;
                }
            ch = lex.Get();
            if ( lex.Val( tmpint ) == KErrNone )
                {
                iPlaybackPosInSeconds = iPlaybackPosInSeconds + tmpint;
                }
            }
        if( aTrackLength.Length() )
            {
            iTotalLengthInSeconds = 0;
            TLex8 lex1( aTrackLength );
            TInt tmpint;
            TChar ch;
            if ( lex1.Val( tmpint ) == KErrNone )
                {
                iTotalLengthInSeconds = tmpint * 3600;
                }
            ch = lex1.Get();
            if ( lex1.Val( tmpint ) == KErrNone )
                {
                iTotalLengthInSeconds = iTotalLengthInSeconds + tmpint * 60;
                }
            ch = lex1.Get();
            if ( lex1.Val( tmpint ) == KErrNone )
                {
                iTotalLengthInSeconds = iTotalLengthInSeconds + tmpint;
                
                }
            }
        }// if( KErrNone == aError )
    
    if ( iPlaybackPosInSeconds > 0 )
        {
        // after knowing remote renderer has started to play at the first
        // time, set GetPositonInfo frequency and update the elapsed time.
        if ( iGetPositionInfoInterval != KGetPosInfoBeforePlaybackInterval )
            {
            iGetPositionInfoInterval = KGetPosInfoPlaybackInterval;
            UpdateNaviPaneL();
            }
        if ( iPause )
            {
            iRendererPlaying = EFalse;
            }
        else
            {
            iRendererPlaying = ETrue;
            }
        }    
    __LOG( "CUPnPVideoPlayerDlg::PositionInfoResult End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::SetURIResult
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::SetURIResult( TInt aError )
    {
    
    TRAP_IGNORE( SetURIResultL( aError ) );
    __LOG( "CUPnPVideoPlayerDlg::SetURIResult End" );
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ContinueAfterPauseL
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::ContinueAfterPauseL()
    {  
    iPause = EFalse;
    UpdateSoftkeysL( EUPnPVideoPlay );
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::SetURIResultL
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::SetURIResultL( TInt aError )
    {
    __LOG1( "CUPnPVideoPlayerDlg::SetURIResult %d", aError );
    
    
    if( iExitCode == EEikBidCancel )
        {
        __LOG( "CUPnPVideoPlayerDlg::SetURIResult Cancelled" );
        return;
        }    
    
    if( KErrNone != aError )
        {
        FinishWaitNoteL();
        UpdateSoftkeysL( EUPnPVideoStop );
        iAction = CUPnPCommonUI::EUPnPShow;
        HandleErrorL( aError );
        }
    else
        {
        iGetPositionInfoInterval = KGetPosInfoBeforePlaybackInterval;
        iRendSession.PlayL();
        }
    __LOG( "CUPnPVideoPlayerDlg::SetURIResult End" );
    }

//for media server or WLAN disappeared case
void CUPnPVideoPlayerDlg::DismissItselfL( TInt aError )
    {
    HandleErrorL( aError );
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::MediaRendererDisappeared
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::MediaRendererDisappeared( 
                                TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "[UPnPCommonUI]\t CUPnPVideoPlayerDlg: \
Source Device Dissapeared" );
    // only for renderer disappearing
    if( aReason == EDisconnected )
        {
        TRAP_IGNORE( HandleErrorL( KErrSessionClosed ) );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayerDlg::HandleResourceChange
// Called by framework when resource change event detected
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::HandleResourceChange(TInt aType)
    {
    __LOG( "CUPnPVideoPlayerDlg::HandleResourceChange" );

    CAknDialog::HandleResourceChange( aType );

    if ( aType == KAknsMessageSkinChange )
        {
        iContainer->DrawDeferred();
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        if (!Layout_Meta_Data::IsLandscapeOrientation())
            {
            TRAP_IGNORE( iEikonEnv->AppUiFactory()->StatusPane()->
                       SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL ) );
            }
        SizeChanged();
        iContainer->DrawDeferred();
        }
    else
        {
        // pass
        }
    __LOG( "CUPnPVideoPlayerDlg::HandleResourceChange End" );    
    }

// --------------------------------------------------------------------------
// CUPnPMusicPlayerDlg::SizeChanged
// Called by framework after the dialog size has been changed
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::SizeChanged()
    {
    __LOG( "CUPnPVideoPlayerDlg::SizeChanged" );

    if ( iContainer )
        {
        TRect clientRect = Rect();

        iContainer->SetRect( clientRect );
        iContainer->DrawDeferred();
        }
    CAknDialog::SizeChanged();
    __LOG( "CUPnPVideoPlayerDlg::SizeChanged End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ResolveTargetDeviceCapabilitiesL
// Resolves the target UPnP device capabilites.
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::ResolveTargetDeviceCapabilitiesL()
    {
    __LOG( "CUPnPVideoPlayerDlg::ResolveTargetDeviceCapabilitiesL" );
    if( iTargetDevice )
        {
        iPauseCapability = iTargetDevice->PauseCapability();
        iVolumeCapability = iTargetDevice->VolumeCapability();
        __LOG1( "iPauseCapability:: %d", iPauseCapability );
        __LOG1( "iVolumeCapability:: %d", iVolumeCapability );
        
        }

    __LOG( "CUPnPVideoPlayerDlg::ResolveTargetDeviceCapabilitiesL End" );    
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::HandleErrorL
// Resolves the target UPnP device capabilites.
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::HandleErrorL( TInt aError )
    {
    iCommonUI.GetUpnpAction( iAction );
    if( KErrSessionClosed == aError ||
        KErrDisconnected == aError )
        {
        iExitCode = aError;
        TRAP_IGNORE( iRendSession.StopL() );
        if( iVideoPlaybackWaitNote )
            {
            TRAP_IGNORE( iVideoPlaybackWaitNote->ProcessFinishedL() );
            delete iVideoPlaybackWaitNote;
            iVideoPlaybackWaitNote = NULL;
            }
            
        // When wait note is display or playback is ongoing, if received 
        // KErrDisconnected or KErrSessionClosed, then to exit the 
        // VideoPlayerDlg.
        TryExitL( iExitCode );    
        }
     else //common error codes
        {
        iCommonUI.HandleCommonErrorL( aError, NULL );
        iAction = CUPnPCommonUI::EUPnPNone;
        }
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::StartWaitNoteL
// --------------------------------------------------------------------------
//
TInt CUPnPVideoPlayerDlg::StartWaitNoteL()
    {
    
    __LOG( "CUPnPVideoPlayerDlg::StartWaitNoteL" );
    
    TInt ret = KErrNone;
    //start preparing playback
    if( !iVideoPlaybackWaitNote )
        {
        iVideoPlaybackWaitNote = new(ELeave)CAknWaitDialog(
                                ( REINTERPRET_CAST( CEikDialog**, 
                                &iVideoPlaybackWaitNote ) ), ETrue );
    
        // Register to get the responses from the dialog 
        iVideoPlaybackWaitNote->SetCallback( this );  
        
        ret = iVideoPlaybackWaitNote->ExecuteLD( 
                    R_UPNPCOMMONUI_VIDEO_PLAYBACK_WAIT_NOTE_DIALOG );
        }   
    
    __LOG1( "CUPnPVideoPlayerDlg::StartWaitNoteL %d", ret );
    return ret;
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::FinishWaitNoteL
// Finish the video playback wait note
// --------------------------------------------------------------------------
// 
void CUPnPVideoPlayerDlg::FinishWaitNoteL() 
    {
    if( iVideoPlaybackWaitNote )
        {
        iVideoPlaybackWaitNote->ProcessFinishedL();
        }
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::DialogDismissedL
// --------------------------------------------------------------------------
// 
void CUPnPVideoPlayerDlg::DialogDismissedL( TInt aButtonId ) 
    {
    if( aButtonId == EEikBidCancel )
        {
        iExitCode = EEikBidCancel;

        __LOG( "CUPnPVideoPlayerDlg::DialogDismissedL -Cancelled" );
        }
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::PreparePlayBackL
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::PreparePlayBackL()
    {
    __LOG( "CUPnPVideoPlayerDlg::PreparePlayBackL" );
    if( !iTargetDevice )
        {
        __LOG( "CUPnPVideoPlayerDlg::PreLayoutDynInitL: \
                                                TargetDeviceL FAILED!" );
        User::Leave( KErrNotReady );
        }   
    const CUpnpItem* item = static_cast<const CUpnpItem*>( iItem );
    HBufC8* uri 
            = UPnPItemUtility::ResourceFromItemL( *item ).Value().AllocLC();
     // Set the URI
    iRendSession.SetURIL( *uri, *item );
    CleanupStack::PopAndDestroy ( uri ); 
        
    __LOG( "CUPnPVideoPlayerDlg::PreparePlayBackL - End" );
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::StartDisplayTimer
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::StartDisplayTimer()
    {
    //display time on the navipane: 00:00/00:00(total length)
    if ( !iTimer->IsActive() )
        {
        iTimer->Start(
        0,
        KMPOneSecond,
        TCallBack( CUPnPVideoPlayerDlg::TimerIndicatorL,
        this ) );
        }  
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::DoChangeVolumeL
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::DoChangeVolumeL()
    {
    __LOG( "CUPnPVideoPlayerDlg::DoChangeVolumeL" );
    
    __LOG1( " iCurrentVolume = %d ", iCurrentVolume );
    
    if( iAdjustingVolume || !iVolumeCapability )
        {
        return;
        }
        
    if( iVolumeUp )
        {
        iNewVolume = iCurrentVolume + KVolumeInterval;
        if( iNewVolume > KMaxVolume )
            {
            iNewVolume = KMaxVolume;
            }    
        }
    else
        {
        iNewVolume = iCurrentVolume - KVolumeInterval;
        if( iNewVolume < KMiniumVolume )
            {
            iNewVolume = KMiniumVolume;
            }
        }
    
    iRendSession.SetVolumeL( iNewVolume );
    
    iAdjustingVolume = ETrue;
    
    __LOG( "CUPnPVideoPlayerDlg::DoChangeVolumeL - End" );
    }
    
// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::ChangeVolume
// --------------------------------------------------------------------------
//
TInt CUPnPVideoPlayerDlg::ChangeVolume( TAny* aPtr )
    {    
    CUPnPVideoPlayerDlg* self = 
    static_cast<CUPnPVideoPlayerDlg*>( aPtr );
    
    TRAPD( err, self->DoChangeVolumeL() );
    
    __LOG1( "CUPnPVideoPlayerDlg::DoChangeVolumeL %d", err );
    
    if( KErrNone == err )    
        {
        self->iCurrentVolume = self->iNewVolume;
        }
    else if( err == KErrSessionClosed || err == KErrDisconnected )
        {
        //either MS or MR disappears
        TRAP_IGNORE( self->TryExitL( err ) );
        }    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPVideoPlayerDlg::MrccatoCommand
// --------------------------------------------------------------------------
//
void CUPnPVideoPlayerDlg::MrccatoCommand(
                        TRemConCoreApiOperationId aOperationId,
                        TRemConCoreApiButtonAction aButtonAct )
    {
    __LOG( "CUPnPVideoPlayerDlg::MrccatoCommand" );    
    __LOG2( "CUPnPVideoPlayerDlg::MrccatoCommand (0x%X,%d)", 
        aOperationId, aButtonAct );
   
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    iVolumeUp = ETrue;
                    iVolumeKeyPressHold = ETrue;
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start( KMPOneSecond,
                                 KMPOneSecond, 
                                 TCallBack( ChangeVolume, this ) );
                    break;
                    }    
                case ERemConCoreApiButtonRelease:
                    {
                    iVolumeKeyPressHold = EFalse;
                    iVolumeTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    iVolumeKeyPressHold = EFalse;
                    iVolumeUp = ETrue;
                    TRAP_IGNORE( DoChangeVolumeL() );
                    break;
                    }
                default:
                    break;
                }
            break;
            }
        case ERemConCoreApiVolumeDown:
            {
            switch ( aButtonAct )
                {
                case ERemConCoreApiButtonPress:
                    {
                    iVolumeKeyPressHold = ETrue;
                    iVolumeUp = EFalse;
                    iVolumeTimer->Cancel();
                    iVolumeTimer->Start( KMPOneSecond,
                                 KMPOneSecond, 
                                 TCallBack( ChangeVolume, this ) );
                    break;
                    }    
                case ERemConCoreApiButtonRelease:
                    {                        
                    iVolumeKeyPressHold = EFalse;
                    iVolumeTimer->Cancel();
                    break;
                    }
                case ERemConCoreApiButtonClick:
                    {
                    iVolumeKeyPressHold = EFalse;
                    iVolumeUp = EFalse;
                    TRAP_IGNORE( DoChangeVolumeL() );
                    break;
                    }
                default:
                    break;
                }
            break;
            }
        
        // Only volume keys handled    
        case ERemConCoreApiStop:
        case ERemConCoreApiRewind:
        case ERemConCoreApiFastForward:
        case ERemConCoreApiPausePlayFunction:
        case ERemConCoreApiPause:
        default:
            break;
        }
    __LOG( "CUPnPVideoPlayerDlg::MrccatoCommand - End" );
    }

// ===========================================================================
// CCustomCtrlDlgCustomControl Implementation
// ===========================================================================

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::CCustomCtrlDlgCustomControl
// --------------------------------------------------------------------------
//
CCustomCtrlDlgCustomControl::~CCustomCtrlDlgCustomControl()
    {
    __LOG( "CCustomCtrlDlgCustomControl::~CCustomCtrlDlgCustomControl" );
    delete iImage;
    iImage = NULL;
    __LOG( "CCustomCtrlDlgCustomControl::~CCustomCtrlDlgCustomControl End" );
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::Draw() const
// Draws the display
// --------------------------------------------------------------------------
//
void CCustomCtrlDlgCustomControl::Draw( const TRect& /*aRect*/ ) const
    {
    __LOG( "CCustomCtrlDlgCustomControl::Draw" );

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    CWindowGc& gc=SystemGc();
    gc.Clear( rect );
    gc.SetClippingRect( rect );
    gc.DrawRect( rect );
    __LOG( "CCustomCtrlDlgCustomControl::Draw End" );
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::SizeChanged() const
// Set window size
// --------------------------------------------------------------------------
//
void CCustomCtrlDlgCustomControl::SizeChanged()
    {
    __LOG( "CCustomCtrlDlgCustomControl::SizeChanged" );

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    rect.iTl = TPoint( 0,0 );
    iPosition = TPoint( rect.iTl );

    iImage->SetPosition( iPosition );
    iImage->SetSize( rect.Size() );

    __LOG( "CCustomCtrlDlgCustomControl::SizeChanged End" );
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::ConstructFromResourceL() 
// Set window size
// --------------------------------------------------------------------------
//
void CCustomCtrlDlgCustomControl::ConstructFromResourceL(
                                           TResourceReader& /*aReader*/)
    {
    __LOG( "CCustomCtrlDlgCustomControl::ConstructFromResourceL" );

    iImage = new(ELeave) CImage( *iRendSession );
    iImage->SetContainerWindowL( *this );

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
    rect.iTl = TPoint( 0,0 );

    SetRect( rect );
    ActivateL();
    __LOG( "CCustomCtrlDlgCustomControl::ConstructFromResourceL End" );
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::CountComponentControls() const
// Return number of components
// --------------------------------------------------------------------------
//
TInt CCustomCtrlDlgCustomControl::CountComponentControls() const
    {
    return 1;
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::ComponentControl() const
// return control pointer
// --------------------------------------------------------------------------
//
CCoeControl* CCustomCtrlDlgCustomControl::ComponentControl(
    TInt aIndex ) const
    {
    if ( aIndex==0 )
        {
        return iImage;
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CCustomCtrlDlgCustomControl::SetRenderingSession()
// --------------------------------------------------------------------------
//
void CCustomCtrlDlgCustomControl::SetRenderingSession(
                                    MUPnPAVRenderingSession& aRendSession )
    {
    __LOG( "CCustomCtrlDlgCustomControl::SetRenderingSession" );
    iRendSession = &aRendSession;
    __LOG( "CCustomCtrlDlgCustomControl::SetRenderingSession End" );
    }

// ===========================================================================
// CImage Implementation
// ===========================================================================

// --------------------------------------------------------------------------
// CImage::CImage()
// --------------------------------------------------------------------------
//
CImage::CImage( MUPnPAVRenderingSession& aRendSession):
                                        iRendSession(aRendSession)
    {
    __LOG( "CImage::CImage" );

    iBitmap = NULL;
    iMask = NULL;

    TFileName mbmFileName( KAknMultiViewsMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRAP_IGNORE( AknsUtils::CreateIconL(
        skin,
        KAknsIIDDefault,
        iBitmap,
        iMask,
        mbmFileName,
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer,
        EMbmUpnpcommonuiQgn_graf_upnp_ext_renderer_mask ) );
    __LOG( "CImage::CImage End" );
    }

// --------------------------------------------------------------------------
// CImage::~CImage()
// --------------------------------------------------------------------------
//
CImage::~CImage()
    {
    __LOG( "CImage::~CImage");
    delete iBitmap;
    iBitmap = NULL;
    delete iMask;
    iMask = NULL;
    __LOG( "CImage::~CImage End");
    }

// --------------------------------------------------------------------------
// CImage::Draw()
// Draw display
// --------------------------------------------------------------------------
//
void CImage::Draw(const TRect& /*aRect*/) const
    {
    __LOG( "CImage::Draw" );

    //change image size
    TSize imageSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EMainPane, imageSize );
    AknIconUtils::SetSize( iBitmap,imageSize );

    TRAPD( error,DrawL( imageSize ) );
    if ( error )
        {
        __LOG1( "CImage::DrawL %d", error );
        }
        
    __LOG( "CImage::Draw End" );
    }

// --------------------------------------------------------------------------
// CImage::DrawL()
// Draw display
// --------------------------------------------------------------------------
//
void CImage::DrawL( const TRect& aRect ) const
    {
    __LOG( "CImage::DrawL" );

    CWindowGc& gc = SystemGc();
    gc.Clear();
    gc.DrawBitmap( aRect, iBitmap );

    const CUpnpAVDevice* device = &( iRendSession.Device() );
    
    const CFont* normalFont = iEikonEnv->NormalFont();
    TInt baseline = ( aRect.Height() / 12 + 
                    ( normalFont->AscentInPixels() / 5 ) ); 
    gc.UseFont( normalFont );
    gc.SetPenColor( KRgbBlack );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );

    // Get device friendly name and replace illegal characters.
    HBufC8* tmpfriendlyname = 
        UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
        ( ( CUpnpAVDevice* )device )->FriendlyName() );
            
    CleanupStack::PushL( tmpfriendlyname );
    TPtrC8 friendlyname = *tmpfriendlyname;
    
    HBufC* tmpbuf = UpnpString::ToUnicodeL( friendlyname.Left( KLength ) );
      
    //Get max number of characters fits in the screen
    TInt num = normalFont->TextCount( *tmpbuf, aRect.Width() );
    //if the name has the length more than the screen can have
    if( tmpbuf->Length() > num )
        {
        HBufC* tmpbuf2 = HBufC::NewL( num );
        
        tmpbuf2->Des().Copy( tmpbuf->Des().Left( num - KDotLength ) );
        tmpbuf2->Des().Append( KDot );
        gc.DrawText( *tmpbuf2, aRect, baseline, CGraphicsContext::ELeft );    
        delete tmpbuf2;
        }
    else
        {
        gc.DrawText( *tmpbuf, aRect, baseline, CGraphicsContext::ECenter );    
        }
    
    delete tmpbuf;
    CleanupStack::PopAndDestroy( tmpfriendlyname );
    
    __LOG( "CImage::DrawL End" );
    }

// End of file
