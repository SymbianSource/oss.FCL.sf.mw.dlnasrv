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
* Description:  Collects metadata based on user selections
*
*/


// INCLUDES
#include <e32std.h>
#include "cmsqlpropertycontainer.h"
#include "cmsqlpropertyitem.h"
#include "cmsqlpropertycollector.h"
#include "msmetadatacollector.h"
#include "msappui.h"
#include "msengine.h"
#include "msdebug.h"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CMSMetadataCollector* CMSMetadataCollector::NewL(
                                                    CMSAppUi& aMSAppUi,
                                                    TCmMediaType aMediaType,
                                                    RArray<TInt>& aGenreArray,
                                                    RArray<TInt>& aArtistArray,
                                                    RArray<TInt>& aAlbumArray,
                                                    RArray<TInt>& aTrackArray )
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::NewL"));
    
    CMSMetadataCollector* self = CMSMetadataCollector::NewLC(
                                                        aMSAppUi,
                                                        aMediaType,
                                                        aGenreArray,
                                                        aArtistArray,
                                                        aAlbumArray,
                                                        aTrackArray );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CMSMetadataCollector* CMSMetadataCollector::NewLC(
                                                    CMSAppUi& aAppUi,
                                                    TCmMediaType aMediaType,
                                                    RArray<TInt>& aGenreArray,
                                                    RArray<TInt>& aArtistArray,
                                                    RArray<TInt>& aAlbumArray,
                                                    RArray<TInt>& aTrackArray )
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::NewLC"));
    
    CMSMetadataCollector* self = new ( ELeave ) CMSMetadataCollector(
                                                        aAppUi,
                                                        aMediaType,
                                                        aGenreArray,
                                                        aArtistArray,
                                                        aAlbumArray,
                                                        aTrackArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMSMetadataCollector::~CMSMetadataCollector()
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::~CMSMetadataCollector"));
    
    delete iMetadataCollector;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMSMetadataCollector::CMSMetadataCollector( CMSAppUi& aMSAppUi,
                                            TCmMediaType aMediaType,
                                            RArray<TInt>& aGenreArray,
                                            RArray<TInt>& aArtistArray,
                                            RArray<TInt>& aAlbumArray,
                                            RArray<TInt>& aTrackArray ) :
    iMSAppUi( &aMSAppUi ),
    iMediaType( aMediaType ),
    iGenreArray( &aGenreArray ),
    iArtistArray( &aArtistArray ),
    iAlbumArray( &aAlbumArray ),
    iTrackArray( &aTrackArray )
    {
    }

// ---------------------------------------------------------------------------
// CMSMetadataCollector::ConstructL
// ---------------------------------------------------------------------------
//
void CMSMetadataCollector::ConstructL()
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::ConstructL"));
    
    iMetadataCollector = CCmSqlPropertyCollector::NewL();
    AddContainersL();
    
    GetFilteredMetadataL();
    }


// ---------------------------------------------------------------------------
//CMSMetadataCollector::GetFilteredMetadataL
// ---------------------------------------------------------------------------
//
void CMSMetadataCollector::GetFilteredMetadataL()
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::GetFilteredMetadataL"));
    
    DeleteAllItems();

    // TRACKS - this is both video titles and music tracks
    for ( TInt index = 0; index < iTrackArray->Count(); index++ )
        {
        CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewLC();

        item->SetId( (*iTrackArray)[index] );


        CCmSqlPropertyContainer* container =
                        iMetadataCollector->PropertyContainer( ETracks );
        container->AddPropertyItemL( item );
        CleanupStack::Pop( item );
        }

    if ( iMediaType == ECmAudio )
        {
        // ARTISTS
        for ( TInt index = 0; index < iArtistArray->Count(); index++ )
            {
            CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewLC();
            item->SetId( (*iArtistArray)[index] );

            CCmSqlPropertyContainer* container =
                        iMetadataCollector->PropertyContainer( EArtists );
            container->AddPropertyItemL( item );
            CleanupStack::Pop( item );
            }

        // GENRES
        for ( TInt index = 0; index < iGenreArray->Count(); index++ )
            {
            CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewLC();
            item->SetId( (*iGenreArray)[index] );

            CCmSqlPropertyContainer* container =
                        iMetadataCollector->PropertyContainer( EGenres );
            container->AddPropertyItemL( item );

            CleanupStack::Pop( item );
            }

        // ALBUMS
        for ( TInt index = 0; index < iAlbumArray->Count(); index++ )
            {
            CCmSqlPropertyItem* item = CCmSqlPropertyItem::NewLC();
            item->SetId( (*iAlbumArray)[index] );

            CCmSqlPropertyContainer* container =
                        iMetadataCollector->PropertyContainer( EAlbums );
            container->AddPropertyItemL( item );

            CleanupStack::Pop( item );
            }
        }

    // Start asynchronous metadata query.
    // Engine observer's ReadyL will be called when ready
    iMSAppUi->MSEngine()->GetFilteredMetadataL( iMetadataCollector );
    }

// --------------------------------------------------------------------------
//CMSMetadataCollector::DeleteAllItems
// --------------------------------------------------------------------------
//
void CMSMetadataCollector::DeleteAllItems()
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::DeleteAllItems"));
    
    for ( TInt index = 0;
          index < iMetadataCollector->PropertyContainerCount();
          index++ )
        {
        CCmSqlPropertyContainer* container =
                            iMetadataCollector->PropertyContainer( index );
        for ( TInt idx = 0; idx < container->PropertyItemCount(); idx++ )
            {
            container->DeletePropertyItem( idx-- );
            }
        }
    }

// --------------------------------------------------------------------------
//CMSMetadataCollector::AddContainersL
// --------------------------------------------------------------------------
//
void CMSMetadataCollector::AddContainersL()
    {
    LOG(_L("[MediaServant]\t CMSMetadataCollector::AddContainersL"));
    
    // tracks
    // this container is used both videos and music    
    CCmSqlPropertyContainer* trackContainer =
                                CCmSqlPropertyContainer::NewLC();
    trackContainer->SetType( ECmTitle );
    iMetadataCollector->AddPropertyContainerL( trackContainer );
    CleanupStack::Pop( trackContainer );

    if ( iMediaType == ECmAudio )
        {
        // artists  
        CCmSqlPropertyContainer* artistContainer =
                                CCmSqlPropertyContainer::NewLC();            
        artistContainer->SetType( ECmArtist );
        iMetadataCollector->AddPropertyContainerL( artistContainer );
        CleanupStack::Pop( artistContainer );

        // genres
        CCmSqlPropertyContainer* genreContainer =
                                CCmSqlPropertyContainer::NewLC();
        genreContainer->SetType( ECmGenre );
        iMetadataCollector->AddPropertyContainerL( genreContainer );
        CleanupStack::Pop( genreContainer );

        // albums
        CCmSqlPropertyContainer* albumContainer =
                                CCmSqlPropertyContainer::NewLC();
        albumContainer->SetType( ECmAlbum );
        iMetadataCollector->AddPropertyContainerL( albumContainer );
        CleanupStack::Pop( albumContainer );
        }
    }

// ---------------------------------------------------------------------------
//CMSMetadataCollector::Genres
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer* CMSMetadataCollector::Genres()
    {
    return iMetadataCollector->PropertyContainer( EGenres );
    }

// ---------------------------------------------------------------------------
//CMSMetadataCollector::Artists
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer* CMSMetadataCollector::Artists()
    {
    return iMetadataCollector->PropertyContainer( EArtists );
    }

// ---------------------------------------------------------------------------
//CMSMetadataCollector::Albums
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer* CMSMetadataCollector::Albums()
    {
    return iMetadataCollector->PropertyContainer( EAlbums );
    }

// ---------------------------------------------------------------------------
//CMSMetadataCollector::Tracks
// ---------------------------------------------------------------------------
//
CCmSqlPropertyContainer* CMSMetadataCollector::Tracks()
    {
    return iMetadataCollector->PropertyContainer( ETracks );
    }

// End of file

