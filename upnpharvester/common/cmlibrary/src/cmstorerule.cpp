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
* Description:      Capsulating store rule
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmmediaserver.h"
#include "cmstorerule.h"
#include "msdebug.h"

// CONSTANTS
const TInt KArrayGranularity = 16;
const TInt KArrayGranularityContainer = 8;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreRule* CCmStoreRule::NewL()
    {    
    CCmStoreRule* self = CCmStoreRule::NewLC();
    CleanupStack::Pop( self ); 
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmStoreRule* CCmStoreRule::NewLC()
    {    
    CCmStoreRule* self = new ( ELeave ) CCmStoreRule();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmStoreRule::~CCmStoreRule()
    {
    delete iName;

    iRuleDefs.Reset();
    iRuleDefs.Close();

    iMediaServers.ResetAndDestroy();
    iMediaServers.Close();
    
    iExcPlayLists->Reset();
    iExcAlbums->Reset();

    delete iExcPlayLists;
    delete iExcAlbums;      
    }

// ---------------------------------------------------------------------------
// Add new rule into StoreRule
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmStoreRule::AddStoreRuleL( TCmMediaType aMediaType )
    {    
    iRuleDefs.AppendL( aMediaType );
    TInt index = iRuleDefs.Count() - 1;    
    return index;
    }

// ---------------------------------------------------------------------------
// Get rule
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::StoreRule( TInt aIndex, TCmMediaType* aMediaType )
    {
    *aMediaType = (TCmMediaType)iRuleDefs[aIndex];
    }
        
// ---------------------------------------------------------------------------
// Add new media server into FillRule
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmStoreRule::AddMediaServerL( const TDesC8& aUDN )
    {
    CCmMediaServer* server = CCmMediaServer::NewLC();
    server->SetUDNL( aUDN );    
    iMediaServers.AppendL( server );
    CleanupStack::Pop( server );    
    TInt index = iMediaServers.Count() - 1;    
    return index;        
    }

// ---------------------------------------------------------------------------
// Delete media server from the store rule
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::DeleteMediaServer( const TDesC8& aUDN )
    {
    TBool deleted( EFalse );
    for( TInt i = 0; i < iMediaServers.Count() && !deleted; i++ )
        {
        if( KErrNone == iMediaServers[i]->MediaServer().Compare(aUDN) )
            {
            delete iMediaServers[i];
            iMediaServers.Remove(i);
            iMediaServers.Compress();
            deleted = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// Adds one album into excluded list
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::AddExcAlbumL( const TDesC& aAlbum )
    {
    TRACE(Print(_L("[COMMON]\t AddExcAlbumL( %S )"), &aAlbum ));
    HBufC* temp = aAlbum.AllocL();
    CleanupStack::PushL( temp );
    iExcAlbums->AppendL( *temp );
    CleanupStack::Pop( temp );
    }

// ---------------------------------------------------------------------------
// Adds one playlist into excluded list
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmStoreRule::AddExcPlayListL( const TDesC& aPlayList )
    {
    HBufC* temp = aPlayList.AllocL();
    CleanupStack::PushL( temp );
    iExcPlayLists->AppendL( *temp );
    CleanupStack::Pop( temp );
    }

// ---------------------------------------------------------------------------
// Removes one album from the excluded list
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::RemoveExcAlbum( const TDesC& aAlbum )
    {
    TInt pos( KErrNone );
    TInt err( iExcAlbums->Find( aAlbum, pos, ECmpNormal ) );
    if( err == KErrNone )
        {
        iExcAlbums->Delete( pos );
        iExcAlbums->Compress();
        }
    }

// ---------------------------------------------------------------------------
// Removes one playlist from the excluded list
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmStoreRule::RemoveExcPlayList( const TDesC& aPlayList )
    {
    TInt pos( KErrNone );
    TInt err( iExcPlayLists->Find( aPlayList, pos, ECmpNormal ) );
    if( err == KErrNone )
        {
        iExcPlayLists->Delete( pos );
        iExcPlayLists->Compress();
        }    
    }

// ---------------------------------------------------------------------------
// Returns excluded albums
// ---------------------------------------------------------------------------
//
EXPORT_C CDesCArray& CCmStoreRule::ExcAlbums()
    {
    return *iExcAlbums;
    }

// ---------------------------------------------------------------------------
// Returns excluded playlists
// ---------------------------------------------------------------------------
//    
EXPORT_C CDesCArray& CCmStoreRule::ExcPlayLists()
    {
    return *iExcPlayLists;
    }
            
// ---------------------------------------------------------------------------
// Get media server
// ---------------------------------------------------------------------------
//    
EXPORT_C const TDesC8& CCmStoreRule::MediaServerL( TInt aIndex )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iMediaServers.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    return iMediaServers[aIndex]->MediaServer();
    }
    
// ---------------------------------------------------------------------------
// Returns count of rules
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmStoreRule::DefCount() const
    {
    return iRuleDefs.Count();
    }

// ---------------------------------------------------------------------------
// Returns count of media servers
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmStoreRule::MediaServerCount() const
    {
    return iMediaServers.Count();
    }    

// ---------------------------------------------------------------------------
// Sets FillRule name
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();        
    }

// ---------------------------------------------------------------------------
// Get Name
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmStoreRule::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// Sets list id, when storing data into db this values isn't used as a 
// db list id
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::SetListId( const TUint aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// Get list id
// ---------------------------------------------------------------------------
//    
EXPORT_C TUint CCmStoreRule::ListId() const
    {
    return iId;
    }
            
// ---------------------------------------------------------------------------
// Sets selected state
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::SetSelected( TCmFillRuleStatus aSelected )
    {
    iSelected = aSelected;
    }

// ---------------------------------------------------------------------------
// Returns method
// ---------------------------------------------------------------------------
//
EXPORT_C TCmFillRuleStatus CCmStoreRule::Selected() const
    {
    return iSelected;
    }

// ---------------------------------------------------------------------------
// Sets rule's status ( e.g ECmToBeRemoved )
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmStoreRule::SetStatus( TCmListItemStatus aStatus )
    {
    iStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// Returns status 
// ---------------------------------------------------------------------------
//
EXPORT_C TCmListItemStatus CCmStoreRule::Status() const
    {
    return iStatus;
    }
    
// ---------------------------------------------------------------------------
// CCmStoreRule::ExternalizeL
// ---------------------------------------------------------------------------
//
void CCmStoreRule::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iName->Length() );
    if ( iName )
        {
        aStream << *iName;
        }
    else
        {
        aStream << KNullDesC8();
        }      
        
    aStream.WriteInt16L((TInt)iSelected );    
    aStream.WriteInt16L((TInt)iStatus );
    
    aStream.WriteInt16L( iRuleDefs.Count() );
    for ( TInt index(0); index < iRuleDefs.Count(); index++ )
        {
        aStream.WriteInt16L((TInt)iRuleDefs[index] );
        }
        
    aStream.WriteInt16L( iMediaServers.Count() );
    for ( TInt index(0); index < iMediaServers.Count(); index++ )
        {
        iMediaServers[index]->ExternalizeL( aStream );
        }
    TRACE(Print(_L("[COMMON]\t iExcAlbums->Count() = %d"), 
        iExcAlbums->Count() ));    
    aStream.WriteInt16L( iExcAlbums->Count() );
    for ( TInt index(0); index < iExcAlbums->Count(); index++ )
        {
        TRACE(Print(_L("[COMMON]\t iExcAlbums[index].Length() = %d"), 
            iExcAlbums->MdcaPoint(index).Length() ));                
        aStream.WriteInt32L( iExcAlbums->MdcaPoint(index).Length() );
        aStream << iExcAlbums->MdcaPoint(index);
        }
    TRACE(Print(_L("[COMMON]\t iExcPlayLists->Count() = %d"), 
        iExcPlayLists->Count() ));         
    aStream.WriteInt16L( iExcPlayLists->Count() );
    for ( TInt index(0); index < iExcPlayLists->Count(); index++ )
        {
        TRACE(Print(_L("[COMMON]\t iExcPlayLists[index].Length() = %d"), 
            iExcPlayLists->MdcaPoint(index).Length() ));
        aStream.WriteInt32L( iExcPlayLists->MdcaPoint(index).Length() );
        aStream << iExcPlayLists->MdcaPoint(index);
        }
    }
        
// ---------------------------------------------------------------------------
// CCmFillRule::InternalizeL
// ---------------------------------------------------------------------------
//
void CCmStoreRule::InternalizeL( RReadStream& aStream )
    {
    // Content
    if ( iName )
        {
        delete iName;
        iName = NULL;
        }
    TInt bufLength = aStream.ReadInt32L();    
    iName = HBufC8::NewL( aStream, bufLength );
    
    // cleanup
    iRuleDefs.Reset();
 
    iSelected = (TCmFillRuleStatus)aStream.ReadInt16L();
    iStatus = (TCmListItemStatus)aStream.ReadInt16L();
    
    // rule count 
    TInt ruleCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < ruleCount; index++ )
        {
        iRuleDefs.AppendL((TCmMediaType)aStream.ReadInt16L());   
        }
        
    // cleanup
    iMediaServers.ResetAndDestroy();
    
    // media Server count
    TInt mediaServerCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < mediaServerCount; index++ )
        {
        CCmMediaServer* server = CCmMediaServer::NewLC();    
        server->InternalizeL( aStream );
        iMediaServers.AppendL( server );
        CleanupStack::Pop( server );   
        server = NULL;
        }
    iExcAlbums->Reset();   
    TInt excAlbumsCount( aStream.ReadInt16L() );
    bufLength = KErrNone;
    HBufC* temp;
    TRACE(Print(_L("[COMMON]\t excAlbumsCount = %d"), excAlbumsCount ));    
    for ( TInt index(0); index < excAlbumsCount ; index++ )
        {
        bufLength = aStream.ReadInt32L();
        TRACE(Print(_L("[COMMON]\t bufLength = %d"), bufLength )); 
        temp = HBufC::NewL( aStream, bufLength );
        CleanupStack::PushL( temp );
        iExcAlbums->AppendL( *temp );
        CleanupStack::Pop( temp );
        temp = NULL;
        }         
    iExcPlayLists->Reset();
    TInt excPlayListCount( aStream.ReadInt16L() );
    bufLength = KErrNone;
    TRACE(Print(_L("[COMMON]\t excPlayListCount = %d"), excPlayListCount ));
    for ( TInt index(0); index < excPlayListCount ; index++ )
        {
        bufLength = aStream.ReadInt32L();
        TRACE(Print(_L("[COMMON]\t bufLength = %d"), bufLength ));
        temp = HBufC::NewL( aStream, bufLength );
        CleanupStack::PushL( temp );
        iExcPlayLists->AppendL( *temp );
        CleanupStack::Pop( temp );
        temp = NULL;
        }                 
    }
                
// ---------------------------------------------------------------------------
// Default constructor
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//    
CCmStoreRule::CCmStoreRule() : 
    iRuleDefs( KArrayGranularity ), 
    iMediaServers( KArrayGranularity )
    {
    
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmStoreRule::ConstructL()
    {
    iExcAlbums = new ( ELeave ) CDesCArrayFlat( KArrayGranularityContainer );
    iExcPlayLists = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularityContainer );
    }    

// End of file

