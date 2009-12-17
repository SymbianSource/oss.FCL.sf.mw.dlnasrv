/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating property item objects
*
*/






#ifndef __CMSQLPROPERTYCONTAINER_H
#define __CMSQLPROPERTYCONTAINER_H

// INCLUDES
#include <e32base.h>
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmSqlPropertyItem;

/**
 *  CCmSqlPropertyContainer class
 *  Capsulating property item objects
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmSqlPropertyContainer : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmSqlPropertyContainer class.
     * @param None
     * @return  pointer to CCmSqlPropertyContainer class
     */
    IMPORT_C static CCmSqlPropertyContainer* NewL();

    /**
     * Creates new CCmSqlPropertyContainer class and
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmSqlPropertyContainer class
     */
    IMPORT_C static CCmSqlPropertyContainer* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmSqlPropertyContainer();

public:

    /**
    *
    * @since Series 60 3.1
    * @param CCmSqlPropertyItem*
    */
    IMPORT_C void AddPropertyItemL( CCmSqlPropertyItem* aItem );

    /**
    *
    * @since Series 60 3.1
    * @param aIndex
    */
    IMPORT_C void DeletePropertyItem( TInt aIndex );

    /**
    *
    * @since Series 60 3.1
    * @param aIndex
    * @return CCmSqlPropertyItem*
    */
    IMPORT_C CCmSqlPropertyItem* PropertyItem( TInt aIndex );

    /**
    *
    * @since Series 60 3.1
    * @return TInt
    */
    IMPORT_C TInt PropertyItemCount() const;

    /**
    *
    * @since Series 60 3.1
    * @return TInt
    */
    IMPORT_C void SetType( TCmMetadataField aType );

    /**
    *
    * @since Series 60 3.1
    * @return TCmMetadataField
    */
    IMPORT_C TCmMetadataField Type( );

    /**
    *
    * @since Series 60 3.1
    * @param aItem, item to be checked
    * @return ETrue if duplicate
    */
    IMPORT_C TBool IsDuplicate( CCmSqlPropertyItem& aItem );

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


    /**
     * Sort item by alphabet
     * @since S60 5.1
     *
     * @param None
     * @return none
     */
    IMPORT_C void SortPropertyItem( );

protected:

    /**
     * Constructor.
     */
    CCmSqlPropertyContainer();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * The function which determines the order of two class T type objects
     *
     * @since S60 5.1
     * @param aItemOne, first item
     * @param aItemTwo, second item
     * @return TInt, match value
     */
    static TInt CompareItem( const CCmSqlPropertyItem& aItemOne,
                                const CCmSqlPropertyItem& aItemTwo );
                                
private: // data

    RPointerArray<CCmSqlPropertyItem> iPropertyItems; // items owned
    TCmMetadataField                  iType;
    };


#endif //  __CMSQLPROPERTYCONTAINER_H