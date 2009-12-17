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
* Description:      Post filter class defintion
*
*/







#ifndef __CUPNPAPPPOSTFILTER_H__
#define __CUPNPAPPPOSTFILTER_H__

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <MCLFPostFilter.h>
#include <CLFContentListing.hrh>
// this will be included when it back in SDK
// #include <CLFContentListingExtended.hrh>


#include "upnpcontentserverdefs.h"

// FORWARD DECLARATIONS
class MCLFItem;

// CLASS DECLARATION

/**
*  CLF Post Filter.
*  Example of Post Filter that can be used to filter items from the
*  list model. With post filter your can select, which kind of items are
*  excluded from the list model. For example, you can include only music
*  files of some specific artist and remove all other items.
*/
class CUpnpPostFilter
    : public CBase,
      public MCLFPostFilter
    {
    public:  // Constructors and destructor

        /**
        * Creates and returns a new instance of this class.
        * @param aFilterCriteria Filtering criteria (for example
        *        ECLFFielIdAlbum or ECLFFieldIdArtist)
        * @param aFilterParameter Filtering parameter (for example
        *        name of album or artist)
        * @return Pointer to the CCEPostFilter object
        */
        static CUpnpPostFilter* NewL(
                                const TCLFExtendedFieldId aFilterCriteria,
                                const TDesC& aFilterParameter,
                                TBool aRemoveDuplicates );

        /**
        * Creates and returns a new instance of this class. Pointer is
        * left to the CleanupStack.
        * @param aFilterCriteria Filtering criteria (for example
        *        ECLFFielIdAlbum or ECLFFieldIdArtist)
        * @param aFilterParameter Filtering parameter (for example
        *        name of album or artist)
        * @return Pointer to the CCEPostFilter object
        */
        static CUpnpPostFilter* NewLC(
                                const TCLFExtendedFieldId aFilterCriteria,
                                const TDesC& aFilterParameter,
                                TBool aRemoveDuplicates);

        /**
        * Destructor.
        */
        virtual ~CUpnpPostFilter();

    private:    // Constructors
        /**
        * C++ default constructor.
        * @param aFilterCriteria Filtering criteria
        */
        CUpnpPostFilter( const TCLFExtendedFieldId aFilterCriteriaconst,
                         TBool aRemoveDuplicates );


        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL( const TDesC& aFilterParameter );

    protected:  // Functions from base classes

        /**
        * From MCLFPostFilter. Method for filtering the source list.
        * Will get called when post filtering operation is executed by
        * calling refresh for the list model.
        * @since S60 3.1
        * @param aItemList Source item list
        * @param aFilteredItemList Filtered/destination item list
        */
        void FilterItemsL( const TArray<MCLFItem*>& aItemList,
                           RPointerArray<MCLFItem>& aFilteredItemList );

    /**
     * Remove duplicate items
     * @since S60 3.1
     * @param aFilteredItemList, Already added items
     * @param aFieldValue, value to search
     */
    TBool IsAdded( RPointerArray<MCLFItem>& aFilteredItemList,
                   TPtrC& aFieldValue );

    private:    // Data

        // Parameter for filtering (owned)
        HBufC* iFilterParameter;

        // Criteria for filtering

        TCLFExtendedFieldId iFilterCriteria;
        // remove duplicate items if ETrue
        TBool iRemoveDuplicates;

    };

#endif // __CUPNPAPPPOSTFILTER_H__

// End of File
