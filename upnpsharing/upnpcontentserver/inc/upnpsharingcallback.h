/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Callback for filesharing related async operations
*
*/






#ifndef __UPNPSHARINGCALLBACK_H__
#define __UPNPSHARINGCALLBACK_H__

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>


/**
 * Callback class for informing about completion of unsharing, sharing and
 * container checking.
 * @since S60 3.1
 */
class MUpnpSharingCallback
    {
public:
    /**
     * Enumeration for stop type
     */
    enum TSharingStopType
        {
        ESharingNoStop,
        ESharingFullStop,
        ESharingPause
        };

    /**
     * Completes the asynchronous sharing / unsharing / container checking
     * Normal completion
     * @since S60 3.1
     */
    virtual void CompleteSharingOperationL(
        const TInt& aErr, const TInt& aType ) = 0;

    /**
     * Completes the asynchronous sharing / unsharing / container checking
     * Error/cancel situation
     * @since S60 3.1
     */
    virtual void CancelSharingOperationL(
        const TInt& aErr ) = 0;

    /**
     * Sets the progress of operation
     * @since S60 3.1
     */
    virtual void SetProgressL(
        const TInt& aProgress ) = 0;

    /**
     * Virtual destructor
     */
    virtual ~MUpnpSharingCallback() {};
    };

#endif //__UPNPSHARINGCALLBACK_H__

// End of File
