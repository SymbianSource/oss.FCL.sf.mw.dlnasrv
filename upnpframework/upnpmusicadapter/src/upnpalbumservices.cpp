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
* Description:  provides album handling services for UPnP framework
*
*/

#include "upnpapplicationcrkeys.h" // KCRUidUPnPApplication
#include <mpxcollectionutility.h>
#include <mpxcollectionframeworkdefs.h> // main attribute keys
#include <mpxmessagegeneraldefs.h>
#include <mpxmediageneraldefs.h> // commonly used attribute keys
#include <mpxmediacontainerdefs.h> // container-specific attribute keys
#include <mpxcollectionpath.h>
#include <mpxcollectionmessage.h>
#include <mpxmediadrmdefs.h>

#include <mpxmedia.h>
#include <mpxmediaarray.h>

#include "upnpalbumservices.h" // ourselves

// debug
_LIT16( KComponentLogfile, "musicadapter.txt" );
#include "upnplog.h"

// Constant definitions
#define KAlbumsCollectionUid 0x20007197 // album plugin uid

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CUPnPAlbumServices::NewL
// 1st phase constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPAlbumServices* CUPnPAlbumServices::NewL()
    {
    __LOG( "CUPnPAlbumServices::NewL" );

    CUPnPAlbumServices* self = new(ELeave) CUPnPAlbumServices();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::CUPnPAlbumServices
// Default constructor.
// --------------------------------------------------------------------------
//    
CUPnPAlbumServices::CUPnPAlbumServices() : iStatus( KErrNone )
    {
    // None.
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::ConstructL
// 2nd phase constructor
// --------------------------------------------------------------------------
//    
void CUPnPAlbumServices::ConstructL()
    {
    __LOG( "CUPnPAlbumServices::ConstructL" );
    iAlbumNames = new (ELeave) CDesCArrayFlat(5);
    iAlbumIds = new (ELeave) CDesCArrayFlat(5);

    iCollectionUtility = MMPXCollectionUtility::NewL(
        (MMPXCollectionObserver*)this, KMcModePlaylist );

    // Open album collection plugin. Callback to HandleCollectionMessage.
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KAlbumsCollectionUid );
    iCollectionUtility->Collection().OpenL( *path );
 	CleanupStack::PopAndDestroy( path ); 

    // Wait until signalled to proceed
    iWait = new (ELeave) CActiveSchedulerWait();    
    iWait->Start();     

    __LOG1( "CUPnPAlbumServices::ConstructL - End %d", iStatus );
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::ListAlbumsL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAlbumServices::ListAlbumsL(
    CDesCArray& aAlbumIds,
    CDesCArray& aAlbumNames )
    {
    __LOG( "CUPnPAlbumServices::ListAlbumsL" );
    
    // Check if already listed
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPAlbumServices::ListAlbumsL iStatus %d return", iStatus );
        return;
        }
    
    if( iAlbumNames->Count() > 0 && iAlbumIds->Count() > 0 )
        {
        __LOG( "CUPnPAlbumServices::ListAlbumsL: Already fetched." );
        // Copy from member
        TInt count = iAlbumNames->Count();
        __LOG1( "Album count=%d", count );
        for( TInt i = 0; i < count; i++ )
            {
            aAlbumNames.AppendL( iAlbumNames->MdcaPoint(i) );
            aAlbumIds.AppendL( iAlbumIds->MdcaPoint(i) );
            }
        __LOG( "CUPnPAlbumServices::ListAlbumsL: Return." );
        return;
        }

    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( path );
    path->SelectAllL();

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( KMPXMediaGeneralId );
    attrs.AppendL( KMPXMediaGeneralTitle );

    // Get album list from collection. Callback to HandleCollectionMediaL
    iCollectionUtility->Collection().MediaL( *path, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( path );

    // Wait until signalled to proceed
    iWait->Start();

    if( iStatus != KErrNone )
        {
        if( iStatus == KErrNotFound )
            {
            // No albums available.
            __LOG( "ListAlbumsL: No albums available!" );
            return;
            }
        else // Some other problem
            {
            User::Leave( iStatus );
            }
        }

    if ( iMedia != 0 )
        {
        TMPXAttribute mediaArrayAttr( KMPXMediaIdContainer,
            EMPXMediaArrayContents );
                
        if( iMedia->IsSupported( mediaArrayAttr ) )
            {
            __LOG("getting album array");
            CMPXMediaArray* medias = iMedia->ValueCObjectL<CMPXMediaArray>(
                mediaArrayAttr );

            if( medias )
                {
                CleanupStack::PushL( medias );
                // Album count
                TInt count = medias->Count();        
                __LOG1( "Album count=%d", count );

                for( TInt i = 0; i < count; ++i )
                    {
                    const CMPXMedia* entry = (*medias)[i];
                    const TDesC& title = entry->ValueText(
                        KMPXMediaGeneralTitle );
                    __LOG3( "Title: %d/%d [%S]",i, count, &title );
                    TMPXItemId id = *entry->Value<TMPXItemId>(
                        KMPXMediaGeneralId );
                    aAlbumNames.AppendL( title );
                    iAlbumNames->AppendL( title ); // for later usage
                    aAlbumIds.AppendL( Id2Desc( id ) );
                    iAlbumIds->AppendL( Id2Desc( id ) ); // for later usage
                    }
                CleanupStack::PopAndDestroy( medias );
                }
            }
        else
            {
            __LOG( "getting single item" );
            const TDesC& title = iMedia->ValueText(
                KMPXMediaGeneralTitle );
            __LOG1( "Title: %S", &title );
            TMPXItemId id = *iMedia->Value<TMPXItemId>(
                KMPXMediaGeneralId );
            aAlbumNames.AppendL( title );
            iAlbumNames->AppendL( title ); // for later usage
            aAlbumIds.AppendL( Id2Desc( id ) );
            iAlbumIds->AppendL( Id2Desc( id ) ); // for later usage
            }
        delete iMedia;
        iMedia = 0;
        }

    __LOG( "CUPnPAlbumServices::ListAlbumsL -End" );
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::OpenAlbumL
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPAlbumServices::OpenAlbumL(
    const TDesC& aAlbumId,
    CDesCArray& aContentMedia )

    {
    

    __LOG1( "CUPnPAlbumServices::OpenAlbumL(%S)", &aAlbumId );
    
  
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPAlbumServices::OpenAlbumL iStatus %d return", iStatus );
        return;
        }
        
    CMPXCollectionPath* path = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( path );   
    path->AppendL( Desc2Id( aAlbumId ) );

    iCollectionUtility->Collection().OpenL( *path );
 	CleanupStack::PopAndDestroy( path );

 	// Wait until signalled to proceed
    iWait->Start();

 	if( iStatus != KErrNone )
        {
        // Open album failed.
        User::Leave( iStatus );
        }

    CMPXCollectionPath* mediaPath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( mediaPath );
    mediaPath->SelectAllL();
    
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.Append( KMPXMediaGeneralUri );

    // Get metadata of given album. Callback to HandleCollectionMediaL
    iCollectionUtility->Collection().MediaL( *mediaPath, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( mediaPath );

    // Wait until signalled to proceed
    iWait->Start();

    if( iStatus != KErrNone )
        {
        if( iStatus == KErrNotFound )
            {
            // Album is empty
            __LOG( "ListAlbumsL: Album is empty" );
            return;
            }
        else // Some other problem
            {
            User::Leave( iStatus );
            }
        }

    if ( iMedia != 0 )
        {
        __LOG( "CUPnPAlbumServices::iMedia exist! ");
        TMPXAttribute mediaArrayAttr( KMPXMediaIdContainer,
            EMPXMediaArrayContents );

        // Check if there is more than one item in album
        if( iMedia->IsSupported( mediaArrayAttr ) )
            {
            __LOG("getting album array");
            CMPXMediaArray* medias = iMedia->ValueCObjectL<CMPXMediaArray>(
                mediaArrayAttr );

            if( medias )
                {
                __LOG( "CUPnPAlbumServices::album array exist! ");
                CleanupStack::PushL( medias );
                // Album count
                TInt count = medias->Count();        
                __LOG1( "Album count=%d", count );

                for( TInt i = 0; i < count; ++i )
                    {
                    __LOG2( "accessing album: item %d of %d",i, count );
                    const CMPXMedia* entry = (*medias)[i];
                    const TDesC& uri = entry->ValueText(
                        KMPXMediaGeneralUri );
                    __LOG1( "Item uri:[%S]", &uri );
                    aContentMedia.AppendL( entry->ValueText(
                        KMPXMediaGeneralUri ) );
                    }
                 CleanupStack::PopAndDestroy( medias );
                 }
            }
         else // Handle single item
            {
            __LOG("getting single item");
            const TDesC& uri = iMedia->ValueText( KMPXMediaGeneralUri );
            __LOG1( "Item uri:[%S]", &uri );
            aContentMedia.AppendL( iMedia->ValueText( KMPXMediaGeneralUri ) );
            }
            
        delete iMedia;
        iMedia = 0;
        }
    __LOG( "CUPnPAlbumServices::OpenAlbumL - End ");
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::IsValidAlbumL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CUPnPAlbumServices::IsValidAlbumL(
    const TDesC& aAlbumName )
    {
    TBool found = EFalse;

    __LOG1( "IsValidAlbumL(%S)", &aAlbumName );
    
    if( iStatus != KErrNone)
        {
        __LOG1( "CUPnPAlbumServices::IsValidAlbumL iStatus %d return", iStatus );
        return EFalse;
        }
        
    // Check if already listed
    if( iAlbumNames->Count() > 0 )
        {
        __LOG( "CUPnPAlbumServices::IsValidAlbumL: Already fetched." );
        TInt count = iAlbumNames->Count();
        __LOG1( "Album count=%d", count );
        for( TInt i = 0; i < count; i++ )
            {
            if( aAlbumName.Compare( iAlbumNames->MdcaPoint(i) ) == 0 )
                {
                __LOG( "IsValidAlbum -> True ");
                i = count;
                found = ETrue;
                }
            }
        __LOG( "CUPnPAlbumServices::IsValidAlbumL: Return." );
        return found;
        }

    // List albums
    CMPXCollectionPath* mediaPath = iCollectionUtility->Collection().PathL();
    CleanupStack::PushL( mediaPath );
    mediaPath->SelectAllL();

    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( KMPXMediaGeneralId );
    attrs.AppendL( KMPXMediaGeneralTitle );

    // Get album data. Callback to HandleCollectionMediaL
    iCollectionUtility->Collection().MediaL( *mediaPath, attrs.Array() );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PopAndDestroy( mediaPath );

    // Wait until signalled to proceed
    iWait->Start();

    if( iStatus != KErrNone )
        {
        if( iStatus == KErrNotFound )
            {
            // No albums available
            __LOG( "ListAlbumsL: No albums available" );
            return EFalse;
            }
        else // Some other problem
            {
            User::Leave( iStatus );
            }
        }

    if ( iMedia != 0 )
        {
        __LOG( "CUPnPAlbumServices::iMedia exist! ");
        TMPXAttribute mediaArrayAttr( KMPXMediaIdContainer,
            EMPXMediaArrayContents );
                
        if( iMedia->IsSupported( mediaArrayAttr ) )
            {
            __LOG("getting album array");
            CMPXMediaArray* medias = iMedia->ValueCObjectL<CMPXMediaArray>(
                mediaArrayAttr );

            if( medias )
                {
                __LOG( "CUPnPAlbumServices::album array exist! ");
                CleanupStack::PushL( medias );
                // Album count
                TInt count = medias->Count();        
                __LOG1( "Album count=%d", count );

                for( TInt i = 0; i < count; ++i )
                    {
                    const CMPXMedia* entry = (*medias)[i];
                    if( aAlbumName.Compare( 
                        entry->ValueText( KMPXMediaGeneralTitle ) ) == 0 )
                        {
                        __LOG( "IsValidAlbum -> True ");
                        found = ETrue;
                        break;
                        }
                    }
                CleanupStack::PopAndDestroy( medias );
                }
            }
        else
            {
            __LOG( "iMedia->IsSupported = False!" );
            }
        delete iMedia;
        iMedia = 0;
        }

    __LOG( "CUPnPAlbumServices::IsValidAlbumL - End ");
    return found;
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::~CUPnPAlbumServices
// Destructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPAlbumServices::~CUPnPAlbumServices()
    {
    __LOG( "CUPnPAlbumServices::~CUPnPAlbumServices" );
    iAlbumNames->Reset();
    delete iAlbumNames;
    iAlbumNames = 0;
    iAlbumIds->Reset();
    delete iAlbumIds;
    iAlbumIds = 0;
    
    if ( iCollectionUtility )
        {
        iCollectionUtility->Close();
        iCollectionUtility = 0;
        }

    delete iWait;
    iWait = 0;

    if( iMedia )
        {
        delete iMedia;
        iMedia = 0;
        }
    }


// Methods from MMPXCollectionObserver:

// --------------------------------------------------------------------------
// CUPnPAlbumServices::HandleCollectionMessage
// Callback for Collection->OpenL()
// --------------------------------------------------------------------------
// 
void CUPnPAlbumServices::HandleCollectionMessage( CMPXMessage* aMsg,
    TInt aErr )

    {
    __LOG1( "HandleCollectionMessage: err=%d", aErr );
	if( aErr == KErrNone)
	    {
	    TRAPD( error, DoHandleCollectionMessageL( aMsg ) );
        if ( error != KErrNone )
            {
            __LOG1( "DoHandleCollectionMessageL: leave with %d", error );
            iStatus = error;
            iWait->AsyncStop();
            }
	    }
	 else
	    {
	    iStatus = aErr;
	    iWait->AsyncStop();
	    }
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::HandleCollectionMediaL
// --------------------------------------------------------------------------
//

void CUPnPAlbumServices::HandleCollectionMediaL( const CMPXMedia& aMedia,
    TInt aError )

    {
    __LOG1( "CUPnPAlbumServices::HandleCollectionMediaL: err=%d", aError );
    if( aError == KErrNone )
        {                    
        // delete if already exist
        if( iMedia )
            {
            delete iMedia;
            iMedia = 0;
            }
        iMedia = CMPXMedia::NewL( aMedia );
        }

    iStatus = aError;
    iWait->AsyncStop();

    __LOG("CUPnPAlbumServices::HandleCollectionMediaL - End");
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::HandleOpenL
// Callback for Collection->OpenL(path, attrs)
// --------------------------------------------------------------------------
// 
void CUPnPAlbumServices::HandleOpenL( const CMPXMedia& /*aEntries*/,
    TInt /*aIndex*/, TBool /*aComplete*/, TInt aError )
    {
    __LOG1( "CUPnPAlbumServices::HandleOpenL %d", aError);
    
    if( aError != KErrNone)
        {
        iStatus = aError;
        iWait->AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::HandleOpenL
// --------------------------------------------------------------------------
// 
void CUPnPAlbumServices::HandleOpenL( 
    const CMPXCollectionPlaylist& /*aPlaylist*/, TInt aError )
    {
    __LOG1( "CUPnPAlbumServices::HandleOpenL %d", aError);
    
    if( aError != KErrNone)
        {
        iStatus = aError;
        iWait->AsyncStop();
        }
    }

// --------------------------------------------------------------------------
// Converts an ID from TMPXItemId form to descriptor form.
// --------------------------------------------------------------------------
//
const TDesC& CUPnPAlbumServices::Id2Desc( const TMPXItemId& aId )
    {  
    iTempBuffer.Num( aId );
    return iTempBuffer;
    }

// --------------------------------------------------------------------------
// Converts an ID from descriptor form to TMPXItemId form.
// --------------------------------------------------------------------------
//
TMPXItemId CUPnPAlbumServices::Desc2Id( const TDesC& aDesc )
    {
    TLex convert( aDesc );
    TUint temp;
    convert.Val( temp, EDecimal );
    TMPXItemId id(temp);
    return id;
    }

// --------------------------------------------------------------------------
// CUPnPAlbumServices::DoHandleCollectionMessageL
// --------------------------------------------------------------------------
//
void CUPnPAlbumServices::DoHandleCollectionMessageL( CMPXMessage* aMsg )
    {
    __LOG( "CUPnPAlbumServices::DoHandleCollectionMessageL" );

    if( aMsg && aMsg->IsSupported(KMPXMessageGeneralEvent ) &&
			aMsg->IsSupported(KMPXMessageGeneralType ) )
        {
	    TInt event( *aMsg->Value<TInt>( KMPXMessageGeneralEvent ) );
	    TInt type( *aMsg->Value<TInt>( KMPXMessageGeneralType ) );
	    TInt data( *aMsg->Value<TInt>( KMPXMessageGeneralData ) );
	    
	    __LOG1("Event: %d", event );
	    __LOG1("Type: %d", type );
	    __LOG1("Data: %d", data );
	     	        
	    switch ( event )
            {
            case TMPXCollectionMessage::EPathChanged:
                {
                __LOG( "TMPXCollectionMessage::EPathChanged" );
                if( type == EMcPathChangedByOpen )
                    {
                    __LOG( " Type: EMcPathChangedByOpen " );
                    // Album collection is ready for use
                    iStatus = KErrNone;
                    iWait->AsyncStop();
                    }
                }    
            break;
            default:
                {
                // do nothing
                } 
                break;
            }
	   }
    }


