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
* Description:      Cds Sync Implementation
*
*/






#include <hash.h>
#include <xml/matchdata.h>
#include "cmsqlmainfactory.h"
#include "mcmsqlmain.h"
#include "cmsqlbaseitem.h"
#include "cmsqlgenericitem.h"
#include "cmsqlaudioitem.h"
#include "cmsqlimageitem.h"
#include "cmsqlvideoitem.h"
#include "cmsqlpropertyitem.h"
#include "cmsqlresolutionpropertyitem.h"

#include "cdssyncimplsql.h"
#include "cdssyncsqlao.h"
#include "msdebug.h"

// Constants
_LIT8( KXmlMimeType, "text/xml" );
_LIT8( KSymbian, "Symbian");
_LIT8( KSemicolon, ";");

_LIT8( KXmlCdsDefaultNamespace, 
       "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/");
_LIT8( KXmlCdsDcNamespace,"http://purl.org/dc/elements/1.1/");
_LIT8( KXmlCdsUpnpNamespace,"urn:schemas-upnp-org:metadata-1-0/upnp/");

_LIT8( KAlbum,      "album");
_LIT8( KArtist,     "artist");
_LIT8( KClass,      "class");
_LIT8( KDescription,"description");
_LIT8( KDate,       "date");
_LIT8( KGenre,      "genre");
_LIT8( KId,         "id");
_LIT8( KItem,       "item");
_LIT8( KRes,        "res");
_LIT8( KTitle,      "title");
_LIT8( KAlbumArtUri, "albumArtURI");

_LIT8( KBitrate,        "bitrate");
_LIT8( KDuration,       "duration");
_LIT8( KProtocolInfo,   "protocolInfo");
_LIT8( KResolution,     "resolution");
_LIT8( KSize,           "size");

_LIT8( KAudioItem,      "audioItem");
_LIT8( KImageItem,      "imageItem");
_LIT8( KVideoItem,      "videoItem");
_LIT8( KAudioBroadCastItem, "audioItem.audioBroadcast");
_LIT8( KVideoBroadCastItem, "videoItem.videoBroadcast");
_LIT8( KDlnaPn,         "DLNA.ORG_PN=" );

const TInt KAlbumIndex =        0;
const TInt KArtistIndex =       1;
const TInt KClassIndex =        2;
const TInt KGenreIndex =        3;
const TInt KResolutionIndex =   4;
const TInt KUpnpProfileIndex =  5;

const TCmMetadataField KMetadataTypes[] = 
    {
    ECmAlbum, ECmArtist, ECmUpnpClass, ECmGenre, ECmResolution, ECmProfileId
    };

const TInt KMetadataTypeCount = 6;

const TInt KParseChunkSize = 100 * KKilo; // 100 kB

const TInt KCdsSyncMaxBufLength = 512;

const TInt KCdsSyncPriority = CActive::EPriorityIdle; 

// --------------------------------------------------------------------------
// CItemResource::NewLC()
// --------------------------------------------------------------------------
CItemResource* CItemResource::NewLC()
    {
    CItemResource* self=new (ELeave) CItemResource();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CItemResource::~CItemResource()
// --------------------------------------------------------------------------
CItemResource::~CItemResource() // destruct - virtual, so no export
    {
    delete iDuration;
    delete iBitrate;
    delete iSize;    
    delete iResolution;    
    delete iProtocol;    
    delete iUri;    
    }

// --------------------------------------------------------------------------
// CItemResource::CItemResource()
// --------------------------------------------------------------------------
CItemResource::CItemResource()
    {
    }

// --------------------------------------------------------------------------
// CItemResource::ConstructL()
// --------------------------------------------------------------------------
void CItemResource::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::NewL()
// --------------------------------------------------------------------------
CCdsSyncImpl* CCdsSyncImpl::NewL()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::NewL"));
    CCdsSyncImpl* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::NewLC()
// --------------------------------------------------------------------------
CCdsSyncImpl* CCdsSyncImpl::NewLC()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::NewLC"));

    CCdsSyncImpl* self = new (ELeave) CCdsSyncImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::~CCdsSyncImpl()
// --------------------------------------------------------------------------
CCdsSyncImpl::~CCdsSyncImpl() 
    {
    
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::~CCdsSyncImpl"));

    if ( IsActive() )
        {
        Cancel();
        }
        
    // iCurrentContent is owned and needs to be deleted here before it is set
    // to NULL at CleanItemData-method.
    if ( iCurrentContent )
        {
        delete iCurrentContent;    
        }    
    CleanItemData();
    
    iState = ECdsSyncIdle;
    if ( iBackground )
        {
        delete iBackground;    
        }
    
    if ( iParser )
        {
        delete iParser;    
        }
    
    if ( iCurrentDocument )
        {
        delete iCurrentDocument;    
        }
    
    iItemsInDb.ResetAndDestroy();
    iItemsToAdd.ResetAndDestroy();
    for ( TInt i = 0; i < iNames.Count(); i++ )
        {
        iNames[ i ]->ResetAndDestroy();
        }
    iNames.ResetAndDestroy();
    iFs.Close();
    
    if ( iSqlAo )
        {
        delete iSqlAo;    
        }
    
    if ( iMetadataDb )
        {
        iMetadataDb->Close();    
        }    
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::CCdsSyncImpl()
// --------------------------------------------------------------------------
CCdsSyncImpl::CCdsSyncImpl() : 
    CActive( EPriorityStandard ),
    iHashOrder( CCmSqlBaseItem::CompareByHash ),
    iNameOrder( CCmSqlPropertyItem::CompareItemsByName )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ConstructL() 
// --------------------------------------------------------------------------
void CCdsSyncImpl::ConstructL() 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::ConstructL"));

    CMatchData *matchData = CMatchData::NewL();
    CleanupStack::PushL( matchData );
    matchData->SetMimeTypeL( KXmlMimeType );
    User::LeaveIfError( iFs.Connect() );
    matchData->SetVariantL( KSymbian );

    iParser  = CParser::NewL( *matchData, *this );
    CleanupStack::PopAndDestroy( matchData );
    
    iBackground = CIdle::NewL( KCdsSyncPriority );
    iMetadataDb = CCmSqlMainFactory::NewCmSqlMainL( iFs );

    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::ResetL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ResetL() 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::ResetL"));
    LOG(_L("[Cds Sync]\t cleaning objects.."));
    iBackground->Cancel();
    if( iSqlAo && iSqlAo->IsActive() && iMetadataDb )
        {
        iMetadataDb->CancelAsyncOperation();
        }
  
    delete iSqlAo; 
    iSqlAo = NULL;
    
    iItemsInDb.ResetAndDestroy();
    iItemsToAdd.ResetAndDestroy();
    for ( TInt i = 0; i < iNames.Count(); i++ )
        {
        iNames[ i ]->ResetAndDestroy();
        }
    iNames.ResetAndDestroy();
    CleanItemData();           
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::InitL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::InitL( RPointerArray<HBufC8>& aSourceDataArray,
                                   const TInt& aDeviceId, 
                                   MCdsSyncObserver& aObserver,
                                   TInt aAddGranularity )
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::InitL"));
    iSearchIndex = 0;
    ResetL();
    
    LOG(_L("[Cds Sync]\t initializing variables.."));
    iObserver = &aObserver;
    iSourceDataArray = &aSourceDataArray;
    iSourceDataComplete = EFalse;
    iState = ECdsSyncInitializing;
    iUnchangedItemCount = 0;
        
    LOG(_L("[Cds Sync]\t creating ao.."));
    iSqlAo = CCdsSyncSqlAo::NewL( 
        *iMetadataDb, *this, iItemsToAdd, 
        iItemsInDb, aAddGranularity );
    
    LOG(_L("[Cds Sync]\t Requesting existing metadata.."));
    iMetadataDb->SetMsId( aDeviceId );
    iMetadataDb->GetItemsL( iItemsInDb, iSqlAo->iStatus );
    
    iSqlAo->iState = CCdsSyncSqlAo::ECdsSyncSqlAoInitializing;
    iSqlAo->Activate();
        
    LOG(_L("[Cds Sync]\t done."));
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::RunL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::RunL()
    {
    TRACE( Print( _L("[Cds Sync]\t CCdsSyncImpl::RunL,\
    iStatus %d iState %d"), iStatus.Int(), (TInt)iState ) );
    
    switch ( iState )
        {
        case ECdsSyncInitializing:
            {
            TInt namesCount = iNames.Count();
            TRACE( Print( 
                _L("[Cds Sync]\t iNames array size is %d out of %d"),
                namesCount, 
                KMetadataTypeCount ) );
            
            if ( namesCount == KMetadataTypeCount )
                {
                // all arrays ready, sort them
                iItemsInDb.Sort( iHashOrder );
                for ( TInt i = 0; i < KMetadataTypeCount; i++ )
                    {
                    iNames[ i ]->Sort( iNameOrder );
                    }
                iState = ECdsSyncReadyToParse;
                iSqlAo->iState = CCdsSyncSqlAo::ECdsSyncSqlAoIdle;
                NotifySourceDataAddedL();
                }
            else 
                {
                RPointerArray<CCmSqlPropertyItem>* nameArray =
                    new (ELeave) RPointerArray<CCmSqlPropertyItem>();
                
                iMetadataDb->GetPropertyValuesL( 
                    *nameArray,
                    iSqlAo->iStatus,
                    KMetadataTypes[namesCount]
                );
                iSqlAo->Activate();
                iNames.AppendL( nameArray );
                }
            break;
            }
        default: 
            {
            TRACE( Print( _L("[Cds Sync]\t RunL iState is \
            not ECdsSyncInitializing" ) ));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::DoCancel()
// --------------------------------------------------------------------------
void CCdsSyncImpl::DoCancel()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::DoCancel"));
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::NotifySourceDataAddedL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::NotifySourceDataAddedL( 
    TBool aSourceDataComplete ) 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::NotifySourceDataAdded"));
    
    if ( aSourceDataComplete )
        {
        iSourceDataComplete = ETrue;
        }
    
    if ( iState == ECdsSyncReadyToParse ) 
        {
        if ( iSourceDataArray->Count() )
            {
            iCurrentDocument = ( *iSourceDataArray )[ 0 ];
            iSourceDataArray->Remove( 0 );
            iChunkIndex = 0;
            iParser->ParseBeginL();
            iBackground->Start( 
                TCallBack( CCdsSyncImpl::BackgroundParseL, this) );
            iState = ECdsSyncParsing;
            }
        else if ( iSourceDataComplete ) 
            {
            TRACE( Print( _L
            ("[Cds Sync]\t parsing complete, %d items to add"), 
                iItemsToAdd.Count() ));
            TRACE( Print( _L("[Cds Sync]\t and %d items to remove"), 
                iItemsInDb.Count() ));
     
            iState = ECdsSyncIdle;
            
            RemoveUnchangedItems( );
            
            iSqlAo->NotifyItemsAddedL( ETrue );
            }
        }
    }
    
// --------------------------------------------------------------------------
// CCdsSyncImpl::ProgressL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ProgressL( TInt aItemCount )
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::ProgressL"));
    iObserver->ProgressL( aItemCount );
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::ChunkCount()
// --------------------------------------------------------------------------
TInt CCdsSyncImpl::ChunkCount()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::ChunkCount"));
    TInt chunkCount = iSourceDataArray->Count();
    if ( iCurrentDocument )
        {
        chunkCount++;
        }
    return chunkCount;
    }

// --------------------------------------------------------------------------
// Increment successfully processed number 
// --------------------------------------------------------------------------
//
TInt CCdsSyncImpl::ProcessedItemCount()
	{
	return iProcessedItems;
	}
	
// --------------------------------------------------------------------------
// Increment chuch number ( search index )
// --------------------------------------------------------------------------
//
void CCdsSyncImpl::SetSearchIndex( const TInt aSearchIndex )
    {
    iSearchIndex = aSearchIndex;
    } 
        
// --------------------------------------------------------------------------
// CCdsSyncImpl::ChunkCompleteL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ChunkCompleteL()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::ChunkCompleteL"));
    iObserver->ChunkCompleteL();
    iProcessedItems = 0;
    }
    
// --------------------------------------------------------------------------
// CCdsSyncImpl::OperationsCompleteL()
// --------------------------------------------------------------------------
#ifdef _DEBUG
void CCdsSyncImpl::OperationsCompleteL( TInt aErrCode ) 
#else // _DEBUG
void CCdsSyncImpl::OperationsCompleteL( TInt /*aErrCode*/ ) 
#endif // _DEBUG
    {
    TRACE( Print( _L
    ("[Cds Sync]\t CCdsSyncImpl::OperationsCompleteL (err %d)"), 
        aErrCode ));
    
    if ( iState == ECdsSyncInitializing )
        {
        RunL();
        }
    else 
        {
        iObserver->SyncCompleteL();    
        iProcessedItems = 0;  
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::BackgroundParseL()
// --------------------------------------------------------------------------
TInt CCdsSyncImpl::BackgroundParseL( TAny* aCdsSync )
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::BackgroundParseL"));
    
    return ((CCdsSyncImpl*)aCdsSync)->DoBackgroundParseL();
    }
 

// --------------------------------------------------------------------------
// CCdsSyncImpl::DoBackgroundParseL()
// --------------------------------------------------------------------------
TInt CCdsSyncImpl::DoBackgroundParseL()
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::DoBackgroundParseL"));
    if ( iCurrentDocument ) 
        {

    #ifdef _DEBUG
        iHashTime = 0;
        TTime timeBefore;
        timeBefore.HomeTime();
    #endif
        
        TBool parseMore = 
            ( iCurrentDocument->Length() - iChunkIndex ) > KParseChunkSize;
        TPtrC8 parseChunk = 
            parseMore ?
            iCurrentDocument->Mid( iChunkIndex, KParseChunkSize ) :
            iCurrentDocument->Mid( iChunkIndex );
        
        iUnchangedItemCount = 0;
            
        // in case of leave that is caused by out of memory
        TRAPD( error, iParser->ParseL( parseChunk ) );
    	if ( error != KErrNone )
        	{
        	TRACE( Print( _L("[Cds Sync]\t Parse error = %d"), error ));
        	}
        
    #ifdef _DEBUG
        TTime timeAfter;
        timeAfter.HomeTime();
        TRACE( Print( _L
            ("[Cds Sync]\t parsing of %d bytes of XML took %ld microsec"),
            parseChunk.Size(), 
            timeAfter.MicroSecondsFrom( timeBefore ).Int64() ) );
        TRACE( Print( _L
            ("[Cds Sync]\t of which hash comparison took %ld microsec"), 
            iHashTime));
    #endif

        if ( parseMore ) 
            {
            iChunkIndex += KParseChunkSize;
            }
        else 
            {
            // in case of leave that is caused by out of memory
            TRAPD( err, iParser->ParseEndL() );
    		if ( err != KErrNone )
        		{
        		TRACE( Print( _L("[Cds Sync]\t \
        		ParseEndL error = %d"), err ));
        		}
            iState = ECdsSyncReadyToParse;
            delete iCurrentDocument; iCurrentDocument = NULL;
            // check if there's more to parse and trap the leave
            TRAPD( errOne, NotifySourceDataAddedL() );
            if ( errOne != KErrNone )
                {
                TRACE( Print( _L("[Cds Sync]\t NotifySourceDataAdded \
                error = %d"), errOne ));
                }
            ChunkCompleteL();
            }
        iSqlAo->NotifyItemsAddedL();
        
        return parseMore;
        
        }
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::BackgroundParseL END"));
    return EFalse;
    }
  

// --------------------------------------------------------------------------
// CCdsSyncImpl::FindAttribute()
// --------------------------------------------------------------------------
TInt CCdsSyncImpl::FindAttribute( const TDesC8& aName, 
                                  const TDesC8& aPref, 
                                  const RAttributeArray& aAttributes ) const
    {
    TInt returnvalue = KErrNotFound;
    for ( TInt i = 0; i < aAttributes.Count(); i++ ) 
        {
        if ( ( aAttributes[i].Attribute().LocalName().
               DesC().Compare( aName ) == 0) &&
             ( aAttributes[i].Attribute().Prefix().
               DesC().Compare( aPref ) == 0) )
            {
            returnvalue = i;
            // break out from the loop
            i = aAttributes.Count();
            }
        }
    return returnvalue;
    }


// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseResolution()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ParseResolution( const TDesC8& aDes,
                                    TUint& aWidth,
                                    TUint& aHeight ) const
    {
    TLex8 lex( aDes );
    if ( lex.Val( aWidth ) != KErrNone ) 
        {
        aWidth = 0;
        }
    if ( lex.Get() != 'x' || lex.Val( aHeight ) != KErrNone ) 
        {
        aHeight = 0;
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseUint()
// --------------------------------------------------------------------------
TUint CCdsSyncImpl::ParseUint( const TDesC8& aDes ) const
    {
    TUint res = 0;
    TLex8 lex( aDes );
    if ( lex.Val( res ) != KErrNone ) 
        {
        res = 0;
        }
    return res;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseInt64()
// --------------------------------------------------------------------------
TInt64 CCdsSyncImpl::ParseInt64( const TDesC8& aDes ) const
    {
    TInt64 res = 0;
    TLex8 lex( aDes );
    if ( lex.Val( res ) != KErrNone )
        {
        res = 0;
        }
    return res;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseTime()
// --------------------------------------------------------------------------
TTime CCdsSyncImpl::ParseTime( const TDesC8& aDes ) const 
    {
    TUint year = 0;
    TUint month = 1;
    TUint day = 1;
    TLex8 lex;
    
    TInt dashpos = aDes.Find( _L8("-") );
    TInt dashpos2 = aDes.Mid( dashpos + 1 ).Find( _L8("-") ) + dashpos + 1;
    
    lex = aDes.Left(4);
    if ( lex.Val( year ) != KErrNone )
        {
        year = 0;
        }        
    if ( dashpos2-dashpos > 1 ) 
        {
        lex = aDes.Mid( dashpos + 1,dashpos2 - dashpos - 1 );
        if ( lex.Val(month) != KErrNone )
            {
            month = 1;
            }
            
        }
    if (aDes.Length() - dashpos2 > 1) 
        {
        lex = aDes.Mid(dashpos2 + 1);
        if ( lex.Val(day) != KErrNone ) 
            {
            day = 1;
            }            
        }
    TDateTime time;
    if ( time.Set(year, TMonth(month-1), day-1, 0,0,0,0) != KErrNone )
        {
        return TTime( TDateTime(0, TMonth(0), 0, 0,0,0,0) );
        }
    else
        {
        return TTime( time );
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseDuration()
// --------------------------------------------------------------------------
TReal CCdsSyncImpl::ParseDuration( const TDesC8& aDes ) const
    {
    TInt multiplier = 1;
    TInt plusminuspos = aDes.Find( _L8("-") );
    if ( plusminuspos == KErrNotFound ) 
        {
        plusminuspos = aDes.Find( _L8("+") );
        }
    else 
        {
        multiplier = -1;
        }
        
    TInt hourminutepos = aDes.Find( _L8(":") );
    TInt minutesecondpos = aDes.Mid( hourminutepos + 1 ).
         Find( _L8(":") ) + hourminutepos + 1;
    TInt dotpos = aDes.Find( _L8(".") );
    TInt slashpos = aDes.Find( _L8("/") );

    TInt hours = 0; TInt minutes = 0; 
    TReal seconds = 0; TReal f0 = 0; TReal f1=1;    
    TLex8 lex;
    
    if ( hourminutepos - plusminuspos > 1 ) 
        {
        lex = aDes.Mid( plusminuspos + 1, hourminutepos - plusminuspos - 1 );
        if ( lex.Val( hours ) != KErrNone )
            {
            hours = 0;
            }            
        }
    if ( minutesecondpos - hourminutepos > 1 ) 
        {
        lex = aDes.Mid( hourminutepos + 1, 
                        minutesecondpos - hourminutepos - 1 );
        if ( lex.Val(minutes) != KErrNone ) 
            {
            minutes = 0;
            }
        }
    if ( (dotpos == KErrNotFound || slashpos == KErrNotFound) 
          && minutesecondpos != KErrNotFound ) 
        {
        lex = aDes.Mid( minutesecondpos + 1 );
        if ( lex.Val(seconds) != KErrNone )
            {
            seconds = 0;
            }            
        } 
    else if (slashpos - dotpos > 1 && dotpos - minutesecondpos > 1 
            && aDes.Length() - slashpos > 1) 
        {
        lex = aDes.Mid( minutesecondpos + 1, dotpos - minutesecondpos - 1 );
        if ( lex.Val( seconds ) != KErrNone ) 
            {
            seconds = 0;
            }
        lex = aDes.Mid( dotpos + 1, slashpos - dotpos - 1 );
        if ( lex.Val(f0) != KErrNone ) 
            {
            f0 = 0;
            }            
        lex = aDes.Mid( slashpos + 1 );
        if ( lex.Val(f1) != KErrNone ) 
            {
            f1 = 1;
            }            
        }
    
    return multiplier*(hours * 3600 + minutes * 60 + seconds + f0/f1);
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::CalculateHashL()
// --------------------------------------------------------------------------
HBufC8* CCdsSyncImpl::CalculateHashL() const
    {
    CSHA1* sha1 = CSHA1::NewL();
    CleanupStack::PushL( sha1 );
    sha1->Reset();
    
    if ( iArtist ) 
        {
        sha1->Update( *iArtist );    
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
    
    if ( iAlbum ) 
        {
        sha1->Update( *iAlbum );    
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
    
    if ( iTitle ) 
        {
        sha1->Update( *iTitle );
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
    
    if ( iClass ) 
        {
        sha1->Update( *iClass );
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
    
    if ( iGenre ) 
        {
        sha1->Update( *iGenre );
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
        
    if ( iDate ) 
        {
        sha1->Update( *iDate );
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }
        
    if( iAlbumArtUri )
        {
        sha1->Update( *iAlbumArtUri );
        }
    else 
        {
        sha1->Update( KNullDesC8() );
        }        
    
    for ( TInt i = 0; i < iResources.Count(); i++ ) 
        {
        CItemResource* res = iResources[i];
        
        if ( res->iBitrate ) 
            {
            sha1->Update( *res->iBitrate );    
            }
        else 
            {
            sha1->Update( KNullDesC8() );
            }
        
        if ( res->iSize ) 
            {
            sha1->Update( *res->iSize );
            }
        else 
            {
            sha1->Update( KNullDesC8() );
            }
        
        
        if ( res->iResolution ) 
            {
            sha1->Update( *res->iResolution );
            }
        else 
            {
            sha1->Update( KNullDesC8() );    
            }
        
        
        if ( res->iDuration ) 
            {
            sha1->Update( *res->iDuration );
            }
        else 
            {
            sha1->Update( KNullDesC8() );    
            }
        
        if ( res->iProtocol ) 
            {
            sha1->Update( *res->iProtocol );
            }
        else 
            {
            sha1->Update( KNullDesC8() );
            }
        
        if ( res->iUri ) 
            {
            sha1->Update( *res->iUri );
            }
        else 
            {
            sha1->Update( KNullDesC8() );            
            }
        }

    // get the final hash value.
    TPtrC8 hash = sha1->Final();
    
    // create an object that can be returned and copy hash value there.
    HBufC8* retval = hash.AllocL();

    // delete SHA1 object.
    CleanupStack::PopAndDestroy( sha1 );
    sha1 = NULL;
    
    // return 
    return retval;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::EscapeHashLC()
// --------------------------------------------------------------------------
HBufC* CCdsSyncImpl::EscapeHashLC( const TDesC8& aHash ) const
    {
    HBufC* escapedHash = HBufC::NewLC( 40 );
    TPtr ptr = escapedHash->Des();
    for ( TInt i=0; i < 20; i++ ) 
        {
        if ( aHash[i] == 0 ) 
            {
            ptr.Append( _L("\\0") );
            } 
        else if ( aHash[i] == '\\' )
            {
            ptr.Append( _L("\\\\") );
            } 
        else
            {
            ptr.Append( aHash[i] );
            }
        }
    return escapedHash;    
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::GetPropertyIdL()
// --------------------------------------------------------------------------
TInt64 CCdsSyncImpl::GetPropertyIdL( TInt aMetadataIndex,
                                     const TDesC8& aValue ) const
    {
    TInt64 id = 0;
    
    TCmMetadataField metadataType = KMetadataTypes[ aMetadataIndex ];
    
    CCmSqlPropertyItem* property = NULL;
    if ( metadataType == ECmResolution )
        {
        property = CCmSqlResolutionPropertyItem::NewLC();
        }
    else 
        {
        property = CCmSqlPropertyItem::NewLC();
        }
        
    
    property->SetNameL( aValue );
    CCmSqlPropertyItemArray* properties = iNames[ aMetadataIndex ];
    TInt index = properties->FindInOrder( property, iNameOrder );

    
    if ( index == KErrNotFound )
        {
        if ( metadataType == ECmResolution ) 
            {
            TUint width = 0;
            TUint height = 0;
            ParseResolution( aValue, width, height );
            
            CCmSqlResolutionPropertyItem* resolutionProperty = 
                static_cast<CCmSqlResolutionPropertyItem*> ( property );
            resolutionProperty->SetWidth( width );
            resolutionProperty->SetHeight( height );
            resolutionProperty->SetPixelCount( width * height );
            }
        
        property->SetStatus( EFalse );
        
        iMetadataDb->SyncAddPropertyItemL( 
            *property, metadataType );
        properties->InsertInOrder( property, iNameOrder );
        id = property->Id();
        CleanupStack::Pop( property );
        }
    else 
        {
        id = (*properties)[ index ]->Id();
        CleanupStack::PopAndDestroy( property );
        }

    return id;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::CleanItemData()
// --------------------------------------------------------------------------
void CCdsSyncImpl::CleanItemData() 
    {
    iCurrentContent = NULL;

    delete iItemId; 
    iItemId = NULL;

    delete iArtist; 
    iArtist = NULL;

    delete iAlbum; 
    iAlbum = NULL;

    delete iTitle; 
    iTitle = NULL;

    delete iClass; 
    iClass = NULL;

    delete iGenre; 
    iGenre = NULL;

    delete iDate; 
    iDate = NULL;

    delete iDescription; 
    iDescription = NULL;
    
    delete iAlbumArtUri;
    iAlbumArtUri = NULL;

    iResources.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnStartDocumentL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnStartDocumentL( const RDocumentParameters&/*aDocParam*/, 
                                     TInt aErrorCode ) 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::OnStartDocumentL"));

    if ( aErrorCode ) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
        
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnEndDocumentL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnEndDocumentL( TInt aErrorCode ) 
    {
    LOG(_L("[Cds Sync]\t CCdsSyncImpl::OnEndDocumentL"));

    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
        

    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnStartElementL(
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnStartElementL( const RTagInfo& aElement, 
                                    const RAttributeArray& aAttributes,
                                    TInt aErrorCode )
    {
    
    if ( aErrorCode ) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
        
    delete iCurrentContent;
    iCurrentContent = NULL;
        
    const TDesC8& name = aElement.LocalName().DesC();
    const TDesC8& uri = aElement.Uri().DesC();

    if ( !uri.Compare( KXmlCdsDefaultNamespace ) )
        {
        if ( !name.Compare( KItem ) ) // <item>
            {
            TInt id = FindAttribute( KId, _L8(""), aAttributes );
            if ( id != KErrNotFound) 
                {
                CleanItemData();
                iItemId = aAttributes[ id ].Value().DesC().AllocL();
                }
            }
        else if ( !name.Compare( KRes ) ) // <res>
            {
            
            CItemResource* res = CItemResource::NewLC();
            
            for (TInt i = 0; i < aAttributes.Count(); i++) 
                {
                
                const TDesC8& attrName = 
                    aAttributes[i].Attribute().LocalName().DesC();
                HBufC8* attrValue = aAttributes[i].Value().DesC().AllocL();
                
                if ( !attrName.Compare( KBitrate ) )  
                    {
                    res->iBitrate = attrValue; 
                    } 
                else if ( !attrName.Compare( KSize ) )  
                    {
                    res->iSize = attrValue; 
                    } 
                else if ( !attrName.Compare( KDuration ) )  
                    {
                    res->iDuration = attrValue; 
                    } 
                else if ( !attrName.Compare( KResolution ) )  
                    {
                    res->iResolution = attrValue; 
                    } 
                else if ( !attrName.Compare( KProtocolInfo ) )  
                    {
                    res->iProtocol = attrValue; 
                    } 
                }
            iResources.AppendL( res );
            CleanupStack::Pop( res );
            }     
        }
    }  

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnEndElementL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnEndElementL( const RTagInfo& aElement, 
                                  TInt aErrorCode )
    {
    if ( aErrorCode ) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
        
    const TDesC8& name = aElement.LocalName().DesC();
    const TDesC8& uri = aElement.Uri().DesC();
    
    if ( !uri.Compare( KXmlCdsDefaultNamespace ) ) 
        // element from DIDL-Lite namespace
        {
        if ( !name.Compare( KRes ) && iCurrentContent ) // </res>
            {
            // remove "http://" and IP from URI
            TInt httpPos = iCurrentContent->Find( _L8("http://") );
            if ( httpPos != KErrNotFound ) 
                {
                TInt ipLength = 
                    iCurrentContent->Mid( httpPos + 7 ).Find(_L8("/"));
                if ( ipLength != KErrNotFound ) 
                    {
                    iCurrentContent->Des().Delete(0, httpPos + 7 + ipLength);
                    } 
                } 
            iResources[ iResources.Count() - 1 ]->iUri = iCurrentContent;
            }
        
        else if ( !name.Compare( KItem )) // </item>
            {
            // calc hash and construct item here
            HBufC8* hash = CalculateHashL();
            CleanupStack::PushL( hash );
            HBufC* escapedHash = EscapeHashLC( *hash );
            CleanupStack::Pop( escapedHash );

            CleanupStack::PopAndDestroy( hash );
            hash = NULL;
            CleanupStack::PushL( escapedHash );
            
            CCmSqlBaseItem* baseItem = CCmSqlBaseItem::NewLC();
            baseItem->SetHashL( *escapedHash );
            TInt index = iItemsInDb.FindInOrder( baseItem, iHashOrder );
            CleanupStack::PopAndDestroy( baseItem );
            
            if ( index == KErrNotFound ) // new or modified item
                {
                CCmSqlGenericItem* item = NULL;
                
                // find relevant resources
                CItemResource* httpRes = NULL;
                CItemResource* internalRes = NULL;
                for ( TInt i=0; i < iResources.Count(); i++ )
                    {
                    HBufC8* protocol = iResources[ i ]->iProtocol;
                    if ( protocol )
                        {
                        if ( !httpRes && 
                              protocol->Find(_L8("http-get:")) 
                                != KErrNotFound )
                            {
                            httpRes = iResources[ i ];
                            }
                        else if ( !internalRes &&
                                   protocol->Find(_L8("internal:")) 
                                    != KErrNotFound )
                            {
                            internalRes = iResources[ i ];
                            }
                        }
                    }
                
                if( iClass && iClass->Find( KAudioBroadCastItem ) != 
                    KErrNotFound )
                    {
                    // create audio item and set audio specific properties
                    CCmSqlAudioItem* audioItem = CCmSqlAudioItem::NewLC();
                    audioItem->SetMediaType( ECmAudioBroadCast );
                    if ( iGenre ) 
                        {
                        audioItem->SetGenreId( 
                            GetPropertyIdL( KGenreIndex, *iGenre ) );                        
                        }
                    item = audioItem;
                    CleanupStack::Pop( audioItem );
                    }
                    
                else if( iClass && iClass->Find( KVideoBroadCastItem ) != 
                    KErrNotFound  )
                    {
                    // create video item and set image specific properties
                    CCmSqlVideoItem* videoItem = CCmSqlVideoItem::NewLC();
                    videoItem->SetMediaType( ECmVideoBroadCast );
                    if ( iGenre ) 
                        {
                        videoItem->SetGenreId( 
                            GetPropertyIdL( KGenreIndex, *iGenre ) );                        
                        }
                    item = videoItem;
                    CleanupStack::Pop( videoItem );                    
                    }
                    
                else if ( iClass && 
                          iClass->Find( KAudioItem ) != KErrNotFound )
                    {
                    // create audio item and set audio specific properties
                    CCmSqlAudioItem* audioItem = CCmSqlAudioItem::NewLC();
                    audioItem->SetMediaType( ECmAudio );
                    if ( iAlbum ) 
                        {
                        audioItem->SetAlbumId( 
                            GetPropertyIdL( KAlbumIndex, *iAlbum ) );                        
                        }
                    if ( iArtist ) 
                        {
                        audioItem->SetArtistId( 
                            GetPropertyIdL( KArtistIndex, *iArtist ) );                        
                        }
                    if ( iGenre ) 
                        {
                        audioItem->SetGenreId( 
                            GetPropertyIdL( KGenreIndex, *iGenre ) );                        
                        }
                    if( iAlbumArtUri )
                        {
                        audioItem->SetAlbumArtUriL( *iAlbumArtUri );
                        }
                    if ( httpRes && httpRes->iDuration )
                        {
                        audioItem->SetDuration( 
                            (TInt) ParseDuration( *httpRes->iDuration ) );
                        }
                    if ( httpRes && httpRes->iBitrate )
                        {
                        audioItem->SetBitrate( 
                            ParseUint( *httpRes->iBitrate ) );
                        }
                    item = audioItem;
                    CleanupStack::Pop( audioItem );
                    }
                
                else if ( iClass && 
                          iClass->Find( KImageItem ) != KErrNotFound )
                    {
                    // create image item and set image specific properties
                    CCmSqlImageItem* imageItem = CCmSqlImageItem::NewLC();
                    imageItem->SetMediaType( ECmImage );
                    
                    if ( iDescription )
                        {
                        imageItem->SetDescriptionL( *iDescription );
                        }
                    if ( internalRes && internalRes->iResolution )
                        {
                        imageItem->SetResolutionId(
                            GetPropertyIdL( KResolutionIndex, 
                                *internalRes->iResolution ));
                        }
                    item = imageItem;
                    CleanupStack::Pop( imageItem );
                    }
                else if ( iClass && 
                          iClass->Find( KVideoItem ) != KErrNotFound )
                    {
                    // create video item and set image specific properties
                    CCmSqlVideoItem* videoItem = CCmSqlVideoItem::NewLC();
                    videoItem->SetMediaType( ECmVideo );
                    if ( iGenre ) 
                        {
                        videoItem->SetGenreId( 
                            GetPropertyIdL( KGenreIndex, *iGenre ) );                        
                        }
                    item = videoItem;
                    CleanupStack::Pop( videoItem );
                    }

                if ( item )
                    {
                    CleanupStack::PushL( item );
                    // Parsing dlna profile id from protocol info
                    ParseProfileIdL( *item, *httpRes );
                    ParseProfileIdL( *item, *internalRes );
                    // set general properties
                    item->SetHashL( *escapedHash );
                    item->SetCdsIdL( *iItemId );
                    if ( iClass ) 
                        {
                        item->SetUpnpclassId( 
                            GetPropertyIdL( KClassIndex, *iClass ) );                        
                        }
                    if ( iTitle )
                        {
                        item->SetTitleL( *iTitle );
                        }
                    if ( iDate )
                        {
                        item->SetDate( ParseTime( *iDate ) );
                        }
                    if ( httpRes && httpRes->iUri )
                        {
                        item->SetUriL( *httpRes->iUri );
                        }
                    if ( httpRes && httpRes->iSize )
                        {
                        item->SetSize( ParseUint( *httpRes->iSize ) );
                        }
                    else if ( internalRes && internalRes->iSize ) 
                        {
                        item->SetSize( ParseUint( *internalRes->iSize ) );
                        }
                    TTime currentTime;
                    currentTime.HomeTime();
                    item->SetHarvestDate( currentTime );
                    item->SetSearchId( iSearchIndex );
                    iItemsToAdd.AppendL( item ); // transfer ownership
                    ProgressL(1);
                    iProcessedItems++;
                    CleanupStack::Pop( item );
                    }
                else
                    {
                    LOG(_L("[Cds Sync]\t item == NULL "));
                    }                    
                }
            else // unchanged item
                {
                CCmSqlBaseItem* itemToRemove = iItemsInDb[ index ];
                iItemsInDb.Remove( index );
                ProgressL(1);
                iProcessedItems++;
                delete itemToRemove;
                }
            CleanItemData();
            CleanupStack::PopAndDestroy( escapedHash );
            }
        }
    
    else if ( !uri.Compare( KXmlCdsDcNamespace ) ) 
        // element from dc namespace
        {
        if ( !name.Compare( KTitle ) ) // </title>
            {
            iTitle = iCurrentContent;
            }
        else if ( !name.Compare( KDate ) ) // </date>
            {
            iDate = iCurrentContent;
            }
        else if ( !name.Compare( KDescription ) ) // </description>
            {
            iDescription = iCurrentContent;
            }          
        }
    
    else if ( !uri.Compare( KXmlCdsUpnpNamespace ) ) 
        // element from UPnP namespace
        {
        if ( !name.Compare( KAlbum ) ) // </album>
            {
            iAlbum = iCurrentContent;
            }
        else if ( !name.Compare( KArtist ) ) // </artist>
            {
            iArtist = iCurrentContent;
            }
        else if ( !name.Compare( KClass ) ) // </class>
            {
            iClass = iCurrentContent;
            }
        else if ( !name.Compare( KGenre ) ) // </genre>
            {
            iGenre = iCurrentContent;
            }
        else if ( !name.Compare( KAlbumArtUri ) ) // </albumarturi>
            {
            // Parse uri removes ip and port for iCurrentContent
            ParseUri();
            iAlbumArtUri = iCurrentContent;
            }              
        }
    else
        {
        delete iCurrentContent;
        }        
    
    iCurrentContent = NULL;
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnContentL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    if ( aErrorCode ) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }                
    if ( iCurrentContent )
        {
        if( iCurrentContent->Length() + aBytes.Length() < 
            KCdsSyncMaxBufLength )
            {
            iCurrentContent->Des().Append( aBytes );
            }
        else
            {
            iCurrentContent = iCurrentContent->ReAllocL( 
                iCurrentContent->Length() + aBytes.Length() );
            iCurrentContent->Des().Append( aBytes );                
            }
        }
    else 
        {
        iCurrentContent = HBufC8::NewL( KCdsSyncMaxBufLength );
        iCurrentContent->Des().Append( aBytes );
        }        
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnStartPrefixMappingL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnStartPrefixMappingL( const RString& /* aPrefix */, 
                                          const RString& /* aUri */, 
                                          TInt aErrorCode ) 
    {
    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
        
    
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnEndPrefixMappingL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnEndPrefixMappingL( const RString& /* aPrefix */, 
                                        TInt aErrorCode )
    {
    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnIgnorableWhiteSpaceL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnIgnorableWhiteSpaceL( const TDesC8& /* aBytes */, 
                                           TInt aErrorCode ) 
    {
    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnSkippedEntityL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnSkippedEntityL( const RString& /* aName */, 
                                     TInt aErrorCode ) 
    {
    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::OnProcessingInstructionL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnProcessingInstructionL( const TDesC8& /* aTarget */, 
                                             const TDesC8& /* aData */, 
                                             TInt aErrorCode ) 
    {
    if (aErrorCode) 
        {
        iObserver->SyncErrorL( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// void CCdsSyncImpl::OnError() 
// --------------------------------------------------------------------------
void CCdsSyncImpl::OnError( TInt aErrorCode ) 
    {
    if (aErrorCode && iState != ECdsSyncIdle) 
        {
        TRAP_IGNORE( iObserver->SyncErrorL( aErrorCode ) );
        }
        
    
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::GetExtendedInterface() 
// --------------------------------------------------------------------------
TAny* CCdsSyncImpl::GetExtendedInterface( const TInt32 /* aUid */ ) 
    {
    return NULL;    
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseProfileIdL()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ParseProfileIdL( 
    CCmSqlGenericItem& aItem, 
    const CItemResource& aRes ) const
    {
    if( &aRes && &aItem )
        {
        TInt index( aRes.iProtocol->Find( KDlnaPn() ) );
        if( KErrNotFound != index )
            {
            TInt index2( aRes.iProtocol->Find( KSemicolon() ) );
            if( KErrNotFound != index2 )
                {                        
                HBufC8* temp = 
                    aRes.iProtocol->Mid( 
                        index + KDlnaPn().Length(), 
                        index2 - ( index + KDlnaPn().Length() ) ).AllocLC();
                aItem.SetUpnpProfileId( 
                    GetPropertyIdL( KUpnpProfileIndex, *temp ) );
                CleanupStack::PopAndDestroy( temp );
                }
            }
        }     
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::RemoveUnchangedItems()
// --------------------------------------------------------------------------
void CCdsSyncImpl::RemoveUnchangedItems()
    {
    for( TInt i = iItemsInDb.Count() - 1; i >= 0; i-- )
        {
        if( iItemsInDb[i]->SearchId() < iSearchIndex )
            {
            CCmSqlBaseItem* itemToRemove = iItemsInDb[ i ];
            iItemsInDb.Remove( i );
            delete itemToRemove;            
            }
        }    
    }

// --------------------------------------------------------------------------
// CCdsSyncImpl::ParseUri()
// --------------------------------------------------------------------------
void CCdsSyncImpl::ParseUri()
    {
    // Removes ip and port from iCurrentContent
    TInt httpPos = iCurrentContent->Find( _L8("http://") );
    if ( httpPos != KErrNotFound ) 
        {
        TInt ipLength = 
            iCurrentContent->Mid( httpPos + 7 ).Find(_L8("/"));
        if ( ipLength != KErrNotFound ) 
            {
            iCurrentContent->Des().Delete(0, httpPos + 7 + ipLength);
            } 
        }  
    }
    
// End of file
