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
* Description:  CMSMetadataCollector class definition
*
*/


#ifndef __CMSMETADATACOLLECTOR_H
#define __CMSMETADATACOLLECTOR_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSMetadataCollector;
class CCmSqlPropertyContainer;
class CCmSqlPropertyCollector;

/**
 *  CMSMetadataCollector class
 *  Collects metadata
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMetadataCollector ): public CBase
    {
    private:
        // Container types
        enum TMSContainerTypes
            {
            ETracks = 0,
            EArtists,
            EGenres,
            EAlbums            
            };

    public:
        
        /**
         * two-phase constructor
         *
         * @Since S60 5.1
         * @param aAppUi, application ui
         * @param aMediaType, mediatype
         * @param aGenreArray, genre container
         * @param aArtistArray, artist container
         * @param aAlbumArray, album container
         * @param aTrackArray, track/video title container
         * @return pointer to CMSMetadataCollector class
         */
        IMPORT_C static CMSMetadataCollector* NewL( CMSAppUi& aAppUi,
                                                    TCmMediaType aMediaType,
                                                    RArray<TInt>& aGenreArray,
                                                    RArray<TInt>& aArtistArray,
                                                    RArray<TInt>& aAlbumArray,
                                                    RArray<TInt>& aTrackArray );
    
        /**
         * two-phase constructor
         *
         * @Since S60 5.1
         * @param aAppUi, application ui
         * @param aMediaType, mediatype
         * @param aGenreArray, genre container
         * @param aArtistArray, artist container
         * @param aAlbumArray, album container
         * @param aTrackArray, track/video title container
         * @return pointer to CMSMetadataCollector class
         */
        IMPORT_C static CMSMetadataCollector* NewLC( CMSAppUi& aAppUi,
                                                     TCmMediaType aMediaType,
                                                     RArray<TInt>& aGenreArray,
                                                     RArray<TInt>& aArtistArray,
                                                     RArray<TInt>& aAlbumArray,
                                                     RArray<TInt>& aTrackArray );
        
        /**
         * Destructor.
         */
        virtual ~CMSMetadataCollector();
    
    public:
    
        /**
         * Starts collecting metadata items
         *
         * @since S60 5.1         
         * @param None
         * @return None
         */
        void GetFilteredMetadataL();
        
        /**
         * Returns genre array
         *
         * @since S60 5.1
         * @param None
         * @return CCmSqlPropertyContainer, genre container
         */
        CCmSqlPropertyContainer* Genres();

        /**
         * Returns artist array
         *
         * @since S60 5.1
         * @param None
         * @return CCmSqlPropertyContainer, artist container
         */
        CCmSqlPropertyContainer* Artists();
        
        /**
         * Returns album array
         *
         * @since S60 5.1
         * @param None
         * @return CCmSqlPropertyContainer, album container
         */
        CCmSqlPropertyContainer* Albums();
        
        /**
         * Returns track/video title array
         * @since S60 5.1
         * @param None
         * @return CCmSqlPropertyContainer, track container
         */
        CCmSqlPropertyContainer* Tracks();
                             
    protected:

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aAppUi, application ui
         * @param aMediaType, mediatype
         * @param aGenreArray, genre container
         * @param aArtistArray, artist container
         * @param aAlbumArray, album container
         * @param aTrackArray, track/video title container
         * @return None
         */
        CMSMetadataCollector( CMSAppUi& aAppUi,
                              TCmMediaType aMediaType,
                              RArray<TInt>& aGenreArray,
                              RArray<TInt>& aArtistArray,
                              RArray<TInt>& aAlbumArray,
                              RArray<TInt>& aTrackArray );

        /**
         * Performs the second phase construction.
         */
        void ConstructL();

    private:
        /**
         * Deletes all items from containers
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
         
        void DeleteAllItems();
        /**
         * Adds container to collector
         *
         * @since S60 5.1
         * @param None
         * @return None
         */        
        void AddContainersL();
        
    private:
        /**
         * Application UI
         */
        CMSAppUi*                             iMSAppUi;         // not owned
        /**
         * MediaType indicator
         */
         TCmMediaType                         iMediaType;
        /**
         * Artist container
         */
        CCmSqlPropertyContainer*              iGenreContainer;  // not owned       
        /**
         * Artist container
         */
        CCmSqlPropertyContainer*              iArtistContainer; // not owned
        /**
         * Artist container
         */
        CCmSqlPropertyContainer*              iAlbumContainer;  // not owned        
        /**
         * Artist container
         */
        CCmSqlPropertyContainer*              iTrackContainer;  // not owned        
        /**
         * Container of MetadataContainers
         */
        CCmSqlPropertyCollector*              iMetadataCollector; // owned        
        /**
         * Selected genre array
         */
        RArray<TInt>*                         iGenreArray;        //owned
        /**
         * Selected artist array
         */
        RArray<TInt>*                         iArtistArray;       //owned
        /**
         * Selected album array
         */
        RArray<TInt>*                         iAlbumArray;        //owned
        /**
         * Selected track array
         */
        RArray<TInt>*                         iTrackArray;        //owned
        
    };

#endif //  __CMSMETADATACOLLECTOR_H