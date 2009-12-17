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
* Description:      Capsulating sql items
*
*/






#ifndef __CMSQLITEMRESOURCE_H
#define __CMSQLITEMRESOURCE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

/**
 *  CCmSqlItemResource class
 *  Capsulating sql items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
NONSHARABLE_CLASS( CCmSqlItemResource ) : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlItemResource class.
     * @param None
     * @return  pointer to CCmSqlItemResource class
     */
    static CCmSqlItemResource* NewL();
    
    /**
     * Creates new CCmSqlItemResource class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlItemResource class
     */
    static CCmSqlItemResource* NewLC();

    /**
     * Creates new CCmSqlItemResource class and 
     * leaves the instance in the cleanup stack.
     * @param aUri
     * @param aSize
     * @param aDuration
     * @param aBitrate
     * @param aResolutionId
     * @return pointer to CCmSqlItemResource class
     */
    static CCmSqlItemResource* NewLC( const TDesC8& aUri, 
    const TInt aSize, const TInt aDuration, const TInt aBitrate,
    const TInt64 aResolutionId );
        
    /**
     * Destructor.
     */
    virtual ~CCmSqlItemResource();

public:

    /**
    * Sets uri of the resource
    * @since Series 60 3.1
    * @param aUri, resource uri
    * @return None
    */ 
    void SetUriL( const TDesC8& aUri );
    
    /**
    * Sets size of resource
    * @since Series 60 3.1
    * @param aSize, size of the resource
    * @return None
    */     
    void SetSize( const TInt aSize );
    
    /**
    * Sets duration of resource
    * @since Series 60 3.1
    * @param aDuration, duration of the resource
    * @return None
    */     
    void SetDuration( const TInt aDuration );
    
    /**
    * Sets bitrate of resource
    * @since Series 60 3.1
    * @param aBitrate, bitrate of the resource
    * @return None
    */     
    void SetBitrate( const TInt aBitrate );
    
    /**
    * Sets resolution id of resource
    * @since Series 60 3.1
    * @param aResolutionId, id of resolution
    * @return None
    */     
    void SetResolutionId( const TInt64 aResolutionId );    
    
    /**
    * Gets Uri
    * @since Series 60 3.1
    * @param None
    * @return iUri
    */     
    TDesC8& Uri() const;
    
    /**
    * Gets size 
    * @since Series 60 3.1
    * @param None
    * @return iSize
    */     
    TInt Size() const;
    
    /**
    * Gets duration
    * @since Series 60 3.1
    * @param None
    * @return iDuration
    */     
    TInt Duration() const;
    
    /**
    * Gets bitrate
    * @since Series 60 3.1
    * @param None
    * @return iBitrate
    */ 
    TInt Bitrate() const;
    
    /**
    * Gets resolution id
    * @since Series 60 3.1
    * @param None
    * @return iResolutionId
    */ 
    TInt ResolutionId() const;    
    
protected:

    /**
     * Constructor.
     */
    CCmSqlItemResource();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Second-phase constructor.
     */
    void ConstructL( const TDesC8& aUri, 
        const TInt aSize, const TInt aDuration, const TInt aBitrate,
        const TInt64 aResolutionId );    

protected: // data  
    
    // Name
    HBufC8* iUri; // owned
    
    // Size of the resource
    TInt iSize;

    // Duration of the resource
    TInt iDuration;
        
    // Bitrate of the resource
    TInt iBitrate;
     
    // Id to resolution
    TInt64 iResolutionId;
    
    };

#endif //  __CMSQLITEMRESOURCE_H