/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for the CUpnpPlaylistHandler class
*
*/


// INCLUDES
#include <bautils.h>                            // CDesCArray

#include "upnpplaylistservices.h"               // CUPnPPlaylistServices
#include <pathinfo.h>                           // PathInfo
#include "upnpplaylisthandler.h"                // CUpnpPlaylistHandler
#include <upnpitem.h>                           // CUpnpItem
#include "upnpsettingsengine.h"                 // CUpnpSettingsEngine

_LIT( KComponentLogfile, "filetransferengine.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KItemArrayGranularity =  16;

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::NewL
// NewL.
// --------------------------------------------------------------------------
//
CUpnpPlaylistHandler* CUpnpPlaylistHandler::NewL()
    {
    __LOG( "[UpnpPlaylistHandler] CUpnpPlaylistHandler: NewL" );

    CUpnpPlaylistHandler* self = NULL;
    self = new (ELeave) CUpnpPlaylistHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// Constuctor
// --------------------------------------------------------------------------
//
CUpnpPlaylistHandler::CUpnpPlaylistHandler()
    {
    __LOG( "[UpnpPlaylistHandler] Constructor" );
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CUpnpPlaylistHandler::~CUpnpPlaylistHandler()
    {
    __LOG( "[UpnpPlaylistHandler] Destructor" );

    // If there are items on the playlists, create the playlists now before
    // deleting the content
    
    delete iPlaylistServices;
    iPlaylistServices = NULL;
    
    if( iPlaylistName )
        {
        if( iFilesForAudioPlaylist )
            {
            if( iFilesForAudioPlaylist->Count() > 0 )
                {
                TRAP_IGNORE( CreateMusicPlaylistL() );
                }
            }
        if( iFilesForVideoAlbum.Count() > 0 )
            {
            TRAP_IGNORE( CreateVideoAlbumL() );
            }
        if( iFilesForImageAlbum.Count() > 0 )
            {
            TRAP_IGNORE( CreateImageAlbumL() );
            }
        }

    // Empty and destroy the playlist arrays
    Reset();
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::ConstructL
// Second phase constructor
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::ConstructL()
    {
    __LOG( "[UpnpPlaylistHandler] ConstructL" );
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::Reset
// Empties the playlists.
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::Reset()
    {
    __LOG( "[UpnpPlaylistHandler] Reset" );

    // Delete the playlist name
    delete iPlaylistName;
    iPlaylistName = NULL;

    // Delete the audio playlist item arrays
    delete iFilesForAudioPlaylist;
    iFilesForAudioPlaylist = NULL;

    // Delete the video album item array
    iFilesForVideoAlbum.ResetAndDestroy();
    iFilesForVideoAlbum.Close();

    // Delete the image album item array
    iFilesForImageAlbum.ResetAndDestroy();
    iFilesForImageAlbum.Close();    
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::SetPlaylistNameL
// Sets the name for the playlist
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::SetPlaylistNameL( const TDesC& aPlaylistName )
    {
    __LOG( "[UpnpPlaylistHandler] SetPlaylistNameL" );
    
    // Check parameter
    if( aPlaylistName == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // If a playlist name is already provided, delete it and replace with
    // this new one
    delete iPlaylistName; iPlaylistName = NULL;
    iPlaylistName = aPlaylistName.AllocL();
    }    
    
// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::AddAudioItemL
// Adds an audio item to the playlist (playlist will not be generated yet, it
// has to be created by calling a separate method).
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::AddAudioItemL( const TDesC& aFilePath )
    {
    __LOG( "[UpnpPlaylistHandler] AddAudioItemL" );
    
    // Check parameters
    if( aFilePath == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // If the item arrays for audio item are not yet created, create them now
    if( !iFilesForAudioPlaylist )
        {
        iFilesForAudioPlaylist = 
                new (ELeave) CDesCArrayFlat( KItemArrayGranularity );
        }

    // Add the item information in the arrays
    iFilesForAudioPlaylist->AppendL( aFilePath );
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::AddImageItemL
// Adds an image item to the playlist (playlist will not be generated yet, it
// has to be created by calling a separate method).
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::AddImageItemL( const TDesC& aFilePath )
    {
    __LOG( "[UpnpPlaylistHandler] AddImageItemL" );
    
    // Check parameters
    if( aFilePath == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // Add the item information in the array
    iFilesForImageAlbum.AppendL( aFilePath.AllocL() );
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::AddVideoItemL
// Adds a video item to the playlist (playlist will not be generated yet, it
// has to be created by calling a separate method).
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::AddVideoItemL( const TDesC& aFilePath )
    {
    __LOG( "[UpnpPlaylistHandler] AddVideoItemL" );

    // Check parameters
    if( aFilePath == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // Add the item information in the array
    iFilesForVideoAlbum.AppendL( aFilePath.AllocL() );
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::AudioItemCount
// Returns the number of items in the audio item array.
// --------------------------------------------------------------------------
//
TInt CUpnpPlaylistHandler::AudioItemCount()
    {
    __LOG( "[UpnpPlaylistHandler] AudioItemCount" );
    
    TInt returnValue = 0;
    if( iFilesForAudioPlaylist )
        {
        returnValue = iFilesForAudioPlaylist->Count();
        }

    return returnValue;
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::ImageItemCount
// Returns the number of items in the image item array.
// --------------------------------------------------------------------------
//
TInt CUpnpPlaylistHandler::ImageItemCount()
    {
    __LOG( "[UpnpPlaylistHandler] ImageItemCount" );

    return iFilesForImageAlbum.Count();
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::VideoItemCount
// Returns the number of items in the video item array.
// --------------------------------------------------------------------------
//
TInt CUpnpPlaylistHandler::VideoItemCount()
    {
    __LOG( "[UpnpPlaylistHandler] VideoItemCount" );

    return iFilesForVideoAlbum.Count();
   }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::CreateMusicPlaylistL
// Creates a MPX playlist (if there are music items added).
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::CreateMusicPlaylistL()
    {
    __LOG( "[UpnpPlaylistHandler] CreateMusicPlaylistL" );

    // Create playlist only if required data exists
    if( iPlaylistName &&
        iFilesForAudioPlaylist &&
        iFilesForAudioPlaylist->Count() > 0 )
        {
        TBool location = PlaylistFileLocation();
        if( location )
            {
            if ( !iPlaylistServices )
                {
                iPlaylistServices = CUPnPPlaylistServices::NewL();
                }

            TRAPD( createPlaylistError,
                  iPlaylistServices->CreatePlaylistL(
                            *iPlaylistName,
                            *iFilesForAudioPlaylist,
                            NULL ) );
            if( createPlaylistError != KErrNone )
                {
                __LOG( "[UpnpPlaylistHandler] Failed to create a \
playlist!" );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::CreateImageAlbumL
// Creates an image album (if there are image items added).
// Not in use in upnp 2.1
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::CreateImageAlbumL()
    {
    __LOG( "[UpnpPlaylistHandler] CreateImageAlbumL" );

    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::CreateVideoAlbumL
// Creates a video album (if there are video items added).
// Not in use in upnp 2.1
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::CreateVideoAlbumL()
    {
    __LOG( "[UpnpPlaylistHandler] CreateVideoAlbumL" );
     
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::PlaylistFileLocation
// Checks that Playlist location is valid.
// --------------------------------------------------------------------------
//
TBool CUpnpPlaylistHandler::PlaylistFileLocation()
    {
    __LOG( "[UpnpPlaylistHandler] PlaylistFileLocation" );

    TBool returnValue = EFalse;
    TInt error = KErrGeneral; 
    TParse parse;
    TDriveNumber defauldrive;
    
    // Default download drive
    TRAP( error,
        CUPnPSettingsEngine* engine = CUPnPSettingsEngine::NewLC();
        engine->GetCopyLocationDriveL( defauldrive );
        CleanupStack::PopAndDestroy( engine );
        );
       
    // Playlist location
    if( error == KErrNone )
        {   
        // There are audio files on the playlist
        if( iFilesForAudioPlaylist &&
            iFilesForAudioPlaylist->Count() > 0 )
            {
            error = parse.Set( iFilesForAudioPlaylist->MdcaPoint(0), 
                                NULL, NULL );
            }

        // There are image files on the playlist
        else if( iFilesForImageAlbum.Count() > 0 )
            {    
            error = parse.Set( *iFilesForImageAlbum[0], NULL, NULL );
            }

        // There are video files on the playlist
        else if( iFilesForVideoAlbum.Count() > 0 )
            {
            error = parse.Set( *iFilesForVideoAlbum[0], NULL, NULL );
            }
        else
            {
            // error code is already set
            }
        } 
        
    // Playlist must be located to default download drive    
    if( error == KErrNone )
        {
        TPtrC drive = parse.Drive();
        TDriveUnit driveUnit = TDriveUnit( drive );
        TDriveNumber playlistdrive = (TDriveNumber)((TInt)driveUnit);
 
        if( playlistdrive == defauldrive )
            {
            returnValue = ETrue;
            }
        }   
        
    __LOG1( "[UpnpPlaylistHandler] PlaylistFileLocation %d end", returnValue );  
    
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUpnpPlaylistHandler::NotifyNewAudioFileL
// Notifies MPX of a new audio file.
// --------------------------------------------------------------------------
//
void CUpnpPlaylistHandler::NotifyNewAudioFileL(
                                            const TDesC& aFilePath,
                                            const CUpnpItem& aMetadata )
    {
    __LOG( "[UpnpPlaylistHandler] NotifyNewAudioFileL" );

    if( aFilePath == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    if ( !iPlaylistServices )
        {
        iPlaylistServices = CUPnPPlaylistServices::NewL();
        }

    TRAPD( notifyError,
        iPlaylistServices->CreateTrackL( aFilePath, aMetadata ) );
    if ( notifyError != KErrNone )
        {
        __LOG( "[UpnpPlaylistHandler] Failed to notify audio track" );
        }
    }

// End of file
