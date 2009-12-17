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
* Description:      Capsulating store rules
*
*/






#ifndef __CMSTORERULECONTAINER_H
#define __CMSTORERULECONTAINER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmStoreRule;

/**
 *  CCmStoreRuleContainer class
 *    Capsulating store rules
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmStoreRuleContainer : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmStoreRuleContainer class.
     * @param None
     * @return  pointer to CCmStoreRuleContainer class
     */
    IMPORT_C static CCmStoreRuleContainer* NewL();
    
    /**
     * Creates new CCmStoreRuleContainer class and 
     * leaves the instance in the cleanup stack.
     * @param None
     * @return  pointer to CCmStoreRuleContainer class
     */
    IMPORT_C static CCmStoreRuleContainer* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmStoreRuleContainer();

public:

    /**
     * AddStoreRuleL.
     * @since Series 60 3.1     
     * @param aRule
     * @returns index of the rule
     */
    IMPORT_C TInt AddStoreRuleL( CCmStoreRule* aRule );

    /**
     * DeleteStoreRule.
     * @since Series 60 3.1
     * @param Index
     * @returns None
     */    
    IMPORT_C void DeleteStoreRule( TInt aIndex );

    /**
     * StoreRule.
     * @since Series 60 3.1     
     * @param aIndex rule index
     * @returns store rule
     */    
    IMPORT_C CCmStoreRule* StoreRule( TInt aIndex );

    /**
     * StoreRuleCount.
     * @since Series 60 3.1     
     * @param None
     * @returns count
     */    
    IMPORT_C TInt StoreRuleCount() const;
    
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
    CCmStoreRuleContainer();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    // Array rules included to fill rule. 
    RPointerArray<CCmStoreRule> iStoreRuleArray; // Items owned

    };

#endif //  __CMSTORERULECONTAINER_H
