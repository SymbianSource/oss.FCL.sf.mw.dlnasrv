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
* Description:  Content Directory factory class
*
*/


#ifndef UPNPCONTENTDIRECTORYSERVICE_H
#define UPNPCONTENTDIRECTORYSERVICE_H

//  INCLUDES
#include <e32base.h>
#include <upnpobject.h>
#include "upnpcdsettings.h"
#include <in_sock.h>

// FORWARD DECLARATION
class CUpnpService;
class MUpnpThumbnailCreatorObserver;
class CUpnpContentDirectory;
class CUpnpItem;
class CUpnpItemList;
class CUpnpContainerList;
class CUpnpBrowseCriteria;
class CUpnpContainer;
class CUpnpMetadataStorage;
class CUpnpLocalStorage;
class MUpnpContentDirectoryEventObserver;
class TUpnpCdSettings;
class MUpnpTransfer;
class CUpnpCdsSender;
class CUpnpTransferHandler;

/**
*  Content Directory factory class.
*  
*
*  @lib ?library
*  @since Series60 3.1
*/
class CUpnpContentDirectoryService : public CBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor for creating Content Directory Service (CDS).
    * @since Series60 3.1
    * @param aService Parent service 
    */
    IMPORT_C static CUpnpContentDirectoryService* NewL( TUpnpCdSettings aSettings );

    /**
    * Two-phased constructor for creating Content Directory Service (CDS).
    * @since Series60 3.1
    * @param aService Parent service 
    */
    IMPORT_C static CUpnpContentDirectoryService* NewLC( TUpnpCdSettings aSettings );
        
    /**
    * Two-phased constructor for offline mode.
    * @since Series60 3.1
    */
    IMPORT_C static CUpnpContentDirectoryService* NewL();

    /**
    * Two-phased constructor for offline mode.
    * @since Series60 3.1
    */
    IMPORT_C static CUpnpContentDirectoryService* NewLC();
        
    /**
    * Destructor.
    */
    virtual ~CUpnpContentDirectoryService();

public: // New functions
        
    /**
    * Sets download directory
    * @since Series S60 3.1
    * @param aDownloadDir this directory is going to be set as download directory
    * @return KerrNone is return if OK
    */
    IMPORT_C void SetDownloadDirectoryL(const TDesC& aDownloadDir);
           
    /**
    * Sets thumbnail directory
    * @since Series S60 3.1
    * @param aDownloadDir this directory is going to be set as thumbnail directory
    * @return KerrNone is return if OK
    */
    IMPORT_C void SetThumbnailDirectoryL(const TDesC& aDownloadDir);
    
    /**
    * Gets local inet address 
    *
    * @since Series S60 3.1
    * @return address
    */
    IMPORT_C TInetAddr GetAddress();    
    
    /**
    * Gets pointer to CUpnpContentDirectory 
    *
    * @since Series S60 3.1
    * @return pointer to CUpnpContentDirectory 
    */
    IMPORT_C CUpnpContentDirectory* GetContentDirectory();
    
    /**
    * Handles address change event    
    * used only by media server engine
    */
    IMPORT_C void AddressChangeL();


public: // Metadata API 
    /**    
    * Shares a single object item/container.
    * After creation input object is updated with:
    * - iId - generated object identifier
    * - "res" element value - in offline sharing there is no possible to set ip:port 
    *    information in URL. "Res" value example: http:///00000001/filename. 
    *
    * @since S60 3.1
    * @param aObject object to be shared.
    */            
    IMPORT_C void ShareObjectL( CUpnpObject* aObject );
    
	/**    
    * Creates a single 'reference object' upon given refId
    * 
    * @since S60 5.0 HN
    * @param aItem reference object to be created
    */            
	IMPORT_C void ShareReferenceL( CUpnpItem* aItem );

	/**
    * Creates some reference items, located at specific container, pointing onto specific objects
    *
    * @since S60 5.0 HN
    * @param aItemList 'reference item' list which are to be shared
    * @param aExecutionStatus reference pointing to the index of item list
    */
	IMPORT_C void ShareReferenceListL( CUpnpItemList* aItemList, 
                                  TInt* aExecutionStatus );

    /**
    * Unshares existing item from the Media Server.
    * Item with specified id is removed from database regardless "restricted" property value.           
    *
    * @since S60 3.1
    * @param aItemId item which is unshared
    */
    IMPORT_C void UnshareItemL( TInt aItemId );
        
    /**
    * Unshares existing container from the Media Server.
    * Container with its whole branch is removed from database regardless "restricted" 
    * property value for this container and any object in branch.
    * Object removing is a bottom-up process. If removing of particular object fails 
    * whole operation is broken and error is returned.     
    *
    * Note: The iId value is not verified whether it points to a container or item. 
    * Therefore, clients must not use UnshareContainer method with ids pointing to an 
    * item or other way round. 
    *	
    * @since S60 3.1
    * @param aContainerId container which is unshared
    */
    IMPORT_C void UnshareContainerL( TInt aContainerId );    
    
    /**
    * Gets shared item information from MediaServer. 
    * This operation returns all properties. Browse criteria is not used.
    * Information is returned to item reference.
    *
    * @since S60 3.1
    * @param aItemId item id
    * @return aItem item
    */
    IMPORT_C CUpnpItem* GetSharedItemL( TInt aItemId );    
    
    /**
    * Shares a list.
    * Database transaction is committed each 11 objects created into database. 
    * This empirical value is hard coded in current implementation. 
    * If an execution of task fails when creating object (e.g. 776-th item on list) 
    * error code is retuned (aExecStatus), which points to last successfully created item index [1...total count] 
    * (e.g. aExecutionStatus = 770 last item in database is object number 770, 
    * if no object is cretated index = 0, if all are created index = total count).
    *
    * For each created item respective input object is updated with:
    * - iId
    * - "res" element value
    *
    * @since S60 3.1
    * @param aItemList reference to item list which are to be shared.
    * @param aExecutionStatus reference pointing to the index of item list
    */
    IMPORT_C void ShareItemListL( CUpnpItemList* aItemList, 
                                  TInt* aExecutionStatus );

    /**
    * Unshares existing items from the MediaServer.
    * Each Item from list is removed from database regardless "restricted" property value.
    * Database transaction is committed each 11 objects removed database. 
    * If an execution of task fails when creating object error code is retuned, 
    * which points to last successfully removed item index.
    *
    * @since S60 3.1
    * @param aItemList list of items which are to be unshared
    * @param aExecutionStatus reference pointing to the index of item list    
    */                                  
    IMPORT_C void UnshareItemListL( RArray<TInt>& aItemList, 
                                    TInt* aExecutionStatus);
                                    

    /**
    * Gets items information according to supported browse criteria.
    * Note: There are only two possible values of filter:
    * -	"" (empty filter) only required properties will be returned
    * -	"*" (asterisk)  all properties will be returned
    * Information is returned to container reference.    
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aItemList reference to item list where information is updated.
    */                                    
    IMPORT_C void GetItemListL( TInt aContainerId, 
                                CUpnpBrowseCriteria* aBrowseCriteria, 
                                TInt* aTotalMatches, 
                                CUpnpItemList* aItemList );
                                
                                
    /**
    * Gets containers information according to supported browse criteria.
    * Note: There are only two possible values of filter:
    * -	"" (empty filter) only required properties will be returned
    * -	"*" (asterisk)  all properties will be returned
    * Information is returned to container reference. 
    * Nested containers are not returned.   
    *
    * @since S60 3.1
    * @param aContainerId id of the container of which items are to be returned
    * @param aBrowseCriteria specifies request details, such as requestCount, startingIndes, filter
    * @param aContainerList reference to item list where information is updated.
    * @param aTotalMatches total matches
    */                              
    IMPORT_C void GetContainerListL( TInt aContainerId, 
                                     CUpnpBrowseCriteria* aBrowseCriteria, 
                                     TInt* aTotalMatches, 
                                     CUpnpContainerList* aContainerList );
                                     
    /**
    * Gets container information 
    * Nested container and items are not returned.
    * This operation returns all properties. Browse criteria is not used.
    *
    * @since S60 3.1
    * @param aContainerId container id
    * @return container object
    */                                     
    IMPORT_C CUpnpContainer* GetSingleContainerL( TInt aContainerId );
    
    
private:

    /**
    * C++ default constructor.
    */
    CUpnpContentDirectoryService();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( TUpnpCdSettings aSettings );
    void ConstructL();

    /**
    * Database corruption recovery handlers
    */
    void HandleDbErrorL( TInt aError ) ;

    /**
    * Recreates data base
    */
    void RecreateDbL( TInt aError ) ;

    /**
    * Checks if data base is created
    * if not tries to recover it
    * if fails leaves
    */
    void CheckDbL();
    
    /**
    * Checks if server is in online mode
    */
    TBool IsOnlineMode();

private:  
    //
    CUpnpContentDirectory*  iCd;                // owned
    //
    CUpnpMetadataStorage*   iMetadataStorage;   // owned
    //
    CUpnpLocalStorage*      iLocalStorage;      // owned
    //
    CUpnpCdsSender*         iSender;            // owned
        
    CUpnpTransferHandler*   iTransferHandler;    
    
    
    };

#endif      // UPNPCONTENTDIRECTORYSERVICE_H   
        
// End of File
