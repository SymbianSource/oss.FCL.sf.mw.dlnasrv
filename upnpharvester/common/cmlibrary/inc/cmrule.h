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
* Description:      Capsulating Rules
*
*/






#ifndef __CMRULE_H
#define __CMRULE_H

// INCLUDES
#include <e32base.h>
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmParam;

/**
 *  CCmRule class
 *    Capsulating rules
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CCmRule ) : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmRule class.
     * @param aDataField
     * @param aOperator
     * @return  pointer to CCmRule class
     */
    static CCmRule* NewL();
    
    /**
     * Creates new CCmRule class and 
     * leaves the instance in the cleanup stack.
     * @param aDataField
     * @param aOperator
     * @return  pointer to CCmRule class
     */
    static CCmRule* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmRule();

public:

    /**
     * AddRuleParamL
     * @since Series 60 3.1     
     * @param aParam
     * @return  index of the param
     */
    TInt AddRuleParamL( const TDesC8& aParam );

    /**
     * AddRuleParamL ( indexed param )
     * @since Series 60 3.1     
     * @param aParam
     * @return  index of the param
     */
    TInt AddRuleParamL( TInt aParam );
    
    /**
     * Rule
     * @since Series 60 3.1          
     * @param aDataField, metadatafield on return
     * @param aOperator, operator on return
     * @param aParamCount, param count on return
     * @return None
     */    
    void Rule( TCmMetadataField* aDataField, TCmOperatorType* aOperator,
        TInt* aParamCount );

    /**
     * RuleParam
     * @since Series 60 3.1          
     * @param aParamIndex, parameter index
     * @param aParam, param
     * @return None
     */        
    void RuleParamL( TInt aParamIndex, TPtrC8* aParam );
    
    /**
     * RuleParam
     * @since Series 60 3.1          
     * @param aParamIndex, parameter index
     * @param aParam, param
     * @return None
     */        
    void RuleParamL( TInt aParamIndex, TInt& aParam );
    
    /**
     * SetMetadataField
     * @since Series 60 3.1          
     * @param aDataField, sets comparison field
     * @return None
     */        
    void SetMetadataField( TCmMetadataField aDataField );

    /**
     * SetOperator
     * @since Series 60 3.1          
     * @param aOperator, sets operator of the rule
     * @return None
     */            
    void SetOperator( TCmOperatorType aOperator );
        
    /**
     * RuleParamsCount
     * @since Series 60 3.1          
     * @return Count of rule params
     */    
    TInt RuleParamsCount() const;
    
    /**
    * Externalizes container information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Internalizes container information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    void InternalizeL( RReadStream& aStream );        
    
private:

    /**
     * Constructor.
     */
    CCmRule();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    /** Metadata field */
    TCmMetadataField iMetadataField;
    
    /** Operator */
    TCmOperatorType iOperator;
    
    /** Array rules included to fill rule*/
    RPointerArray<CCmParam> iRuleParamsArray; // items owned

    };
    
#endif //  __CMRULE_H
