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
* Description:      Capsulating property items
*
*/






#ifndef __CMSQLPROPERTYITEM_H
#define __CMSQLPROPERTYITEM_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmSqlPropertyItem class
 *  Capsulating property items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlPropertyItem : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlPropertyItem class.
     * @param None
     * @return  pointer to CCmSqlPropertyItem class
     */
    IMPORT_C static CCmSqlPropertyItem* NewL();

    /**
     * Creates new CCmSqlPropertyItem class and
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlPropertyItem class
     */
    IMPORT_C static CCmSqlPropertyItem* NewLC();

    IMPORT_C CCmSqlPropertyItem( const CCmSqlPropertyItem& aItem );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlPropertyItem();

public:

    /**
    * Sets database id
    * @since Series 60 3.1
    * @param aId, database id
    * @return None
    */
    IMPORT_C void SetId( const TInt64 aId );

    /**
    * Sets value of the property in textual format
    * @since Series 60 3.1
    * @param aName, text field
    * @return None
    */
    IMPORT_C void SetNameL( const TDesC8& aName );

    /**
    * Sets status of the item
    * @since Series 60 3.1
    * @param aStatus, ETrue if item already in db
    * @return None
    */
    IMPORT_C void SetStatus( const TBool aStatus );

    /**
    * Gets database id
    * @since Series 60 3.1
    * @param None
    * @return Database id
    */
    IMPORT_C TInt64 Id() const;

    /**
    * Gets property text value
    * @since Series 60 3.1
    * @param None
    * @return Property value
    */
    IMPORT_C TDesC8& Name() const;

    /**
    * Gets status of the item
    * @since Series 60 3.1
    * @param None
    * @return Status ( ETrue if item already in db )
    */
    IMPORT_C TBool Status() const;

    /**
    * Compares items
    * @since Series 60 3.1
    * @param aFirst, first item
    * @param aSecond, second item
    * @return Comparison result
    */
    IMPORT_C static TInt CompareItemsByName( const CCmSqlPropertyItem& aFirst,
                               const CCmSqlPropertyItem& aSecond );

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
    CCmSqlPropertyItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();


private: // data

    // Database id
    TInt64 iId;

    // Property value
    HBufC8* iName; // owned

    // Status value
    TBool iStatus;


    };


#endif //  __CMSQLPROPERTYITEM_H