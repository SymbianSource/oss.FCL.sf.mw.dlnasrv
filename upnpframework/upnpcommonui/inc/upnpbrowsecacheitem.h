/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the UPnPSelectionDialog class implementation
*
*/


#ifndef __UPNP_BROWSE_CACHE_ITEM_H__
#define __UPNP_BROWSE_CACHE_ITEM_H__

// FORWARD DECLARATIONS
class CUpnpObject;

class CUPnPBrowseCacheItem : public CBase
    {

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        //IMPORT_C static CUPnPBrowseCacheItem* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUPnPBrowseCacheItem* NewL(
                            const TDesC8& aContainer,
                            TInt aHighLightedItem,
                            TInt aFirstItem);

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUPnPBrowseCacheItem();

        /**
        * Returns the container Id
        * @since Series 60 3.1
        * @param None
        * @return HBufC8& the container Id 
        */
        IMPORT_C const HBufC8& ContainerId() const;

        /**
        * Sets the item array
        * @since Series 60 3.1
        * @param aBrowseArray CUpnpObjectList Pointer
        * @return None
        */
        IMPORT_C void SetItemArrayL(
                        const RPointerArray<CUpnpObject>& aBrowseArray );

        /**
        * Sets the index of the first item
        * @since Series 60 3.1
        * @param aFirstItem (TInt) index of the first item
        * @return None
        */
        IMPORT_C void SetFirstItem( const TInt aFirstItem );

        /**
        * Sets the total count of items in this container
        * @since Series 60 3.2
        * @param aTotalCount (TInt) number of items in this container
        * @return None
        */
        void SetTotalCount( const TInt aTotalCount );

        /**
        * Sets the index of the highlighted item
        * @since Series 60 3.1
        * @param aHighLightedItem (TInt) index of the first item
        * @return None
        */
        IMPORT_C void SetHighLightedItem(const TInt aHighLightedItem);

        /**
        * Returns an object
        * @since Series 60 3.1
        * @param aItemToGet (TInt) index of the item
        * @return CUpnpObject& the object
        */
        IMPORT_C const CUpnpObject* GetItem(const TInt aItemToGet);

        /**
        * Returns the number of items
        * @since Series 60 3.1
        * @param None
        * @return TInt& the number of items
        */
        IMPORT_C TInt GetNumberOfItems() const;

        /**
        * Returns the index of the top most item
        * @since Series 60 3.1
        * @param None
        * @return TInt& the index of the top most item
        */
        IMPORT_C TInt GetUpperMostItem() const;

        /**
        * Returns the total count of items in this container
        * @since Series 60 3.2
        * @param None
        * @return TInt total number of items
        */
        TInt GetTotalCount() const;

        /**
        * Returns the index of the highlighted item
        * @since Series 60 3.1
        * @param None
        * @return TInt& the index the highlighted item
        */
        IMPORT_C TInt GetHighlightedItem() const;

    protected:

        /**
        * C++ default constructor.
        */
        CUPnPBrowseCacheItem();

    private:

        /**
        * 2nd constructor
        */
        void ConstructL( const TDesC8& aContainer,
                         TInt aHighLightedItem,
                         TInt aFirstItem );

    protected: // Data

        // Object ID
        HBufC8*                     iContainerData; //owned

        //array of items
        RPointerArray<CUpnpObject>  iItemArray; // owned

        //first Item in Array
        TInt                        iFirstItem;

        //first Item in Array
        TInt                        iTotalCount;

        //highlighted item
        TInt                        iHighLightedItem;
      
      };

#endif // __UPNP_BROWSE_CACHE_ITEM_H__

// End of file
