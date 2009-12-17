/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Proxy for downloading upnp files and playing locally
*
*/






// INCLUDES
#include <upnpitem.h>
#include "upnpitemutility.h" // for ResourceFromItemL
#include "upnpavcontroller.h"
#include "upnpmusicdownloadproxy.h"
#include "upnpsingleton.h"
#include "upnptrack.h"

_LIT( KComponentLogfile, "musicplugins.txt");
#include "upnplog.h"

#include "upnppluginserrortranslation.h"

// CONSTANTS
// ** a message to music player to complete an immediate exit
const TInt KMusicPluginMessageExit = 3001;
const TUid KLocalPlayerDefinitionUid = { 0x101FFC06 };

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPMusicDownloadProxy* CUPnPMusicDownloadProxy::NewL( 
    MMPXPlaybackPluginObserver& aObs )
    {
    CUPnPMusicDownloadProxy* p = new ( ELeave )
        CUPnPMusicDownloadProxy( aObs );
    __LOG1( "DownloadProxy: NewL this=%d", p );
    CleanupStack::PushL( p );
    p->ConstructL();
    CleanupStack::Pop();
    return p;
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::CUPnPMusicDownloadProxy
// Default constructor.
// --------------------------------------------------------------------------
// 
CUPnPMusicDownloadProxy::CUPnPMusicDownloadProxy(
    MMPXPlaybackPluginObserver& aObs )
    : iProxyState( EStateUninitialised )
    {
    iObs = &aObs;
    iLocalPlayer = NULL;
    iSingleton = NULL;
    iTrack = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::ConstructL
// 1st phase constructor.
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::ConstructL()
    {
    __LOG( "DownloadProxy: ConstructL" );

    // Construct the underlying local plugin
    iLocalPlayer = CMPXPlaybackPlugin::NewL(
        KLocalPlayerDefinitionUid );

    // Create handle to singleton
    iSingleton = CUPnPSingleton::GetInstanceL();
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::~CUPnPMusicDownloadProxy
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPMusicDownloadProxy::~CUPnPMusicDownloadProxy()
    {
    __LOG1( "DownloadProxy: destructor [%d]",
       this );
    Cleanup();

    // Free memory of owned members
    delete iLocalPlayer;
    CUPnPSingleton::LoseInstance( iSingleton );
    }
    

// --------------------------------------------------------------------------
// Methods from CMPXPlaybackPlugin
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::InitialiseL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::InitialiseL( const TDesC& aSong )
    {
    __LOG2( "DownloadProxy::InitialiseL(%S) [%d]",
        &aSong, this );

    __ASSERTD( iProxyState == EStateUninitialised,__FILE__, __LINE__ );

    iProxyState = EStatePreparing;

    // Create local track object to resolving URI
    delete iTrack;
    iTrack = 0;
    iTrack = CUPnPTrack::NewL( iSingleton->AVC() );
    iTrack->ResolveURIL( aSong, *this, CUPnPTrack::EDirectionLocal );
    iProxyState = EStateDownloading;
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::InitialiseL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::InitialiseL( RFile& /*aSong*/ )
    {
    // Not used. Only remote file is supported
    __PANICD( __FILE__, __LINE__ );
    User::Leave( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::HandleMetaData
// from MUPnPTrackMetaDataObserver
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::ResolveURIComplete( TInt aError )
    {
    __LOG1( "DownloadProxy::ResolveURIComplete [%d]", this );
    __ASSERTD( iProxyState == EStateDownloading,__FILE__, __LINE__ );

    if ( aError == KErrNone )
        {                        
        TRAPD( initerror, iLocalPlayer->InitialiseL( 
            iTrack->FilePath() ) );

        if ( initerror != KErrNone )
            {
            __LOG1( "DownloadProxy:local player init leaves: %d",
                initerror );
            Cleanup();
            iProxyState = EStateUninitialised;
            //translate error
            initerror = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                initerror );
            iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPInitialised, 0, initerror );
            }
        else
            {
            iProxyState = EStateInitialisingLocally;
            }
        }
    else
        {
        __LOG1("DownloadProxy::ResolveURIComplete: error %d",
            aError );
        Cleanup();
        iProxyState = EStateUninitialised;
        aError = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                aError );
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPInitialised,
            0, aError );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::CommandL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::CommandL( TMPXPlaybackCommand aCmd, 
    TInt aData )
    {
    __LOG2( "DownloadProxy::CommandL(%d) [%d]", aCmd, this );
    
    if( iProxyState == EStateActive )
        {
        // ACTIVE state: all commands are forwarded to local plugin
        __LOG( "DownloadProxy: command in active state" );
        iLocalPlayer->CommandL( aCmd, aData );
        }
    else if ( iProxyState == EStatePreparing && aCmd == EPbCmdClose )
        {
        __LOG( "DownloadProxy: Close during prepare" );
        Cleanup();
        iProxyState = EStateUninitialised;
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPClosed,
            0, KErrNone );
        }
    else if( iProxyState == EStateDownloading && aCmd == EPbCmdClose )
        {
        __LOG( "DownloadProxy: Close during download" );
        Cleanup(); // this will cancel ongoing copy
        iProxyState = EStateUninitialised;
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPClosed,
            0, KErrNone );
        }
    else if( iProxyState == EStateInitialisingLocally && 
        aCmd == EPbCmdClose )
        {
        __LOG( "DownloadProxy: Close when initialising locally" );
        iLocalPlayer->CommandL( aCmd, aData );
        }
    else if( iProxyState == EStateUninitialised && aCmd == EPbCmdClose )
        {
        __LOG( "DownloadProxy: Close when uninitialized" );
        iObs->HandlePluginEvent( MMPXPlaybackPluginObserver::EPClosed,
            0, KErrNone );
        }
    else if( ( iProxyState == EStatePreparing ||
        iProxyState == EStateDownloading ||
        iProxyState == EStateInitialisingLocally ||
        iProxyState == EStateUninitialised ) && aCmd == EPbCmdStop )
        {
        __LOG( "DownloadProxy: Stop during any state" );
        iLocalPlayer->CommandL( aCmd, aData );
        }
    else // Not initialized
        {
        __LOG1( "DownloadProxy: Command not supported in this state: %d",
            iProxyState );
        User::Leave( KErrNotReady );    
        }

    // check for messages from upnp framework (aData param)
    if ( aCmd == EPbCmdClose &&
        aData == KMusicPluginMessageExit )
        {
        __LOG( "CommandL(Close, exit) -> cleanup and terminate" );

        // cancel things that are ongoing
        Cleanup();
        iLocalPlayer->CancelRequest();
        iProxyState = EStateUninitialised;

        // Create event to user
        
        iObs->HandlePluginEvent(
            MMPXPlaybackPluginObserver::EPPlayerUnavailable,
            0, KErrDisconnected);
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SetL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::SetL( TMPXPlaybackProperty aProperty,
    TInt aValue )
    {
    __LOG1( "DownloadProxy::SetL [%d]", this );
    __LOG1( "Property=%d", (TInt)aProperty );
    iLocalPlayer->SetL( aProperty, aValue );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::ValueL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::ValueL( TMPXPlaybackProperty aProperty ) const
    {
    __LOG1( "DownloadProxy::ValueL [%d]", this );
    iLocalPlayer->ValueL( aProperty );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SubPlayerNamesL
// --------------------------------------------------------------------------
//  
void CUPnPMusicDownloadProxy::SubPlayerNamesL()
    {
    iLocalPlayer->SubPlayerNamesL();
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SelectSubPlayerL
// --------------------------------------------------------------------------
//   
void CUPnPMusicDownloadProxy::SelectSubPlayerL( TInt aIndex )
    {
    iLocalPlayer->SelectSubPlayerL( aIndex );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SubPlayerName
// Returns current sub player name
// --------------------------------------------------------------------------
// 
const TDesC& CUPnPMusicDownloadProxy::SubPlayerName()
    {
    return iLocalPlayer->SubPlayerName();
    }
    
// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SubPlayerIndex
// --------------------------------------------------------------------------
//  
TInt CUPnPMusicDownloadProxy::SubPlayerIndex() const
    {
    return iLocalPlayer->SubPlayerIndex();
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::MediaL
// --------------------------------------------------------------------------
//
void CUPnPMusicDownloadProxy::MediaL( const TArray<TMPXAttribute>& aAttrs )
    {
    // Get metadata from media server
    if( iTrack )
        {
        __LOG1( "DownloadProxy: Get metadata [%d]", this );
        iTrack->GetMetaDataL( aAttrs, *iObs );
        }
    else
        {
        __LOG( "DownloadProxy: Get metadata - no track!" );
        User::Leave( KErrNotReady );
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::CancelRequest
// --------------------------------------------------------------------------
//
void CUPnPMusicDownloadProxy::CancelRequest()
    {
    __LOG( "CUPnPMusicDownloadProxy::CancelRequest" );
    if( iProxyState == EStateDownloading )
        {
        Cleanup(); // this will cancel ongoing copy
        iProxyState = EStateUninitialised;
        // Send "downloading cancelled" event to the user 
        iObs->HandlePluginEvent( 
            MMPXPlaybackPluginObserver::EPDownloadingCanceled,
            0, KErrGeneral );
        }
    iLocalPlayer->CancelRequest();
    }


// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::SetObserver
// --------------------------------------------------------------------------
//
void CUPnPMusicDownloadProxy::SetObserver(MMPXPlaybackPluginObserver& aObs)
    {
    CMPXPlaybackPlugin::SetObserver( aObs );
    
    iLocalPlayer->SetObserver( *this ); 
    }

// --------------------------------------------------------------------------
// Methods from MMPXPlaybackPluginObserver
// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::HandlePluginEvent
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::HandlePluginEvent( TEvent aEvent,
    TInt aData, TInt aError )
    {
    __LOG1( "DownloadProxy::HandlePluginEvent [%d]", this );

    switch( aEvent )
        {
        case EPInitialised:
            {
            __ASSERTD( iProxyState == EStateInitialisingLocally,__FILE__, 
                __LINE__ );
            __LOG1( "DownloadProxy: initialised event, err=%d",
                aError );

            // Proxied local plugin initialized
            if ( aError == KErrNone )
                {
                iProxyState = EStateActive;
             
                iObs->HandlePluginEvent( aEvent, aData, aError );
                }
            else
                {
                iProxyState = EStateClosing;
                TRAP_IGNORE (
                    iLocalPlayer->CommandL( EPbCmdClose, 0 ) );
                }
            
            if ( iProxyState == EStateActive )
                {
                TRAP_IGNORE(
                    iTrack->SendMediaChangedEventL( *iObs );
                    );
                }
            break;
            }
        case EPPlaying:             // fall through
        case EPPlayComplete:        // fall through
        case EPPaused:              // fall through
        case EPStopped:             // fall through
        case EPDownloadStarted:     // fall through
        case EPDownloadingComplete: // fall through
        case EPSubPlayersChanged:   // fall through
        case EPVolumeChanged:       // fall through
        case EPSetComplete:
            {
            __LOG2( "DownloadProxy plugin event=%d err=%d",
                (TInt)aEvent, aError );

            // Just forward event to the user.
            aError = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                    aError );
            iObs->HandlePluginEvent( aEvent, aData, aError );
            break;
            }
        case EPClosed:
            {
            __LOG( "DownloadProxy: closed event" );

            // a Stop command has changed. It will send back a Stopped event 
            // and then a Closed event. Following Pause command will not be 
            // accepted in Uninitialized state and CommandL() will leave. 
            // This change in MPX was necessary to let engine know the file 
            // was closed. One additional aData parameter is passed in 
            // iObs->HandlePluginEvent(MMPXPlaybackPluginObserver::EPClosed, 
            //    EPbCmdStop, KErrNone); to know the difference between a 
            // real Close and a Stop/Close. When this happens, EPClosed event
            // should be ignored.
            if( aData == EPbCmdStop )
                {
                break;
                }

            Cleanup();

            // If iProxyState is EStateClosing, initialise of 
            // localplaybackplugin is failed -> send initialised event with
            // an error code to the user.
            // Otherwise forward event to the user normally.
            if( iProxyState == EStateClosing )
                {
                iObs->HandlePluginEvent( 
                    MMPXPlaybackPluginObserver::EPInitialised,
                    0, KErrNotSupported );
                }
            else
                {
                aError = TUpnpPluginsErrorTranslation::ErrorTranslate( 
                        aError );
                iObs->HandlePluginEvent( aEvent, aData, aError );
                }
            iProxyState = EStateUninitialised;
            break;
            }
        default:
            {
            __LOG( "DownloadProxy: Default event?" );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::HandleProperty
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::HandleProperty( TMPXPlaybackProperty aProperty,
    TInt aValue, TInt aError )
    {
    __LOG1( "DownloadProxy::HandleProperty [%d]", this );
        
    iObs->HandleProperty( aProperty, aValue, aError );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::HandleSubPlayerNames
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::HandleSubPlayerNames( TUid aPlayer, 
    const MDesCArray* aSubPlayers, TBool aComplete, TInt aError )
    {
    iObs->HandleSubPlayerNames( aPlayer, aSubPlayers, aComplete, aError );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::HandleMedia
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::HandleMedia( const CMPXMedia& aProperties,
    TInt aError )
    {
    __LOG1( "DownloadProxy::HandleMedia [%d]", this );
        
    iObs->HandleMedia( aProperties, aError );
    }

// --------------------------------------------------------------------------
// CUPnPMusicDownloadProxy::CleanUpSession
// --------------------------------------------------------------------------
// 
void CUPnPMusicDownloadProxy::Cleanup()
    {
    __LOG1( "DownloadProxy::Cleanup [%d]", this );
    if( iTrack )
        {
        delete iTrack;
        iTrack = 0;
        }
    __LOG( "DownloadProxy::Cleanup - End");
    }

