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






#ifndef __CMSQLVIDEOITEM_H
#define __CMSQLVIDEOITEM_H

// INCLUDES
#include <e32base.h>
#include "cmsqlgenericitem.h"

// FORWARD DECLARATIONS

/**
 *  CCmSqlVideoItem class
 *  Capsulating sql items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlVideoItem : public CCmSqlGenericItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlVideoItem class.
     * @param None
     * @return  pointer to CCmSqlVideoItem class
     */
    IMPORT_C static CCmSqlVideoItem* NewL();
    
    /**
     * Creates new CCmSqlVideoItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlVideoItem class
     */
    IMPORT_C static CCmSqlVideoItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlVideoItem();
  
public:

    /**
    * Sets genre id
    * @since Series 60 3.1
    * @param aGenreId, genre
    * @return None
    */
    IMPORT_C void SetGenreId( const TInt64 aGenreId );

    /**
    * Gets genre id
    * @since Series 60 3.1
    * @param None
    * @return genre id
    */     
    IMPORT_C TInt64 GenreId() const;
    
protected:

    /**
     * Constructor.
     */
    CCmSqlVideoItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    // Genre id
    TInt64 iGenreId;
    
    };

#endif //  __CMSQLVIDEOITEM_H