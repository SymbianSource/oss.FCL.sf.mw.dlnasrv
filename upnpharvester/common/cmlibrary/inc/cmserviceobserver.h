/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Callback interface to Fill manager
*
*/







#ifndef M_CMSERVICEOBSERVER_H
#define M_CMSERVICEOBSERVER_H

/**
 *  Cm service observer
 *  @lib cmlibrary.lib
 *  @since S60 v3.1
 */

//  INCLUDES
#include    "cmcommon.h"


// CLASS DECLARATION

class MCmServiceObserver
    {
    public:
        virtual void ServiceExecuted( TCmService aService, 
                                      TInt aErrCode ) = 0;
        virtual void ExecuteServiceL( TCmService aService ) = 0;
    };


#endif // M_CMFMTRANSFEROBSERVER_H
