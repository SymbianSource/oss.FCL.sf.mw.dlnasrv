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
* Description: Implementation of CUpnpCdsLiteObject
*
*/

// INCLUDE FILES
#include "upnpcdsliteobject.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObject* CUpnpCdsLiteObject::NewL( const TDesC8 &aName,
                                              const TCdsLiteObjectType aType,
                                              const TDesC8 &aObjectId,
                                              const TDesC8 &aParentId,
                                              const TDesC8 &aObjectClass )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObject::NewL" );

    // Verify the validity of the parameters
    if( aName == KNullDesC8 ||
        aObjectId == KNullDesC8 ||
        aParentId == KNullDesC8 ||
        aObjectClass == KNullDesC8 ||
        ( aType != EContainer &&
          aType != EItem &&
          aType != EItemReference ) )
        {
        User::Leave( KErrArgument );
        }

    CUpnpCdsLiteObject* self = new ( ELeave ) CUpnpCdsLiteObject();
    CleanupStack::PushL( self );
    self->ConstructL( aName, aType, aObjectId, aParentId, aObjectClass );
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::CUpnpCdsLiteObject
// --------------------------------------------------------------------------
//
CUpnpCdsLiteObject::CUpnpCdsLiteObject()
    {
    iType = EUnknown;
    iName = NULL;
    iObjectId = NULL;
    iParentId = NULL;
    iObjectClass = NULL;
    iOriginalItemId = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::~CUpnpCdsLiteObject
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObject::~CUpnpCdsLiteObject()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObject::\
~CUpnpCdsLiteObject" );
    delete iName;
    iName = NULL;

    delete iObjectId;
    iObjectId = NULL;

    delete iParentId;
    iParentId = NULL;

    delete iObjectClass;
    iObjectClass = NULL;

    delete iOriginalItemId;
    iOriginalItemId = NULL;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpCdsLiteObject::ConstructL( const TDesC8 &aName,
                                     const TCdsLiteObjectType aType,
                                     const TDesC8 &aObjectId,
                                     const TDesC8 &aParentId,
                                     const TDesC8 &aObjectClass )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObject::ConstructL" );

    iType = aType;
    iName = aName.AllocL();
    iObjectId = aObjectId.AllocL();
    iParentId = aParentId.AllocL();
    iObjectClass = aObjectClass.AllocL();
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::Name
// --------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpCdsLiteObject::Name()
    {
    return *iName;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::Type
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObject::TCdsLiteObjectType CUpnpCdsLiteObject::Type()
    {
    return iType;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::ObjectId
// --------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpCdsLiteObject::ObjectId()
    {
    return *iObjectId;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::ParentId
// --------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpCdsLiteObject::ParentId()
    {
    return *iParentId;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::ObjectClass
// --------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpCdsLiteObject::ObjectClass()
    {
    return *iObjectClass;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::OriginalItemIdL
// --------------------------------------------------------------------------
//
EXPORT_C TDesC8& CUpnpCdsLiteObject::OriginalItemIdL()
    {
    if( iType != EItemReference )
        {
        User::Leave( KErrNotSupported );
        }

    return *iOriginalItemId;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObject::SetOriginalItemIdL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCdsLiteObject::SetOriginalItemIdL(
                                            const TDesC8 &aOriginalItemId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObject::\
SetOriginalItemIdL" );

    // Check the parameter
    if( aOriginalItemId == KNullDesC8 )
        {
        User::Leave( KErrArgument );
        }

    // Delete the previously set value (if set)
    if( iOriginalItemId )
        {
        delete iOriginalItemId;
        iOriginalItemId = NULL;
        }

    iOriginalItemId = aOriginalItemId.AllocL();
    }

// End of file
