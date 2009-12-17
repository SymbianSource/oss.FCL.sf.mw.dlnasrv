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
* Description:      Capsulating resolution property items
*
*/






#ifndef __CMSQLRESOLUTIONPROPERTYITEM_H
#define __CMSQLRESOLUTIONPROPERTYITEM_H

// INCLUDES
#include "cmsqlpropertyitem.h"

// FORWARD DECLARATIONS

/**
 *  CCmSqlResolutionPropertyItem class
 *  Capsulating resolution property items
 *  @lib cmcommon.lib
 *  @since S60 v3.1
 */
class CCmSqlResolutionPropertyItem : public CCmSqlPropertyItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlResolutionPropertyItem class.
     * @param None
     * @return  pointer to CCmSqlResolutionPropertyItem class
     */
    IMPORT_C static CCmSqlResolutionPropertyItem* NewL();
    
    /**
     * Creates new CCmSqlResolutionPropertyItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlResolutionPropertyItem class
     */
    IMPORT_C static CCmSqlResolutionPropertyItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlResolutionPropertyItem();

public: 

    /**
    * Sets width of the image
    * @since Series 60 3.1
    * @param aWidth, width of the image
    * @return None
    */
    IMPORT_C void SetWidth( const TInt aWidth );
    
    /**
    * Sets height of the image
    * @since Series 60 3.1
    * @param aHeight, height of the image
    * @return None
    */    
    IMPORT_C void SetHeight( const TInt aHeight );
   
    /**
    * Sets pixel count of the item 
    * @since Series 60 3.1
    * @param aSize, size of video or image item ( in bytes )
    * @return None
    */   
    IMPORT_C void SetPixelCount( const TInt aPixelCount );

    /**
    * Gets width of the image
    * @since Series 60 3.1
    * @param None
    * @return Duration
    */    
    IMPORT_C TInt Width() const;
    
    /**
    * Gets height of the image
    * @since Series 60 3.1
    * @param None
    * @return Bitrate
    */    
    IMPORT_C TInt Height() const;
    
    /**
    * Gets pixel count of the image
    * @since Series 60 3.1
    * @param None
    * @return Bitrate
    */    
    IMPORT_C TInt PixelCount() const;    
    
    /**
    * Compares items
    * @since Series 60 3.1
    * @param aFirst, first item
    * @param aSecond, second item
    * @return Comparison result
    */     
    IMPORT_C static TInt CompareItemsByName( 
        const CCmSqlResolutionPropertyItem& aFirst,
        const CCmSqlResolutionPropertyItem& aSecond );
                                                  
public:
        
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

protected:

    /**
     * Constructor.
     */
    CCmSqlResolutionPropertyItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
  
    // Width of image
    TInt iWidth;
    
    // Height of image
    TInt iHeight;
   
    // Pixel count of image 
    TInt iPixelCount; 
     
    };

                                                          
#endif //  __CMSQLRESOLUTIONPROPERTYITEM_H