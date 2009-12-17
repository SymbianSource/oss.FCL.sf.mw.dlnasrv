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
* Description:      File tranfer session
*
*/






#ifndef M_UPNPFILETRANSERSESSIONOBSERVER_H
#define M_UPNPFILETRANSERSESSIONOBSERVER_H

// EXTERNAL INCLUDES
#include <e32std.h>

// INTERNAL INCLUDES
#include "upnpavsessionobserverbase.h"

// CLASS DECLARATION
/**
 *  AV Controller file transfer session observer interface.
 *
 *  @since S60 v3.2
 */
 class MUPnPFileTransferSessionObserver : public MUPnPAVSessionObserverBase
    {   
    
public:

    /**
     * Notifies that the transfer has been started
     *
     * @param aKey identifies the transfer
     * @param aStatus status (error) code
     */        
    virtual void TransferStarted( TInt aKey, TInt aStatus ) = 0;
    
    /**
     * Notifies that the transfer has been completed
     *
     * @param aKey identifies the transfer
     * @param aStatus status (error) code
     * @param aFilePath 
     */        
    virtual void TransferCompleted( TInt aKey, TInt aStatus,
        const TDesC& aFilePath ) = 0;
    
    /**
     * Notifies transfer progress
     * NOTE: Transfer progress is not guaranteed
     *
     * @param aKey identifies the transfer
     * @param aBytes amount of bytes downloaded
     * @param aTotalBytes total amount of bytes
     */        
    virtual void TransferProgress( TInt aKey, TInt aBytes,
        TInt aTotalBytes ) = 0;

    /**
     * Notifies that the Media Server we have a session with has
     * disappeared. Session is now unusable and must be closed. 
     *
     * @since Series 60 3.1
     * @param aReason reason code
     * @return None
     */        
    virtual void MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason aReason ) = 0;
    
    };

#endif // M_UPNPFILETRANSERSESSIONOBSERVER_H

// End of file

