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
* Description:      Observer interface used in CUPnPSingleton
*
*/






#ifndef M_UPNPRENDERERSELECTOROBSERVER_H
#define M_UPNPRENDERERSELECTOROBSERVER_H

/**
 * Observer interface for receiving a notification when the renderer 
 * list has changed
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class MUPnPRendererSelectorObserver
    {

public:

    /**
     * Notification for the sub player list is available
     *
     * @since Series 60 3.1
     * @param aSubPlayers a list of sub players
     * @param aComplete ETrue no more sub players. EFalse more subplayer 
     *                  expected
     * @param aError Error code
     */
     virtual void HandleSubPlayerNames( const MDesCArray* aSubPlayers,
         TBool aComplete, TInt aError ) = 0;
        
    /**
     * Notification for the sub player list has changed
     *
     * @since Series 60 3.1
     */
     virtual void RendererListChanged() = 0;
     
    };


#endif // M_UPNPRENDERERSELECTOROBSERVER_H
