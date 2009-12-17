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
* Description:  store file manager
*
*/



#ifndef __CMSMCLFMNGR_H
#define __CMSMCLFMNGR_H

#include <e32base.h>
#include <MCLFOperationObserver.h>
#include <MCLFItemListModel.h>
#include <badesca.h>
#include "cmcommontypes.h"
#include "cmcommon.h"
#include "cmsmcommon.h"

// Forward declarations
class MCLFContentListingEngine;
class MCLFItemListModel;
class MCLFSortingStyle;
class MCmSmMusicMngrObserver;
class CCmSmFileMngr;

// CLASS DECLARATION      
  
 /**
  * CCmSmClfMngr class definition
  * This class processes music files metadata.
  *
  * @lib cmstoremanager.lib
  *
  * @since S60 5.1
  */
NONSHARABLE_CLASS( CCmSmClfMngr ) : public CActive, 
                     public MCLFOperationObserver
    {         
public:
        
    /**
     * Two-phased constructor.
     * Creates new CCmSmClfMngr class
     *
     * @since S60 5.1
     * @param aObserver, observer pointer
     * @param aFileMngr, file manager reference
     * @param aItems, store list items
     * @return  pointer to CCmSmClfMngr class
     */
    static CCmSmClfMngr* NewL( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems );
    
    /**     
     * Two-phased constructor.
     * Creates new CCmSmClfMngr class     
     *
     * @since S60 5.1     
     * @param aObserver, observer pointer
     * @param aFileMngr, file manager reference
     * @param aItems, store list items
     * @return  pointer to CCmSmClfMngr class
     */
    static CCmSmClfMngr* NewLC( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Destructor.
     */
    virtual ~CCmSmClfMngr();

    /**
     * Cancels list processing
     *
     * @since S60 5.1
     * @param None
     * @return None
     */              
    void CancelOperation();
    
    /**
     * Cancel clf refresh
     *
     * @since S60 5.1
     * @param None
     * @return None
     */     
    void CancelClfRefresh();
    
    /**
     * Setting up clf lists
     *
     * @since S60 5.1
     * @param aMediaType, media type
     * @return None
     */                        
    void SetupCLFListModelsL( TCmMediaType aMediaType );

    /**
     * CLF refresh
     *
     * @since S60 5.1
     * @param aMediaType, media type
     * @return None
     */        
    void DoRefreshL( TCmMediaType aMediaType );
    
    /**
     * Filters files from the clf lists
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void FilterItemsL();

    /**
     * Filters defined media files from the clf lists
     *
     * @since S60 5.1
     * @param aNewItems, list model
     * @param aDeviceIds, devices
     * @param aType, media type
     * @return None
     */        
    void FilterNewItemsL( MCLFItemListModel* aNewItems, 
        RArray<TInt> aDeviceIds, TCmMediaType aType );  

// From base class MCLFOperationObserver

    /**
     * From MCLFOperationObserver
     * See base class definition
     */
    void HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                TInt aError ); 

protected:

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
            
private:

    /**
     * Completes request
     *
     * @since S60 5.1
     * @param aStatus, defined id for the completing 
     * @return None
     */     
    void CompleteRequest( TCmSmFileProcessingStatus aStatus );                   
        
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * since S60 5.1
     * @param aObserver, music manager observer
     * @param aFileMngr, file manager
     * @param aItems, store list items
     */
    CCmSmClfMngr( MCmSmMusicMngrObserver& aObserver, 
    CCmSmFileMngr& aFileMngr, RPointerArray<CCmStoreListItem>& aItems );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

private:

    /**
     * Pointer to observer class
     */
    MCmSmMusicMngrObserver& iObserver;    
    /**
     * Main file manager
     */
    CCmSmFileMngr& iFileMngr;        
    /**
     * Clf engine
     */
    MCLFContentListingEngine* iEngine;          // owned    
    /**
     * Audio list model to be refreshed
     */
    MCLFItemListModel* iNewAudioItems;          // owned    
    /**
     * Image list model to be refreshed
     */
    MCLFItemListModel* iNewImageItems;          // owned    
    /**
     * Video list model to be refreshed
     */
    MCLFItemListModel* iNewVideoItems;          // owned
    /**
     * Sorting style for item ordering.
     */       
    MCLFSortingStyle* iSortingStyle;            // owned
    /**
     * Store file list items
     */
    RPointerArray<CCmStoreListItem>& iItems;
    /**       
     * Filter file index
     */
    TInt iFilterIndex;      
    };

#endif //  __CMSMCLFMNGR_H

