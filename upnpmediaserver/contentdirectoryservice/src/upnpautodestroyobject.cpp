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
#include "upnpautodestroyobject.h"
#include "upnpcontentdirectory.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::CUpnpAutoDestroyObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObject::CUpnpAutoDestroyObject(CUpnpContentDirectory* aCd, 
                                               TInt aObjId, TInt aResId)
{
    iResId = aResId;
    iCd = aCd;
    iObjectId = aObjId;
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObject::ConstructL()
{
    iEventTimer = CUpnpNotifyTimer::NewL( this );
    iEventTimer->After( KAutoDestroyTimeLimit, EFalse );
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObject* CUpnpAutoDestroyObject::NewLC(
                                            CUpnpContentDirectory* aCd, 
                                            TInt aObjId, TInt aResId )
{
    CUpnpAutoDestroyObject* self = 
                    new( ELeave ) CUpnpAutoDestroyObject(aCd, aObjId, aResId);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObject* CUpnpAutoDestroyObject::NewL(
                                            CUpnpContentDirectory* aCd, 
                                            TInt aObjId, TInt aResId )
{
    CUpnpAutoDestroyObject* self = NewLC(aCd, aObjId, aResId);
    CleanupStack::Pop(self);
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::~CUpnpAutoDestroyObject
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObject::~CUpnpAutoDestroyObject()
{
    if(iEventTimer)
    {
        iEventTimer->Cancel();
        delete iEventTimer;
    }
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::TimerEventL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObject::TimerEventL(CUpnpNotifyTimer* aTimer)
{
    if ( aTimer == iEventTimer )
    {
        iTimeElapsed = ETrue;
        if( !iTransferStarted )
        {
            iCd->AutoDestroyEventL( iObjectId, ETrue );
        }
    }
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::GetResId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpAutoDestroyObject::GetResId()
{
    return iResId;
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::GetObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpAutoDestroyObject::GetObjId()
{
    return iObjectId;
}
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::SetTransferStartedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObject::SetTransferStartedL(TBool aVal)
{
    iTransferStarted = aVal;
    
    if( iTimeElapsed && !iTransferStarted )
    {
        iCd->AutoDestroyEventL( iObjectId, ETrue );
    }
}

// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObject::RenewTimerL
// Renews timer instead of recreating it
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObject::RenewTimerL()
{
    iTimeElapsed = EFalse;
    
    if(iEventTimer)
    {
        iEventTimer->Cancel();                        
        iEventTimer->After( KAutoDestroyTimeLimit, EFalse );
    }
}

//  End of File  
