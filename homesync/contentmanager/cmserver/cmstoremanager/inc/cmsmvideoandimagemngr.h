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
* Description:  store file manager for images and videos
*
*/



#ifndef __CMSMVIDEOANDIMAGEMNGR_H
#define __CMSMVIDEOANDIMAGEMNGR_H

#include <e32base.h>
#include <badesca.h>
#include <mdequery.h>
#include "cmcommontypes.h"
#include "cmcommon.h"
#include "cmsmcommon.h"

// Forward declarations
class CMdESession;
class CMdENamespaceDef;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdEPropertyDef;
class CCmStoreListItem;
class CCmSmFileMngr;
class CMdEObject;
class MCmSmVideoAndImageMngrObserver;

// CLASS DECLARATION 
    
 /**
  *  Video and Image manager class definition
  *
  *  @lib cmstoremanager.lib
  *
  *  @since S60 5.1
  */
NONSHARABLE_CLASS( CCmSmVideoAndImageMngr ): public CActive,
                               public MMdEQueryObserver
    {      
         
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1     
     * @param aObserver, observer class
     * @param aFileMngr, file manager
     * @param aSession, mde session
     * @param aItems, store list items
     * @return  pointer to CCmSmVideoAndImageMngr class
     */
    static CCmSmVideoAndImageMngr* NewL( 
        MCmSmVideoAndImageMngrObserver& aObserver,
        CCmSmFileMngr& aFileMngr, CMdESession& aSession, 
        RPointerArray<CCmStoreListItem>& aItems );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1     
     * @param aObserver, observer class
     * @param aFileMngr, file manager
     * @param aSession, mde session
     * @param aItems, store list items     
     * @return  pointer to CCmSmVideoAndImageMngr class
     */
    static CCmSmVideoAndImageMngr* NewLC(  
        MCmSmVideoAndImageMngrObserver& aObserver,
        CCmSmFileMngr& aFileMngr, CMdESession& aSession,
        RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Destructor.
     */
    virtual ~CCmSmVideoAndImageMngr();

public: 

    /**
     * Starts video/image list processing
     *
     * @since S60 5.1
     * @param aType, media type to be processed
     * @return None
     */
    void ProcessMedia( TCmMediaType aType );  
        
    /**
     * Canceling file processing
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CancelOperation();

    /**
     * Canceling clf refress
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void CancelListRefresh();                    
    
    /**
     * Filters files from the clf lists
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void FilterItems();

    /**
     * Filters defined media files from the clf lists
     *
     * @since S60 5.1
     * @param aDeviceIds, devices
     * @param aType, media type
     * @return None
     */        
    void FilterNewItemsL( RArray<TInt>aDeviceIds, 
    TCmMediaType aType );

protected:

// From base class MMdEQueryObserver

    /**
     * From MMdEQueryObserver
     * See base class definition
     */
    void HandleQueryNewResults(CMdEQuery& aQuery,
                                       TInt aFirstNewItemIndex,
                                       TInt aNewItemCount);
          
    /**
     * From MMdEQueryObserver
     * See base class definition
     */
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);  

// From base class CActive

    /**
     * From CActive
     * See base class definition
     */
    void RunL();

    /**
     * From CActive
     * See base class definition
     */
    void DoCancel();
    
    /**
     * From CActive
     * See base class definition
     */  
    TInt RunError( TInt aError );        
    
private:

   /**
     * Creates query for defined media type
     *
     * @since S60 5.1
     * @param aType, media type
     * @return None
     */ 
    void SetQueryAndStartFindL( TCmMediaType aType );
    
   /**
     * Sets property filters
     *
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void SetPropertyFiltersL( );
    
   /**
     * Sets logic condition
     *
     * @since S60 5.1
     * @param aType, media type ( phone or other )
     * @return None
     */
    void SetLogicConditionL( TCmMediaType aType );

   /**
     * Resets mde query
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void ResetQuery();
    
   /**
     * Completes request
     *
     * @since S60 5.1
     * @param aStatus, defined id for the completing 
     * @return None
     */     
    void CompleteRequest( TCmSmFileProcessingStatus aStatus );

   /**
     * Collects item info from mde objects
     *
     * @since S60 5.1
     * @param aType, media type
     * @return TCmListItemStatus, list status value
     */     
    TCmListItemStatus CollectItemDataL( TCmMediaType aType );            
            
    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, video and image manager observer
     * @param aFileMngr, file manager
     * @param aSession, Mde session
     * @param aItems, Store items
     * @return None
     */
    CCmSmVideoAndImageMngr(  
        MCmSmVideoAndImageMngrObserver& aObserver,
        CCmSmFileMngr& aFileMngr, CMdESession& aSession,
        RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

private: 

    /**
     * Observer
     */
    MCmSmVideoAndImageMngrObserver& iObserver;
    
    /**
     * Reference to main file manager
     */
    CCmSmFileMngr& iFileMngr;
    
    /**
     * MdE session
     */
    CMdESession& iSession;                                
        
    /**
     * Reference to store list items
     */
    RPointerArray<CCmStoreListItem>& iItems;
    
    /**
     * mde namespace
     */
    CMdENamespaceDef* iNamespace;                           // owned
    
    /**
     * Object definition ( Image/Video )
     */
    CMdEObjectDef* iObjectDef;                              // owned
        
    /**
     * Object definition ( Image/Video )
     */
    CMdEObjectDef* iMediaTypeDef;                           // owned
    
    /**
     * Object query
     */
    CMdEObjectQuery* iQuery;                                // owned
    
    /**
     * Queried property
     */
    CMdEPropertyDef* iTitleDef;                             // owned
    
    /**
     * Queried property
     */
    CMdEPropertyDef* iSizeDef;                              // owned
    
    /**
     * Queried property
     */
    CMdEPropertyDef* iDateDef;                              // owned
    
    /**
     * Metadata object array
     */
    RPointerArray<CMdEObject> iObjects;                     // Owned
    
    /**
     * Defines which media query is active
     */
    TCmMediaType iQueriedMedia;
    
    };

#endif //  __CMSMVIDEOANDIMAGEMNGR_H

