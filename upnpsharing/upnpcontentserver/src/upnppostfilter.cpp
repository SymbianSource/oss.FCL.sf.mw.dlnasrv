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
* Description:      CUPnPPostFilter class implementation
*
*/







// INCLUDE FILES
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItem.h>
#include <CLFContentListing.hrh>

#include "upnppostfilter.h"
#include "upnpcontentserverdefs.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUpnpPostFilter::CUpnpPostFilter
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//

CUpnpPostFilter::CUpnpPostFilter(
                            const TCLFExtendedFieldId aFilterCriteria,
                            TBool aRemoveDuplicates )
    : iFilterCriteria( aFilterCriteria ),
      iRemoveDuplicates( aRemoveDuplicates )
    {
    }

// --------------------------------------------------------------------------
// CCEPostFilter::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpPostFilter::ConstructL( const TDesC& aFilterParameter )
    {
    iFilterParameter = aFilterParameter.AllocL();
    }

// --------------------------------------------------------------------------
// CCEPostFilter::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//

CUpnpPostFilter* CUpnpPostFilter::NewL(
                            const TCLFExtendedFieldId aFilterCriteria,
                            const TDesC& aFilterParameter,
                            TBool aRemoveDuplicates )
    {
    CUpnpPostFilter* self =
        CUpnpPostFilter::NewLC( aFilterCriteria, aFilterParameter,
                                aRemoveDuplicates );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpPostFilter::NewLC
// Two-phased constructor, pointer is left to the CleanupStack.
// --------------------------------------------------------------------------
//
CUpnpPostFilter* CUpnpPostFilter::NewLC(
                            const TCLFExtendedFieldId aFilterCriteria,
                            const TDesC& aFilterParameter,
                            TBool aRemoveDuplicates )
    {
    CUpnpPostFilter* self = new (ELeave) CUpnpPostFilter(
        aFilterCriteria, aRemoveDuplicates );
    CleanupStack::PushL( self );
    self->ConstructL( aFilterParameter );
    return self;
    }

// Destructor
CUpnpPostFilter::~CUpnpPostFilter()
    {
    delete iFilterParameter;
    }

// --------------------------------------------------------------------------
// CCEPostFilter::FilterItemsL
// Method for filtering the source list.
// --------------------------------------------------------------------------
//
void CUpnpPostFilter::FilterItemsL( const TArray<MCLFItem*>& aItemList,
                                  RPointerArray<MCLFItem>& aFilteredItemList )
    {
    // Process all items in the item list
    TInt count( aItemList.Count() );
    for( TInt i = 0; i < count; i++ )
        {
        // Get value of the field defined in iFilterCriteria.
        // For example, if iFilterCriteria contains ECLFFieldIdAlbum,
        // name of the album is stored to fieldValue variable.
        TPtrC fieldValue;
        MCLFItem* currentItem = aItemList[i];
        TInt error( currentItem->GetField(
            iFilterCriteria, fieldValue ) );

        // Add the music file to the filtered list,
        // if value of of the field is same as iFilterParameter.
        // For example, if it is from the desired album.
        if( error == KErrNone )
            {
            TBool add( EFalse );
            if ( iRemoveDuplicates )
                {
                if ( !IsAdded( aFilteredItemList, fieldValue ) )
                    {
                    add = ETrue;
                    }
                }
            else
                {
                if( fieldValue == iFilterParameter->Des() )
                    {
                    add = ETrue;
                    }
                }
            if( add )
                {
                aFilteredItemList.AppendL( currentItem );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpPostFilter::IsAdded
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
TBool CUpnpPostFilter::IsAdded( RPointerArray<MCLFItem>& aFilteredItemList,
                                TPtrC& aFieldValue )
    {
    TBool isAdded( EFalse );
    for( TInt i(0); i<aFilteredItemList.Count(); i++ )
        {
        MCLFItem* currentItem = aFilteredItemList[i];

        TPtrC fieldValue;
        TInt error( currentItem->GetField(
            iFilterCriteria, fieldValue ) );

        if( error == KErrNone )
            {
            if ( fieldValue == aFieldValue )
                {
                isAdded = ETrue;
                __LOG("found duplicate");
                }
            }
        }
    return isAdded;
    }

//  End of File
