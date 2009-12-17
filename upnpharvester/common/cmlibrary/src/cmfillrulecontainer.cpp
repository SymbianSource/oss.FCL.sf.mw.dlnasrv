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






#include <e32std.h>
#include <s32mem.h>
#include "cmfillrulecontainer.h"
#include "cmfillrule.h"
#include "msdebug.h"

// CONSTANTS
const TInt KFillRuleArrayGranularity = 16;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillRuleContainer* CCmFillRuleContainer::NewL()
    {
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::NewL() start")));    
    CCmFillRuleContainer* self = CCmFillRuleContainer::NewLC();
    CleanupStack::Pop( self );
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::NewL() end"))); 
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmFillRuleContainer* CCmFillRuleContainer::NewLC()
    {
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::NewLC() start")));    
    CCmFillRuleContainer* self = new ( ELeave ) CCmFillRuleContainer();
    CleanupStack::PushL( self );
    self->ConstructL();
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::NewLC() end"))); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillRuleContainer::~CCmFillRuleContainer()
    {
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::~CCmFillRuleContainer()\
         start")));
    iFillRuleArray.ResetAndDestroy();
    iFillRuleArray.Close();
    TRACE(Print(_L("[COMMON]\t CCmFillRuleContainer::~CCmFillRuleContainer()\
        end")));     
    }

// ---------------------------------------------------------------------------
// Add new rule into FillRule
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRuleContainer::AddFillRuleL( CCmFillRule* aRule )
    {
    iFillRuleArray.AppendL( aRule );
    TInt index = iFillRuleArray.Count() - 1;    
    return index;
    }

// ---------------------------------------------------------------------------
// Deletes fill rule with the given index
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRuleContainer::DeleteFillRule( TInt aIndex )
    {
    if( iFillRuleArray.Count() > aIndex )
        {
        delete iFillRuleArray[aIndex];
        iFillRuleArray.Remove(aIndex);
        iFillRuleArray.Compress();
        }
    }
    
// ---------------------------------------------------------------------------
// Get rule
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillRule* CCmFillRuleContainer::FillRule( TInt aIndex ) const
    {
    return iFillRuleArray[aIndex];
    }

// ---------------------------------------------------------------------------
// Returns count of rules
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRuleContainer::FillRuleCount() const
    {
    return iFillRuleArray.Count();
    }    

// ---------------------------------------------------------------------------
// CCmFillRuleContainer::ExternalizeL
// Writes the content to stream.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRuleContainer::ExternalizeL( 
    RWriteStream& aStream ) const
    {
    // Let's write the count of fill rules to stream first
    aStream.WriteInt16L( iFillRuleArray.Count() );
    for ( TInt index = 0; index < iFillRuleArray.Count(); index++ )
        {
        CCmFillRule* rule = iFillRuleArray[index];    
        // Then the object itself
        rule->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------------------------
// CCmFillRuleContainer::InternalizeL
// Fills container information from stream
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRuleContainer::InternalizeL( RReadStream& aStream )
    {
    // Then internalize the objects
    if ( iFillRuleArray.Count() > KErrNone )
        {
        iFillRuleArray.ResetAndDestroy();
        }
    // First the count of fill rules
    TInt ruleCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for ( TInt index = 0; index < ruleCount; index++ )
        {
        CCmFillRule* newItem = CCmFillRule::NewL();
        CleanupStack::PushL( newItem );
        newItem->InternalizeL( aStream );
        AddFillRuleL( newItem );
        CleanupStack::Pop( newItem );
        newItem = NULL;
        }
    }
                  
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmFillRuleContainer::CCmFillRuleContainer() : 
    iFillRuleArray( KFillRuleArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmFillRuleContainer::ConstructL()
    {
    }    

// End of file

