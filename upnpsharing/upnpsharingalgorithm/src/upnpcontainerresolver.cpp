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
* Description:  Implementation of the class CUpnpContainerResolver.
*
*/

#include "upnpconstantdefs.h"        // upnpobject class types
#include <utf.h>                     // for CnvUtfConverter
#include <bautils.h>                 // for BaflUtils
#include <pathinfo.h>                // for PathInfo
#include <upnpitem.h>                // for CUpnpItem
#include <upnpelement.h>             // for CUpnpElement
#include "upnpitemutility.h"         // for CUpnpItemUtility
#include <upnpsharingalgorithm.rsg>
#include <mm/mmcleanup.h>            // for array cleanup

#include "upnpcontainerresolver.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"
#include "upnpcdsliteobject.h"

const TInt KDateStringLength = 10;
const TInt KMaxDateStringLength = 30;
const TInt KMonthAndDayLength = 2;
const TInt KMonthAndDayDigitLimit = 10;
const TInt KSeparatorLength = 1;
const TInt KDateYearLength = 4;
const TInt KSecondSeparatorPosition = 7;

_LIT( KZero, "0" );

// Year, e.g. "2008"
_LIT( KDateTimeYear,"%Y%3" );

// Month, e.g. "October"
_LIT( KDateTimeMonth,"%N%2" );

// Day, e.g. "4th Mon"
_LIT( KDateTimeDay,"%X%*D%1 %*E" );


// --------------------------------------------------------------------------
// CUpnpContainerResolver::NewL
// --------------------------------------------------------------------------
//
CUpnpContainerResolver* CUpnpContainerResolver::NewL(
    CUpnpCdsLiteObjectArray& aCdsLiteObjectArray )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::NewL" );

    CUpnpContainerResolver* self =
        new ( ELeave ) CUpnpContainerResolver( aCdsLiteObjectArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ResolveContainerStructureL
// --------------------------------------------------------------------------
//
RPointerArray<CUpnpContainer>
    CUpnpContainerResolver::ResolveContainerStructureL( CUpnpItem& aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ResolveContainerStructureL" );

    // List of needed containers
    RPointerArray<CUpnpContainer> containers;
    CleanupResetAndDestroyPushL( containers );

    RPointerArray<HBufC8> containerNames;
    CleanupResetAndDestroyPushL( containerNames );

    HBufC8* itemParent = NULL;

    if ( aItem.ObjectClass() == KClassAudioMusicTrack )
        {
        // Fill container name array, containers must be in
        // hierarchical order staring from root
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_MUSIC_TEXT );
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_ALL_TEXT );

        // Create non-existing containers
        itemParent = CreateContainersL( containerNames,
                                        containers,
                                        KMusicClassContainer );
        CleanupStack::PushL( itemParent );
        }
    else if ( aItem.ObjectClass() == KClassImage )
        {
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_IMAGE_TEXT );
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_ALL_TEXT );

        itemParent = CreateContainersL( containerNames,
                                        containers,
                                        KImageClassContainer );
        CleanupStack::PushL( itemParent );
        }
    else if ( aItem.ObjectClass() == KClassVideo )
        {
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_VIDEO_TEXT );
        AppendContainerNameToArrayL( containerNames,
                                     R_UPNP_SHARE_ALL_TEXT );

        itemParent = CreateContainersL( containerNames,
                                        containers,
                                        KVideoClassContainer );
        CleanupStack::PushL( itemParent );
        }

    // If new containers are not needed, set item parent id here,
    // because caller does not know the item parent container
    if ( !containers.Count() )
        {
        aItem.SetParentIdL( *itemParent );
        }
    if ( itemParent )
        {
        CleanupStack::PopAndDestroy( itemParent );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( &containerNames );
    CleanupStack::Pop( &containers );

    return containers;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ResolveReferenceContainersL
// --------------------------------------------------------------------------
//
RPointerArray<CUpnpContainer>
    CUpnpContainerResolver::ResolveReferenceContainersL( TInt aType,
                                                         CUpnpItem& aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ResolveReferenceContainersL" );

    // List of needed containers
    RPointerArray<CUpnpContainer> containers;
    CleanupResetAndDestroyPushL( containers );

    RPointerArray<HBufC8> containerNames;
    CleanupResetAndDestroyPushL( containerNames );

    HBufC8* itemParent = NULL;
    TInt error( KErrNone );

    // Get object elements from where metadata is fetched
    const RUPnPElementsArray& elements = aItem.GetElements();

    switch( aType )
        {
        case EGenreType :
            {
            // Fill container name array, containers must be in
            // hierarchical order starting from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_MUSIC_TEXT );
            // Append "By Genre" container
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_BY_GENRE_TEXT );

            // Modify "Genre" element and append it to the array
            TRAP( error, ModifyAndAppendElementL( containerNames,
                                                  elements,
                                                  KElementGenre ));

            // If metadata is missing, containers are not created
            if ( error == KErrNone )
                {
                // Create non-existing containers
                itemParent = CreateContainersL( containerNames,
                                                containers,
                                                KMusicClassContainer );
                CleanupStack::PushL( itemParent );
                }
            break;
            }
        case EAlbumType :
            {
            // Fill container name array, containers must be in
            // hierarchical order staring from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_MUSIC_TEXT );
            // Append "By Album" container
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_BY_ALBUM_TEXT );

            // Modify "Album" element and append it to the array
            TRAP( error, ModifyAndAppendElementL( containerNames,
                                                  elements,
                                                  KElementAlbum ));

            // If metadata is missing, containers are not created
            if ( error == KErrNone )
                {
                // Create non-existing containers
                itemParent = CreateContainersL( containerNames,
                                                containers,
                                                KMusicClassContainer );
                CleanupStack::PushL( itemParent );
                }
            break;
            }
        case EArtistType :
            {
            // Fill container name array, containers must be in
            // hierarchical order staring from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_MUSIC_TEXT );
            // Append "By Artist" container
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_BY_ARTIST_TEXT );

            // Modify "Artist" element and append it to the array
            TRAP( error, ModifyAndAppendElementL( containerNames,
                                                  elements,
                                                  KElementArtist ));

            // If metadata is missing, containers are not created
            if ( error == KErrNone )
                {
                // Create non-existing containers
                itemParent = CreateContainersL( containerNames,
                                                containers,
                                                KMusicClassContainer );
                CleanupStack::PushL( itemParent );
                }
            break;
            }
        case EMusicByDateType :
            {
            // Fill container name array, containers must be in
            // hierarchical order staring from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_MUSIC_TEXT );
            // Append "By Date" container
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_BY_DATE_TEXT );

            TTime time;
            HBufC8* elementValue = GetElementL( elements, KElementDate );
            if ( elementValue )
                {
                CleanupStack::PushL( elementValue );
                HBufC8* adjustedDate = AdjustDateL( *elementValue );
                CleanupStack::PopAndDestroy( elementValue );
                CleanupStack::PushL( adjustedDate );

                error = UPnPItemUtility::UPnPDateAsTTime( *adjustedDate,
                                                          time );

                CleanupStack::PopAndDestroy( adjustedDate );

                if ( error == KErrNone &&
                     adjustedDate->Length() > 0 )
                    {
                    TBuf<KMaxDateStringLength> finalTime;

                    // Append Year container
                    time.FormatL( finalTime, KDateTimeYear );
                    HBufC8* timeStr =
                        CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                        finalTime );
                    CleanupStack::PushL( timeStr );
                    containerNames.Append( timeStr );
                    CleanupStack::Pop( timeStr );

                    // Create non-existing containers
                    itemParent = CreateContainersL( containerNames,
                                                    containers,
                                                    KMusicClassContainer );
                    CleanupStack::PushL( itemParent );
                    }
                }
            else
                {
                error = KErrNotFound;
                }

            break;
            }
        case EImageByDateType :
            {
            // Fill container name array, containers must be in
            // hierarchical order staring from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_IMAGE_TEXT );

            error = AddByDateContainersL( containerNames, elements );

            // If metadata is missing, containers are not created
            if ( error == KErrNone )
                {
                // Create non-existing containers
                itemParent = CreateContainersL( containerNames,
                                                containers,
                                                KImageClassContainer );
                CleanupStack::PushL( itemParent );
                }
            break;
            }
        case EVideoByDateType :
            {
            // Fill container name array, containers must be in
            // hierarchical order staring from root
            AppendContainerNameToArrayL( containerNames,
                                         R_UPNP_SHARE_VIDEO_TEXT );

            error = AddByDateContainersL( containerNames, elements );

            // If metadata is missing, containers are not created
            if ( error == KErrNone )
                {
                // Create non-existing containers
                itemParent = CreateContainersL( containerNames,
                                                containers,
                                                KVideoClassContainer );
                CleanupStack::PushL( itemParent );
                }
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    // If new containers are not needed, set item parent id here,
    // because caller does not know the item parent container.
    // If metadata is not found, item is left untouched.
    if ( !containers.Count() &&
         error == KErrNone )
        {
        aItem.SetParentIdL( *itemParent );
        }

    if ( itemParent )
        {
        CleanupStack::PopAndDestroy( itemParent );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( &containerNames );
    CleanupStack::Pop( &containers );

    return containers;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ResolveEmptyContainersL
// --------------------------------------------------------------------------
//
RPointerArray<CUpnpCdsLiteObject>
    CUpnpContainerResolver::ResolveEmptyContainersL( TDesC8& aContainerId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ResolveEmptyContainersL" );

    RPointerArray<CUpnpCdsLiteObject> emptyContainers;
    TBool empty( ETrue );
    const TDesC8* parentId = &aContainerId;

    // Search empty containers from the array
    while ( empty &&
            parentId->Match( KRootContainer ) == KErrNotFound )
        {
        TInt count = iCdsLiteObjectArray.ChildCount( aContainerId );
        if ( iCdsLiteObjectArray.ChildCount( *parentId ) > 1 )
            {
            // There is more than one item inside container
            // Current container must not be removed, exit loop
            empty = EFalse;
            }
        else
            {
            // Get object index from CDS object array
            TInt objectIndex =
                iCdsLiteObjectArray.FindByObjectId( *parentId );

            // Append correct container from CDS object array
            emptyContainers.Append(
                &iCdsLiteObjectArray.ObjectAtL( objectIndex ) );

            // Get parent container id of the current container
            parentId =
                &iCdsLiteObjectArray.ObjectAtL( objectIndex ).ParentId();
            }
        }
    return emptyContainers;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpContainerResolver::ConstructL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ConstructL" );

    // Open file server session
    User::LeaveIfError( iFs.Connect() );

    TFileName resourceFileName( KSharingAlgorithmRscFile );

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( iFs, resourceFileName );

    TInt err( KErrNone );
    // Open resource file
    TRAP( err, iResourceFile.OpenL( iFs, resourceFileName ) );

    if ( err != KErrNone )
        {
        // If not found, try to read resource file from memory card
        resourceFileName.Copy( PathInfo::MemoryCardRootPath() );
        resourceFileName.Delete( 2, 2 ); // remove //
        resourceFileName.Append( KSharingAlgorithmRscFile );
        iResourceFile.OpenL( iFs, resourceFileName );
        }
    }

// --------------------------------------------------------------------------
// CCUpnpContainerResolver::~CUpnpContainerResolver
// --------------------------------------------------------------------------
//
CUpnpContainerResolver::~CUpnpContainerResolver()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
~CUpnpContainerResolver" );

    // Close resource file
    iResourceFile.Close();

    // Close file server session
    iFs.Close();
    }

// --------------------------------------------------------------------------
// CCUpnpContainerResolver::CreateContainerL
// --------------------------------------------------------------------------
//
CUpnpContainer* CUpnpContainerResolver::CreateContainerL(
    const TDesC8& aTitle,
    const TDesC8& aClass )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
CreateContainerL" );

    CUpnpContainer* container = CUpnpContainer::NewL();
    CleanupStack::PushL( container );

    // Set container info
    container->SetTitleL( aTitle );
    container->SetObjectClassL( aClass );

    CleanupStack::Pop( container );

    return container;
    }

// --------------------------------------------------------------------------
// CCUpnpContainerResolver::CreateContainersL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpContainerResolver::CreateContainersL(
        RPointerArray<HBufC8>& aContainerNames,
        RPointerArray<CUpnpContainer>& aContainers,
        const TDesC8& aMediaClass )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
CreateContainersL" );

    HBufC8* parentId = NULL;
    TInt index( 1 );

    // Find first media container, e.g. "Music"
    TInt objectIndex ( iCdsLiteObjectArray.FindByMediaClassAndParentId(
        aMediaClass, KRootContainer ) );

    if ( objectIndex == KErrNotFound )
        {
        // Create media root container and append it to the container array.
        // Parent container is root container ("0")
        if ( aContainerNames.Count() )
            {
            CUpnpContainer* container = CreateContainerL(
                                          *aContainerNames[0], aMediaClass );
            CleanupStack::PushL( container );
            aContainers.Append( container );
            CleanupStack::Pop( container );
            }
        parentId = KRootContainer().AllocL();
        }
    else
        {
        // Remember media root container id
        parentId = iCdsLiteObjectArray.ObjectAtL(
                            objectIndex ).ObjectId().AllocL();
        }

    // Find the first container that does not exist
    for ( ; index < aContainerNames.Count(); index++ )
        {
        // Container is found by container name and parent id.
        // If search cannot find the container, it needs to be created.
        objectIndex = iCdsLiteObjectArray.FindByNameAndParentId(
            *aContainerNames[index], *parentId );

        if ( objectIndex == KErrNotFound )
            {
            break;
            }
        else
            {
            // Remember last found container id
            parentId = iCdsLiteObjectArray.ObjectAtL(
                                    objectIndex ).ObjectId().AllocL();
            }
        }

    // Create needed containers
    for ( TInt index2( index ); index2 < aContainerNames.Count(); index2++ )
        {
        CUpnpContainer* container = CreateContainerL(
                                     *aContainerNames[index2], aMediaClass );
        CleanupStack::PushL( container );
        aContainers.Append( container );
        CleanupStack::Pop( container );
        }

    // Use the last found object id as parent id of the first new container
    if ( aContainers.Count() )
        {
        aContainers[0]->SetParentIdL( *parentId );
        }

    return parentId;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ReadResourceTextL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpContainerResolver::ReadResourceTextL( TInt aResourceId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ReadResourceTextL" );

    // Load resource text
    HBufC8* resourceString8 = iResourceFile.AllocReadLC( aResourceId );

    TResourceReader theReader;
    theReader.SetBuffer( resourceString8 );
    HBufC16* resourceString16 = theReader.ReadHBufC16L();
    CleanupStack::PushL( resourceString16 );

    HBufC8* returnString8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
        resourceString16->Des() );

    CleanupStack::PopAndDestroy( resourceString16 );
    CleanupStack::PopAndDestroy( resourceString8 );

    return returnString8;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::GetElementL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpContainerResolver::GetElementL(
        const RUPnPElementsArray& aElements,
        const TDesC8& aElement )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
GetElementL" );

    HBufC8* elementValue = NULL;

    // Find the element
    for( TInt index = 0; index < aElements.Count(); index++ )
        {
        if( aElements[index]->Name().Match( aElement ) == KErrNone )
            {
            elementValue = aElements[index]->Value().AllocL();
            index = aElements.Count();
            }
        }

    return elementValue;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::AdjustDateL
// --------------------------------------------------------------------------
//
HBufC8* CUpnpContainerResolver::AdjustDateL( const TDesC8& aDate )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
AdjustDateL" );

    HBufC* date = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aDate );
    CleanupStack::PushL( date );
    TBuf<KMaxDateStringLength> modifiedDate;

    TBuf<KMonthAndDayLength> month;
    TBuf<KMonthAndDayLength> day;

    if( aDate.Length() >= KDateStringLength )
        {
        // Month and day are decreased by 1, because
        // both are started from zero in TTime::Set()
        // Formats date e.g. 2008-10-30 --> 2008-09-29

        // Add year and separator e.g. "2008-"
        modifiedDate.Copy( date->Des().Left(
            ( KDateYearLength + KSeparatorLength ) ) );

        // Adjust month "10" --> "9"
        month.Append( date->Des().Mid(
            ( KDateYearLength + KSeparatorLength ),
            KMonthAndDayLength) );
        TLex monthLex( month );
        TInt monthInt( 0 );
        monthLex.Val( monthInt );
        monthInt--;
        TBuf<KMonthAndDayLength> adjustedMonth;
        adjustedMonth.Num( monthInt );

        // Month and day values need to contain
        // two digits. Append extra zero, if needed,
        // e.g. "9" --> "09"
        if ( monthInt < KMonthAndDayDigitLimit )
            {
            modifiedDate.Append( KZero );
            }

        // Add modified month string
        modifiedDate.Append( adjustedMonth );

        // Add separator "-" that is between month and day
        modifiedDate.Append( date->Des().Mid(
            KSecondSeparatorPosition,
            KSeparatorLength ) );

        // Adjust day e.g. "30" --> "29"
        day.Append( date->Des().Mid(
            ( KSecondSeparatorPosition + KSeparatorLength ),
            KMonthAndDayLength) );
        TLex dayLex( day );
        TInt dayInt( 0 );
        dayLex.Val( dayInt );
        dayInt--;
        TBuf<KMonthAndDayLength> adjustedDay;
        adjustedDay.Num( dayInt );

        if ( dayInt < KMonthAndDayDigitLimit )
            {
            modifiedDate.Append( KZero );
            }

        // Add modified day string
        modifiedDate.Append( adjustedDay );
        }

    CleanupStack::PopAndDestroy( date );

    HBufC8* adjustedDate =
        CnvUtfConverter::ConvertFromUnicodeToUtf8L( modifiedDate );

    return adjustedDate;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::AddByDateContainersL
// --------------------------------------------------------------------------
//
TInt CUpnpContainerResolver::AddByDateContainersL(
                                RPointerArray<HBufC8>& aContainerNames,
                                const RUPnPElementsArray& aElements )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
AddByDateContainersL" );

    TInt metadataError( KErrNone );
    TTime time;

    // Append "By Date" container
    aContainerNames.AppendL(
        ReadResourceTextL( R_UPNP_SHARE_BY_DATE_TEXT ) );

    HBufC8* elementValue = GetElementL( aElements, KElementDate );
    if ( elementValue )
        {
        CleanupStack::PushL( elementValue );
        // Get date element and adjust it to match TTime::Set() time
        HBufC8* adjustedDate = AdjustDateL( *elementValue );
        CleanupStack::PopAndDestroy( elementValue );
        CleanupStack::PushL( adjustedDate );

        if ( adjustedDate->Length() <= 0 )
            {
            metadataError = KErrNotFound;
            }

        // Convert date to TTime
        TInt adjustError ( UPnPItemUtility::UPnPDateAsTTime(
            *adjustedDate, time ) );

        CleanupStack::PopAndDestroy( adjustedDate );

        if ( adjustError == KErrNone &&
             metadataError == KErrNone )
            {
            TBuf<KMaxDateStringLength> finalTime;

            // Append Year container
            time.FormatL( finalTime, KDateTimeYear );
            aContainerNames.Append(
                CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                    finalTime ) );

            // Append Month container
            time.FormatL( finalTime, KDateTimeMonth );
            aContainerNames.Append(
                CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                    finalTime ) );

            // Append Day container
            time.FormatL( finalTime, KDateTimeDay );
            aContainerNames.Append(
                CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                    finalTime ) );
            }
        }
    else
        {
        metadataError = KErrNotFound;
        }

    return metadataError;
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::ModifyAndAppendElementL
// --------------------------------------------------------------------------
//
void CUpnpContainerResolver::ModifyAndAppendElementL(
                                RPointerArray<HBufC8>& aContainerNames,
                                const RUPnPElementsArray& aElements,
                                const TDesC8& aElementType )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
ModifyAndAppendElementL" );

    // Find correct element
    HBufC8* elementValue = GetElementL( aElements, aElementType );
    CleanupStack::PushL( elementValue );
    if ( !elementValue ||
         elementValue->Length() <= 0 )
        {
        User::Leave( KErrNotFound );
        }

    // Set all letters to lower case
    elementValue->Des().LowerCase();

    // Capitalize the first letter of the string
    elementValue->Des().Capitalize();

    // Append modified element value to container names array.
    // aContainerNames array is responsible for deleteting objects in it.
    // Transfer ownership.
    aContainerNames.AppendL( elementValue );

    CleanupStack::Pop( elementValue );
    }

// --------------------------------------------------------------------------
// CUpnpContainerResolver::AppendContainerNameToArrayL
// --------------------------------------------------------------------------
//
void CUpnpContainerResolver::AppendContainerNameToArrayL(
                                            RPointerArray<HBufC8>& aArray,
                                            TInt aResourceTextId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpContainerResolver::\
AppendContainerNameToArrayL" );

    HBufC8* name = ReadResourceTextL( aResourceTextId );
    CleanupStack::PushL( name );
    aArray.AppendL( name );
    CleanupStack::Pop( name );
    }

// End of file
