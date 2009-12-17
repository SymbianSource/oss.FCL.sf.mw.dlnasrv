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






#ifndef __CMPARAM_H
#define __CMPARAM_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmParam class
 *    Capsulating rule params
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CCmParam ): public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmParam class.
     * @return pointer to CCmParam class
     */
    static CCmParam* NewL();
    
    /**
     * Creates new CCmParam class and 
     * leaves the instance in the cleanup stack.
     * @return pointer to CCmParam class
     */
    static CCmParam* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmParam();

public:

    /**
     * Param
     * @since Series 60 3.1          
     * @param aParam, on return parameter value
     * @return  None
     */
    void Param( TPtrC8* aParam );

    /**
     * Param
     * @since Series 60 3.1          
     * @param aParam, on return parameter value
     * @return  None
     */    
    void Param( TInt& aParam );
    
    /**
     * SetComparisonDataL
     * @since Series 60 3.1          
     * @param aComparisonData, datafield value
     * @return  None
     */
    void SetComparisonDataL( const TDesC8& aComparisonData );
    
    /**
     * SetComparisonData (indexed)
     * @since Series 60 3.1          
     * @param aComparisonData, datafield value
     * @return  None
     */
    void SetComparisonData( TInt aComparisonData );
    
    /**
     * ComparisonData
     * @since Series 60 3.1          
     * @param None
     * @return  returns datafield
     */    
    const TDesC8& ComparisonData() const;
    
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
    CCmParam();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    /** datafield */
    HBufC8* iDataField; // owned
    
    /** indexed data field */
    TInt iDataFieldIndexed;

    };

#endif //  __CMPARAM_H