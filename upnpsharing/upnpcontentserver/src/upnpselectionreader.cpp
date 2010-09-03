/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUpnpSelectionReader class implementation
*
*/






// INCLUDE FILES

#include <upnpstring.h>
#include <s32file.h>
#include <centralrepository.h>
// #include <clfcontentlistingextended.hrh>

#include <MCLFItemListModel.h>
#include <MCLFItem.h>
#include "upnpcontentservercrkeys.h"
#include "upnpselectionreader.h"
#include "upnpcontentmetadatautility.h"
#include "upnpcontentserverdefs.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

//CONSTANTS
// Format string for listbox items
_LIT(KItemFormatString, "1\t%S");
const TInt KDefaultStringArrGranularity( 10 );
const TInt KDefaultSelectionGranularity( 2 );

const TInt KShareNoneIndex = 0;
const TInt KShareAllIndex = 1;
const TInt KPredefinedSelections = 2; // Share none and share all

using namespace UpnpContentServer;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUpnpSelectionReader::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpSelectionReader* CUpnpSelectionReader::NewL(
                        CUpnpContentMetadataUtility* aUtility )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpSelectionReader* self =
        new(ELeave) CUpnpSelectionReader();

    CleanupStack::PushL(self);
    self->ConstructL( aUtility );
    CleanupStack::Pop(self);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpSelectionReader::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpSelectionReader::ConstructL(
                        CUpnpContentMetadataUtility* aUtility  )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iMetadataUtility = aUtility;

    iPlIdArray = new (ELeave) CDesCArrayFlat( KDefaultStringArrGranularity );
    iCollIdArray = new (ELeave) 
        CDesCArrayFlat( KDefaultStringArrGranularity );

    iPlaylistNames = new (ELeave) 
        CDesCArrayFlat( KDefaultStringArrGranularity );
    iCollectionNames = new (ELeave) 
        CDesCArrayFlat( KDefaultStringArrGranularity );

    iSelectedImages  = new (ELeave) RArray<TInt>( 
        KDefaultSelectionGranularity);
    iSelectedMusic  = new (ELeave) RArray<TInt>( 
        KDefaultSelectionGranularity);

    GetVisualSharingStateL( iVisualSharingSelection );
    GetMusicSharingStateL( iMusicSharingSelection );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpSelectionReader::CUpnpSelectionReader
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpSelectionReader::CUpnpSelectionReader()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iImageContainers = NULL;
    iPlaylistNames = NULL;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::~CUpnpSelectionReader
// C++ default destructor.
// --------------------------------------------------------------------------
//
CUpnpSelectionReader::~CUpnpSelectionReader()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iMetadataUtility = NULL;

    if ( iSelectedImages )
        {
        iSelectedImages->Close();
        }

    delete iSelectedImages;

    if ( iSelectedMusic )
        {
        iSelectedMusic->Close();
        }

    delete iSelectedMusic;
    delete iPlIdArray;
    delete iCollIdArray;
    delete iPlaylistNames;
    delete iImageContainers;
    delete iCollectionNames;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::IsItemSharedL()
// Checks if album is shared earlier
// --------------------------------------------------------------------------
//
TBool CUpnpSelectionReader::IsItemShared(
            const TDesC& aItemName,
            const CDesCArray& aIDArray ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt pos;
    TBool retVal = !aIDArray.Find( aItemName, pos );

    __LOG1( "CUpnpSelectionReader::IsItemSharedL returns %d",
        retVal );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return retVal;
    }


// --------------------------------------------------------------------------
// CUpnpSelectionReader::FetchCollectionsL()
// Fetch albums from Media Gallery for the listbox
// --------------------------------------------------------------------------
//
void CUpnpSelectionReader::FetchCollectionsL( CDesCArray* 
                                              aSettingsTextArray )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

/*    if ( !aSettingsTextArray )
        {
        User::Leave( KErrGeneral );
        }
    // This is for IsItemSharedL
    iMediaType = EImageAndVideo;

    if( iSelectedImages )
        {
        iSelectedImages->Reset();
        }
    if( iCollIdArray )
        {
        iCollIdArray->Reset();
        }
    if( iCollectionNames )
        {
        iCollectionNames->Reset();
        }

    // Read shared albums
    CDesCArray* sharedStuff = new (ELeave) CDesCArrayFlat( 
        KDefaultStringArrGranularity );
    CleanupStack::PushL( sharedStuff );

    ReadSharedContainerIDsL( sharedStuff,
                             EImageAndVideo );

    // fill iCollIdArray
    SearchCollectionsL();

    // cycle trough albums to collect names to the listbox
    for (TInt index = 0; index < iCollIdArray->MdcaCount(); index++)
        {
        HBufC16* albumName = HBufC16::NewL(KMaxFileName);
        CleanupStack::PushL( albumName );

        // create item string for listbox (icon + album name)
        TPtrC16 collectionName = iCollectionNames->MdcaPoint( index );
        albumName->Des().Format( KItemFormatString, &collectionName ); 

        // append album name to the listbox
        aSettingsTextArray->AppendL( albumName->Des() );

        // check if album is shared earlier
        if ( IsItemShared( iCollIdArray->MdcaPoint( index ), *sharedStuff ) )
            {
            //Index can't be '0' or '1' because there are predefined 
            //selections such as "share nothing" and "share all" first.
            iSelectedImages->AppendL( index + KPredefinedSelections );
            }


        CleanupStack::PopAndDestroy( albumName );
        albumName = NULL;
        }

    GetVisualSharingStateL( iVisualSharingSelection );

    if ( iVisualSharingSelection == EShareAll )
       {
        iSelectedImages->Reset();
        // if all albums were shared mark only "share all"
        iSelectedImages->AppendL( KShareAllIndex );
        }
    else if ( iVisualSharingSelection == EShareNone )
        {
        iSelectedImages->Reset();
        // if no albums were shared mark only "share nothing"
        iSelectedImages->AppendL( KShareNoneIndex );
        }

    iContainerCount = aSettingsTextArray->MdcaCount();

    CleanupStack::PopAndDestroy( sharedStuff );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );*/
    }


// --------------------------------------------------------------------------
// CUpnpSelectionReader::FetchPlaylistsL()
// Fetch playlists from Media Gallery for the listbox.
// --------------------------------------------------------------------------
//
void CUpnpSelectionReader::FetchPlaylistsL( CDesCArray* aSettingsTextArray )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

 /*   if ( !aSettingsTextArray )
        {
        User::Leave( KErrGeneral );
        }

    // This is for IsItemSharedL
    iMediaType = EPlaylist;

    if ( iSelectedMusic )
        {
        iSelectedMusic->Reset();
        }

    if ( iPlIdArray )
        {
        iPlIdArray->Reset();
        }

    if( iPlaylistNames )
        {
        iPlaylistNames->Reset();
        }

    // container where items will be added
    CDesCArray* sharedStuff = new (ELeave) CDesCArrayFlat( 
        KDefaultStringArrGranularity );
    CleanupStack::PushL( sharedStuff );

    // Read previously shared playlist ids for calculating indexes
    ReadSharedContainerIDsL( sharedStuff,
                             EPlaylist );

    // search .m3u files put them to iPlIdArray
    SearchPlaylistFilesL();

    for ( TInt fileIndex = 0; 
          fileIndex < iPlIdArray->MdcaCount(); fileIndex++ )
        {
        HBufC16* listboxItem = HBufC16::NewL(KMaxFileName);
        CleanupStack::PushL(listboxItem);

        TPtrC itemName( iPlaylistNames->MdcaPoint( fileIndex ) );

        TBuf<KMaxFileName> nameBuf;
        nameBuf.Copy( itemName );
        listboxItem->Des().Format(KItemFormatString, &nameBuf );
        aSettingsTextArray->AppendL( listboxItem->Des() );

        if ( IsItemShared( iPlIdArray->MdcaPoint( fileIndex ),
                           *sharedStuff ) )
            {
            //Index can't be '0' or '1' because there are predefined 
            //selections such as "share nothing" and "share all" first.
            iSelectedMusic->AppendL( fileIndex + KPredefinedSelections );
            }

        CleanupStack::PopAndDestroy( listboxItem );
        listboxItem = NULL;
        }

    GetMusicSharingStateL( iMusicSharingSelection );

    if ( iMusicSharingSelection == EShareAll )
        {
        iSelectedMusic->Reset();
        // if all albums were shared mark only "share all"
        iSelectedMusic->AppendL( KShareAllIndex );
        }
    else if ( iMusicSharingSelection == EShareNone )
        {
        iSelectedMusic->Reset();
        // if no albums were shared mark only "share nothing"
        iSelectedMusic->AppendL( KShareNoneIndex );
        }
    iContainerCount = aSettingsTextArray->MdcaCount();
    CleanupStack::PopAndDestroy( sharedStuff );*/
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpSelectionReader::SearchPlaylistFilesL()
// Search playlist files
// --------------------------------------------------------------------------
//
TBool CUpnpSelectionReader::SearchPlaylistFilesL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUPnPPlaylistServices* playlists = CUPnPPlaylistServices::NewL();
    CleanupStack::PushL( playlists );
    
    playlists->ListPlaylistsL( *iPlIdArray, *iPlaylistNames );
    // for debugging

#ifdef _DEBUG
    __LOG("CUpnpSelectionReader: Checking playlist validity.");
    for ( TInt i(0); i<iPlIdArray->MdcaCount(); i++ )
        {
        TBool valid(EFalse);
        TRAPD( err, valid = playlists->IsValidPlaylistL( 
                   iPlaylistNames->MdcaPoint(i) ));
        if ( !err )
            {
            TPtrC id( iPlIdArray->MdcaPoint(i) );
            __LOG3("Playlist[%d] %S IsValid: %d",
                i, &id, valid );
            }
        }
#endif
    CleanupStack::PopAndDestroy( playlists );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ETrue;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::SearchCollectionsL
// fill iCollIdArray
// --------------------------------------------------------------------------
//
TBool CUpnpSelectionReader::SearchCollectionsL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUPnPAlbumServices* albums = CUPnPAlbumServices::NewL();
    CleanupStack::PushL( albums );
    
    albums->ListAlbumsL( *iCollIdArray, *iCollectionNames );

#ifdef _DEBUG
    __LOG("CUpnpSelectionReader: Checking album validity.");
    for ( TInt i(0); i<iCollIdArray->MdcaCount(); i++ )
        {
        TBool valid(EFalse);
        TRAPD( err, valid = albums->IsValidAlbumL( 
                   iCollectionNames->MdcaPoint(i) ));
        if ( !err )
            {
            TPtrC id( iCollIdArray->MdcaPoint(i) );
            __LOG3("Album[%d] %S IsValid: %d",
                i, &id, valid );
            }
        }
#endif
    CleanupStack::PopAndDestroy( albums );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ETrue;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::ReadSharedContainerIDsL
// Reads shared container IDs from file
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::ReadSharedContainerIDsL(
    CDesCArray* aArray,
    const TUpnpMediaType& aContainerType ) const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Leave if NULL
    if ( !aArray )
        {
        User::Leave(KErrArgument);
        }

    RFs fs;
    RFile file;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);

    TInt error = KErrNone;

    if ( aContainerType == EImageAndVideo )
        {
        error = file.Open(fs, KVisualFile, EFileRead );
        }
    else if ( aContainerType == EPlaylist )
        {
        error = file.Open(fs, KMusicFile, EFileRead );
        }
    else
        {
        // Not valid container type
        error = KErrNotFound;
        }

    if ( error == KErrNone )
        {
        CleanupClosePushL( file );
        RFileReadStream readStream( file );
        CleanupClosePushL( readStream );

        HBufC* collectionName = HBufC::NewL(KMaxFileName);
        CleanupStack::PushL( collectionName );
        TPtr collection = collectionName->Des();

        do
            {
            TRAPD(err, readStream >> collection); //leaves when eof
            if (err)
                {
                error = err;
                }
            else
                {
                aArray->AppendL(collection);
                }

            }  while(error == KErrNone); // end of file

        CleanupStack::PopAndDestroy( collectionName );
        CleanupStack::PopAndDestroy( &readStream ); // Close readStream
        CleanupStack::PopAndDestroy( &file ); // Close file
        }

     CleanupStack::PopAndDestroy( &fs );

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return error;
    }


// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::GetSelectionIndexesL
// Reads shared container IDs from file
// --------------------------------------------------------------------------
//
void CUpnpSelectionReader::GetSelectionIndexesL(
    RArray<TInt>& aSelections,
    const TUpnpMediaType& aType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    RArray<TInt>* list( NULL );
    if ( aType == EImageAndVideo )
        {
        if ( !iSelectedImages )
            {

            iImageContainers->Reset();
            FetchCollectionsL( iImageContainers );
            iImageContainers->Reset();
            }
        else if ( !iImageContainers )
            {
            iImageContainers = new (ELeave) CDesCArrayFlat( 
                KDefaultStringArrGranularity );
            FetchCollectionsL( iImageContainers );
            }
        else 
            {
            FetchCollectionsL( iImageContainers );
            }
            
        list = iSelectedImages;
        }
    else
        {
        if ( !iSelectedMusic )
            {
            iPlaylistNames->Reset();
            FetchPlaylistsL( iPlaylistNames );
            iPlaylistNames->Reset();
            }
        else if ( iPlaylistNames )
            {
            iPlaylistNames = new (ELeave) 
                CDesCArrayFlat( KDefaultStringArrGranularity );
            FetchPlaylistsL( iPlaylistNames );
            }
        else 
            {
            FetchPlaylistsL( iPlaylistNames );
            }
        
        list = iSelectedMusic;
        }

    for ( TInt i(0); i< list->Count(); i++ )
        {
        aSelections.AppendL( list->operator[]( i ) );
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::GetVisualSharingStateL
// (other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::GetVisualSharingStateL( TInt& aShareAllState )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CRepository* rep = CRepository::NewL( KCrUidUpnpContentserver );
    CleanupStack::PushL( rep );
    TInt err( rep->Get( KUPnPAppShareAllVisualFiles, aShareAllState ));
    CleanupStack::PopAndDestroy( rep );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::GetMusicSharingStateL
// (other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::GetMusicSharingStateL( TInt& aShareAllState )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CRepository* rep = CRepository::NewL( KCrUidUpnpContentserver );
    CleanupStack::PushL( rep );
    TInt err( rep->Get( KUPnPAppShareAllMusicFiles, aShareAllState ));
    CleanupStack::PopAndDestroy( rep );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::GetContainerCount
// (other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::GetContainerCount() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return iContainerCount;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::PlayListIdsL
// (other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::PlayListIdsL( CDesCArray& aPlaylistIds, 
                                        CDesCArray& aPlaylistNames )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt err( KErrNone );
    if ( iPlIdArray && iPlaylistNames )
        {
        for ( TInt i(0); i < iPlIdArray->MdcaCount(); i++ )
            {
            aPlaylistIds.AppendL( iPlIdArray->MdcaPoint( i ) );
            aPlaylistNames.AppendL( iPlaylistNames->MdcaPoint( i ) );
            }
        }
    else
        {
        err = KErrGeneral;
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::CollectionIdsL
// (other items are commented in header )
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionReader::CollectionIdsL( CDesCArray& aCollectionIds,
                                             CDesCArray& aCollectionNames )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt err( KErrNone );
    if ( iCollIdArray && iCollectionNames )
        {
        for ( TInt i(0); i < iCollIdArray->MdcaCount(); i++ )
            {
            aCollectionIds.AppendL( iCollIdArray->MdcaPoint( i ) );
            aCollectionNames.AppendL( iCollectionNames->MdcaPoint( i ) );
            }
        }
    else
        {
        err = KErrGeneral;
        }
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpSelectionReader::SetMetadata
// (other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpSelectionReader::SetMetadata( CUpnpContentMetadataUtility*
    aMetaData )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iMetadataUtility = aMetaData;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// End of file
