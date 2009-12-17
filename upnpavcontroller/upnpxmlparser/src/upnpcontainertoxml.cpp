/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      XML generation from an UPNP container
*
*/






// INCLUDE FILES
// System
#include <e32base.h>
#include <s32mem.h>

// upnp stack api
#include <upnpstring.h>
#include <upnpcontainer.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h"

// xmlparser internal
#include    "upnpcontainertoxml.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpContainerToXML::CUpnpContainerToXML
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
CUpnpContainerToXML::CUpnpContainerToXML( const CUpnpContainer& aContainer ) :
    iContainer( aContainer )
    {
    }

// --------------------------------------------------------------------------
// CUpnpContainerToXML::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUpnpContainerToXML::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CUpnpContainerToXML::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpContainerToXML* CUpnpContainerToXML::NewL( 
                                        const CUpnpContainer& aContainer )
    {
    CUpnpContainerToXML* self = CUpnpContainerToXML::NewLC( aContainer );    
    CleanupStack::Pop();
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpContainerToXML::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpContainerToXML* CUpnpContainerToXML::NewLC(
    const CUpnpContainer& aContainer )
    {
    CUpnpContainerToXML* self = new( ELeave ) CUpnpContainerToXML(
        aContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
    
// Destructor
CUpnpContainerToXML::~CUpnpContainerToXML()
    {
    }
    
// --------------------------------------------------------------------------
// CUpnpContainerToXML::AsXmlL
// Returns XML buffer
// (other items were commented in a header).
// --------------------------------------------------------------------------
HBufC8* CUpnpContainerToXML::AsXmlL()
    {
    // Not implemented
    return NULL;
    }

// --------------------------------------------------------------------------
// CUpnpContainerToXML::AsEmptyXmlL
// Returns XML buffer
// (other items were commented in a header).
// --------------------------------------------------------------------------
HBufC8* CUpnpContainerToXML::AsEmptyXmlL()
    {
    // Not implemented
    return NULL;
    }

// --------------------------------------------------------------------------
// CUpnpContainerToXML::CreateUnDecodedXmlL
// See upnpcontainertoxml.h
// --------------------------------------------------------------------------
HBufC8* CUpnpContainerToXML::CreateUnDecodedXmlL( const TDesC8& /*aData*/ )
    {
    // Not implemented
    return NULL;
    }

//  End of File
