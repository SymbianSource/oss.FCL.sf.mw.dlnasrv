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
* Description:  AV Controller UPnP actions handler
*
*/


#include "upnpavdevice.h"
#include "upnpxmlparser.h"
#include "upnpavcontroller.h"
#include "upnpavbrowsingsession.h"
#include "upnpconstantdefs.h"
#include "upnpitem.h"
#include <w32std.h>

#include "upnpitemutility.h"
#include "cmfilllistitem.h"
#include "cmfmupnpactionobserver.h"
#include "cmfmupnpmngr.h"
#include "msdebug.h"

_LIT8( KCmBrowseFilter,    "*" ); // No filter
_LIT8( KCmSortCriteria,    "" ); // No sorting
const TInt KStartIndex = 0; // Start from beginning
const TInt KRequestedCount = 1; // Only one item browsed at a time
const TInt KScreenWidth = 128;
const TInt KScreenHeight = 128;
_LIT8( KCmFmXMark, "x" );

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::NewL
// ---------------------------------------------------------------------------
//
CCmFmUpnpMngr* CCmFmUpnpMngr::NewL( MCmFmUPnPActionObserver* aObserver )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::NewL() start"));    
    CCmFmUpnpMngr* self = CCmFmUpnpMngr::NewLC( aObserver );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::NewLC
// ---------------------------------------------------------------------------
//   
CCmFmUpnpMngr* CCmFmUpnpMngr::NewLC( MCmFmUPnPActionObserver* aObserver )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::NewLC() start"));    
    CCmFmUpnpMngr* self = new ( ELeave ) CCmFmUpnpMngr( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::~CCmFmUpnpMngr
// ---------------------------------------------------------------------------
//
CCmFmUpnpMngr::~CCmFmUpnpMngr()
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::~CCmFmUpnpMngr()"));
    
    CancelOperation();

    delete iURI;
    delete iParser;
    delete iItem;
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::CCmFmUpnpMngr
// ---------------------------------------------------------------------------
//        
CCmFmUpnpMngr::CCmFmUpnpMngr( MCmFmUPnPActionObserver* aObserver )
    : iObserver( aObserver ), iScreenSize( KScreenWidth, KScreenHeight )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::CCmFmUpnpMngr()"));
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::ConstructL
// ---------------------------------------------------------------------------
// 
void CCmFmUpnpMngr::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::ConstructL()"));
    
    iParser = CUPnPXMLParser::NewL();
    ScreenSizeL();
    TRACE(Print(_L("[FILL MNGR]\t Phones screen size = %d x %d"), 
        iScreenSize.iWidth, iScreenSize.iHeight ));
    }    

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::SetAvController
// ---------------------------------------------------------------------------
// 
void CCmFmUpnpMngr::SetAvController( MUPnPAVController* aAVController )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::SetAvController()"));
    
    if( aAVController )
        {
        aAVController->SetDeviceObserver( *this );
        }    
    iAVController = aAVController;
    iBrowseSession = NULL;
    delete iURI;
    iURI = NULL;
    delete iItem;
    iItem = NULL;       
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::GetMediaServersL
// ---------------------------------------------------------------------------
//     
void CCmFmUpnpMngr::GetMediaServersL( CUpnpAVDeviceList*& aDevices )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::GetMediaServersL()"));
    
    if( iAVController )
        {
        aDevices = iAVController->GetMediaServersL();
        }
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::CheckURIL
// ---------------------------------------------------------------------------
//     
void CCmFmUpnpMngr::CheckURIL( CUpnpAVDevice* aDevice,
                               CCmFillListItem& aItem )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::CheckURIL()"));
    if( iAVController )
        {
        iSelectOptimalImageSize = EFalse;
        if( aItem.Status() == ECmToBeShrinked )
            {
            iSelectOptimalImageSize = ETrue;
            }
			
	    if( !iBrowseSession )
			{
            // session not found -> create
            iBrowseSession =
                &iAVController->StartBrowsingSessionL( *aDevice );
            iBrowseSession->SetObserver( *this );   
			iDevice = aDevice;
			}	
        else if( ( aDevice != iDevice ) ||
	        ( aDevice->Uuid() != iDevice->Uuid() ) )
			{
            // device is different than previosly or pointer to it
            // has been changed -> create new session
			CancelOperation();           
			iBrowseSession =
				&iAVController->StartBrowsingSessionL( *aDevice );
			iBrowseSession->SetObserver( *this );  
			iDevice = aDevice;    
			}
		
        iBrowseSession->BrowseL( aItem.ItemId(), KCmBrowseFilter, 
            MUPnPAVBrowsingSession::EMetadata, KStartIndex, 
            KRequestedCount, KCmSortCriteria );
        iBrowseStarted.HomeTime();
        TRACE(Print(_L("[FILL MNGR]\t BROWSE STARTED")));
#ifdef _DEBUG
        TBuf<KMaxName> temp;
        if( aDevice->Uuid().Length() < KMaxName )
            {
            temp.Copy( aDevice->Uuid() );
            TRACE(Print(_L("[FILL MNGR]\t TARGET DEVICE %S"), &temp ));
            }
        temp.Zero();
        if( aItem.ItemId().Length() < KMaxName )
            {
            temp.Copy( aItem.ItemId() );
            TRACE(Print(_L("[FILL MNGR]\t TARGET ITEM %S"), &temp ));        
            }
        temp.Zero();       
#endif         
        }
    else
        {
        TRACE(Print(_L("[FILL MNGR]\t iAVController == NULL"))); 
        iObserver->URICheckResult( ENoUriAvailable );
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::CancelOperation
// ---------------------------------------------------------------------------
//     
void CCmFmUpnpMngr::CancelOperation()
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::CancelOperation()"));
    
    if( iBrowseSession )
        {
        iAVController->StopBrowsingSession( *iBrowseSession );
		iBrowseSession = NULL;
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::Uri
// ---------------------------------------------------------------------------
// 
TDesC8& CCmFmUpnpMngr::Uri() const
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::Uri()"));
    
    return *iURI;
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::UpnpItem
// ---------------------------------------------------------------------------
// 
CUpnpItem& CCmFmUpnpMngr::UpnpItem() const
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::UpnpItem()"));
    
    return *iItem;
    }
    
// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::BrowseResponse
// ---------------------------------------------------------------------------
// 
void CCmFmUpnpMngr::BrowseResponse(
                    const TDesC8& aBrowseResponse,
                    TInt aError,
                    TInt /*aMatches*/,
                    TInt aTotalCount,
                    const TDesC8& /*aUpdateId*/
                    )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::BrowseResponse()"));   
    
    TRACE(Print(_L("[FILL MNGR]\t ParseResultDataL error = %d"), aError ));
    
    TRAPD( err, BrowseResponseL( aError, aTotalCount, aBrowseResponse ) );
    if ( err )
        {
        TRACE(Print(_L("[FILL MNGR]\t BrowseResponseL err = %d"), err ));
        }    
    }
                    
// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::BrowseResponseL
// ---------------------------------------------------------------------------
//    
void CCmFmUpnpMngr::BrowseResponseL( TInt aStatus,
                                     TInt /*aTotalCount*/,
                                     const TDesC8& aResultArray )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::BrowseResponseL()"));

    RPointerArray<CUpnpObject> tempArray;
    CleanupClosePushL( tempArray );
    TInt err( aStatus );
    TRACE(Print(_L("[FILL MNGR]\t BrowseResponse err = %d"), err )); 
    if( !err )
        {
        TRAP( err, iParser->ParseResultDataL(
            tempArray, aResultArray ) );        
        }
                   
    /** Browse finished */
    iBrowseFinished.HomeTime();
    TTimeIntervalMicroSeconds usecsFrom = 
        iBrowseFinished.MicroSecondsFrom( iBrowseStarted );
    TRACE(Print(_L("[FILL MNGR]\t Browse took = %ld microseconds"), 
        usecsFrom.Int64() ));
    
    if( !err )
        {
    	if ( tempArray.Count() )
    		{
    		if( iItem )
    		    {
    		    delete iItem;
    		    iItem = NULL;
    		    }
    		iItem = CUpnpItem::NewL();
    		// first object is an item, safe to downcast
    		CUpnpItem* item = static_cast<CUpnpItem*>( tempArray[0] );
    		iItem->CopyL( *tempArray[0] );

    		const CUpnpElement& res = UPnPItemUtility::ResourceFromItemL( *item );      		
    		if( iSelectOptimalImageSize )
    		    {
    		    RUPnPElementsArray array;
    		    UPnPItemUtility::GetResElements( *tempArray[0], array );
    		    const CUpnpElement& res = ParseImageResolutions( array );  		    
    		    }

            if(iURI)
                {
                delete iURI;
                iURI = NULL;            
                }                  	        	
            iURI = res.Value().AllocL();
            iObserver->URICheckResult( EUriChecked, item, &res );
    		}
        else
            {
            LOG(_L("[FILL MNGR]\t tempArray.Count() = 0"));
            iObserver->URICheckResult( ENoUriAvailable );
            }        
        }
    else
        {
        LOG(_L("[FILL MNGR]\t err != 0"));
        iObserver->URICheckResult( ENoUriAvailable );
        }

    tempArray.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &tempArray );            
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::SearchResponse
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::SearchResponse( 
                const TDesC8& /*aSearchResponse*/,
                 TInt /*aError*/,
                 TInt /*aMatches*/,
                 TInt /*aTotalCount*/,
                 const TDesC8& /*aUpdateId*/
                 )
    {
    // None
    }
        
// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::SearchResponse
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::SearchResponse( 
                 TInt /*aStatus*/,
                 TInt /*aTotalCount*/,
                 const RPointerArray<CUpnpObject>& /*aResultArray*/ 
                 )
    {
    // None
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::SearchCapabilitiesResponse
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::SearchCapabilitiesResponse( 
                 TInt /*aStatus*/,
                 const TDesC8& /*aSearchCapabilities*/ 
                 )
    {
    // None
    }                 

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::CreateContainerResponse
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::CreateContainerResponse( TInt /*aError*/, 
                                             const TDesC8& /*aObjectId*/ )
    {
    // None
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::DeleteObjectResponse
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::DeleteObjectResponse( TInt /*aStatus*/ )
    {
    // None
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::MediaServerDisappeared
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::MediaServerDisappeared(
    TUPnPDeviceDisconnectedReason /*aReason*/ )
    {
    // None
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::ReserveLocalMSServicesCompleted
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::ReserveLocalMSServicesCompleted( TInt /*aStatus*/ )
    {
    // None
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::UPnPDeviceDiscovered
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::UPnPDeviceDiscovered( const CUpnpAVDevice& /*aDevice*/ )
    {
    }
  
// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::UPnPDeviceDisappeared
// ---------------------------------------------------------------------------
//    
void CCmFmUpnpMngr::UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::UPnPDeviceDisappeared"));
    if( iDevice )
        {
        if( KErrNotFound != iDevice->Uuid().Match( aDevice.Uuid() ) )
            {
            LOG(_L("[FILL MNGR]\t Used server disappeared!"));
            LOG(_L("[FILL MNGR]\t Canceling"));
            iObserver->URICheckResult( ECanceled );
            }        
        }
    }    

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::WLANConnectionLost
// ---------------------------------------------------------------------------
// 
void CCmFmUpnpMngr::WLANConnectionLost()
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::WLANConnectionLost"));
    iObserver->URICheckResult( ECanceled );
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::ParseImageResolutions
// ---------------------------------------------------------------------------
// 
const CUpnpElement& CCmFmUpnpMngr::ParseImageResolutions( 
    RUPnPElementsArray& aResElementsArray )
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::ParseImageResolutions"));
    
    TInt elementIndex( KErrNone );
    TSize matchingSize( 0, 0 );
    for( TInt i = 0 ; i < aResElementsArray.Count(); i++ )
        {
        const CUpnpAttribute* attribute = 
            UPnPItemUtility::FindAttributeByName( *aResElementsArray[i], 
                KAttributeResolution() ); 
        
        if( attribute )
            {
            TPtrC8 resolution( attribute->Value() );
            TInt index = resolution.Find( KCmFmXMark );
            if( index != KErrNotFound )
                {
                TLex8 lexH( resolution.Mid( index + 1 ) );
                TInt height( KErrNone );
                TInt err = lexH.Val( height ); 
                TRACE(Print(_L("[FILL MNGR]\t Val( height ) = %d"), err ));
                
                TInt width( KErrNone );
                TLex8 lexW( resolution.Mid( 0, index ) );
                err = lexW.Val( width );
                TRACE(Print(_L("[FILL MNGR]\t Val( width ) = %d"), err ));
                
                TRACE(Print(_L("[FILL MNGR]\t Sizes in landscape mode!!!")));
                TRACE(Print(_L("[FILL MNGR]\t Image heigth = %d"), height ));
                TRACE(Print(_L("[FILL MNGR]\t Image width = %d"), width ));
                if( height >= iScreenSize.iWidth && width >= 
                    iScreenSize.iHeight )
                    {
                    if( matchingSize.iHeight == 0 )
                        {
                        matchingSize.iHeight = height;
                        matchingSize.iWidth = width;
                        elementIndex = i;
                        }
                    else
                        {
                        if( matchingSize.iHeight > height && 
                            matchingSize.iWidth > width )
                            {
                            matchingSize.iHeight = height;
                            matchingSize.iWidth = width;
                            elementIndex = i;
                            }
                        
                        }                
                    }                           
                }            
            }
        else
            {
            LOG(_L("[FILL MNGR]\t attribute == NULL"));
            }            
        }
    TRACE(Print(_L("[FILL MNGR]\t Selected height = %d"), 
        matchingSize.iHeight ));
    TRACE(Print(_L("[FILL MNGR]\t Selected width = %d"), 
        matchingSize.iWidth ));
    TRACE(Print(_L("[FILL MNGR]\t Selected elementIndex = %d"), 
        elementIndex ));
    return *aResElementsArray[ elementIndex ];
    }

// ---------------------------------------------------------------------------
// CCmFmUpnpMngr::ScreenSizeL
// ---------------------------------------------------------------------------
//
void CCmFmUpnpMngr::ScreenSizeL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::ScreenSizeL start"));   
    RWsSession session;
    TInt error = session.Connect() ;
    CleanupClosePushL( session );
    if ( !error )
        {
        CWsScreenDevice* screenDevice = 
            new ( ELeave ) CWsScreenDevice( session );
        if ( screenDevice && !screenDevice->Construct() )
            {
            iScreenSize = screenDevice->SizeInPixels();
            }
        delete screenDevice;
        screenDevice = NULL;
        }
    else
        {
        TRACE(Print(_L("[FILL MNGR]\t ScreenSizeL error = %d"), error ));        
        }    

    CleanupStack::PopAndDestroy( &session );
    LOG(_L("[FILL MNGR]\t CCmFmUpnpMngr::ScreenSizeL end"));
    }
    
// End of file
