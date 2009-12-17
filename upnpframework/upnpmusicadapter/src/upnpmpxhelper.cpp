/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class for some common MPX routines
*
*/


// INCLUDES
class MMPXCollection;
#include <mpxcollectionmediator.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxmedia.h>
#include <mpxmediageneraldefs.h> // for KMPXMediaGeneralCollectionId
#include <mpxmediacontainerdefs.h> // for KMPXMediaArrayContents
#include <mpxcollectiontype.h> // for CMPXCollectionType
#include <mpxplaylistenginedefs.h> // for EMPXPlaylistTypeM3U
#include <mpxcollectionplugin.hrh> // for EMPXCollectionPluginHidden and
#include "upnpmusicadapter.h"      // EMPXCollectionPluginTemporary

// ourselves
#include "upnpmpxhelper.h" // myself

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPMpxHelper::NewL
// 1st phase constructor.
// --------------------------------------------------------------------------
//
CUPnPMpxHelper* CUPnPMpxHelper::NewL( const TUid& aModeId )
    {
    CUPnPMpxHelper* self = new(ELeave)
        CUPnPMpxHelper();
    CleanupStack::PushL( self );
    self->ConstructL( aModeId );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPMpxHelper::CUPnPMpxHelper
// Default constructor.
// --------------------------------------------------------------------------
//    
CUPnPMpxHelper::CUPnPMpxHelper()
    {
    }

// --------------------------------------------------------------------------
// CUPnPMpxHelper::ConstructL
// 2nd phase constructor
// --------------------------------------------------------------------------
//    
void CUPnPMpxHelper::ConstructL( const TUid& aModeId )
    {
    iHarvester = CMPXHarvesterFactory::NewL();
    iCollectionUtil = MMPXCollectionUtility::NewL( 0, aModeId );
        
    iMediator = CMPXCollectionMediator::NewL( 
        iCollectionUtil->Collection(), this );
    }

// --------------------------------------------------------------------------
// CUPnPMpxHelper::~CUPnPMpxHelper
// Destructor.
// --------------------------------------------------------------------------
//
CUPnPMpxHelper::~CUPnPMpxHelper()
    {
    if( iHarvester )
        {
        iHarvester->Close();
        }
    
    if( iCollectionUtil )
        {
        iCollectionUtil->Close(); 
        }
    delete iMediator;
    }


// --------------------------------------------------------------------------
// CUPnPMpxHelper::AddTrackL
// --------------------------------------------------------------------------
//
void CUPnPMpxHelper::AddTrackL( CMPXMedia* aMedia )
    {
    // Add to harvester 
    TInt colUid = iHarvester->AddFileL( aMedia );

    // Add to collection, make sure we set the collection ID
    aMedia->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, TUid::Uid( colUid ) );

    iMediator->AddItemL( aMedia );


    }

// --------------------------------------------------------------------------
// CUPnPMpxHelper::AddPlaylistL
// --------------------------------------------------------------------------
//
void CUPnPMpxHelper::AddPlaylistL( CMPXMedia* aMedia )
    {
    // leave if the given media doesn't contain the following attributes
    if ( !aMedia->IsSupported( KMPXMediaGeneralType ) ||
         !aMedia->IsSupported( KMPXMediaGeneralCategory ) )
        {
        User::Leave( KErrArgument );
        }

    // leave if the given media isn't a playlist
    TMPXGeneralType mediaType =
        *aMedia->Value<TMPXGeneralType>( KMPXMediaGeneralType );
    TMPXGeneralCategory mediaCategory =
        *aMedia->Value<TMPXGeneralCategory>( KMPXMediaGeneralCategory );
    if ( mediaType != EMPXItem || mediaCategory != EMPXPlaylist )
        {
        User::Leave( KErrNotSupported );
        }

    // leave if the media doesn't contain mandatory attributes
    if ( !aMedia->IsSupported( KMPXMediaArrayContents ) ||
         !aMedia->IsSupported( KMPXMediaArrayCount ) )
        {
        User::Leave( KErrArgument );
        }

    // check if we are adding new or appending to existing
    if (!aMedia->IsSupported(KMPXMediaGeneralId))
        {
        if (!aMedia->IsSupported(KMPXMediaGeneralTitle) ||
            !aMedia->IsSupported(KMPXMediaGeneralUri))
            {
            User::Leave( KErrArgument );
            }
        else
            {
            // adding new
            // find the collection Id
            HBufC* playlistExtension =
                iHarvester->PlaylistFileExtensionLC( EMPXPlaylistTypeM3U );
            TInt collectionId = FindCollectionIdL( *playlistExtension );                
            CleanupStack::PopAndDestroy( playlistExtension );

            // Add to collection, make sure we set the collection ID.
            aMedia->SetTObjectValueL<TUid>(
                KMPXMediaGeneralCollectionId, TUid::Uid( collectionId ));
            }
        }
    else
        {
        // appending to existing
        FillInPlaylistDetailsL( *aMedia );
        }

    iMediator->AddItemL( aMedia );
    }

// --------------------------------------------------------------------------
// CUPnPMpxHelper::OpenL
// --------------------------------------------------------------------------
//
void CUPnPMpxHelper::AddAndOpenL( const TUid& aHostId, CMPXMedia& aMedia )
    {
    // Grab the in memory collection plugin UID
    // aPluginInfo provides additional resolution to find the plugin
    //
    RArray<TUid> ary;
    CleanupClosePushL( ary );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginHidden ) );
    ary.AppendL( TUid::Uid( EMPXCollectionPluginTemporary ) );
    TUid inMemCollection = iCollectionUtil->CollectionIDL( ary.Array() );
    CleanupStack::PopAndDestroy( &ary );

    // First step is to add this media to the in memory plugin
    // Set the item id to be the host ID
    // 
    aMedia.SetTObjectValueL( KMPXMediaGeneralCollectionId, inMemCollection );
    aMedia.SetTObjectValueL( KMPXMediaGeneralId, aHostId.iUid );
    iCollectionUtil->Collection().AddL( aMedia );

    // Second Step is to construct collection path
    // | collection id | host id |
    //
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( inMemCollection.iUid );
    path->AppendL( aHostId.iUid );

    // Last step is to open this path
    //
    iCollectionUtil->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );
    }

// ---------------------------------------------------------------------------
// retrieve collection from URI
// ResolvePlugin should be able to resolve the plugin without
// the client looking for the collection Id
// ---------------------------------------------------------------------------
//    
TInt CUPnPMpxHelper::FindCollectionIdL( const TDesC& aUri )   
    {
    TInt collectionId(KErrNotFound);

    TParsePtrC parser( aUri );   
            
    RPointerArray<CMPXCollectionType> collectionType;
    iCollectionUtil->Collection().GetSupportedTypesL(collectionType);
    
    TInt index(KErrNotFound);
    TInt count( collectionType.Count() );
    
    for (TInt i = 0; i < count; i++)
        {
        const CDesCArray& extensions = collectionType[i]->Extensions();
        
        if (extensions.FindIsq(parser.Ext(), index) == 0)
            {
            collectionId = collectionType[i]->Uid().iUid;
            break;
            }
        }
        
    collectionType.ResetAndDestroy();
    
    return collectionId;
    }

// ---------------------------------------------------------------------------
// retrieve information for the required attributes
// ---------------------------------------------------------------------------
//
void CUPnPMpxHelper::FillInPlaylistDetailsL( CMPXMedia& aMedia )
    {
    // retrieve info about the playlist itself
    RArray<TMPXAttribute> playlistAttributes;
    CleanupClosePushL(playlistAttributes);
    playlistAttributes.AppendL(KMPXMediaGeneralId);
    playlistAttributes.AppendL(KMPXMediaGeneralTitle);
    playlistAttributes.AppendL(KMPXMediaGeneralUri);
    playlistAttributes.AppendL(KMPXMediaGeneralCollectionId);

    CMPXMedia* playlistSearchResult = iCollectionUtil->Collection()
        .FindAllL( aMedia, playlistAttributes.Array() );
    CleanupStack::PopAndDestroy(&playlistAttributes);
    CleanupStack::PushL(playlistSearchResult);

    const CMPXMediaArray* results =
        playlistSearchResult->Value<CMPXMediaArray>(KMPXMediaArrayContents);

    if ( results->Count() != 1 )
        {
        User::Leave(KErrArgument);
        }

    aMedia.SetTObjectValueL(
        KMPXMediaGeneralId, *(*results)[0]->Value<TMPXItemId>(KMPXMediaGeneralId));
    aMedia.SetTextValueL(
        KMPXMediaGeneralTitle, (*results)[0]->ValueText(KMPXMediaGeneralTitle));
    aMedia.SetTextValueL(
        KMPXMediaGeneralUri, (*results)[0]->ValueText(KMPXMediaGeneralUri));
    aMedia.SetTObjectValueL(
        KMPXMediaGeneralCollectionId, *(*results)[0]->Value<TUid>(KMPXMediaGeneralCollectionId));

    CleanupStack::PopAndDestroy(playlistSearchResult);        
    }


