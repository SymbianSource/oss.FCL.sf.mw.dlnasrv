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
* Description:  Implementation of the class CUpnpSharingAlgorithmImpl.
*
*/

// INCLUDE FILES, SYSTEM
#include <upnpdlnaprofiler.h>
#include <upnpitem.h>
#include "upnpcommonutils.h"
#include "upnpmetadatafetcher.h"
#include "upnpfileutility.h"
#include <escapeutils.h>            // ConvertFromUnicodeToUtf8L
#include <bautils.h>                // File existence check
#include "upnpconstantdefs.h"
#include <mm/mmcleanup.h>           // for array cleanup

// INCLUDE FILES, USER
#include "upnpcontainerresolver.h"
#include "upnpsharingao.h"
#include "upnpcdsreaderao.h"
#include "upnpsharingalgorithmimpl.h"
#include "upnpsharingalgorithmconstants.h"
#include "upnplog.h"
#include "upnpcdsliteobject.h"
#include "upnpcdsliteobjectarray.h"

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithm::NewL
// --------------------------------------------------------------------------
//
CUpnpSharingAlgorithmImpl* CUpnpSharingAlgorithmImpl::NewL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::NewL" );

    CUpnpSharingAlgorithmImpl* self = CUpnpSharingAlgorithmImpl::NewLC();
    CleanupStack::Pop( self );

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::NewLC
// Two-phased constructor
// --------------------------------------------------------------------------
//
CUpnpSharingAlgorithmImpl* CUpnpSharingAlgorithmImpl::NewLC()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::NewLC" );

    CUpnpSharingAlgorithmImpl* self =
        new ( ELeave ) CUpnpSharingAlgorithmImpl();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::~CUpnpSharingAlgorithmImpl
// --------------------------------------------------------------------------
//
CUpnpSharingAlgorithmImpl::~CUpnpSharingAlgorithmImpl()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::\
~CUpnpSharingAlgorithmImpl" );

    delete iContainerResolver;
    delete iSharingAO;
    delete iSharedContentArray;

    iMediaServer.Close();
    }

// --------------------------------------------------------------------------
// CCUpnpSharingAlgorithmImpl::ConstructL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::ConstructL()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::\
ConstructL" );

    // connect to server
    User::LeaveIfError( iMediaServer.Connect() );
    // Start server if not yet started
    StartServer();

    // An array to list all the shared files on the CDS
    iSharedContentArray = CUpnpCdsLiteObjectArray::NewL();
    // container structure resolver
    // contentarray is always up-to-date
    iContainerResolver =
        CUpnpContainerResolver::NewL( *iSharedContentArray );

    CUpnpCdsReaderAO* cdsReader =
        CUpnpCdsReaderAO::NewL( *iSharedContentArray );
    CleanupStack::PushL( cdsReader );
    cdsReader->ReadCdsStructureL();
    CleanupStack::PopAndDestroy( cdsReader );

    // Active object for sharing and unsharing the items
    iSharingAO = CUpnpSharingAO::NewL();
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::ShareFileL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::ShareFileL(
                                        const TFileName & aFileName )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
    ShareFileL" );

    // Check that file exists. Leaves with KErrArgument if not exist.
    CheckFileExistenceL( aFileName );
    
    // Check that file is not protected. Leaves with KAccessDenied if yes.
    CheckFileProtectionL( aFileName );

    if ( !IsFileSharedL( aFileName ) )
        {
        // Start server if not yet started
        StartServer();

        // create upnpitem...
        CUpnpItem* upnpItem =
            UPnPMetadataFetcher::CreateItemFromFileLC( aFileName );

        // Resolve container structure
        RPointerArray<CUpnpContainer> containerArray =
                 iContainerResolver->ResolveContainerStructureL( *upnpItem );
        CleanupResetAndDestroyPushL( containerArray );

        if ( containerArray.Count() )
            {
            // Share containers
            ShareContainersL( containerArray );

            // parent container id for item is the last shared container id
            TPtrC8 itemParentContainerId =
                    containerArray[ containerArray.Count()-1 ]->Id();
            // ... and share it
            iSharingAO->ShareFileL( itemParentContainerId, upnpItem );

            // Add the file to the shared content array
            iSharedContentArray->AppendL( upnpItem );
            }
        else
            {
            // All containers exists already.
            // Resolver put parentid to item
            iSharingAO->ShareFileL( upnpItem->ParentId(), upnpItem );

            // Add the file to the shared content array
            iSharedContentArray->AppendL( upnpItem );
            }

        // SHARE reference containers and items
        ShareReferenceObjectsL( upnpItem );

        // Cleanup
        CleanupStack::PopAndDestroy( &containerArray );
        CleanupStack::PopAndDestroy( upnpItem );
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::UnshareFileL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::UnshareFileL(
                                        const TFileName& aFileName )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::UnshareFileL" );

    // Convert the filename to 8bit
    HBufC8* filename = EscapeUtils::ConvertFromUnicodeToUtf8L( aFileName );
    CleanupStack::PushL( filename );

    // Search the array for the item
    TInt arrayindex = iSharedContentArray->FindByName( *filename );

    // Continue only if the file was found
    if ( arrayindex >= 0 )
        {
        // Start server if not yet started
        StartServer();

        // Get the object reference
        CUpnpCdsLiteObject& object =
            iSharedContentArray->ObjectAtL( arrayindex );

        // unshare references first
        UnshareReferenceObjectsL( object );

        // Checks the container tree and unshares if needed
        // First container id to be checked is item's parentid
        TBool containerUnshared =
                    UnshareEmptyContainersL( object.ParentId() );

        // if container is not unshared the item needs to be unshared
        if ( !containerUnshared )
            {
            // Leave if ObjectId not found
            if( object.ObjectId() == KNullDesC8 )
                {
                __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
                UnshareFileL, ObjectId not found!" );
                User::Leave( KErrCorrupt );
                }

            TInt objectIdInt( KErrNotFound );
            User::LeaveIfError(
             objectIdInt = UPnPCommonUtils::DesC8ToInt( object.ObjectId() ));

            // Unshare the item
            iSharingAO->UnshareFileL( objectIdInt );

            // index must be retrieved again because reference item removal
            // changes indexes
            arrayindex =
                iSharedContentArray->FindByObjectId(object.ObjectId());
            // Remove the file from the shared content array
            iSharedContentArray->RemoveObjectAtL( arrayindex );
            }
        else
            {
            // index must be retrieved again because removal of containers
            // change indexes
            arrayindex = iSharedContentArray->FindByName( *filename );
            // item was also unshared when container was unshared
            // it's enough to remove item from contentarray
            iSharedContentArray->RemoveObjectAtL( arrayindex );
            }
        }
    // Cleanup
    CleanupStack::PopAndDestroy( filename );
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::IsFileSharedL
// --------------------------------------------------------------------------
//
TBool CUpnpSharingAlgorithmImpl::IsFileSharedL(
                                            const TFileName& aFileName )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::\
    IsFileSharedL" );

    TBool returnValue = EFalse;

    // Convert the filename to 8bit
    HBufC8* tempFilename =
        EscapeUtils::ConvertFromUnicodeToUtf8L( aFileName );
    CleanupStack::PushL( tempFilename );

    // Search the file from the array
    TInt index = iSharedContentArray->FindByName( *tempFilename );
    if( index >= 0 )
        {
        returnValue = ETrue;
        }

    // Clean up
    CleanupStack::PopAndDestroy( tempFilename );

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::CheckFileExistenceL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::CheckFileExistenceL(
                                    const TFileName& aFileName )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::\
CheckFileExistenceL" );

    TBool fileExists( EFalse );

    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); // connect session
    CleanupClosePushL( fsSession );

    // check that file exists
    fileExists = BaflUtils::FileExists( fsSession, aFileName );

    CleanupStack::PopAndDestroy( &fsSession );

    if ( !fileExists )
        {
        User::Leave( KErrArgument );
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::UnshareEmptyContainersL
// --------------------------------------------------------------------------
//
TBool CUpnpSharingAlgorithmImpl::UnshareEmptyContainersL( TDesC8& aParentId )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithmImpl::\
UnshareEmptyContainersL" );

    TBool containerUnshared( EFalse );

    // get top container id that does not contain item
    // starting from container where item is.
    RPointerArray<CUpnpCdsLiteObject> containerList =
        iContainerResolver->ResolveEmptyContainersL( aParentId );
    CleanupClosePushL( containerList );

    TInt containerCount = containerList.Count();
    if ( containerCount )
        {
        // Get top containerId which needs to be unshared.
        TPtrC8 containerIdStr = containerList[containerCount-1]->ObjectId();
        TInt objectIdInt( KErrNotFound );
        User::LeaveIfError(
              objectIdInt = UPnPCommonUtils::DesC8ToInt( containerIdStr ));
        // unshares all objecs under this container with single call
        TRAPD( error, iSharingAO->UnshareContainerL( objectIdInt ));
        if ( !error )
            {
            for ( TInt i = 0; i < containerCount; i++ )
                {
                // get container's array index
                TInt arrayIndex =
                    iSharedContentArray->FindByObjectId(
                                              containerList[i]->ObjectId() );

                // Remove the container from the shared content array
                iSharedContentArray->RemoveObjectAtL( arrayIndex );
                }
            }
        containerUnshared = ETrue;
        }

    CleanupStack::PopAndDestroy( &containerList );

    return containerUnshared;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::ShareContainersL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::ShareContainersL(
                    RPointerArray<CUpnpContainer>& aContainerArray )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
ShareContainersL" );

    TPtrC8 previousContainerId( KNullDesC8 );

    // Share containers
    for ( TInt i = 0; i < aContainerArray.Count(); i++ )
        {
        CUpnpContainer* container = aContainerArray[i];

        // first container ( index = 0 ) have parent id ready
        if ( container->ParentId() == KNullDesC8 )
            {
            // parentid is previously shared container id
            container->SetParentIdL( previousContainerId );
            }

        TInt error( KErrNone );
        TRAP( error, iSharingAO->ShareContainerL( container ));
        if ( error )
            {
            // remove shared objects from server because of error
            RemoveSharedObjectsL( aContainerArray );
            User::LeaveIfError( error );
            }
        // container got id on sharing - keep it safe for next container
        // because it it used as parent container id
        previousContainerId.Set( container->Id());

        // no errors - append containers to cdsliteobjectarray
        TRAP( error, iSharedContentArray->AppendL( container ));
        if ( error )
            {
            // remove shared objects from array because of error
            RemoveSharedObjectsL( aContainerArray );
            User::LeaveIfError( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::ShareReferenceObjectsL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::ShareReferenceObjectsL( CUpnpItem* aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
ShareReferenceObjectsL" );

    // Get cds object from array. Reference item ids will be
    // added to this item
    TInt index = iSharedContentArray->FindByObjectId( aItem->Id() );
    CUpnpCdsLiteObject& object = iSharedContentArray->ObjectAtL( index );

    // reset id because a new one will be given when reference is shared
    aItem->SetIdL( KNullDesC8 );
    // reset refid field because item is reference
    aItem->SetRefIdL( KNullDesC8 );
    // reset title
    aItem->SetTitleL( KNullDesC8 );
    // reset parentId
    aItem->SetParentIdL( KNullDesC8 );

    if ( aItem->ObjectClass() == KClassAudioMusicTrack )
        {
        // Resolve genre containers
        RPointerArray<CUpnpContainer> refContainers =
                 iContainerResolver->ResolveReferenceContainersL( EGenreType,
                                                                  *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() ||
             aItem->ParentId() != KNullDesC8 )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );

        // reset parentId
        aItem->SetParentIdL( KNullDesC8 );
        // Resolve album containers
        refContainers =
               iContainerResolver->ResolveReferenceContainersL( EAlbumType,
                                                                *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() ||
             aItem->ParentId() != KNullDesC8 )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );

        // reset parentId
        aItem->SetParentIdL( KNullDesC8 );
        // Resolve artist containers
        refContainers =
               iContainerResolver->ResolveReferenceContainersL( EArtistType,
                                                                *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() ||
             aItem->ParentId() != KNullDesC8 )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );

        // reset parentId
        aItem->SetParentIdL( KNullDesC8 );
        // Resolve artist containers
        refContainers =
           iContainerResolver->ResolveReferenceContainersL( EMusicByDateType,
                                                            *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() ||
             aItem->ParentId() != KNullDesC8 )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );
    }


    else if ( aItem->ObjectClass() == KClassImage )
        {
        // reset parentId
        aItem->SetParentIdL( KNullDesC8 );
        // resolve date container
        RPointerArray<CUpnpContainer> refContainers =
           iContainerResolver->ResolveReferenceContainersL( EImageByDateType,
                                                            *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() ||
             aItem->ParentId() != KNullDesC8 )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );
        }

    else if ( aItem->ObjectClass() == KClassVideo )
        {
        // reset parentId
        aItem->SetParentIdL( KNullDesC8 );
        // resolve date container
        RPointerArray<CUpnpContainer> refContainers =
           iContainerResolver->ResolveReferenceContainersL( EVideoByDateType,
                                                            *aItem );
        CleanupResetAndDestroyPushL( refContainers );
        if ( refContainers.Count() )
            {
            // share container(s) and item
            ShareReferenceObjectL( refContainers, object, *aItem );
            }
        CleanupStack::PopAndDestroy( &refContainers );
        }
    else
        {
        __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
    ShareReferenceObjectsL object class not defined" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::UnshareReferenceObjectsL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::UnshareReferenceObjectsL(
                                        CUpnpCdsLiteObject& aOriginalObject )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
UnshareReferenceObjectsL" );

    TInt refIndex( KErrNotFound );

    do
        {
        // find reference items created from original item
        refIndex = iSharedContentArray->FindRefItemIdByOriginalIdL(
                                                aOriginalObject.ObjectId() );

        if ( refIndex != KErrNotFound )
            {
            CUpnpCdsLiteObject& refObject =
                                  iSharedContentArray->ObjectAtL( refIndex );

            // Checks the container tree and unshares if needed
            // First container id to be checked is item's parentid
            TBool containerUnshared =
                        UnshareEmptyContainersL( refObject.ParentId() );

            // if container is not unshared the item needs to be unshared
            if ( !containerUnshared )
                {
                // Leave if ObjectId not found
                if( refObject.ObjectId() == KNullDesC8 )
                    {
                    __LOG( "[CUpnpSharingAlgorithm]\t \
CUpnpSharingAlgorithm::UnshareFileL, ObjectId not found!" );
                    User::Leave( KErrCorrupt );
                    }

                TInt objectIdInt( KErrNotFound );
                User::LeaveIfError( objectIdInt =
                    UPnPCommonUtils::DesC8ToInt( refObject.ObjectId() ));

                // Unshare the item
                iSharingAO->UnshareFileL( objectIdInt );

                // Remove the file from the shared content array
                iSharedContentArray->RemoveObjectAtL( refIndex );
                }
            else
                {
                // index has changed because container(s) was unshared
                refIndex = iSharedContentArray->FindByObjectId(
                                                     refObject.ObjectId() );
                // item was also unshared when container was unshared
                // it's enough to remove item from contentarray
                iSharedContentArray->RemoveObjectAtL( refIndex );
                }
            }
        } while ( refIndex != KErrNotFound );
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::ShareReferenceObjectL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::ShareReferenceObjectL(
                            RPointerArray<CUpnpContainer>& aContainerArray,
                            CUpnpCdsLiteObject& aOriginalObject,
                            CUpnpItem& aItem )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
ShareReferenceObjectL" );

    TPtrC8 refParentId( KNullDesC8 );
    TInt parentContainerId( KErrNotFound );

    TInt count = aContainerArray.Count();

    if ( count )
        {
        // reference container must be non-restricted
        aContainerArray[count-1]->SetRestricted(EFalse);
        // share containers
        ShareContainersL( aContainerArray );

        refParentId.Set( aContainerArray[count-1]->Id() );
        User::LeaveIfError(
            parentContainerId = UPnPCommonUtils::DesC8ToInt( refParentId ));
        }
    else // share just item
        {
        refParentId.Set( aItem.ParentId() );
        User::LeaveIfError(
            parentContainerId = UPnPCommonUtils::DesC8ToInt( refParentId ));
        }

    TInt originalItemId( KErrNotFound );
    User::LeaveIfError( originalItemId = UPnPCommonUtils::DesC8ToInt(
                                               aOriginalObject.ObjectId() ));

    iSharingAO->ShareReferenceL( parentContainerId, originalItemId, &aItem );
    // Add the file to the shared content array
    iSharedContentArray->AppendL( &aItem );

    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::RemoveSharedObjectsL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::RemoveSharedObjectsL(
                            RPointerArray<CUpnpContainer>& aContainerArray )
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
RemoveSharedObjectsL" );

    // Unshare starting from first container
    TPtrC8 containerIdStr = aContainerArray[0]->Id();
    TInt objectIdInt( KErrNotFound );
    User::LeaveIfError(
            objectIdInt= UPnPCommonUtils::DesC8ToInt( containerIdStr ));
    // unshares all objecs under this container with single call
    iSharingAO->UnshareContainerL( objectIdInt );

    for ( TInt i = 0; i < aContainerArray.Count(); i++)
        {
        // Remove object from the shared content array
        iSharedContentArray->RemoveL( aContainerArray[i] );
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::ReadSharedContent
// --------------------------------------------------------------------------
//
CUpnpCdsLiteObjectArray& CUpnpSharingAlgorithmImpl::ReadSharedContent()
    {
    return *iSharedContentArray;
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::StartServer
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::StartServer()
    {
    __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
StartServer" );

    // start offline if not started
    TInt status( RUpnpMediaServerClient::EStopped );
    iMediaServer.Status( status );
    if ( status == RUpnpMediaServerClient::EStopped )
        {
        iMediaServer.StartOffline();
        __LOG( "[CUpnpSharingAlgorithm]\t CUpnpSharingAlgorithm::\
StartServer started offline" );
        }
    }

// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::Close
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::Close()
    {
    delete this;
    }
    
// --------------------------------------------------------------------------
// CUpnpSharingAlgorithmImpl::CheckFileProtectionL
// --------------------------------------------------------------------------
//
void CUpnpSharingAlgorithmImpl::CheckFileProtectionL(
                            const TFileName& aFileName )
    {
    if ( UPnPFileUtility::IsFileProtectedL( aFileName ) )
        {
        User::Leave( KErrAccessDenied );
        }
    }
    
// End of file
