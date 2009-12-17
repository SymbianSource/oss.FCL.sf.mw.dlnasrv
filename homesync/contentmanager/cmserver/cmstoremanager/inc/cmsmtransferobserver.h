/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef M_CMSMTRANSFEROBSERVER_H
#define M_CMSMTRANSFEROBSERVER_H

#include "cmsmcommon.h"

/**
 *  Callback interface class used to notify client
 *  about status changes in Upnp actions
 *  @lib cmstoremanager.lib
 *  @since S60 v3.1
 */
class MCmSmTransferObserver
    {

public:

    /**
     * Upnp action notifys
     * @since S60 3.1
     * @param aStatus, transfer status
     * @return None
     */
    virtual void TransferStatus( TCmSmTransferStatus aStatus ) = 0;

    };

#endif // M_CMSMTRANSFEROBSERVER_H
