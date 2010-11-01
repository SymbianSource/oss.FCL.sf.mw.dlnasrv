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
* Description:      CUpnpSharingRequest class implementation
*
*/

// INCLUDE FILES
#include "upnpsharingrequest.h"
#include "upnpcontentserverdefs.h"


using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUpnpSharingRequest::CUpnpSharingRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//

CUpnpSharingRequest::CUpnpSharingRequest( 
    TUpnpMediaType aMediaType, 
    TInt aSharingType ) :
    iMediaType( aMediaType ),
    iSharingType( aSharingType )
    {
    // empty
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpSharingRequest* CUpnpSharingRequest::NewL( 
    TUpnpMediaType aMediaType, 
    TInt aSharingType )
    {
    CUpnpSharingRequest* self = 
        new (ELeave) CUpnpSharingRequest( aMediaType, aSharingType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::SetSharingRequestInfoL
// Sets sharing request information arrays
// --------------------------------------------------------------------------
// 
void CUpnpSharingRequest::SetSharingRequestInfo( 
    RArray<TFileName>* aShareArr,
    RArray<TFileName>* aUnshareArr,
    CDesCArray* aClfIds )
    {
    // take ownership of the arrays
    iShareArr = aShareArr;
    iUnshareArr = aUnshareArr;
    iClfIds = aClfIds;
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::~CUpnpSharingRequest
// Destructor
// --------------------------------------------------------------------------
// 
CUpnpSharingRequest::~CUpnpSharingRequest()
    {
    // Destructor
    if ( iShareArr )
        {
        iShareArr->Close();
        delete iShareArr;
        }
    if ( iUnshareArr )
        {
        iUnshareArr->Close();
        delete iUnshareArr;
        }
    if ( iClfIds )
        {
        delete iClfIds;
        }
    }


//  End of File
