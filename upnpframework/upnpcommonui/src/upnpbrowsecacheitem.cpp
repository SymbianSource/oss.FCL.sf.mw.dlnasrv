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
* Description:  Source file for UPnP Browse cache item implementation
*
*/


// INCLUDES
#include <upnpobjectlist.h>
#include "upnpbrowsecacheitem.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::CUPnPBrowseCacheItem
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPBrowseCacheItem::CUPnPBrowseCacheItem()
    {
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUPnPBrowseCacheItem::ConstructL( const TDesC8& aContainer,
                                       TInt aHighLightedItem,
                                       TInt aFirstItem )
    {
    iContainerData = aContainer.AllocL();
    iFirstItem = aFirstItem;
    iHighLightedItem = aHighLightedItem;
    iItemArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
EXPORT_C CUPnPBrowseCacheItem* CUPnPBrowseCacheItem::NewL(
                               const TDesC8& aContainer,
                               TInt aHighLightedItem,
                               TInt aFirstItem )
    {
    CUPnPBrowseCacheItem* self = new( ELeave ) CUPnPBrowseCacheItem;
    
    CleanupStack::PushL( self );
    self->ConstructL( aContainer, aHighLightedItem, aFirstItem );
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUPnPBrowseCacheItem::~CUPnPBrowseCacheItem()
    {
    if ( iItemArray.Count() > 0 )
        {
        iItemArray.ResetAndDestroy();
        }
    iItemArray.Close();    
    delete iContainerData;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::ContainerId
// --------------------------------------------------------------------------
EXPORT_C const HBufC8& CUPnPBrowseCacheItem::ContainerId() const
    {
    return *iContainerData;    
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::SetItemArrayL
// --------------------------------------------------------------------------
EXPORT_C void CUPnPBrowseCacheItem::SetItemArrayL(
                            const RPointerArray<CUpnpObject>& aBrowseArray )
    {
    iItemArray.ResetAndDestroy();
    for ( TInt index=0; index < aBrowseArray.Count(); index++ )
        {        
        iItemArray.AppendL( aBrowseArray[ index ] );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::SetFirstItem
// --------------------------------------------------------------------------
EXPORT_C void CUPnPBrowseCacheItem::SetFirstItem( const TInt aFirstItem ) 
    {
    iFirstItem = aFirstItem;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::SetTotalCount
// --------------------------------------------------------------------------
void CUPnPBrowseCacheItem::SetTotalCount( const TInt aTotalCount ) 
    {
    iTotalCount = aTotalCount;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::SetHighLightedItem
// --------------------------------------------------------------------------
EXPORT_C void CUPnPBrowseCacheItem::SetHighLightedItem(
                                            const TInt aHighLightedItem )
    {
    iHighLightedItem = aHighLightedItem;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::GetItem
// --------------------------------------------------------------------------
EXPORT_C const CUpnpObject* CUPnPBrowseCacheItem::GetItem(
                                                    const TInt aItemToGet )
    {
    return iItemArray[ aItemToGet ];
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::GetNumberOfItems
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPBrowseCacheItem::GetNumberOfItems() const
    {
    return iItemArray.Count();
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::GetTotalCount
// --------------------------------------------------------------------------
TInt CUPnPBrowseCacheItem::GetTotalCount() const
    {
    return iTotalCount;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::GetUpperMostItem
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPBrowseCacheItem::GetUpperMostItem() const
    {
    return iFirstItem;
    }

// --------------------------------------------------------------------------
// CUPnPBrowseCacheItem::GetHighlightedItem
// --------------------------------------------------------------------------
EXPORT_C TInt CUPnPBrowseCacheItem::GetHighlightedItem() const
    {
    return iHighLightedItem;
    }

// End of file
