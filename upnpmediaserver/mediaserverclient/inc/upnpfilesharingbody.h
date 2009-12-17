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
* Description:  Implementation class for UPnP File Sharing class
*
*/


#ifndef C_CUPNPFILESHARINGBODY_H
#define C_CUPNPFILESHARINGBODY_H

#include <e32base.h>
#include "upnpmediaserverclient.h"

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpContainer;
class CUpnpItemList;
class CUpnpBrowseCriteria;
class CUpnpContainerList;

/**
 *  Implementation class for UPnP File Sharing class
 *
 *  @lib MediaServerClient.lib
 *  @since S60 v3.1
 */
class CUpnpFileSharingBody : public CActive
    {

public:
    
    enum TFileSharingAction
        {
        ENoAction = 1,
        EShareItem,
        EShareItemResult,
        EShareContainer,
        EShareContainerResult,
        EShareItemList,
        EShareItemListResult,
        EUnshareContainer,
        EUnshareItem,
        EGetSharedItem,
        EGetSharedItemResult,
        EGetSharedItemList,
        EGetSharedItemListResult,
        EGetSharedContainer,
        EGetSharedContainerResult,
        EGetSharedContainerList,
        EGetSharedContainerListResult,
        EUnshareItemList,
        EUnshareRoot,
        EShareReference,
        EShareReferenceList
        };

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpFileSharingBody* NewL();

    /** 
    * Destructor.
    */
    virtual ~CUpnpFileSharingBody();

public:     // Interface methods
    
    void UnshareRootL(TRequestStatus& aStatus );
    
    /**
    * Unshares existing item list from the MediaServer
    *
    * @since S60 3.1
    * @param aItemList reference to item list which are to be shared.
    * @param aExecStatus reference pointing to the index of item list
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void UnshareItemListL(CUpnpItemList& aItemList, TInt& aExecStatus, TRequestStatus& aStatus);
    
    
    /**
    * Shares item list to the MediaServer
    *
    * @since S60 3.1
    * @param aParentId information to which container the item is shared.
    * @param aItemList reference to item list which are to be shared.
    * @param aExecStatus reference pointing to the index of item list
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void ShareItemListL( const TDesC8& aParentId, CUpnpItemList& aItemList,
                          TInt& aExecStatus, TRequestStatus& aStatus );
    
    /**
    * Gets shared items information from MediaServer. Information is 
    * returned to item list reference
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aItemList reference to item list where information is updated.
    * @param aTotalMatches number of matched items
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void GetSharedItemListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpItemList& aItemList,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus );   
                                    
                                    
    /**
    * Gets shared container information from MediaServer. Information is 
    * returned to item list reference
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aContainerList reference to container list where information is updated.
    * @param aTotalMatches number of matched containers
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */    
    void GetSharedContainerListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpContainerList& aContainerList,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus );   
    
    /**
    * Shares a single item asynchronously to MediaServer.
    * Metadata is fetched for the item within the interface.
    * When API call returns, item contains updated information.
    *
    * @since S60 3.1
    * @param aContainerId information to which container the item is shared.
    * @param aItem reference to item which is shared.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void ShareItemL( const TDesC8& aParentId, CUpnpItem& aItem,
                     TRequestStatus& aStatus ); 


    /**
    * Shares a container asynchronously to MediaServer.
    * One container can contain items and other containers.
    * Metadata is fetched for the items within the interface.
    * When API call returns, container hierarchy contains updated information.
    *
    * @since S60 3.1
    * @param aContainerId information to which container the item is shared.
    * @param aContainer reference to container which is shared.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void ShareContainerL( const TDesC8& aParentId, CUpnpContainer& aContainer,
                          TRequestStatus& aStatus ); 
                                    
    /**
    * Unshares existing item from the MediaServer
    *
    * @since S60 3.1
    * @param aItemId reference to item which is unshared
    */
    void UnshareItemL( TInt aItemId, TRequestStatus& aStatus );
       
     
    /**
    * Unshares existing container from the MediaServer
    *
    * @since S60 3.1
    * @param aContainerId reference to container which is unshared
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void UnshareContainerL( TInt aContainerId,
                            TRequestStatus& aStatus );
     

    /**
    * Gets shared item information from MediaServer. Information is 
    * returned to item reference
    *
    * @since S60 3.1
    * @param aItemId reference to item id
    * @param aItem reference to item where information is updated.
    */
    void GetSharedItemL( TInt aItemId,
                         CUpnpItem& aItem,
                         TRequestStatus& aStatus );
    
    
    /**
    * Gets shared container information from MediaServer. Information is 
    * returned to container reference. Returns the complete item/container
    * branch below the specified container id. Container id "0" (root) 
    * returns the complete CDS hierarchy.
    *
    * @since S60 3.1
    * @param aContainerId reference to container id
    * @param aContaier reference to container where information is updated.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void GetSharedContainerL( TInt aContainerId,
                              CUpnpContainer& aContainer,
                              TRequestStatus& aStatus );


    /**
    * Creates a single reference to passed item asynchronously in Media Server.    
    *
    * @since S60 5.0 HN
    * @param aContainerId - container id upon which the reference will be located.
    * @param aObjectId - the reference is created on the basis of passed object id
    * @param aItem reference item to be filled
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void ShareReferenceL( TInt aContainerId, 
                            TInt aObjectId,
                            CUpnpItem& aItem,
                            TRequestStatus& aStatus );

    /**
    * Creates reference items, located at specific container, pointing specific objects
    *
    * @since S60 5.0 HN
    * @param aItemList 'reference item' list which are to be shared
    * @param aExecStatus reference pointing to the index of item list
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    void ShareReferenceListL( CUpnpItemList& aItemList,
                                  TInt& aExecStatus,
                                  TRequestStatus& aStatus ); 
                                  
    /**
    * Returns reference to media server client
    *
    * @since S60 5.0 HN
    * @return media server client refernce
    */
    RUpnpMediaServerClient& MediaServerClient( );                                   

public: // From CActive
    
    /**
    * Cancellation of ongoing asynchronous operation
    */
    void DoCancel();

    /**
    * RunL
    */
    void RunL();

    /**
    * RunError
    */
    TInt RunError( TInt aErr );
    
private:
    void LeaveIfBusyL();
    void Reset();
    void GetShareItemResultL();
    void FinishShareItemL();
    void GetShareContainerResultL();
    void FinishShareContainerL();
    void GetShareItemListResultL();
    void FinishShareItemListL();
    void FinishUnshareObject();
    void FinishUnshareRoot();
    void FinishGetSharedItemL();
    void GetGetSharedItemResultL();
    void FinishGetSharedContainerL();
    void GetGetSharedContainerResultL();
    void FinishUnshareItemList();
    void FinishGetSharedItemListL();
    void GetGetSharedItemListResultL();
    void FinishGetSharedContainerListL();
    void GetGetSharedContainerListResultL();
    void HandleError();
private:

    CUpnpFileSharingBody();

    void ConstructL();


private: // data
    
    // Ongoing action
    TFileSharingAction  iPendingAction;
    
    // Handle to MediaServer
    RUpnpMediaServerClient  iMediaServer;
    
    // Client's request status pointer (not own)
    TRequestStatus* iClientStatus;
    
    // Buffer to pass data to server
    HBufC8*         iSendBuffer;            // owned
    TPtr8           iSendBufferPtr;         

    
    // Buffer to receive data from server
    HBufC8*         iReceiveBuffer;         // owned
    TPtr8           iReceiveBufferPtr;          
    TInt            iRespBufSize;           
    TPckg<TInt>     iRespBufSizePkg;   
    
    // Pointers to client data structures 
    CUpnpContainer*     iClientContainer;       // not owned
    CUpnpItem*          iClientItem;            // not owned
    CUpnpItemList*      iClientItemList;        // not owned
    CUpnpContainerList* iClientContainerList;   // not owned
    TPckg<TInt>*        iClientExecStatusPkg;   // owned
    
    };


#endif // C_CUPNPFILESHARINGBODY_H
