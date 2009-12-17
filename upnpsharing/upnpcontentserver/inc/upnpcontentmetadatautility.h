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
* Description:      Metadata utility class definition
 *
*/






#ifndef __UPNPCONTENTMETADATAUTILITY_H__
#define __UPNPCONTENTMETADATAUTILITY_H__


// INCLUDES
// System
#include <e32base.h>
#include <MCLFOperationObserver.h>
#include <MCLFChangedItemObserver.h>
#include <MCLFItem.h>
#include <badesca.h>

// upnp stack api
#include <upnpitem.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff

// homeconnect internal
#include "upnpcontentserverdefs.h"
#include "upnpcontentserverhandler.h"


// CONSTANTS
const TInt KMaxRefreshCount = 3;

// FORWARD DECLARATIONS
class MCLFContentListingEngine;
class MCLFItemListModel;
class CUpnpCustomGrouper;
class CUpnpPostFilter;
class MUpnpMetadataObserver;
class CUPnPMetaDataUtility;


// CLASS DECLARATION

/**
 *  Class to collect files from phone to share.
 *  This class collects the media files for Upnp content server to share
 *  The files are obtained using Content Listing Framework.
 *
 *  @since S60 3.1
 */
class CUpnpContentMetadataUtility : public CBase,
                                public MCLFOperationObserver
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUpnpContentMetadataUtility* NewL();

    /**
     * Destructor.
     */
    virtual ~CUpnpContentMetadataUtility();

public: // New functions

    /**
     * Updates metadata for the item
     * @since S60 3.1
     * @param aMediaType Media type of oitem to be updated
     * @param aItem pointer to item
     * @param aFileName Reference to item filename
     * @return ETrue if the item was found from this model
     */
    TBool UpdateMetadataL( const TUpnpMediaType& aMediaType,
                           CUpnpItem* aItem,
                           const TDesC& aFileName );

    /**
     * Returns music model
     * @since S60 3.1
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel& MusicFiles() const;

    /**
     * Returns image model
     * @since S60 3.1
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel& ImageFiles() const;


    /**
     * Returns video model
     * @since S60 3.1
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel& VideoFiles() const;


    /**
     * Returns collection model
     * @since S60 3.1
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel& Collections() const;


    /**
     * Collects items from selected collection to the model
     * @since S60 3.1
     * @param aNameOfCollection, collection name
     */
    void CollectionItemsL( const TDesC& aNameOfCollection );


    /**
     * Extract files from descriptor to the array
     * @since S60 3.1
     * @param aFileArray, file name array
     * @param aFiles, file name descriptor
     */
    void GetCollectionFileNamesL( CDesCArray& aFileArray,
                                  const TDesC& aFiles ) const;

    /**
     * Returns true if refresh is still ongoing
     * @since S60 3.1
     * @return Status of the refresh operation
     */
    TBool RefreshOngoing() const;

    /**
     * Clears all postfilters
     * @since S60 3.1
     *
     */
    void ClearPostFiltersL();

    /**
     * Creates UPnP item from CLF Item
     * @since S60 3.1
     * @param aCLFItem reference to original item
     * @param aParentId Parent id of the item
     * @return CUpnpItem new item allocation
     */
    CUpnpItem* CreateItemL( const MCLFItem& aCLFItem,
                            const TDesC8&  aParentId ) const;

    /**
     * Creates UPnP item from CLF Item
     * @since S60 3.1
     * @param aFullFilenme Filename of the item
     * @param aParentId Parent id of the item
     * @return CUpnpItem new item allocation
     */
    CUpnpItem* CreateItemL(
        const TDesC& aFullFilename,
        const TDesC8& aParentId ) const;

    /**
     * Sets the callback for informing about refresh state
     * @since S60 3.1
     * @param aHandler pointer to observer
     */
    void SetCallback( MUpnpMetadataObserver* aObserver );


protected: // From MCLFOperationObserver

    /**
     * Abstract method to get list model operation events. This method is
     * called when an event is received.
     * @since S60 3.1
     * @param aOperationEvent Operation event code of the event
     * @param aError System wide error code if the operation did not
     *        succeed.
     */
    void HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                TInt aError );

protected:  // From MCLFChangedItemObserver

    /**
     * From MCLFChangedItemObserver Method to handle changed item event.
     * @since S60 3.1
     * @param aItemIDArray List of changed item IDs
     */
    void HandleItemChangeL( const TArray<TCLFItemId>& aItemIDArray );

    /**
     * From MCLFChangedItemObserver Method is used to handle errors in
     * changed item event.
     * @since S60 3.1
     * @param aError System wide error code.
     */
    void HandleError( TInt aError );


private:

    /**
     * C++ default constructor.
     */
    CUpnpContentMetadataUtility();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    // Content listing engine (owned)
    MCLFContentListingEngine* iEngine;

    // Content listing model for music (owned)
    MCLFItemListModel* iMusicModel;

    // Content listing model for images (owned)
    MCLFItemListModel* iImageModel;

    // Content listing model for videos (owned)
    MCLFItemListModel* iVideoModel;

    // Content listing model for collections (owned)
    MCLFItemListModel* iCollectionModel;

    // Custom grouper for collection grouping (owned)
    CUpnpCustomGrouper* iCustomGrouper;

    // Status to indicate if refresh operation is ongoing
    TBool iRefreshOngoing;

    // Keeps count of refreshed models
    TInt iRefreshCounter;

    // Post filter for filtering (owned)
    CUpnpPostFilter* iPostFilter;

    // Callback interface
    // not owned
    MUpnpMetadataObserver* iHandler;

    // The index maintained during the multiple CLF operations
    TInt iClfIndex;
    
    // Utility class for CLF
    CUPnPMetaDataUtility* iMetaDataUtility; // owned
    };

#endif      // __UPNPCONTENTMETADATAUTILITY_H__

// End of File
