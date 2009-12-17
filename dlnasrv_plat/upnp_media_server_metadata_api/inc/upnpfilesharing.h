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
* Description:  File sharing interface class
*
*/


#ifndef C_CUPNPFILESHARING_H
#define C_CUPNPFILESHARING_H

#include <e32base.h>
#include "upnpobject.h"

// FORWARD DECLARATIONS
class CUpnpFileSharingBody;
class CUpnpItem;
class CUpnpContainer;
class CUpnpBrowseCriteria;
class CUpnpItemList;
class CUpnpContainerList;
class RUpnpMediaServerClient;
/**
 *  Interface class for UPnP File Sharing
 *
 *  @lib MediaServerClient.lib
 *  @since S60 v3.1
 */
class CUpnpFileSharing : public CBase
    {

public:
    
    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpFileSharing* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CUpnpFileSharing();

public:     // Interface methods
    /**
    * Database Clearing function
    * Stops mediaserver, delete database file and starts mediaserver
    *
    * @since S60 3.1
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void DropAllContentL(TRequestStatus& aStatus);
    
    /**
    * Shares a single item asynchronously to Media Server.
    * Symbian data base doesn't support foreign key constraints. In order to avoid  
    * adding item with nonexistent parent container, parent id existence is checked 
    * by Media Server. 
    * The item is created with parent id passed as an argument to this operation 
    * (object field iParentId value is ignored). 
    * After creation input object is updated with:
    * - iId - generated object identifier
    * - "res" element value - in offline sharing there is no possible to set ip:port 
    *    information in URL. "Res" value example: http:///00000001/filename. 
    *    For consistency the same approach will be used in online mode.
    *
    * @since S60 3.1
    * @param aContainerId information to which container the item is shared.
    * @param aItem reference to item which is shared.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void ShareItemL( const TDesC8& aContainerId, 
                              CUpnpItem& aItem,
                              TRequestStatus& aStatus ); 

    /**
    * Shares a single container asynchronously to Media Server.
    * Nested container and items are ignored.  The container is created with parent id 
    * passed as an argument to this operation (object field iParentId value is ignored). 
    * Parent id existence is checked by Media Server. 
    * After creation input object is updated with the current:
    * - iId
    *
    * @since S60 3.1
    * @param aContainerId information to which container the item is shared.
    * @param aContainer reference to container which is shared.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void ShareContainerL( const TDesC8& aParentId, 
                                   CUpnpContainer& aContainer,
                                   TRequestStatus& aStatus ); 
    
    /**
    * Shares a list of items asynchronously to Media Server. 
    * Each CUpnpItem of CUpnpItemList is created with the same parent id passed as 
    * an argument to this operation (object field iParentId value is ignored). 
    * Parent id existence is checked by Media Server - only once for this operation. 
    * Database transaction is committed each 11 objects created into database. 
    * This empirical value is hard coded in current implementation. 
    * If an execution of task fails when creating object (e.g. 776-th item on list) 
    * error code is retuned (aExecStatus), which points to last successfully created item index [1...total count] 
    * (e.g. aExecutionStatus = 770 last item in database is object number 770, 
    * if no object is cretated index = 0, if all are created index = total count).
    *
    * Note: There is no support for chunked operations (splitting long operation 
    * into sequence of asynchronical calls). Each operation is atomic from 
    * Media Server Engine perspective. Long lasting operations may block Media Server Engine. 
    * In such case switching Media Server to offline mode is recommended.
    *
    * For each created item respective input object is updated with:
    * - iId
    * - "res" element value
    *
    * @since S60 3.1
    * @param aContainerId information to which container the item is shared.
    * @param aItemList reference to item list which are to be shared.
    * @param aExecStatus reference pointing to the index of item list
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void ShareItemListL( const TDesC8& aContainerId, 
                                  CUpnpItemList& aItemList,
                                  TInt& aExecStatus,
                                  TRequestStatus& aStatus ); 
                                    
    /**
    * Unshares existing item from the Media Server.
    * Item with specified id is removed from database regardless "restricted" property value.
    * Note: This method removes uploaded file associated with item being destroyed. 
    *       Shared files are never removed.
    *       
    *
    * @since S60 3.1
    * @param aItemId item which is unshared
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void UnshareItemL( TInt aItemId,
                                TRequestStatus& aStatus);
       
     
    /**
    * Unshares existing container from the Media Server.
    * Container with its whole branch is removed from database regardless "restricted" 
    * property value for this container and any object in branch.
    * Object removing is a bottom-up process. If removing of particular object fails 
    * whole operation is broken and error is returned. 
    *
    * Note: This method removes uploaded file associated with item being destroyed. 
    *       Shared files are never removed.
    *
    * Note: The iId value is not verified whether it points to a container or item. 
    * Therefore, clients must not use UnshareContainer method with ids pointing to an 
    * item or other way round. 
    *    
    * @since S60 3.1
    * @param aContainerId container which is unshared
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void UnshareContainerL( TInt aItemId,
                                     TRequestStatus& aStatus );
     
    /**
    * Unshares existing items from the MediaServer.
    * Each Item from list is removed from database regardless "restricted" property value.
    * Database transaction is committed each 11 objects removed database. 
    * If an execution of task fails when creating object error code is retuned, 
    * which points to last successfully removed item index.
    *
    * Note: This method removes uploaded file associated with item being destroyed. 
    *       Shared files are never removed.
    *
    * @since S60 3.1
    * @param aItemList list of items which are to be unshared
    * @param aExecStatus reference pointing to the index of item list    
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void UnshareItemListL( CUpnpItemList& aItemList,
                                TInt& aExecStatus,
                                TRequestStatus& aStatus);

    /**
    * Gets shared item information from MediaServer. 
    * This operation returns all properties. Browse criteria is not used.
    * Information is returned to item reference.
    *
    * @since S60 3.1
    * @param aItemId item id
    * @param aItem reference to item where information is updated.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void GetSharedItemL( TInt aItemId,
                                  CUpnpItem& aItem,
                                  TRequestStatus& aStatus );
    
    
    /**
    * Gets shared container information from Media Server. 
    * Nested container and items are not returned.
    * This operation returns all properties. Browse criteria is not used.
    * Information is returned to container reference.
    *
    * @since S60 3.1
    * @param aContainerId container id
    * @param aContainer reference to container where information is updated.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void GetSharedContainerL( TInt aItemId,
                                       CUpnpContainer& aContainer,
                                       TRequestStatus& aStatus );

    /**
    * Gets shared items information from Media Server according to supported browse criteria.
    * Note: There are only two possible values of filter:
    * -    "" (empty filter) only required properties will be returned
    * -    "*" (asterisk)  all properties will be returned
    * Information is returned to container reference.    
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aItemList reference to item list where information is updated.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void GetSharedItemListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpItemList& aItemList,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus );
    
    /**
    * Gets shared containers information from Media Server according to supported browse criteria.
    * Note: There are only two possible values of filter:
    * -    "" (empty filter) only required properties will be returned
    * -    "*" (asterisk)  all properties will be returned
    * Information is returned to container reference. 
    * Nested container are not returned.   
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aItemList reference to item list where information is updated.
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void GetSharedContainerListL( TInt aContainerId,
                                    CUpnpBrowseCriteria& aBrowseCriteria,
                                    CUpnpContainerList& aItem,
                                    TInt& aTotalMatches,
                                    TRequestStatus& aStatus );
    
    /**
    * Creates a single reference upon passed item asynchronously in Media Server.    
    *
    * @since S60 5.0 HN
    * @param aContainerId - container id upon which the reference will be located.
    * @param aObjectId - the reference is created on the basis of passed object id
    * @param aItem reference item to be filled
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void ShareReferenceL( TInt aContainerId, 
                            TInt aObjectId,
                            CUpnpItem& aItem,
                            TRequestStatus& aStatus );

    /**
    * Creates some reference items, located at specific container, pointing specific objects
    *
    * @since S60 5.0 HN
    * @param aItemList 'reference item' list which are to be shared
    * @param aExecStatus reference pointing to the index of item list
    * @param aStatus reference to TRequestStatus which is completed when procedure is over
    */
    IMPORT_C void ShareReferenceListL( CUpnpItemList& aItemList,
                                  TInt& aExecStatus,
                                  TRequestStatus& aStatus ); 
    /**
    * Returns reference to media server client
    *
    * @since S60 5.0 HN
    * @return media server client refernce
    */                              
    IMPORT_C RUpnpMediaServerClient& CUpnpFileSharing::MediaServerClient( );

private:

	/**
    * C++ default constructor.
    */
    CUpnpFileSharing();
	
	/**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private: // data

    /**
     * Actual implementation class for the interface
     * Own.
     */
     CUpnpFileSharingBody* iBody;
    };


#endif // C_CUPNPFILESHARING_H
