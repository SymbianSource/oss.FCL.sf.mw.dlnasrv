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






#ifndef __CMFILLRULECONTAINER_H
#define __CMFILLRULECONTAINER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmFillRule;

/**
 *  CCmFillRuleContainer class
 *    Capsulating fill rules
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmFillRuleContainer : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmFillRuleContainer class.
     * @param None
     * @return  pointer to CCmFillRuleContainer class
     */
    IMPORT_C static CCmFillRuleContainer* NewL();
    
    /**
     * Creates new CCmFillRuleContainer class and 
     * leaves the instance in the cleanup stack.
     * @param None
     * @return  pointer to CCmFillRuleContainer class
     */
    IMPORT_C static CCmFillRuleContainer* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmFillRuleContainer();

public:

    /**
     * AddFillRuleL.
     * @since Series 60 3.1     
     * @param aRule
     * @returns index of the rule
     */
    IMPORT_C TInt AddFillRuleL( CCmFillRule* aRule );
 
     /**
     * DeleteFillRule.
     * @since Series 60 3.1
     * @param aIndex
     * @returns None
     */   
    IMPORT_C void DeleteFillRule( TInt aIndex );

    /**
     * FillRule.
     * @since Series 60 3.1     
     * @param aIndex rule index
     * @returns fill rule
     */    
    IMPORT_C CCmFillRule* FillRule( TInt aIndex ) const;

    /**
     * FillRuleCount.
     * @since Series 60 3.1     
     * @param None
     * @returns count
     */    
    IMPORT_C TInt FillRuleCount() const;
    
    /**
    * Externalizes container information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Internalizes container information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );    
    
private:

    /**
     * Constructor.
     */
    CCmFillRuleContainer();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    /** Array rules included to fill rule */
    RPointerArray<CCmFillRule> iFillRuleArray; // items owned
    

    };

#endif //  __CMFILLRULECONTAINER_H
