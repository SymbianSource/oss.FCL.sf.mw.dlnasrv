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
* Description: Implementation of CUpnpCdsLiteObjectArray
*
*/

// INCLUDE FILES, SYSTEM
#include <f32file.h>        // TParse
#include <e32def.h>         // const_cast
#include <escapeutils.h>    // ConvertFromUnicodeToUtf8L
#include <upnpobject.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include "upnpconstantdefs.h"

// INCLUDE FILES, USER
#include "upnpcdsliteobject.h"
#include "upnpcdsliteobjectarray.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"

// CONSTANTS
_LIT8( KSpace, " ");

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObjectArray* CUpnpCdsLiteObjectArray::NewL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::NewL" );

    CUpnpCdsLiteObjectArray* self = new ( ELeave ) CUpnpCdsLiteObjectArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::~CUpnpCdsLiteObjectArray
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObjectArray::~CUpnpCdsLiteObjectArray()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
~CUpnpCdsLiteObjectArray" );

    // Empty and close the array
    iArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::AppendL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCdsLiteObjectArray::AppendL( CUpnpCdsLiteObject *aObject )
    {
    // Validate the parameter
    if( !aObject ||
        aObject->Name() == KNullDesC8 ||
        aObject->ObjectId() == KNullDesC8 ||
        aObject->ParentId() == KNullDesC8 ||
        aObject->ObjectClass() == KNullDesC8 ||
        ( aObject->Type() != CUpnpCdsLiteObject::EContainer &&
          aObject->Type() != CUpnpCdsLiteObject::EItem &&
          aObject->Type() != CUpnpCdsLiteObject::EItemReference ) ||
        ( aObject->Type() == CUpnpCdsLiteObject::EItemReference &&
          aObject->OriginalItemIdL() == KNullDesC8 ) )
        {
        User::Leave( KErrArgument );
        }

    // Append the item into the array. Leave if there is a problem.
    TInt appendError = iArray.Append( aObject );
    if( appendError != KErrNone )
        {
        User::Leave( appendError );
        }
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::FindByName
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::FindByName( TDesC8& aName )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::FindByName" );
    TInt returnvalue = KErrNotFound;

    // Continue only if the given parameter is valid
    if( aName != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, exit the loop
            if( aName.Match( iArray[index]->Name() ) >= 0 )
                {
                returnvalue = index;
                index = iArray.Count() + 1;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::FindByNameAndParentId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::FindByNameAndParentId(
                                                   const TDesC8& aName,
                                                   const TDesC8& aParentId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
FindByNameAndParentId" );

    TInt returnvalue = KErrNotFound;

    // Continue only if the given parameter is valid
    if( aName != KNullDesC8 && aParentId != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, exit the loop
            if( aName.Match( iArray[index]->Name() ) >= 0 &&
                aParentId.Match( iArray[index]->ParentId() ) >= 0 )
                {
                returnvalue = index;
                index = iArray.Count() + 1;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::FindByMediaClassAndParentId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::FindByMediaClassAndParentId(
                                                  const TDesC8& aMediaClass,
                                                  const TDesC8& aParentId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
FindByMediaClassAndParentId" );

    TInt returnvalue = KErrNotFound;

    // Continue only if the given parameter is valid
    if( aMediaClass != KNullDesC8 &&
        aParentId != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, exit the loop
            if( aMediaClass.Match( iArray[index]->ObjectClass() ) >= 0 &&
                aParentId.Match( iArray[index]->ParentId() ) >= 0 )
                {
                returnvalue = index;
                index = iArray.Count() + 1;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::FindByObjectId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::FindByObjectId(
                                                 const TDesC8& aObjectId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
FindByObjectId" );

    TInt returnvalue = KErrNotFound;

    // Continue only if the given parameter is valid
    if( aObjectId != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, exit the loop
            if( aObjectId.Match( iArray[index]->ObjectId() ) >= 0 )
                {
                returnvalue = index;
                index = iArray.Count() + 1;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::FindRefItemIdByOriginalIdL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::FindRefItemIdByOriginalIdL(
                                                 const TDesC8& aOriginalId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
FindRefItemIdByOriginalIdL" );

    TInt returnvalue = KErrNotFound;

    // Continue only if the given parameter is valid
    if( aOriginalId != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, exit the loop
            if( iArray[index]->Type() ==
                    CUpnpCdsLiteObject::EItemReference &&
                aOriginalId.Match( iArray[index]->OriginalItemIdL() ) >= 0 )
                {
                returnvalue = index;
                index = iArray.Count() + 1;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::ChildCount
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::ChildCount( const TDesC8& aObjectId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::ChildCount" );

    TInt returnvalue ( 0 );

    // Continue only if the given parameter is valid
    if( aObjectId != KNullDesC8 )
        {
        // Go through the array
        for( TInt index=0; index<iArray.Count(); index++ )
            {
            // if a match is found, increase child count
            if( aObjectId.Match( iArray[index]->ParentId() ) >= 0 )
                {
                returnvalue++;
                }
            }
        }
    return returnvalue;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::Count
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpCdsLiteObjectArray::Count( )
    {
    return iArray.Count();
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::ObjectAtL
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpCdsLiteObject& CUpnpCdsLiteObjectArray::ObjectAtL(
                                                           TUint aIndex )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::ObjectAtL" );

    // Validate the parameter
    if( aIndex > iArray.Count() )
        {
        User::Leave( KErrArgument );
        }

    // Return a reference to the object
    CUpnpCdsLiteObject* object = iArray[aIndex];
    return *object;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::RemoveObjectAtL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCdsLiteObjectArray::RemoveObjectAtL( TUint aIndex )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
RemoveObjectAtL" );

    // Validate the parameter
    if( aIndex > iArray.Count() )
        {
        User::Leave( KErrArgument );
        }

    // Get the pointer from the array
    CUpnpCdsLiteObject* object = iArray[aIndex];

    // Remove the object and compress the array
    iArray.Remove( aIndex );
    iArray.Compress();

    // Delete the object
    delete object;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::AppendL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCdsLiteObjectArray::AppendL( const CUpnpObject *aObject )
    {
    // First, create a new CreateUpnpCdsLiteObject
    CUpnpCdsLiteObject* object = CreateUpnpCdsLiteObjectL( aObject );
    CleanupStack::PushL( object );

    // Append the item to the array
    AppendL( object );
    CleanupStack::Pop( object ); // ownership transfered
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::RemoveL
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpCdsLiteObjectArray::RemoveL( const CUpnpObject *aObject )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::RemoveL" );

    // First, create a new CreateUpnpCdsLiteObject
    CUpnpCdsLiteObject* object = CreateUpnpCdsLiteObjectL( aObject );
    CleanupStack::PushL( object );

    // Search the array and remove the object if found
    TInt index = FindByObjectId( object->ObjectId() );
    if( index >= 0 )
        {
        RemoveObjectAtL( index );
        }

    // Clean up
    CleanupStack::PopAndDestroy( object );
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::CreateUpnpCdsLiteObjectL
// --------------------------------------------------------------------------
//
CUpnpCdsLiteObject* CUpnpCdsLiteObjectArray::CreateUpnpCdsLiteObjectL(
                                                const CUpnpObject *aObject )
    {
    // Check the param
    if( !aObject )
        {
        User::Leave( KErrArgument );
        }

    CUpnpCdsLiteObject* object = NULL;

    // Object is a container
    if( aObject->ObjectClass().Find( KContainerClass() ) >= 0 )
        {
        // Cast to container object
        CUpnpContainer* container = (CUpnpContainer*)aObject;

        // Create the object accordingly
        object = CUpnpCdsLiteObject::NewL( container->Title(),
                                           CUpnpCdsLiteObject::EContainer,
                                           container->Id(),
                                           container->ParentId(),
                                           container->ObjectClass() );
        CleanupStack::PushL( object );
        }

    // Object is an item / reference item
    else if( aObject->ObjectClass().Find( KItemClass() ) >= 0 )
        {
        // Cast to container object
        CUpnpItem* item = (CUpnpItem*)aObject;

        HBufC8* fileName = NULL;
        // Resolve the type
        CUpnpCdsLiteObject::TCdsLiteObjectType type =
                                    CUpnpCdsLiteObject::EUnknown;
        if( item->RefId() == KNullDesC8 )
            {
            type = CUpnpCdsLiteObject::EItem;
            fileName = ResolveFileNameL( aObject );
            CleanupStack::PushL( fileName );
            }
        else
            {
            type = CUpnpCdsLiteObject::EItemReference;
            // Set space for reference filename.
            // Item validation requires that CDS lite object's name field must
            // be different that KNullDesC8.
            // Mediaserver doesn't store this field for reference item and it
            // is KNullDesC8 when structrure is read from server.
            fileName = KSpace().AllocLC();
            }

        // Create the object accordingly
        object = CUpnpCdsLiteObject::NewL( *fileName,
                                           type,
                                           item->Id(),
                                           item->ParentId(),
                                           item->ObjectClass() );

        if ( fileName )
            {
            CleanupStack::PopAndDestroy( fileName );    
            }

        CleanupStack::PushL( object );

        // Set the reference ID, if necessary
        if( type == CUpnpCdsLiteObject::EItemReference )
            {
            object->SetOriginalItemIdL( item->RefId() );
            }
        }

    // Object is neither a container nor an item, leave
    else
        {
        User::Leave( KErrArgument );
        }

    CleanupStack::Pop( object );

    return object;
    }

// --------------------------------------------------------------------------
// CUpnpCdsLiteObjectArray::ResolveFileNameL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpCdsLiteObjectArray::ResolveFileNameL(
                                        const CUpnpObject *aObject )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpCdsLiteObjectArray::\
ResolveFileNameL" );

    // Check parameter
    if( !aObject )
        {
        User::Leave( KErrArgument );
        }

    HBufC8* returnValue = NULL;

    // Get all the elements
    const RUPnPElementsArray& elements =
                    ( const_cast<CUpnpObject*>(aObject) )->GetElements();

    // Find the "res" element
    for( TInt index=0; index<elements.Count(); index++ )
        {
        if( elements[index]->Name() == KElementRes )
            {
            // Parse the filename
            TParse fp;
            fp.Set( elements[index]->FilePath(), 0, 0 );

            // Full filename (path and drive letter included)
            returnValue =
                EscapeUtils::ConvertFromUnicodeToUtf8L( fp.FullName() );

            // Exit the loop
            index = elements.Count() + 1;
            }
        }

    return returnValue;
    }

// End of file
