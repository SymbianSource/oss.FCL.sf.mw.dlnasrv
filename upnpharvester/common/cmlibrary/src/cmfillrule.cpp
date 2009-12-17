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
* Description:      Capsulating fill rule
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmmediaserver.h"
#include "cmfillrule.h"
#include "cmrule.h"
#include "msdebug.h"

// CONSTANTS
const TInt KArrayGranularity = 16;
const TInt KArrayGranularityContainer = 8;

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillRule* CCmFillRule::NewL()
    {   
    CCmFillRule* self = CCmFillRule::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmFillRule* CCmFillRule::NewLC()
    {    
    CCmFillRule* self = new ( ELeave ) CCmFillRule();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmFillRule::~CCmFillRule()
    {
    delete iName;

    iRuleArray.ResetAndDestroy();
    iRuleArray.Close();
    iMediaServerArray.ResetAndDestroy();
    iMediaServerArray.Close();
    
    iExcPlayLists->Reset();
    iExcAlbums->Reset();
            
    delete iExcPlayLists;
    delete iExcAlbums;             
    }

// ---------------------------------------------------------------------------
// Add new rule into FillRule
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::AddRuleL( TCmMetadataField aDataField, 
    TCmOperatorType aOperator )
    {
    CCmRule* rule = CCmRule::NewLC();
    rule->SetMetadataField(aDataField);
    rule->SetOperator(aOperator);
    iRuleArray.AppendL( rule ); // transfer ownership
    CleanupStack::Pop( rule );
    TInt index = iRuleArray.Count() - 1;    
    return index;
    }

// ---------------------------------------------------------------------------
// Get rule
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::RuleL( TInt aIndex, TCmMetadataField* aDataField, 
    TCmOperatorType* aOperator, TInt* aParamCount )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iRuleArray[aIndex]->Rule( aDataField, aOperator, aParamCount );
    }

// ---------------------------------------------------------------------------
// Delete rule
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRule::DeleteRule( TCmMetadataField aDataField,
    TCmOperatorType aOper )
    {
    TCmMetadataField dataField; TCmOperatorType oper;
    TInt count;
    for( TInt i = 0; i < iRuleArray.Count(); i++ )
        {
        iRuleArray[i]->Rule( &dataField, &oper, &count );
        if( dataField == aDataField && oper == aOper )
            {
            delete iRuleArray[i];
            iRuleArray.Remove(i);
            iRuleArray.Compress();            
            }
        }
    }

// ---------------------------------------------------------------------------
// Delete rule
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRule::DeleteRule( TCmMetadataField aDataField )
    {
    TCmMetadataField dataField; TCmOperatorType oper;
    TInt count;
    for( TInt i = 0; i < iRuleArray.Count(); i++ )
        {
        iRuleArray[i]->Rule( &dataField, &oper, &count );
        if( dataField == aDataField )
            {
            delete iRuleArray[i];
            iRuleArray.Remove(i);
            iRuleArray.Compress();            
            }
        }
    }    
// ---------------------------------------------------------------------------
// Add new rule param into FillRule
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::AddRuleParamL( TInt aIndex, const TDesC8& aParam )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    return iRuleArray[aIndex]->AddRuleParamL( aParam );
    }    

// ---------------------------------------------------------------------------
// Add new rule param into FillRule ( Indexed param )
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::AddRuleParamL( TInt aIndex, TInt aParam )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    return iRuleArray[aIndex]->AddRuleParamL( aParam );
    } 
    
// ---------------------------------------------------------------------------
// Get rule param
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::RuleParamL( TInt aRuleIndex, TInt aParamIndex, 
    TPtrC8* aParam )
    {
    // check parameter
    if ( aRuleIndex < 0 || aRuleIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iRuleArray[aRuleIndex]->RuleParamL( aParamIndex, aParam );
    }

// ---------------------------------------------------------------------------
// Get rule param
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::RuleParamL( TInt aRuleIndex, TInt aParamIndex, 
    TInt& aParam )
    {
    // check parameter
    if ( aRuleIndex < 0 || aRuleIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    iRuleArray[aRuleIndex]->RuleParamL( aParamIndex, aParam );
    }
        
// ---------------------------------------------------------------------------
// Add new media server into FillRule
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmFillRule::AddMediaServerL( const TDesC8& aUDN )
    {
    CCmMediaServer* server = CCmMediaServer::NewLC();
    server->SetUDNL( aUDN );
    iMediaServerArray.AppendL( server );
    TInt index = iMediaServerArray.Count() - 1;    
    CleanupStack::Pop(server);
    return index;        
    }

// ---------------------------------------------------------------------------
// Add new media server into FillRule
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmFillRule::AddMediaServerL( TInt aDbId )
    {
    CCmMediaServer* server = CCmMediaServer::NewLC();
    server->SetDbId( aDbId );
    iMediaServerArray.AppendL( server );
    TInt index = iMediaServerArray.Count() - 1;    
    CleanupStack::Pop(server);
    return index;        
    }
    
// ---------------------------------------------------------------------------
// Delete media server from the fill rule
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::DeleteMediaServer( const TDesC8& aUDN )
    {
    TBool deleted( EFalse );
    for( TInt i = 0; i < iMediaServerArray.Count() && !deleted; i++ )
        {
        if( KErrNone == iMediaServerArray[i]->MediaServer().Compare(aUDN) )
            {
            delete iMediaServerArray[i];
            iMediaServerArray.Remove(i);
            iMediaServerArray.Compress();
            deleted = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// Delete media server from the fill rule
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::DeleteMediaServer( TInt aDbId )
    {
    TBool deleted( EFalse );
    for( TInt i = 0; i < iMediaServerArray.Count() && !deleted; i++ )
        {
        if( iMediaServerArray[i]->DbId() == aDbId )
            {
            delete iMediaServerArray[i];
            iMediaServerArray.Remove(i);
            iMediaServerArray.Compress();
            deleted = ETrue;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Adds one album into excluded list
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::AddExcAlbumL( const TDesC& aAlbum )
    {
    iExcAlbums->AppendL( aAlbum );
    }

// ---------------------------------------------------------------------------
// Adds one playlist into excluded list
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRule::AddExcPlayListL( const TDesC& aPlayList )
    {
    iExcPlayLists->AppendL( aPlayList );
    }    

// ---------------------------------------------------------------------------
// Removes defined album from the excluded list
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::RemoveExcAlbum( const TDesC& aAlbum )
    {
    TInt pos( KErrNone );
    TInt err( iExcAlbums->Find( aAlbum, pos ) );
    if( err == KErrNone )
        {
        iExcAlbums->Delete( pos );
        }
    }

// ---------------------------------------------------------------------------
// Removes defined playlist from the excluded list
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRule::RemoveExcPlayList( const TDesC& aPlayList )
    {
    TInt pos( KErrNone );
    TInt err( iExcPlayLists->Find( aPlayList, pos ) );
    if( err == KErrNone )
        {
        iExcPlayLists->Delete( pos );
        }    
    }    

// ---------------------------------------------------------------------------
// Returns excluded albums
// ---------------------------------------------------------------------------
//
EXPORT_C CDesCArray& CCmFillRule::ExcAlbums() const
    {
    return *iExcAlbums;
    }

// ---------------------------------------------------------------------------
// Returns excluded playlists
// ---------------------------------------------------------------------------
//    
EXPORT_C CDesCArray& CCmFillRule::ExcPlayLists() const
    {
    return *iExcPlayLists;
    }
        
// ---------------------------------------------------------------------------
// Get media server
// ---------------------------------------------------------------------------
//    
EXPORT_C const TDesC8& CCmFillRule::MediaServerL( TInt aIndex )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iMediaServerArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    return iMediaServerArray[aIndex]->MediaServer();
    }

// ---------------------------------------------------------------------------
// Get media server
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCmFillRule::MediaServerL( TInt aIndex, TInt& aServer )
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iMediaServerArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    aServer = iMediaServerArray[aIndex]->DbId();
    }
        
// ---------------------------------------------------------------------------
// Returns count of rules
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::RuleCount() const
    {
    return iRuleArray.Count();
    }

// ---------------------------------------------------------------------------
// Returns count of media servers
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CCmFillRule::MediaServerCount() const
    {
    return iMediaServerArray.Count();
    }    

// ---------------------------------------------------------------------------
// returns count of params ( in specific rule )
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::ParamCountL( TInt aIndex ) const
    {
    // check parameter
    if ( aIndex < 0 || aIndex >= iRuleArray.Count() ) 
        {
        User::Leave( KErrArgument );
        }
        
    return iRuleArray[aIndex]->RuleParamsCount();
    }
    
// ---------------------------------------------------------------------------
// Sets FillRule name
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmFillRule::SetNameL( const TDesC8& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    return KErrNone;        
    }
    
// ---------------------------------------------------------------------------
// Get Name
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmFillRule::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------------------------
// Sets list id, when storing data into db this values isn't used as a 
// db list id
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetListId( const TUint aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
// Get list id
// ---------------------------------------------------------------------------
//    
EXPORT_C TUint CCmFillRule::ListId() const
    {
    return iId;
    }
    
// ---------------------------------------------------------------------------
// Sets amount
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetAmount( TUint32 aAmount )
    {
    iAmount = aAmount;
    }

// ---------------------------------------------------------------------------
// Returns amount
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmFillRule::Amount() const
    {
    return iAmount;
    }
        
// ---------------------------------------------------------------------------
// Sets limit type ( MB or pcs. )
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetLimitType( TCmLimitType aLimitType )
    {
    iLimitType = aLimitType;
    }

// ---------------------------------------------------------------------------
// Returns limit type
// ---------------------------------------------------------------------------
//
EXPORT_C TCmLimitType CCmFillRule::LimitType() const
    {
    return iLimitType;
    }

// ---------------------------------------------------------------------------
// Sets media type
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetMediaType( TCmMediaType aMediaType )
    {
    iMediaType = aMediaType;
    }

// ---------------------------------------------------------------------------
// Returns media type
// ---------------------------------------------------------------------------
//    
EXPORT_C TCmMediaType CCmFillRule::MediaType() const
    {
    return iMediaType;
    }    
        
// ---------------------------------------------------------------------------
// Sets method ( random, newest, oldest etc. )
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetMethod( TCmFillMethod aMethod )
    {
    iMethod = aMethod;
    }

// ---------------------------------------------------------------------------
// Returns method
// ---------------------------------------------------------------------------
//
EXPORT_C TCmFillMethod CCmFillRule::Method() const
    {
    return iMethod;
    }

// ---------------------------------------------------------------------------
// Sets selected state
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetSelected( TCmFillRuleStatus aSelected )
    {
    iSelected = aSelected;
    }

// ---------------------------------------------------------------------------
// Returns method
// ---------------------------------------------------------------------------
//
EXPORT_C TCmFillRuleStatus CCmFillRule::Selected() const
    {
    return iSelected;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetStatus( TCmListItemStatus aStatus )
    {
    iStatus = aStatus;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TCmListItemStatus CCmFillRule::Status() const
    {
    return iStatus;
    }
    
// ---------------------------------------------------------------------------
// Set priority
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetPriority( TUint8 aPriority )
    {
    iPriority = aPriority;
    }

// ---------------------------------------------------------------------------
// Returns priority
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CCmFillRule::Priority() const
    {
    return iPriority;
    }    
    
// ---------------------------------------------------------------------------
// Set template id
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetTemplateId( TUint8 aTemplateId )
    {
    iTemplateId = aTemplateId;
    }

// ---------------------------------------------------------------------------
// Returns template id
// ---------------------------------------------------------------------------
//
EXPORT_C TUint8 CCmFillRule::TemplateId() const
    {
    return iTemplateId;
    }

// ---------------------------------------------------------------------------
// Set list's real size in Bytes
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetListRealSizeInBytes( TUint32 aRealSize )
    {
    iRealSize = aRealSize;
    }

// ---------------------------------------------------------------------------
// Returns list's real size in Bytes
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmFillRule::ListRealSizeInBytes() const
    {
    return iRealSize;
    }

// ---------------------------------------------------------------------------
// Set list's real size in pieces
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmFillRule::SetListRealCount( TUint32 aRealCount )
    {
    iRealCount = aRealCount;
    }

// ---------------------------------------------------------------------------
// Returns ist's real size in pieces
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CCmFillRule::ListRealCount() const
    {
    return iRealCount;
    }
                
// ---------------------------------------------------------------------------
// CCmFillRule::ExternalizeL
// ---------------------------------------------------------------------------
//
void CCmFillRule::ExternalizeL( RWriteStream& aStream ) const
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
    aStream.WriteInt32L( iAmount );
    aStream.WriteInt32L( iRealSize );
    aStream.WriteInt32L( iRealCount );
    aStream.WriteInt16L( (TInt)iLimitType );
    aStream.WriteInt16L( (TInt)iMethod );
    aStream.WriteInt16L( (TInt)iMediaType );
    aStream.WriteInt16L((TInt)iSelected );
    aStream.WriteInt16L((TInt)iStatus );
    aStream.WriteInt8L(iPriority);
    aStream.WriteInt8L( iTemplateId );
    aStream.WriteInt32L( iId );
    
    aStream.WriteInt16L( iRuleArray.Count() );
    for ( TInt index(0); index < iRuleArray.Count(); index++ )
        {
        iRuleArray[index]->ExternalizeL( aStream );
        }
        
    aStream.WriteInt16L( iMediaServerArray.Count() );
    for ( TInt index(0); index < iMediaServerArray.Count(); index++ )
        {
        iMediaServerArray[index]->ExternalizeL( aStream );
        }

    aStream.WriteInt16L( iExcAlbums->Count() );
    for ( TInt index(0); index < iExcAlbums->Count(); index++ )
        {
        aStream.WriteInt32L( iExcAlbums[index].Length() );
        aStream << iExcAlbums->MdcaPoint(index);
        }
        
    aStream.WriteInt16L( iExcPlayLists->Count() );
    for ( TInt index(0); index < iExcPlayLists->Count(); index++ )
        {
        aStream.WriteInt32L( iExcPlayLists[index].Length() );
        aStream << iExcPlayLists->MdcaPoint(index);
        }             
    }
        
// ---------------------------------------------------------------------------
// CCmFillRule::InternalizeL
// ---------------------------------------------------------------------------
//
void CCmFillRule::InternalizeL( RReadStream& aStream )
    {
    // Content
    delete iName;
    iName = NULL;

    TInt bufLength = aStream.ReadInt32L();    
    iName = HBufC8::NewL( aStream, bufLength );
    
    // cleanup
    iRuleArray.ResetAndDestroy();
    
    iAmount = aStream.ReadInt32L();
    iRealSize = aStream.ReadInt32L();
    iRealCount = aStream.ReadInt32L();
    iLimitType = (TCmLimitType)aStream.ReadInt16L();
    iMethod = (TCmFillMethod)aStream.ReadInt16L();
    iMediaType = (TCmMediaType)aStream.ReadInt16L();
    iSelected = (TCmFillRuleStatus)aStream.ReadInt16L();
    iStatus = (TCmListItemStatus)aStream.ReadInt16L();
    iPriority = (TUint8)aStream.ReadInt8L();
    iTemplateId = (TUint8)aStream.ReadInt8L();
    iId = (TUint)aStream.ReadInt32L();
    
    // rule count 
    TInt ruleCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < ruleCount; index++ )
        {
        CCmRule* newRule = CCmRule::NewLC();
        newRule->InternalizeL( aStream );   
        iRuleArray.AppendL( newRule );
        CleanupStack::Pop( newRule ); 
        newRule = NULL;
        }
        
    // cleanup
    iMediaServerArray.ResetAndDestroy();
    
    // media Server count
    TInt mediaServerCount = aStream.ReadInt16L();
    
    // Then internalize them from the stream one by one
    for (TInt index = 0; index < mediaServerCount; index++ )
        {
        CCmMediaServer* newServer = CCmMediaServer::NewLC();
        newServer->InternalizeL( aStream );     
        iMediaServerArray.AppendL( newServer );
        CleanupStack::Pop( newServer ); 
        newServer = NULL;
        }
        
    TInt excAlbumsCount( aStream.ReadInt16L() );
    bufLength = KErrNone;
    HBufC* temp = NULL;
    for ( TInt index(0); index < excAlbumsCount ; index++ )
        {
        bufLength = aStream.ReadInt32L();
        temp = HBufC::NewLC( aStream, bufLength );
        iExcAlbums->AppendL( *temp );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }         

    TInt excPlayListCount( aStream.ReadInt16L() );
    bufLength = KErrNone;
    for ( TInt index(0); index < excPlayListCount ; index++ )
        {
        bufLength = aStream.ReadInt32L();
        temp = HBufC::NewLC( aStream, bufLength );
        iExcPlayLists->AppendL( *temp );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;
        }                         
    }
                
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmFillRule::CCmFillRule() : 
    iRuleArray( KArrayGranularity ), 
    iMediaServerArray( KArrayGranularity )
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmFillRule::ConstructL()
    {
    iExcAlbums = new ( ELeave ) CDesCArrayFlat( KArrayGranularityContainer );
    iExcPlayLists = 
        new ( ELeave ) CDesCArrayFlat( KArrayGranularityContainer );    
    }    

// End of file
