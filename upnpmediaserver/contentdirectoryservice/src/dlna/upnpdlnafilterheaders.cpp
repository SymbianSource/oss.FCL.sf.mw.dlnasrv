/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies  this distribution, and is available 
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  CUpnpDlnaFilterHeaders implementation.
 *
 */
// INCLUDES

#include "upnpdlnafilterheaders.h"
#include "upnphttpmessage.h"

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::NewL()
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders* CUpnpDlnaFilterHeaders::NewL()
    {
    CUpnpDlnaFilterHeaders* self = new (ELeave) CUpnpDlnaFilterHeaders();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::CUpnpDlnaFilterHeaders()
// Constructor
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders::CUpnpDlnaFilterHeaders()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::~CUpnpDlnaFilterHeaders()
// -----------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders::~CUpnpDlnaFilterHeaders()
    {
    delete iHeaderContainer;
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::AddHeaderL
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilterHeaders::AddHeaderL( const TDesC8 &aName,
    const TDesC8 &aValue )
    {
    iHeaderContainer->AddPairL( aName, aValue );
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::RemoveHeaderL
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilterHeaders::RemoveHeaderL( const TDesC8 &aName )
    {
    iHeaderContainer->RemovePairL( aName );
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::QueryHeader
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpDlnaFilterHeaders::QueryHeader( const TDesC8& aHeaderName )
    {
    return iHeaderContainer->GetHeaderValue( aHeaderName );
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpDlnaFilterHeaders::ConstructL()
    {
    iHeaderContainer = CUpnpHttpMessage::NewL();
    }

// -----------------------------------------------------------------------------
// CUpnpDlnaFilterHeaders::Sender
// -----------------------------------------------------------------------------
//
TInetAddr& CUpnpDlnaFilterHeaders::Sender()
    {
    return iHeaderContainer->Sender();
    }

