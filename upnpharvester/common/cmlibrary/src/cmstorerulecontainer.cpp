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
* Description:      Capsulating fill rules
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmstorerulecontainer.h"
#include "cmstorerule.h"
#include "msdebug.h"
 
// CONSTANTS
const TInt KStoreRuleArrayGranularity = 16;
 
// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreRuleContainer* CCmStoreRuleContainer::NewL()
    {
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::NewL() start")));    
    CCmStoreRuleContainer* self = CCmStoreRuleContainer::NewLC();
    CleanupStack::Pop( self );
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::NewL() end"))); 
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmStoreRuleContainer* CCmStoreRuleContainer::NewLC()
    {
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::NewLC() start")));    
    CCmStoreRuleContainer* self = new ( ELeave ) CCmStoreRuleContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::NewLC() end"))); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreRuleContainer::~CCmStoreRuleContainer()
    {
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::\
        ~CCmStoreRuleContainer() start")));
    iStoreRuleArray.ResetAndDestroy();
    iStoreRuleArray.Close();
    TRACE(Print(_L("[COMMON]\t CCmStoreRuleContainer::\
        ~CCmStoreRuleContainer() end")));     
    }

// ---------------------------------------------------------------------------
// Add new rule into store rule
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmStoreRuleContainer::AddStoreRuleL( CCmStoreRule* aRule )
    {
    iStoreRuleArray.AppendL( aRule );
    TInt index = iStoreRuleArray.Count() - 1;    
    return index;
    }

// ---------------------------------------------------------------------------
// Deletes store rule with the given index
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRuleContainer::DeleteStoreRule( TInt aIndex )
    {
    if( iStoreRuleArray.Count() > aIndex )
        {
        delete iStoreRuleArray[aIndex];
        iStoreRuleArray.Remove(aIndex);
        iStoreRuleArray.Compress();
        }
    }
    
// ---------------------------------------------------------------------------
// Get rule
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreRule* CCmStoreRuleContainer::StoreRule( TInt aIndex )
    {
    return iStoreRuleArray[aIndex];
    }

// ---------------------------------------------------------------------------
// Returns count of rules
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmStoreRuleContainer::StoreRuleCount() const
    {
    return iStoreRuleArray.Count();
    }    

// ---------------------------------------------------------------------------
// CCmFillRuleContainer::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRuleContainer::ExternalizeL( 
    RWriteStream& aStream ) const
    {
    // Let's write the count of fill rules to stream first
    aStream.WriteInt16L( iStoreRuleArray.Count() );
    for ( TInt index = 0; index < iStoreRuleArray.Count(); index++ )
        {
        CCmStoreRule* rule = iStoreRuleArray[index];    
        // Then the object itself
        rule->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------------------------
// CUPnPContainer::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmStoreRuleContainer::InternalizeL( RReadStream& aStream )
    {
    // Then internalize the objects
    if ( iStoreRuleArray.Count() > KErrNone )
        {
        iStoreRuleArray.ResetAndDestroy();
        }
    // First the count of fill rules
    TInt ruleCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < ruleCount; index++ )
        {
        CCmStoreRule* newItem = CCmStoreRule::NewL();
        CleanupStack::PushL( newItem );
        newItem->InternalizeL( aStream );
        AddStoreRuleL( newItem );
        CleanupStack::Pop( newItem ); 
        newItem = NULL;
        }
    }
                    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmStoreRuleContainer::CCmStoreRuleContainer() : 
    iStoreRuleArray( KStoreRuleArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmStoreRuleContainer::ConstructL()
    {
    }    

// End of file

