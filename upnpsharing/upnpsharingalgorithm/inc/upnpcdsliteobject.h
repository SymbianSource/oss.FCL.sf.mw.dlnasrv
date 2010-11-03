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
* Description: Lite object for storing container/item structure
*
*/

#ifndef C_UPNP_CDS_LITE_OBJECT_H
#define C_UPNP_CDS_LITE_OBJECT_H

//  INCLUDES
#include <e32base.h>

/**
 * Defines the CUpnpCdsLiteObject class. Used to store container/item
 * structure of the CDS.
 *
 * @lib upnpsharingalgorithm.lib
 * @since S60 5.2
 */
class CUpnpCdsLiteObject : public CBase
    {

    public: // Enums

        enum TCdsLiteObjectType
            {
            EContainer = 1,
            EItem,
            EItemReference,
            EUnknown
            };

    public: // Constructors and destructors

        /**
         * Two-phased constructor. Leaves with KErrArgument if the given
         * parameters are not valid. Can also leave in case of out-of-memory.
         *
         * @since S60 5.2
         * @param aName, filename/container title of the object
         * @param aType, type of the object
         * @param aObjectId, CDS object ID of the object
         * @param aParentId, CDS object ID of the parent object
         * @param aObjectClass, object class type
         * @return CDSLite object
         */
         IMPORT_C static CUpnpCdsLiteObject* NewL( const TDesC8 &aName,
                                         const TCdsLiteObjectType aType,
                                         const TDesC8 &aObjectId,
                                         const TDesC8 &aParentId,
                                         const TDesC8 &aObjectClass );

        /**
         * Destructor.
         *
         * @since S60 5.2
         */
        IMPORT_C virtual ~CUpnpCdsLiteObject();

    public: // Business logic methods

        /**
        * Returns the name of the object. For UpnpItems and UpnpItemRefences
        * this is the filename (including filepath). For UpnpContainer this
        * it the title of the container.
        *
        * @since S60 5.2
        * @return object name
        */
        IMPORT_C TDesC8& Name();

        /**
        * Returns the type of the object
        *
        * @since S60 5.2
        * @return object type
        */
        IMPORT_C CUpnpCdsLiteObject::TCdsLiteObjectType Type();

        /**
        * Returns the CDS object ID of the object
        *
        * @since S60 5.2
        * @return object id
        */
        IMPORT_C TDesC8& ObjectId();

        /**
        * Returns the CDS object ID of the parent object
        *
        * @since S60 5.2
        * @return parent object id
        */
        IMPORT_C TDesC8& ParentId();

        /**
        * Returns the object class
        *
        * @since S60 5.2
        * @return object class
        */
        IMPORT_C TDesC8& ObjectClass();

        /**
        * Returns the CDS object ID of the original object. Leaves with
        * KErrNotSupported if the type of the object is not EItemReference.
        * Can also leave in case of out-of-memory.
        *
        * @since S60 5.2
        * @return original item id
        */
        IMPORT_C TDesC8& OriginalItemIdL();

        /**
        * Sets the CDS object ID of the original object. Leaves with
        * KErrArgument if the given parameter is not valid. Can also leave
        * in case of out-of-memory.
        *
        * @since S60 5.2
        * @param aOriginalItemId, CDS object ID of the original UpnpItem
        */
        IMPORT_C void SetOriginalItemIdL( const TDesC8 &aOriginalItemId );

    private: // Constructors

        /**
         * Default c++ constructor
         *
         * @since S60 5.2
         */
        CUpnpCdsLiteObject();

        /**
         * Second-phase constructor.
         *
         * @since S60 5.2
         * @param aName, filename/container title of the object
         * @param aType, type of the object
         * @param aObjectId, CDS object ID of the object
         * @param aParentId, CDS object ID of the parent object
         * @param aObjectClass, object class type
         */
        void ConstructL( const TDesC8 &aName,
                         const TCdsLiteObjectType aType,
                         const TDesC8 &aObjectId,
                         const TDesC8 &aParentId,
                         const TDesC8 &aObjectClass );

    private: // Data members

        /**
         * The type of the object
         */
        TCdsLiteObjectType          iType;

        /**
         * The name of the object. Owned.
         */
        HBufC8*                     iName;

        /**
         * The CDS object ID. Owned.
         */
        HBufC8*                     iObjectId;

        /**
         * The CDS object ID of the parent container. Owned.
         */
        HBufC8*                     iParentId;

        /**
         * The CDS object class type. Owned.
         */
        HBufC8*                     iObjectClass;

        /**
         * The CDS object ID of the original UpnpItem to which this
         * reference item points to. Used only if the type is EItemReference.
         * Owned.
         */
        HBufC8*                     iOriginalItemId;

    };

#endif /* C_UPNP_CDS_LITE_OBJECT_H */
//  End of File
