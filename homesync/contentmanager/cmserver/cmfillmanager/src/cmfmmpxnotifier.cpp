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
* Description:  Notifies mpx
*
*/


class MMPXCollection;

#include <mpxcollectionmediator.h>
#include <mpxharvesterutility.h>
#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h> // for KMPXMediaGeneralCollectionId
#include <mpxmediamusicdefs.h> // music-specific attribute keys
#include <upnpitem.h> // s60 upnp stack
#include <escapeutils.h> // for unicode conversion
#include "upnpitemutility.h" // for FindElementByName & ResourceFromItemL
#include "upnpconstantdefs.h" // for element names
#include "cmfmmpxnotifier.h"
#include "msdebug.h"    

// CONSTANTS
const TUid KMpxLocalCollectionUid = { 0x101FFC3A };
const TUid KCmServerUid = { 0x10281FA7 };


// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::NewL
// ---------------------------------------------------------------------------
// 
CCmFmMpxNotifier* CCmFmMpxNotifier::NewL( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::NewL() start"));    
    CCmFmMpxNotifier* self = CCmFmMpxNotifier::NewLC( );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmMpxNotifier* CCmFmMpxNotifier::NewLC( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::NewLC() start"));    
    CCmFmMpxNotifier* self = new ( ELeave ) CCmFmMpxNotifier( );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::~CCmFmMpxNotifier
// ---------------------------------------------------------------------------
// 
CCmFmMpxNotifier::~CCmFmMpxNotifier()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::~CCmFmMpxNotifier()"));
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

// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::CCmFmMpxNotifier
// ---------------------------------------------------------------------------
// 
CCmFmMpxNotifier::CCmFmMpxNotifier( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::CCmFmMpxNotifier"));    
    }

// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmMpxNotifier::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::ConstructL"));    
    
    iHarvester = CMPXHarvesterFactory::NewL();
    iCollectionUtil = MMPXCollectionUtility::NewL( 0, KCmServerUid );
        
    iMediator = CMPXCollectionMediator::NewL( 
        iCollectionUtil->Collection(), this );    
    }  
      
// ---------------------------------------------------------------------------
// CCmFmMpxNotifier::NotifyMpxL
// ---------------------------------------------------------------------------
//
void CCmFmMpxNotifier::NotifyMpxL( const TDesC& aTrackPath,
    const CUpnpItem& aTrackMetadata )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::NotifyMpxL"));    
    
    CMPXMedia* item = CMPXMedia::NewL();
    CleanupStack::PushL( item );
    item->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    item->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );
    item->SetTObjectValueL<TMPXGeneralCategory>(
        KMPXMediaGeneralCategory, EMPXSong );
    item->SetTextValueL(
        KMPXMediaGeneralUri, aTrackPath );
    // Insert metadata
    HBufC16* buf = NULL;
    // title
    buf = EscapeUtils::ConvertToUnicodeFromUtf8L( aTrackMetadata.Title() );
    CleanupStack::PushL( buf );
    if( buf )
        {
        item->SetTextValueL(
            KMPXMediaGeneralTitle, *buf );        
        }
    CleanupStack::PopAndDestroy( buf );
    // artist
    if ( ( buf = GetElementL( aTrackMetadata, KElementCreator ) ) != 0 )
        {
        CleanupStack::PushL( buf );
        item->SetTextValueL(
            KMPXMediaMusicArtist, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    else if ( ( buf = GetElementL( aTrackMetadata, KElementArtist ) ) != 0 )
        {
        CleanupStack::PushL( buf );
        item->SetTextValueL(
            KMPXMediaMusicArtist, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    // album
    if ( ( buf = GetElementL( aTrackMetadata, KElementAlbum ) ) != 0 )
        {
        CleanupStack::PushL( buf );
        item->SetTextValueL(
            KMPXMediaMusicAlbum, *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    // genre
    if ( ( buf = GetElementL( aTrackMetadata, KElementGenre ) ) != 0 )
        {
        CleanupStack::PushL( buf );
        item->SetTextValueL(
            KMPXMediaMusicGenre, *buf );
        CleanupStack::PopAndDestroy( buf );
        }

    // tracknumber
    if ( ( buf = GetElementL( aTrackMetadata, KElementTrackNumber ) ) != 0 )
        {
        CleanupStack::PushL( buf );
        TLex16 iLex(*buf);
        TInt tracknumber(0);
        iLex.Val(tracknumber);
        item->SetTObjectValueL<TInt>(
                KMPXMediaMusicAlbumTrack, tracknumber );
        CleanupStack::PopAndDestroy( buf );
        }


    // year
    const CUpnpElement* elem = UPnPItemUtility::FindElementByName(
        aTrackMetadata, KElementDate );
    if ( elem != 0 )
        {
        TTime timestamp;
        TInt err = 
            UPnPItemUtility::UPnPDateAsTTime( elem->Value(), timestamp );
        if( err == KErrNone )
            {
            item->SetTObjectValueL<TInt64>(
                    KMPXMediaMusicYear, timestamp.Int64() );
            }
        }
        
    // duration
    const CUpnpElement* trackResource = 
        &UPnPItemUtility::ResourceFromItemL( aTrackMetadata );
     
    if( trackResource != 0 )
        {
        const CUpnpAttribute* attr = UPnPItemUtility
            ::FindAttributeByName( *trackResource, KAttributeDuration );

        if ( attr != 0 )
            {
            TInt ms = 0;
            UPnPItemUtility
                ::UPnPDurationAsMilliseconds( attr->Value(), ms );
            
            item->SetTObjectValueL<TInt>(
                KMPXMediaGeneralDuration, ms );
            }
        }

    // Add to harvester
    TRACE(Print(_L("[FILL MNGR]\t iHarvester->AddFileL ")));
    TInt colUid = iHarvester->AddFileL( item );

    // Add to collection, make sure we set the collection ID
    item->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, TUid::Uid( colUid ) );

    TRACE(Print(_L("[FILL MNGR]\t iMediator->AddItemL ")));
    iMediator->AddItemL( item );

    CleanupStack::PopAndDestroy( item );
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMpxNotifier::NotifyMpxL end")));    
    }

// --------------------------------------------------------------------------
// CCmFmMpxNotifier::GetElementL
// --------------------------------------------------------------------------
//
HBufC16* CCmFmMpxNotifier::GetElementL(
    const CUpnpItem& aSource, const TDesC8& aSourceField ) const
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxNotifier::GetElementL"));    
    HBufC16* result = 0;
    const CUpnpElement* elem = UPnPItemUtility::FindElementByName(
        aSource, aSourceField );
    if ( elem != 0 )
        {
        result = EscapeUtils::ConvertToUnicodeFromUtf8L( elem->Value() );
        }
    return result;
    }
        
// End of file


