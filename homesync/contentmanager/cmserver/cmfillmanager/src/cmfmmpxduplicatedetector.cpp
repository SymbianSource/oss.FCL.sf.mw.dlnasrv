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
* Description:  Dublicate detector for music
*
*/

#include <mpxcollectionutility.h>
#include <mpxmediageneraldefs.h> // commonly used attribute keys
#include <mpxmediacontainerdefs.h> // container-specific attribute keys
#include <mpxcollectionplugin.hrh> // collection plugin types
#include "cmfilllistitem.h"
#include "cmfmitemhandlerobserver.h"
#include "cmfmmpxduplicatedetector.h"
#include "msdebug.h"    

const TUid KMpxLocalCollectionUid = { 0x101FFC3A };
const TUid KCmServerUid = { 0x10281FA7 };

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::NewL
// ---------------------------------------------------------------------------
// 
CCmFmMpxDuplicateDetector* CCmFmMpxDuplicateDetector::NewL( 
    MCmFmItemHandlerObserver& aObserver )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::NewL() start"));    
    CCmFmMpxDuplicateDetector* self = 
        CCmFmMpxDuplicateDetector::NewLC( aObserver );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmMpxDuplicateDetector* CCmFmMpxDuplicateDetector::NewLC( 
    MCmFmItemHandlerObserver& aObserver )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::NewLC() start"));    
    CCmFmMpxDuplicateDetector* self = 
        new ( ELeave ) CCmFmMpxDuplicateDetector( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::~CCmFmMpxDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmMpxDuplicateDetector::~CCmFmMpxDuplicateDetector()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    ~CCmFmMpxDuplicateDetector()"));
    CloseCollectionPlugin();         
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::CCmFmMpxDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmMpxDuplicateDetector::CCmFmMpxDuplicateDetector( 
    MCmFmItemHandlerObserver& aObserver )
    : iObserver( aObserver )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    CCmFmMpxDuplicateDetector()"));
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmMpxDuplicateDetector::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::ConstructL()"));

    OpenCollectionPluginL(); 
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::CheckMusicL
// ---------------------------------------------------------------------------
//    
void CCmFmMpxDuplicateDetector::CheckMusicL( const CCmFillListItem& aItem )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::CheckMusicL()"));
    
    CMPXMedia* criteria = CMPXMedia::NewL();
    CleanupStack::PushL( criteria );
    criteria->SetTObjectValueL<TUid>(
        KMPXMediaGeneralCollectionId, KMpxLocalCollectionUid );
    criteria->SetTObjectValueL<TMPXGeneralType>(
        KMPXMediaGeneralType, EMPXItem );

    criteria->SetTObjectValueL<TMPXGeneralCategory>(
            KMPXMediaGeneralCategory, EMPXSong );
    
    // Set title, size and date to criteria
    criteria->SetTextValueL( 
                KMPXMediaGeneralTitle, aItem.PrimaryText() );
                
    criteria->SetTObjectValueL<TInt32>(
                KMPXMediaGeneralSize, aItem.Size() );
                
    criteria->SetTObjectValueL<TInt64>(
                KMPXMediaGeneralDate, aItem.Date().Int64() );

    // define attributes fetched
    RArray<TMPXAttribute> attributes;
    CleanupClosePushL( attributes );
    // add a dummy attribute because the query will fail if
    // there are no attributes
    attributes.AppendL( KMPXMediaGeneralId );

    // now find
    iCollectionUtility->Collection()
        .FindAllL( *criteria , attributes.Array(), *this );
    CleanupStack::PopAndDestroy(); // attributes
    CleanupStack::PopAndDestroy( criteria );
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::OpenCollectionPluginL
// ---------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::OpenCollectionPluginL( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    OpenCollectionPluginL()"));
    
    iCollectionUtility = MMPXCollectionUtility::NewL(
        (MMPXCollectionObserver*)this, KCmServerUid );
    // Fetch mpx/gallery collection collection UID
    RArray<TUid> uid;
    CleanupClosePushL( uid );
    uid.AppendL( TUid::Uid( EMPXCollectionPluginMusic ) );
    TInt err = KErrNone;
    TUid collection = iCollectionUtility->CollectionIDL( uid.Array() );
    TRAP( err,iCollectionUtility->Collection().CommandL(
        EMcCmdCollectionInit,  collection.iUid ) );        
    CleanupStack::PopAndDestroy( &uid );    
    }

// ---------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::CloseCollectionPlugin
// ---------------------------------------------------------------------------
//
void CCmFmMpxDuplicateDetector::CloseCollectionPlugin()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    CloseCollectionPlugin()"));
    
    if( iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = NULL;        
        }    
    }      

// --------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::HandleCollectionMessage
// --------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::HandleCollectionMessage( CMPXMessage* /*aMsg*/,
    TInt /*aErr*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    HandleCollectionMessage()"));    
    }

// --------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::HandleCollectionMediaL
// --------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::HandleCollectionMediaL( const CMPXMedia& /*aMedia*/,
    TInt /*aError*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::\
    HandleCollectionMediaL()"));    
    }

// --------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::HandleOpenL
// --------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::HandleOpenL( const CMPXMedia& /*aEntries*/,
    TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::HandleOpenL()"));    
    }

// --------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::HandleOpenL
// --------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::HandleOpenL( 
    const CMPXCollectionPlaylist& /*aPlaylist*/, TInt /*aError*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::HandleOpenL()"));
    }

// --------------------------------------------------------------------------
// CCmFmMpxDuplicateDetector::HandleFindAllL
// Async find
// --------------------------------------------------------------------------
// 
void CCmFmMpxDuplicateDetector::HandleFindAllL( const CMPXMedia& aResults,
    TBool /*aComplete*/, TInt /*aError*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMpxDuplicateDetector::HandleFindAllL()"));    
    
    if( &aResults )
        {
        if( !aResults.Value<CMPXMediaArray>( 
            KMPXMediaArrayContents )->Count() )
            {
            // No results => Not duplicate
            iObserver.DuplicateL( EFalse );
            }
        else
            {
            iObserver.DuplicateL( ETrue );
            }                                
        }
    }
    
// End of file

