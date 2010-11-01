/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP metadata fetching utilities.
*
*/


// INCLUDE FILES
// System
#include <e32std.h>
#include <e32base.h>

// upnp stack api
#include <upnpdevice.h>
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpstring.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h"

// utilities internal
#include "upnpmetadatautility.h"
#include "upnpcommonutils.h" 
#include "upnpmetadatafetcher.h"
#include "upnpcdsreselementutility.h"


// CONSTANTS
_LIT8( KMimeAudio,  "audio" );
_LIT8( KMimeVideo,  "video" );
_LIT8( KMimeImage,  "image" );
_LIT8( KMimeApplication,    "application" );

_LIT8( KItemCreator,    "None" );
_LIT8( KRealMediaVideo,     "application/x-pn-realmedia" );

// --------------------------------------------------------------------------
// UPnPMetadataFetcher::FetchMetadataL
// Resolves the metadata and fills in the CUpnpObject's metadata fields 
// accordingly.
// --------------------------------------------------------------------------
//
EXPORT_C void UPnPMetadataFetcher::FetchMetadataL( CUpnpObject& aObject, 
                                                   const TDesC& aFilePath, 
                                                   const TDesC8& aMimeType )
    {
    CUPnPMetaDataUtility* utility = CUPnPMetaDataUtility::NewL();
    CleanupStack::PushL( utility );

    // APa: Continue even if the loading of meta data fails
    // (in S60 3.2 this leaves)
    TRAP_IGNORE( utility->LoadMetaDataL( aFilePath ) );

    // Get the title from metadata is available, if not, use the filename
    HBufC8* tempBuf = NULL;
    if( utility->Title().Length() > 0 )
        {
        tempBuf = UpnpString::FromUnicodeL( utility->Title() );
        CleanupStack::PushL( tempBuf );
        aObject.SetTitleL( *tempBuf );
        CleanupStack::PopAndDestroy( tempBuf ); tempBuf = NULL;
        }
    else
        {
        // Remove the path and file extension from the filename
        TInt lastDot = 0;
        TInt lastSlash = 0;
        TInt length = 0;
        lastDot = aFilePath.LocateReverseF('.');
        if( KErrNotFound == lastDot ) //if file extension is not found
            {
            lastDot = aFilePath.Length();
            }
        lastSlash = aFilePath.LocateReverseF('\\');
        length = lastDot - lastSlash;
        HBufC* strippedFilename = NULL;
        strippedFilename = aFilePath.Mid((lastSlash+1), (length-1)).AllocL();
        CleanupStack::PushL( strippedFilename );

        // Convert to 8bit, and set the UpnpObject's title value
        tempBuf = UpnpString::FromUnicodeL( *strippedFilename );
        CleanupStack::PushL( tempBuf );
        aObject.SetTitleL( *tempBuf );

        // Clean up
        CleanupStack::PopAndDestroy( tempBuf ); 
        tempBuf = NULL;
        CleanupStack::PopAndDestroy( strippedFilename );
        strippedFilename = NULL;
        }

    if( aMimeType.Find( KMimeAudio ) >= 0 )
        {
        // Set object class
        aObject.SetObjectClassL( KClassAudioMusicTrack );
        
        AddElementToObjectL(aObject,
                        utility->Artist(), 
                        KElementArtist() );
        
        
        // Genre
        // Try to find the genre element and update it if found
        
        AddElementToObjectL(aObject,
                        utility->Genre(), 
                        KElementGenre() );
        
        
        // Album
        // Try to find the album element and update it if found
        
        AddElementToObjectL(aObject,
                        utility->Album(), 
                        KElementAlbum() );
        
       
        }
    else if( aMimeType.Find( KMimeVideo ) >= 0 )
        {
        // Set Object class
        aObject.SetObjectClassL( KClassVideo );
        }
    else if( aMimeType.Find( KMimeImage ) >= 0 )
        {
        // Set Object class
        aObject.SetObjectClassL( KClassImage );
        }
    else if( aMimeType.Find( KMimeApplication ) >= 0 )
        {
        // some special cases here
        // Fixes EAHN-7ETBH7
        if( aMimeType.Find( KRealMediaVideo ) >= 0 )
            {
            aObject.SetObjectClassL( KClassVideo );
            }
        }
    else
        {
        // Do nothing
        }

    // Date
    // Try to find the date element and update it if found
    
    AddElementToObjectL(aObject,
                    utility->Date(), 
                    KElementDate() );
    
                         
    CUpnpElement* element = CUpnpElement::NewLC( KElementCreator );
    element->SetValueL( KItemCreator );
    aObject.AddElementL( element );
    CleanupStack::Pop( element );  
    // Clean up
    CleanupStack::PopAndDestroy( utility );
    utility = NULL;
    }

// --------------------------------------------------------------------------
// UPnPMetadataFetcher::FetchMetadataL
// Resolves the metadata and fills in the CUpnpObject's metadata fields 
// accordingly.
// --------------------------------------------------------------------------
//
EXPORT_C void UPnPMetadataFetcher::FetchMetadataL( CUpnpObject& aObject, 
                                                   const TDesC& aFilePath )
    {
    // Resolve the MIME type of the file
    HBufC8* mimeType = NULL;
    mimeType = UPnPCommonUtils::ResolveMimeTypeL( aFilePath );
    if( mimeType )
        {    
        CleanupStack::PushL( mimeType );
        // Fill in the metadata
        FetchMetadataL( aObject, aFilePath, *mimeType );
        // Clean up
        CleanupStack::PopAndDestroy( mimeType );
        mimeType = NULL;
        }        
    }

// --------------------------------------------------------------------------
// UPnPMetadataFetcher::CreateItemFromFileLC
// Creates a new CUpnpItem from a local file. Resolves the metadata and fills
// in the new CUpnpItem's metadata fields accordingly.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpItem* UPnPMetadataFetcher::CreateItemFromFileLC( 
                                                    const TDesC& aFilePath )
    {
    CUpnpItem* item = NULL;
    item = CUpnpItem::NewL();
    CleanupStack::PushL( item );

    // Add a res-element to the item
    UpnpCdsResElementUtility::AddResElementL( *item, aFilePath );
    // Resolve the metadata
    FetchMetadataL( *item, aFilePath );
    
    return item;
    }

// --------------------------------------------------------------------------
// UPnPMetadataFetcher::CreateItemFromFileL
// Creates a new CUpnpItem from a local file. Resolves the metadata and fills
// in the new CUpnpItem's metadata fields accordingly.
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpItem* UPnPMetadataFetcher::CreateItemFromFileL(
                                                    const TDesC& aFilePath )
    {
    CUpnpItem* item = CreateItemFromFileLC( aFilePath );
    CleanupStack::Pop( item );
    return item;
    }


void UPnPMetadataFetcher::AddElementToObjectL(CUpnpObject& aObject,
                                        const TDesC& aMetaDataUtilityName, 
                                        const TDesC8& aElementName )
    {
    HBufC8* tempBuf = NULL;
    const RUPnPElementsArray& elms = aObject.GetElements();
    TInt count = elms.Count();
    TInt i;
    TBool found;
    tempBuf = UpnpString::FromUnicodeL( aMetaDataUtilityName );
        if( tempBuf )
            {
            CleanupStack::PushL( tempBuf );
            found = EFalse;
            for( i = 0; i < count; i++)
                {
                if( elms[ i ]->Name() == aElementName )
                    {
                    elms[ i ]->SetValueL( *tempBuf );
                    found = ETrue;
                    i = count;
                    }
                }
            if( !found )
                {
                // Not found, create a new element and add it to the item
                CUpnpElement* element = CUpnpElement::NewLC( aElementName );
                element->SetValueL( *tempBuf );
                aObject.AddElementL( element );
                CleanupStack::Pop( element );
                }
            CleanupStack::PopAndDestroy( tempBuf ); tempBuf = NULL;
            }
    
    }

// End of File
