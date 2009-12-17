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






#ifndef __CUPNPMUSICDOWNLOADPROXY_H__
#define __CUPNPMUSICDOWNLOADPROXY_H__

// INCLUDES
#include <badesca.h>
#include <mpxplaybackplugin.h>
#include <mpxplaybackpluginobserver.h>
#include "upnptrackobserver.h"

// FORWARD DECLARATIONS
class CUPnPSingleton;
class CUPnPTrack;
class MDesCArray; // for MPX compatibility - actually this is NOT a class


/**
 * UPnP Proxy class for Local Player.
 *
 * Provides additional UPnP download functionality: first
 * downloads the file and then uses the underlying plugin to
 * execute commands (play,stop,pause) on the local copy of the
 * song
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPMusicDownloadProxy : public CMPXPlaybackPlugin,
                                public MMPXPlaybackPluginObserver,
                                public MUPnPTrackObserver
    {

private:

    /**
     * Defines internal player state
     *
     * EStateUninitialised          - Initial and terminal state
     * EStatePreparing              - Preparatory phases before download
     * EStateDownloading            - Downloading the temporary file
     * EStateInitialisingLocally    - Download is complete, starting to
     *                                initialize proxied local plugin
     * EStateActive                 - Init complete, now acting as a proxy for
     *                                the local plugin to play the temp file
     * @since Series 60 3.1
     */  
    enum TProxyState
        {
        EStateUninitialised = 0, // initial state
        EStatePreparing, // preparatory phases
        EStateDownloading, // download phase
        EStateInitialisingLocally, // local init
        EStateActive, // normal
        EStateClosing // used if error during initialise
        }; 
public:

    /**
     * Static 1st phase constructor.
     * 
     * @since Series 60 3.1
     * @param aObs reference to call back interface
     * @return A new player instance
     */   
    static CUPnPMusicDownloadProxy* NewL( MMPXPlaybackPluginObserver& aObs );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */ 
    ~CUPnPMusicDownloadProxy();
    
    
private:

    /**
     * Private default constructor
     *
     * @since Series 60 3.1
     * @param aObs reference to call back interface
     */
    CUPnPMusicDownloadProxy( MMPXPlaybackPluginObserver& aObs );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */   
    void ConstructL();    

public: // From CMPXPlaybackPlugin
    
    /**
     * See CMPXPlaybackPlugin
     */
    void InitialiseL( const TDesC& aSong );
    
    /**
     * See CMPXPlaybackPlugin
     */
    void InitialiseL( RFile& aSong );

    /**
     * See CMPXPlaybackPlugin
     */
    void CommandL( TMPXPlaybackCommand aCmd, TInt aData );

    /**
     * See CMPXPlaybackPlugin
     */
    void SetL( TMPXPlaybackProperty aProperty, TInt aValue ); 

    /**
     * See CMPXPlaybackPlugin
     */
    void ValueL( TMPXPlaybackProperty aProperty ) const;

    /**
     * See CMPXPlaybackPlugin
     */
    void SubPlayerNamesL();
    
    /**
     * See CMPXPlaybackPlugin
     */
    void SelectSubPlayerL(TInt aIndex);
    
    /**
     * See CMPXPlaybackPlugin
     */
    const TDesC& SubPlayerName();
    
    /**
     * See CMPXPlaybackPlugin
     */
    TInt SubPlayerIndex() const;

    /**
     * See CMPXPlaybackPlugin
     */
    void MediaL( const TArray<TMPXAttribute>& aAttrs );
    
    /**
     * See CMPXPlaybackPlugin
     */
    void CancelRequest();
    
    /**
     * See CMPXPlaybackPlugin
     */
    void SetObserver(MMPXPlaybackPluginObserver& aObs);

protected: // From MMPXPlaybackPluginObserver

    /**
     * See MMPXPlaybackPluginObserver
     */
    void HandlePluginEvent( TEvent aEvent, TInt aData, TInt aError );

    /**
     * see MMPXPlaybackPluginObserver's base class MMPXPlaybackCallback
     */
    void HandleProperty( TMPXPlaybackProperty aProperty, TInt aValue,
                          TInt aError );

    /**
     * see MMPXPlaybackPluginObserver's base class MMPXPlaybackCallback
     */
    void HandleSubPlayerNames( TUid aPlayer, const MDesCArray* aSubPlayers,
        TBool aComplete, TInt aError );

    /**
     * see MMPXPlaybackPluginObserver's base class MMPXPlaybackCallback
     */
    void HandleMedia( const CMPXMedia& aProperties, TInt aError );

protected: // From MUPnPTrackObserver

    /**
     * See MUPnPTrackObserver
     */
    void ResolveURIComplete( TInt aError );
    
private:

    /**
     * cleanup when going in uninitialised state
     */
    void Cleanup();

private: // Data
    
    /**
     * Local plugin instance
     * Own.
     */   
    CMPXPlaybackPlugin*                     iLocalPlayer;
    
    /**
     * The UPnP services singleton
     * Own.
     */
     CUPnPSingleton*                        iSingleton;
        
    /**
     * Player state
     */
    mutable TProxyState                     iProxyState;
    
    /**
     * UPnP track
     * Own.
     */            
    CUPnPTrack*                             iTrack;
    };

#endif // __CUPNPMUSICDOWNLOADPROXY_H__




