/*
* Copyright (c) 2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface that executes rendering operations
*
*/

#ifndef C_UPNPRENDERINGOPERATIONEXECUTOR_H
#define C_UPNPRENDERINGOPERATIONEXECUTOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TUpnpRenderingOperation;

/**
 * An interface that is able to receive rendering operation items
 * and execute them. The interface is being used by the rendering
 * operation queue class.
 */
NONSHARABLE_CLASS( MUpnpRenderingOperationExecutor )
    {
    public:
        
        /**
         * Result of executing or processing an item
         */
        enum TResult
            {
            EQueue = 0, // Wait until other operations finished first
            EContinue = 1, // operation started and it will continue
            ECompleted = 2 // operation finished
            };
        
        /**
         * Executes a new operation
         * @param aOperation new operation to execute
         * @param aCurrent operation currently ongoing,
                or ENone if we are idle
         * @return EQueue if can't execute in parallel,
         *   ECompleted if finished, or
         *   EContinue if this operation will continue running
         */
        virtual TResult ExecuteOperationL(
            const TUpnpRenderingOperation& aOperation,
            const TUpnpRenderingOperation& aCurrent ) = 0;

        /**
         * Processes an operation with some abstract input
         */
        virtual TResult ProcessOperationL(
            const TUpnpRenderingOperation& aOperation, TInt aEvent,
            TInt aError, const TAny* aParam1, const TAny* aParam2 ) = 0;

        /**
         * Executor should handle an error that occurred in ExecuteOperationL
         * @param aOperation currently ongoing operation, the one that failed
         * @param aError the error code
         */
        virtual void HandleAsyncError(
            const TUpnpRenderingOperation& aOperation, TInt aLeave ) = 0;

    };

#endif // C_UPNPRENDERINGOPERATIONEXECUTOR_H

