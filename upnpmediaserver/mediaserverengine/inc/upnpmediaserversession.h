/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media Server session
*
*/



#ifndef C_CUPNPMEDIASERVERSESSION_H
#define C_CUPNPMEDIASERVERSESSION_H



// INCLUDES
#include "upnpmediaserver.h"
#include "upnpmediaserverglobals.h"
#include "upnpmediaserverobserver.h"

#include <e32base.h>


// FORWARD DECLARATIONS
class CUpnpObject;
class CUpnpItemList;
class CUpnpBrowseCriteria;
class CUpnpMediaServerEventHandler;
// CLASS DECLARATION

/**
  An instance of class CUpnpMediaServerSession is created for each client
  */
class CUpnpMediaServerSession : public CSession2
    {
public: // New methods
    /**
    * Create a CUpnpMediaServerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpMediaServerSession* NewL(CUpnpMediaServer* aServer);

    /**
    * Create a CUpnpMediaServerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpMediaServerSession* NewLC(CUpnpMediaServer* aServer);

    /**
    * Destroy the object and release all memory objects
    */
    virtual ~CUpnpMediaServerSession();

    /**
    * Notify client about status change
    */
    void StatusChanged();

public: // From CSession
    /**
    * Called after a service request from client; from class CSession
    * @param aMessage message from client (containing requested operation and any data)
    */
    void ServiceL( const RMessage2& aMessage );

private: // New methods

    /**
    * Perform the first phase of two phase construction
    */
    CUpnpMediaServerSession(CUpnpMediaServer* aServer);

    /**
    * Perform the second phase construction of a CUpnpMediaServerSession object
    */
    void ConstructL() ;

    /**
    * Causes the client thread to panic
    * @param panic code
    */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

private:
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot, CUpnpObject* aObj );
    void ReadItemFromMessageL( const RMessage2& aMessage, TInt aSlot, CUpnpItem* aItem );
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot, CUpnpItemList* aItemList ) ;
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpBrowseCriteria* aBrowseCriteria );

    // functions for various events from client
    void EventMediaServerStatusL( const RMessage2& aMessage );
    void EventMediaServerStartL( const RMessage2& aMessage );
    void EventMediaServerStop( const RMessage2& aMessage );
    void EventMediaServerStopSilent( const RMessage2& aMessage );
    void EventMediaServerSetFriendlyNameL( const RMessage2& aMessage );
    void EventMediaServerRestartL( const RMessage2& aMessage );
    void EventMediaServerShareItemL( const RMessage2& aMessage ) ;
    void EventMediaServerShareContainerL( const RMessage2& aMessage ) ;
    void EventMediaServerShareItemListL( const RMessage2& aMessage );
    void EventMediaServerGetShareItemResultL( const RMessage2& aMessage );
    void EventMediaServerGetShareContainerResultL( const RMessage2& aMessage );
    void EventMediaServerGetShareItemListResultL( const RMessage2& aMessage );
    void EventMediaServerUnshareRootL( const RMessage2& aMessage );
    void EventMediaServerUnshareItemL( const RMessage2& aMessage );
    void EventMediaServerUnshareContainerL( const RMessage2& aMessage );
    void EventMediaServerGetSharedItemL( const RMessage2& aMessage );
    void EventMediaServerGetGetSharedItemResultL( const RMessage2& aMessage );
    void EventMediaServerGetSharedContainerL( const RMessage2& aMessage );
    void EventMediaServerGetGetSharedContainerResultL( const RMessage2& aMessage );
    void ReadIdsFromMessageL( const RMessage2& aMessage, TInt aSlot, RArray<TInt>& aObjIds );
    void EventMediaServerUnshareItemListL( const RMessage2& aMessage );
    void EventMediaServerGetSharedItemListL( const RMessage2& aMessage );
    void EventMediaServerGetGetSharedItemListResultL( const RMessage2& aMessage );
    void EventMediaServerGetSharedContainerListL( const RMessage2& aMessage );
    void EventMediaServerGetGetSharedContainerListResultL( const RMessage2& aMessage );
    void EventMediaServerStartOfflineL( const RMessage2& aMessage );
    void EventMediaServerGetIpAndPortL( const RMessage2& aMessage );
    void EventSetUploadDirectoryL(const RMessage2& aMessage );
    void EventAddIconL( const RMessage2& aMessage );
    void EventUpdateIconL( const RMessage2& aMessage );
    void EventRemoveIconL( const RMessage2& aMessage );
    void EventRemoveIconsL( const RMessage2& aMessage );

    void EventMediaServerShareReferenceL( const RMessage2& aMessage );
    void EventMediaServerShareReferenceListL( const RMessage2& aMessage );
    void EventMediaServerStatusChange( const RMessage2& aMessage );
    void EventMediaServerStatusChangeCancel( const RMessage2& aMessage );

private:

    /** @var iMediaServer pointer to Media Server */
    CUpnpMediaServer*   iMediaServer;
    HBufC8*             iResponseBuffer;
    TInt                iRespLength;
    TPckg<TInt>         iRespLengthPkg;
    TInt                iRespExecStatus;
    TPckg<TInt>         iRespExecStatusPkg;

    CUpnpMediaServerEventHandler* iEventHandler;
    RMessage2           iStatusMessage;
    };

#endif // C_CUPNPMEDIASERVERSESSION_H
