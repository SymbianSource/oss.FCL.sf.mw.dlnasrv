/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating Search response hash
*
*/






#ifndef __CMSEARCHRESPONSEHASH_H
#define __CMSEARCHRESPONSEHASH_H

#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmSearchResponseHash class
 *  Capsulating search responce hash
 *  @lib cmcommon.lib
 *  @since S60 v3.2
 */
class CCmSearchResponseHash : public CBase
    {
public:
    
    /* Constructors and destructor. */

    /**
     * Creates new CCmSearchResponseHash class.
     * @param aStartIndex, starting search index
     * @param aItemCount, count of search items
     * @param aHash, calculated hash code
     * @return  pointer to CCmSearchResponseHash class
     */        
    IMPORT_C static CCmSearchResponseHash* NewL( TInt aStartIndex,
                                         TInt aItemCount,
                                         const TDesC8& aHash );

    /**
     * Creates new CCmSearchResponseHash class.
     * @param None
     * @return  pointer to CCmSearchResponseHash class
     */ 
    IMPORT_C static CCmSearchResponseHash* NewL();                                             
 
    /**
     * Destructor
     */      
    virtual ~CCmSearchResponseHash();
    
public:
        
    /**
     * Returns hash code
     * @param None
     * @return  Hash code 
     */         
    IMPORT_C const TDesC8& Hash() const;

    /**
     * Returns hash code
     * @param None
     * @return  Hash code 
     */ 
    IMPORT_C void SetHashL( const TDesC8& aHash );     
    
    /**
     * Returns starting index of search
     * @param None
     * @return  starting index 
     */     
    IMPORT_C TInt StartIndex() const;
    
    /**
     * Sets starting index
     * @param aStartIndex, starting searh index
     * @return None
     */     
    IMPORT_C void SetStartIndex( TInt aStartIndex );
    
    /**
     * Returns item count
     * @param None
     * @return  Item count 
     */     
    IMPORT_C TInt ItemCount() const;
    
    /**
     * Sets item count
     * @param aItemCount, count of items
     * @return  None
     */     
    IMPORT_C void SetItemCount( TInt aItemCount );
        
     /**
    * Externalizes search has information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Internalizes search hash information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    void InternalizeL( RReadStream& aStream );    
        
private:

    /**
     * Constructor
     */ 
    CCmSearchResponseHash();
        
    /**
     * Constructor
     */         
    void ConstructL( TInt aStartIndex,
                         TInt aItemCount,
                         const TDesC8& aHash );
        
private:

    /** Start search index **/
    TInt    iStartIndex;
    
    /** Item count **/        
    TInt    iItemCount;
        
    /** Hash code **/        
    HBufC8* iHash;          // owned
    
    };

#endif //  __CMSEARCHRESPONCSEHASH_H