/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Element table data handler
*
*/



// INCLUDE FILES
#include "upnpsender.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpcontentdirectory.h"
#include <upnpobject.h>
#include <upnpitem.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCdsSender::CUpnpCdsSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpCdsSender::CUpnpCdsSender(CUpnpContentDirectory* aCd)
{
    iCd = aCd;
}

// -----------------------------------------------------------------------------
// CUpnpCdsSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCdsSender::ConstructL()
{

}
// -----------------------------------------------------------------------------
// CUpnpCdsSender::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCdsSender* CUpnpCdsSender::NewLC(CUpnpContentDirectory* aCd)
{
    CUpnpCdsSender* self = new (ELeave) CUpnpCdsSender(aCd);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpCdsSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCdsSender* CUpnpCdsSender::NewL(CUpnpContentDirectory* aCd)
{
    CUpnpCdsSender* self = NewLC(aCd);
    CleanupStack::Pop();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpCdsSender::~CUpnpCdsSender
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCdsSender::~CUpnpCdsSender()
{
}
// -----------------------------------------------------------------------------
// CUpnpCdsSender::SendL
// -----------------------------------------------------------------------------
//
void CUpnpCdsSender::SendL(CUpnpAction* aAction, TUpnpErrorCode aError)
{
    iCd->SendActionL(aAction, aError);
}
// -----------------------------------------------------------------------------
// CUpnpCdsSender::SendL
// -----------------------------------------------------------------------------
//
void CUpnpCdsSender::SendL( CUpnpHttpMessage* aMessage )
{
    iCd->SendMessageL(aMessage);
}
//  End of File  
