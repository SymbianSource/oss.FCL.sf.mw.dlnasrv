/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Interface for selecting a resource within an item
*
*/






// INCLUDE FILES
#include <upnpitem.h>
#include "upnpelement.h"

// upnpframework / avcontroller helper api
#include "upnpitemutility.h" // ResourceFromItemL, GetResElements
#include "upnpconstantdefs.h" // KElementRes
#include "upnpresourceselector.h"






// ==========================================================================
// METHODS for TUPnPSelectDefaultResource
// ==========================================================================

// --------------------------------------------------------------------------
// TUPnPSelectDefaultResource::SelectResourceL
// this algorithm works BEST
//---------------------------------------------------------------------------
EXPORT_C const CUpnpElement& TUPnPSelectDefaultResource::
    SelectResourceL(
    const CUpnpItem& aItem )
    {
    return UPnPItemUtility::ResourceFromItemL( aItem );
    }


// ==========================================================================
// METHODS for TUPnPSelectFirstResource
// ==========================================================================

// --------------------------------------------------------------------------
// TUPnPSelectFirstResource::SelectResourceL
// this algorithm works FAST
//---------------------------------------------------------------------------
EXPORT_C const CUpnpElement& TUPnPSelectFirstResource::
    SelectResourceL(
    const CUpnpItem& aItem )
    {
    return UPnPItemUtility::FindElementByNameL( aItem, KElementRes );
    }

