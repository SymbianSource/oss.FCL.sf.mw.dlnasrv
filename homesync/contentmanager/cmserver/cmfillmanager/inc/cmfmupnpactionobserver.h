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
* Description:  Callback interface
*
*/



#ifndef M_CMFMUPNPACTIONOBSERVER_H
#define M_CMFMUPNPACTIONOBSERVER_H

#include "cmfmcommon.h"

// Forward declarations
class CUpnpItem;
class CUpnpElement;

/**
 *  UPnP action observer
 *  Callback interface class used to notify CCmFmMain class
 *  about status changes in Upnp actions
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
class MCmFmUPnPActionObserver
    {

public:

    /**
     * Upnp action notify about Browse results
     *
     * @since S60 5.1
     * @param aStatus, uri check result
     * @param aItem, item to be filled
     * @param aRes, resource of item item
     * @return  None
     */
    virtual void URICheckResult( TCmFmUriCheckResult aStatus, 
        const CUpnpItem* aItem = NULL, const CUpnpElement* aRes = NULL ) = 0;
            

    };

#endif // M_CMFMUPNPACTIONOBSERVER_H
