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
#include "upnpautodestroyobjectlist.h"
#include "upnpautodestroyobject.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::CUpnpAutoDestroyObjectList
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObjectList::CUpnpAutoDestroyObjectList()
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObjectList::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::NewLC
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObjectList* CUpnpAutoDestroyObjectList::NewLC()
    {
    CUpnpAutoDestroyObjectList* self = new( ELeave ) CUpnpAutoDestroyObjectList;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::NewLC
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObjectList* CUpnpAutoDestroyObjectList::NewL()
    {
    CUpnpAutoDestroyObjectList* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::~CUpnpAutoDestroyObjectList
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObjectList::~CUpnpAutoDestroyObjectList()
    {
    iArray.ResetAndDestroy();
    }
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::AppendL
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObjectList::AppendL(const CUpnpAutoDestroyObject* aObj)
    {
    iArray.AppendL(aObj);
    }
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::RemoveAndDestroyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CUpnpAutoDestroyObjectList::RemoveAndDestroyL(TInt aIndex)
    {
    if( aIndex < 0 || aIndex >= iArray.Count() )
        {   
        User::Leave(KErrNotFound);
        }    
    delete iArray[aIndex];
    iArray.Remove(aIndex);
    }

// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::FindObjIndex
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpAutoDestroyObjectList::FindObjIndexByObjId(TInt aObjId)
    {
    TInt i;
    for( i = iArray.Count()-1; i >= 0; i-- )
        {
        if( iArray[i]->GetObjId() == aObjId )
            {
            break;
            }
        }
    return i;
    }
    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::FindObjIndex
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpAutoDestroyObjectList::FindObjIndexByResId(TInt aResId)
    {
    TInt i;
    for( i = iArray.Count()-1; i >= 0 ; i-- )
        {
        if( iArray[i]->GetResId() == aResId )
            {
            break;
            }
        }
    return i;
    }
    
// -----------------------------------------------------------------------------
// CUpnpAutoDestroyObjectList::operator[]
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpAutoDestroyObject* CUpnpAutoDestroyObjectList::operator[](TInt aIndex)
    {
    return iArray[aIndex];
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferFailL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpAutoDestroyObjectList::TransferFailL( TInt aResId )
    {
    TInt index = FindObjIndexByResId( aResId );
    if(index != KErrNotFound)
        {
        CUpnpAutoDestroyObject* currADObj = iArray[index];
                                 
        /* If transfer to newly created object [without resource] is timed out [408] due to
        *  many stoppages of transfer, the object is still accessible until auto-destroy deletes it.
        *  So AutoDestroyObject time should be extended by another KAutoDestroyTimeLimit
        */       
        currADObj->RenewTimerL();  
        
        // Setting failed transfer as finished              
        currADObj->SetTransferStartedL(EFalse);                                        
        }
    }

TInt CUpnpAutoDestroyObjectList::Count( )
{
    return iArray.Count();
}


//  End of File  
