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

const TInt KDefaultItemCount = 2^32; //4294967296

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS =============================

// --------------------------------------------------------------------------
// CUpnpSharingRequest::CUpnpSharingRequest
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//

CUpnpSharingRequest::CUpnpSharingRequest( TInt aKind )
    {
    iKind = aKind;
    iItemCount = KDefaultItemCount;
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpSharingRequest::ConstructL( const RArray<TInt>& aArr, 
                                      CDesCArray* aIdArray,
                                      CDesCArray* aNameArray )
    {
    for ( TInt i(0); i<aArr.Count();i++ )
        {
        iSelections.AppendL( aArr[ i ] );
        }
    iObjectIds = aIdArray; // transfer ownership
    iObjectNames = aNameArray;
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpSharingRequest* CUpnpSharingRequest::NewL(
    TInt aKind,
    const RArray<TInt>& aArr,
    CDesCArray* aIdArray,
    CDesCArray* aNameArray )
    {
    CUpnpSharingRequest* self = new (ELeave) CUpnpSharingRequest( aKind );
    CleanupStack::PushL( self );
    self->ConstructL( aArr,
                      aIdArray,
                      aNameArray );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpSharingRequest::NewL
// Destructor
// --------------------------------------------------------------------------
// 
CUpnpSharingRequest::~CUpnpSharingRequest()
    {
    iSelections.Close();
    delete iObjectIds;
    }


//  End of File
