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
* Description:      Capsulating Rule parameters
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>

#include "cmparam.h"
#include "msdebug.h"


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCmParam* CCmParam::NewL()
    {   
    CCmParam* self = CCmParam::NewLC();
    CleanupStack::Pop( self ); 
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
CCmParam* CCmParam::NewLC()
    {    
    CCmParam* self = new ( ELeave ) CCmParam();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmParam::~CCmParam()
    {
    delete iDataField;      
    }

// ---------------------------------------------------------------------------
// Retrieve param info 
// ---------------------------------------------------------------------------
//
void CCmParam::Param( TPtrC8* aParam )
    {
    aParam->Set( *iDataField );
    }

// ---------------------------------------------------------------------------
// Retrieve param info 
// ---------------------------------------------------------------------------
//
void CCmParam::Param( TInt& aParam )
    {
    aParam = iDataFieldIndexed;
    }
        
// ---------------------------------------------------------------------------
// Sets datafield value
// ---------------------------------------------------------------------------
//
void CCmParam::SetComparisonDataL( const TDesC8& aComparisonData )
    {
    delete iDataField;
    iDataField = NULL;
    iDataField = aComparisonData.AllocL();
    }

// ---------------------------------------------------------------------------
// Sets datafield value ( indexed )
// ---------------------------------------------------------------------------
//
void CCmParam::SetComparisonData( TInt aComparisonData )
    {
    iDataFieldIndexed = aComparisonData;
    }
    
// ---------------------------------------------------------------------------
// Returns datafield
// ---------------------------------------------------------------------------
//
const TDesC8& CCmParam::ComparisonData() const
    {
    return *iDataField;
    }
    
// ---------------------------------------------------------------------------
// CCmParam::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmParam::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iDataField->Length() );
    
    if ( iDataField )
        {
        aStream << *iDataField;
        }
    else
        {
        aStream << KNullDesC8();
        }
        
    aStream.WriteInt32L( iDataFieldIndexed );             
    }
        
// ---------------------------------------------------------------------------
// CCmParam::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmParam::InternalizeL( RReadStream& aStream )
    {
    // Content
    delete iDataField;
    iDataField = NULL;

    TInt bufLength = aStream.ReadInt32L();
    iDataField = HBufC8::NewL( aStream, bufLength );
    iDataFieldIndexed = aStream.ReadInt32L();           
    }
        
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmParam::CCmParam()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmParam::ConstructL()
    {
    iDataField = KNullDesC8().AllocL();
    }    

// End of file

