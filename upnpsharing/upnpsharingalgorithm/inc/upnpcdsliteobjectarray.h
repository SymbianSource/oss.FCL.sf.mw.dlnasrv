/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Array to store the CUpnpCdsLiteObject objects
*
*/

#ifndef C_UPNP_CDS_LITE_OBJECT_ARRAY_H
#define C_UPNP_CDS_LITE_OBJECT_ARRAY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpCdsLiteObject;
class CUpnpObject;

/**
 * Defines the CUpnpCdsLiteObjectArray class. Used to store container/item
 * structure of the CDS.
 *
 * @lib upnpsharingalgorithm.lib
 * @since S60 5.2
 */
class CUpnpCdsLiteObjectArray : public CBase
    {

    public: // Constructors and destructors

        /**
         * Two-phased constructor. Leaves in case of out-of-memory.
         *
         * @since S60 5.2
         * @return array of cdsliteobjects
         */
        IMPORT_C static CUpnpCdsLiteObjectArray* NewL();

        /**
         * Destructor.
         *
         * @since S60 5.2
         */
        IMPORT_C virtual ~CUpnpCdsLiteObjectArray();

    public: // Business logic methods

        /**
        * Adds one object to the array. Ownership of the object is transfered.
        * Leaves with KErrArgument if the given object is not valid.
        *
        * @since S60 5.2
        * @param aObject, the object
        */
        IMPORT_C void AppendL( CUpnpCdsLiteObject *aObject );

        /**
        * Adds one object to the array. Leaves with KErrArgument if the given
        * object is not valid.
        *
        * @since S60 5.2
        * @param aObject, the UPnPObject object
        */
        IMPORT_C void AppendL( const CUpnpObject *aObject );

        /**
        * Finds a CUpnpCdsLiteObject object from the array. Returns
        * KErrNotFound if object not found.
        *
        * @since S60 5.2
        * @param aName, title of a container / filename
        * @return index of the array where the object resides
        */
        IMPORT_C TInt FindByName( TDesC8& aName );

        /**
        * Finds a CUpnpCdsLiteObject object with defined parent id
        * from the array. Returns KErrNotFound if object not found.
        *
        * @since S60 5.2
        * @param aName, title of a container / filename
        * @param aParentId, parent id of a container / filename
        * @return index of the array where the object resides
        */
        IMPORT_C TInt FindByNameAndParentId( const TDesC8& aName,
                                    const TDesC8& aParentId );

        /**
        * Finds a CUpnpCdsLiteObject object that have defined media
        * class and parent id. Returns KErrNotFound if object not found.
        *
        * @since S60 5.2
        * @param aMediaClass, class type of a container / filename
        * @param aParentId, parent id of a container / filename
        * @return index of the array where the object resides
        */
        IMPORT_C TInt FindByMediaClassAndParentId( const TDesC8& aMediaClass,
                                          const TDesC8& aParentId );

        /**
        * Finds a CUpnpCdsLiteObject object from the array. Returns
        * KErrNotFound if object not found.
        *
        * @since S60 5.2
        * @param aObjectId, the object ID
        * @return index of the array where the object resides
        */
        IMPORT_C TInt FindByObjectId( const TDesC8& aObjectId );

        /**
        * Finds a CUpnpCdsLiteObject object index from the array using
        * original item's id as a search parameter. Returns KErrNotFound
        * if object not found.
        * Can leave with symbian error codes.
        *
        * @since S60 5.2
        * @param aOriginalId, original item id
        * @return index of the array where the object resides
        */
        IMPORT_C TInt FindRefItemIdByOriginalIdL(
                                        const TDesC8& aOriginalId );

        /**
        * Returns the child count of a container
        *
        * @since S60 5.2
        * @param aParentId, parent object id
        * @return child count
        */
        IMPORT_C TInt ChildCount( const TDesC8& aParentId );

        /**
        * Returns the object count of the array
        *
        * @since S60 5.2
        * @return item count
        */
        IMPORT_C TInt Count( );

        /**
        * Returns an object from the array. Does not remove the object from
        * the array. Leaves with KErrArgument if the given index is out of
        * bounds.
        *
        * @since S60 5.2
        * @param aIndex, index of the array
        * @return the object
        */
        IMPORT_C CUpnpCdsLiteObject& ObjectAtL( TUint aIndex );

        /**
        * Removes an object from the array. Leaves with KErrArgument if the
        * given index is out of bounds.
        *
        * @since S60 5.2
        * @param aIndex, index of the array
        */
        IMPORT_C void RemoveObjectAtL( TUint aIndex );

        /**
        * Searches the array looking for the matching item, if one is found
        * it is removed from the array. Leaves with KErrArgument if the given
        * object is not valid.
        *
        * @since S60 5.2
        * @param aObject, the UPnPObject object
        */
        IMPORT_C void RemoveL( const CUpnpObject *aObject );

    private: // Private business logic methods

        /**
        * Creates an CUpnpCdsLiteObject. Leaves with KErrArgument if the
        * given object is not valid.
        *
        * @since S60 5.2
        * @param aObject, the UPnPObject object
        * @return a new CUpnpCdsLiteObject
        */
        CUpnpCdsLiteObject* CreateUpnpCdsLiteObjectL(
                                        const CUpnpObject *aObject );

        /**
        * Resolves the filename information from the given UpnpObject. Leaves
        * with KErrArgument if the given object is not valid.
        * Allocates memory for the returned descriptor. Does not handle the
        * freeing of the allocation.
        *
        * @since S60 5.2
        * @param aObject, the UPnPObject object
        * @return the filename (including path)
        */
        HBufC8* ResolveFileNameL( const CUpnpObject *aObject );

    private: // Constructors

        /**
         * Default c++ constructor
         *
         * @since S60 5.2
         */
        CUpnpCdsLiteObjectArray() {};

        /**
         * Second-phase constructor.
         *
         * @since S60 5.2
         */
        void ConstructL() {};

    private: // Data members

        /**
         * The array of objects. Owned.
         */
        RPointerArray<CUpnpCdsLiteObject>   iArray;

    };

#endif /* C_UPNP_CDS_LITE_OBJECT_ARRAY_H */
//  End of File
