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
* Description:      announces notifications from helper class
*
*/






#ifndef M_UPNPTRACKOBSERVER_H
#define M_UPNPTRACKOBSERVER_H

/**
 * An interface for observing changes in the track state. 
 * The class announces notifications through this interface
 * to the plugin classes that use it.
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class MUPnPTrackObserver
    {

public:

    /**
     * URI resolving is complete
     *
     * @since Series 60 3.1
     * @param aError error code
     */
    virtual void ResolveURIComplete( TInt aError ) = 0;

    };


#endif // M_UPNPTRACKOBSERVER_H
