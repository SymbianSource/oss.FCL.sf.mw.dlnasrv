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
* Description:      implementation for session towards a media server
*
*/






#ifndef C_UPNPFILETRANSFERSESSIONBASE_H
#define C_UPNPFILETRANSFERSESSIONBASE_H

// INCLUDES
// System
#include <e32base.h>

// upnpframework
#include "upnpavcontroller.h"
#include "httptransferobserver.h"
#include "tupnpfiletransferevent.h"

// FORWARD DECLARATIONS
class CUpnpAVDeviceExtended;
class CUpnpFileTransferItem;
class CUpnpAVControllerServer;

// CLASS DECLARATION
/**
 * Filetransfer session base class
 *
 * @since S60 v3.2
 */
class CUPnPFileTransferSessionBase : public CBase
    {

public:
    
    /**
     * Destructor
     */
    virtual ~CUPnPFileTransferSessionBase();
    
protected:

    /**
     * Private constructor
     *
     * @param aSessionId session id
     */
    CUPnPFileTransferSessionBase( CUpnpAVControllerServer& aServer,
        TInt aSessionId );    
    
    /**
     * 2ns phase constructor
     */
    void ConstructL( const TDesC8& aUuid );
     
public: // New functions

    /**
     * Returns Session Id
     *
     * @return session id
     */
    TInt SessionId() const;
    
    /**
     * Returns device Uuid 
     *
     * @return device Uuid
     */
    const TDesC8& Uuid() const;
     
protected:
    
    /**
     * Checks if the given key (filetransfer item) exists in the transfer
     * que
     *
     * @param aKey filetransfer identifier
     * @return ETrue if the key exists
     */
    TBool CheckIfKeyExists( TInt aKey );
    
    /**
     * Checks if the given key (filetransfer item) exists in the transfer
     * que and returns the index
     *
     * @param aKey filetransfer identifier
     * @param aIndex reference to the index
     * @return ETrue if the key exists
     */
    TBool CheckIfKeyExists( TInt aKey, TInt& aIndex );

    /**
     * Finds and returns filetransfer item
     *
     * @param aItem filetransfer item
     * @param aKey filetransfer identifier
     * @return status (error) code
     */
    TInt FindItemByKey( TUpnpFileTransferEvent& aItem, TInt aKey );
    
    /**
     * Finds and returns filetransfer item
     *
     * @param aItem filetransfer item
     * @param aKey filetransfer identifier
     * @param aIndex reference to the index
     * @return status (error) code
     */
    TInt FindItemByKey( TUpnpFileTransferEvent& aItem, TInt aKey,
        TInt& aIndex );

    /**
     * Reads filetransfer item from the message received from the client
     *
     * @param aMessage message from the client side
     * @param aSlot message slot number
     * @param aItem filetransfer item
     */
    void ReadTransferItemFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpFileTransferItem* aItem );
    
    /**
     * Completes an event
     *
     * @param aEvent event to complete
     */
    void Complete( const TUpnpFileTransferEvent& aEvent );
            
protected:

    /**
     * AV Controller server
     *
     * Not own
     */
    CUpnpAVControllerServer&        iServer;
    
    /**
     * Session id
     */
    TInt                            iSessionId;
    
    /**
     * Target/source device
     *
     * Own
     */
    CUpnpAVDeviceExtended*          iDevice;
    
    /**
     * Array of filetransfer items
     */
    RArray<TUpnpFileTransferEvent>  iTransferItems;
    
    /**
     * Que of events
     */
    RArray<TUpnpFileTransferEvent>  iEventQueu;
    
    /**
     * Async message, used to report a filetransfer event to the client side
     *
     * Own
     */
    RMessage2*                      iEventMsg;
    
    /**
     * Flag to track if progress tracking is enabled or not
     */
    TBool                           iProgressTrackingEnabled;
      
    };

#endif // C_UPNPFILETRANSFERSESSIONBASE_H

// End of file
