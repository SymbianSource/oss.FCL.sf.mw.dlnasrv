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
#include "cmrule.h"
#include "msdebug.h"

// CONSTANTS
const TInt KRuleParamArrayGranularity = 16;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCmRule* CCmRule::NewL()
    {    
    CCmRule* self = CCmRule::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
CCmRule* CCmRule::NewLC()
    {    
    CCmRule* self = new ( ELeave ) CCmRule(  );
    CleanupStack::PushL( self );
    self->ConstructL(  ); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmRule::~CCmRule()
    {
    iRuleParamsArray.ResetAndDestroy();     
    }

// ---------------------------------------------------------------------------
// Adds rule param
// ---------------------------------------------------------------------------
//
TInt CCmRule::AddRuleParamL( const TDesC8& aParam )
    {
    CCmParam* param = CCmParam::NewLC();
    param->SetComparisonDataL( aParam );
    iRuleParamsArray.AppendL( param ); // transfer ownership
    CleanupStack::Pop( param );
    TInt index = iRuleParamsArray.Count() - 1;    
    return index;    
    }

// ---------------------------------------------------------------------------
// Adds rule param ( indexed param )
// ---------------------------------------------------------------------------
//
TInt CCmRule::AddRuleParamL( TInt aParam )
    {
    CCmParam* param = CCmParam::NewLC();
    param->SetComparisonData( aParam );
    iRuleParamsArray.AppendL( param ); // transfer ownership
    CleanupStack::Pop( param );
    TInt index = iRuleParamsArray.Count() - 1;    
    return index;    
    }
    
// ---------------------------------------------------------------------------
// Loads rule
// ---------------------------------------------------------------------------
//
void CCmRule::Rule( TCmMetadataField* aDataField, TCmOperatorType* aOperator, 
    TInt* aParamCount )
    {
    *aDataField = iMetadataField;
    *aOperator = iOperator;
    *aParamCount = iRuleParamsArray.Count();
    }

// ---------------------------------------------------------------------------
// Loads rule param
// ---------------------------------------------------------------------------
//
void CCmRule::RuleParamL( TInt aParamIndex, TPtrC8* aParam )
    {
    // check parameter
    if ( aParamIndex < 0 || aParamIndex >= iRuleParamsArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iRuleParamsArray[aParamIndex]->Param( aParam );
    }

// ---------------------------------------------------------------------------
// Loads rule param
// ---------------------------------------------------------------------------
//
void CCmRule::RuleParamL( TInt aParamIndex, TInt& aParam )
    {
    // check parameter
    if ( aParamIndex < 0 || aParamIndex >= iRuleParamsArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iRuleParamsArray[aParamIndex]->Param( aParam );
    }
        
// ---------------------------------------------------------------------------
// Sets metadata field
// ---------------------------------------------------------------------------
//    
void CCmRule::SetMetadataField( TCmMetadataField aDataField )
    {
    iMetadataField = aDataField;
    }
    
// ---------------------------------------------------------------------------
// Sets operator of the rule
// ---------------------------------------------------------------------------
//    
void CCmRule::SetOperator( TCmOperatorType aOperator )
    {
    iOperator = aOperator;
    }

// ---------------------------------------------------------------------------
// Return count of the parameter 
// ---------------------------------------------------------------------------
//
TInt CCmRule::RuleParamsCount() const
    {
    return iRuleParamsArray.Count();    
    }
    
// ---------------------------------------------------------------------------
// CCmRule::ExternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmRule::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( (TInt)iMetadataField );
       
    aStream.WriteInt16L( (TInt)iOperator );    
    aStream.WriteInt16L( iRuleParamsArray.Count() );    
    
    for ( TInt index(0); index < iRuleParamsArray.Count(); index++ )
        {
        iRuleParamsArray[index]->ExternalizeL( aStream );
        }
          
    }
        
// ---------------------------------------------------------------------------
// CCmRule::InternalizeL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
void CCmRule::InternalizeL( RReadStream& aStream )
    {
    // Content
    iMetadataField = (TCmMetadataField)aStream.ReadInt16L();    
    
    iOperator = (TCmOperatorType)aStream.ReadInt16L();
    
    // cleanup
    iRuleParamsArray.ResetAndDestroy();
    
    // rule param count
    TInt ruleParamCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < ruleParamCount; index++ )
        {
        CCmParam* newRuleParam = CCmParam::NewLC();
        newRuleParam->InternalizeL( aStream );   
        iRuleParamsArray.AppendL( newRuleParam );
        CleanupStack::Pop( newRuleParam ); 
        newRuleParam = NULL;
        }
                     
    }
            
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmRule::CCmRule() : iRuleParamsArray( KRuleParamArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmRule::ConstructL()
    {
    }    

// End of file

