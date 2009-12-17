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
* Description:  Observer class definition
*
*/


#ifndef __MSENGINEOBSERVER_H__
#define __MSENGINEOBSERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include "cmcommon.h"

/**
 *  File sharing observer definition
 *
 *  @lib msengine.lib
 *
 *  @since S60 5.1
 */
class MMSEngineObserver
    {
    public:
        /**
         * Function informs when process is ready
         *
         * @since S60 5.1 
        * @param aService, completed service
        * @param aError, error code        
        * @return None
         */
        virtual void ReadyL( TCmService aService, TInt aError ) = 0;
    };

#endif //__MSENGINEOBSERVER_H__

// End of File
