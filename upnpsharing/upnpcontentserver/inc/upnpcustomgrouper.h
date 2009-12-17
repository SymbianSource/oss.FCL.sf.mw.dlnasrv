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
* Description:      Custom grouper class definition
*
*/








#ifndef __CUPNPAPPCUSTOMGROUPER_H__
#define __CUPNPAPPCUSTOMGROUPER_H__

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <MCLFCustomGrouper.h>
#include <CLFContentListing.hrh>
// this will be included when it back in SDK
// #include <CLFContentListingExtended.hrh>

#include "upnpcontentserverdefs.h"

// FORWARD DECLARATIONS
class MCLFItem;
class MCLFModifiableItem;

// CLASS DECLARATION

/**
*  CLF Custom Grouper.
*  Example of Custom Grouper that can be used to group files in the
*  list model. With Custom Groupers you can create grouping functionality that
*  is not supported in Content Listing Framework by default. This Custom
*  Grouper can group items of a list model by any field. In this example
*  application, this Custom Grouper is used to group music items by artist
*  name.
*/
class CUpnpCustomGrouper
    : public CBase,
      public MCLFCustomGrouper
    {
    public:  // Constructors and destructor

        /**
        * Creates and returns a new instance of this class.
        * @param aGroupingCriteria Group items by this criteria
        * @return Pointer to the CCECustomGrouper object
        */

        static CUpnpCustomGrouper* NewL(
                        TCLFExtendedFieldId aGroupingCriteria );

        /**
        * Destructor.
        */
        virtual ~CUpnpCustomGrouper();

    private:    // Constructors
        /**
        * C++ default constructor.
        * @param aGroupingCriteria Group list items by this criteria
        */
        CUpnpCustomGrouper( TCLFExtendedFieldId aGroupingCriteria );

    protected:     // Functions from base classes

        /**
        * From MCLFCustomGrouper. Method for grouping Content Listing
        * Framework items. Will get called when grouping operation is executed
        * by calling refresh for the list model.
        * @since S60 3.1
        * @param aSourceList Source list
        * @param aGroupedList Grouped/destination list
        */
        void GroupItemsL( const TArray<MCLFItem*>& aSourceList,
                          RPointerArray<MCLFItem>& aGroupedList );

    private:    // New functions

        /**
        * Checks if group has been added to the destination array already.
        * @param aGroupName Name of the group to be checked
        * @return ETrue If group has been added to the model, EFalse if not
        */
        TBool GroupAlreadyExists( const TDesC& aGroupName );

    private:    // Data

        // Array for keeping track, which items have been added to the
        // model. Deletes items when they are no longer needed.
        RPointerArray<MCLFModifiableItem> iModItems;

        // Criteria for grouping
        TCLFExtendedFieldId iGroupingCriteria;

    };

#endif // __CUPNPAPPCUSTOMGROUPER_H__

// End of File
