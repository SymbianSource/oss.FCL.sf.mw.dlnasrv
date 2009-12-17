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
* Description:  Callback interface to Fill manager
*
*/



#ifndef M_CMFMTRANSFEROBSERVER_H
#define M_CMFMTRANSFEROBSERVER_H

#include "cmfmcommon.h"

/**
 *  Download observer
 *  Callback interface class used to notify Fill Manager
 *  about status changes in Fill Manager
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MCmFmTransferObserver )
    {

public:

    /**
     * Status notify to Fill Manager 
     *
     * @since S60 5.1
     * @param aStatus
     * @param aSessionId
     * @param aInternalItemId
     * @param aError 
     * @param aFileSize
     * @return None
     */
    virtual void TransferStatus( TCmFmTransferStatus aStatus, TInt aSessionId,
        TInt aInternalItemId, TInt aError, TInt64 aFSize ) = 0;

    };


#endif // M_CMFMTRANSFEROBSERVER_H
