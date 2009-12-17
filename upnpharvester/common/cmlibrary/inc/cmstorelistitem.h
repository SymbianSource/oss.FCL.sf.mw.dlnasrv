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
* Description:      Capsulating Store file list items
*
*/






#ifndef __CMSTORELISTITEM_H
#define __CMSTORELISTITEM_H

// INCLUDES
#include <e32base.h>
#include "cmbaselistitem.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmStoreListItem class
 *    Capsulating Store file list items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmStoreListItem : public CCmBaseListItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmStoreListItem class.
     * @return  pointer to CCmStoreListItem class
     */
    IMPORT_C static CCmStoreListItem* NewL();
    
    /**
     * Creates new CCmStoreListItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmStoreListItem class
     */
    IMPORT_C static CCmStoreListItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmStoreListItem();

public:
    
    // Sets
    IMPORT_C void SetListId( const TUint aListId );
        
    IMPORT_C void SetDevId( const TUint8 aId, TCmListItemStatus aStatus );

    // Gets
    IMPORT_C TUint ListId() const;
    
    IMPORT_C RArray<TInt> DevIds() const;
    
    IMPORT_C RArray<TCmListItemStatus> StatusValues() const;
    
    IMPORT_C void UpdateFileStatusL( const TUint8 aId, 
                        TCmListItemStatus aStatus );
    
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
    CCmStoreListItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data

    TUint iListId;
    RArray<TInt>iDeviceIds;
    RArray<TCmListItemStatus>iStatusValues;
    };

#endif //  __CMSTORELISTITEM_H