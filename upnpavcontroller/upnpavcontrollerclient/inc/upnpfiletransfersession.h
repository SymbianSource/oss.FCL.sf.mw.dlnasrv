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
* Description:      File tranfer session
*
*/






#ifndef M_UPNPFILETRANSERSESSION_H
#define M_UPNPFILETRANSERSESSION_H

// EXTERNAL INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MUPnPFileTransferSessionObserver;

// CLASS DECLARATION
/**
 *  AV Controller file transfer session interface.
 *
 *  @since S60 v3.2
 */
 class MUPnPFileTransferSession
    {

public:

    /**
     * Sets the file transfer session observer
     *
     * @param aObserver file transfer session observer
     */
    virtual void SetObserver(
        MUPnPFileTransferSessionObserver& aObserver ) = 0;

    /**
     * Removes file transfer session observer
     */
    virtual void RemoveObserver() = 0;
    
    /**
     * Returns the file transfer session observer
     *
     * @return observer
     */
    virtual MUPnPFileTransferSessionObserver* Observer() const = 0;   
    
public:

    /**
     * Start tracking progress
     * NOTE: Transfer progress is not guaranteed
     *
     * @param aKey identifies the transfer
     */        
    virtual void StartTrackingProgressL( TInt aKey ) = 0;
    
    /**
     * Cancel transfer
     *
     * @param aKey identifies the transfer
     */        
    virtual void CancelTransfer( TInt aKey ) = 0;
    
    /**
     * Cancel all transfers
     *
     * @param aKey identifies the transfer
     */        
    virtual void CancelAllTransfers() = 0;
    
    };

#endif // M_UPNPFILETRANSERSESSION_H

// End of file

