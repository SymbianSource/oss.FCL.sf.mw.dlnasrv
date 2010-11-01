/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      server impl. of session against media server
*
*/


// INCLUDE FILES
// System
#include <utf.h> 
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <centralrepository.h>

// upnp stack api
#include <upnpstring.h>
#include <upnpdevice.h>
#include <upnpservice.h>

// dlnasrv / mediaserver api
#include <upnpcontainer.h>
#include <upnpitem.h>
#include <upnpelement.h>
#include <upnpmediaserversettings.h>
#include <upnpmediaserverclient.h>
#include <upnpmediaservernotifier.h>
#include <upnpfiletransferevent.h>

// dlnasrv / avcontroller api
#include "upnpavrenderingsessionobserver.h"

// dlnasrv / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpfileutility.h"
#include "upnpitemutility.h"

// dlnasrv / xmlparser api
#include "upnpxmlparser.h"

// dlnasrv / internal api's
#include "upnpcdsreselementutility.h"
#include "upnpmetadatafetcher.h"

// dlnasrv / avcontroller internal
#include "upnpavdispatcher.h"
#include "upnpavbrowserequest.h"
#include "upnpavrequest.h"
#include "upnpaverrorhandler.h"
#include "upnpavdeviceextended.h"
#include "upnpdevicerepository.h"
#include "upnpavbrowserespparams.h"
#include "upnpavcontrollerserver.h"
#include "upnpavcpstrings.h"
#include "upnpbrowsingsession.h"
#include "upnpavcontrolpoint.h"

using namespace UpnpAVCPStrings;


// CONSTANTS
_LIT8( KDirectChildren,         "BrowseDirectChildren" );
_LIT8( KMetaData,               "BrowseMetadata" );
_LIT8( KBrowseMetadata,         "BrowseMetadata" );
_LIT8( KDefaultStartingIndex,   "0" );
_LIT8( KDefaultRequestedCount,  "1" );

const TInt KDefaultInstanceId   = 0;
const TInt KExpectedCount       = 1;
const TInt KMaxIntLength        = 10;

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::NewL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession* CUPnPBrowsingSession::NewL
    (
    CUpnpAVControllerServer& aServer,
    TInt aSessionId,
    const TDesC8& aUuid
    )
    {
    CUPnPBrowsingSession* self = new (ELeave) CUPnPBrowsingSession(
        aServer, aSessionId );
    CleanupStack::PushL( self );    
    
    self->ConstructL( aUuid );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CUPnPBrowsingSession
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession::CUPnPBrowsingSession
    (
    CUpnpAVControllerServer& aServer,
    TInt aSessionId
    ):
    iServer( aServer ),
    iSessionId( aSessionId ),
    
    iInstanceId( KDefaultInstanceId ),
    iIPSessionId( KErrNotFound ),

    iInternalState( ENone )
    {
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::~CUPnPBrowsingSession
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
CUPnPBrowsingSession::~CUPnPBrowsingSession()
    {
    iIPSessionId = KErrNotFound;
    
    delete iDevice;
    delete iRespBuf;
    delete iRespBuf2;
    
    delete iItemId;
    
    delete iLocalMediaServerUuid; 
    
    delete iActionMessage;
    delete iDeviceMessage;
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ConstructL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPBrowsingSession::ConstructL" );

    // Get the local Media Server Uuid, if available
    const RPointerArray<CUpnpAVDeviceExtended>& devList =
        iServer.DeviceRepository().DeviceList();
    TInt count = devList.Count();
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( devList[ i ]->Local() )
            {
            __ASSERT( !iLocalMediaServerUuid, __FILE__, __LINE__ );
            iLocalMediaServerUuid = devList[i]->Uuid().AllocL();
            }
        if( devList[ i ]->Uuid() == aUuid )
            {
            __ASSERT( !iDevice, __FILE__, __LINE__ );
            iDevice = CUpnpAVDeviceExtended::NewL( *devList[ i ] );
            }             
        }
    if( !iDevice )
        {
        if( aUuid == KNullDesC8 ) // Fix to enable AV Controller helper usage
            {
            iDevice = CUpnpAVDeviceExtended::NewL();
            }
        else
            {
            User::Leave( KErrNotFound );    
            }    
        }
    else
        {
        //
        // Gets related control point device (CUpnpDevice) only if aUuid 
        // is provided. It is needed when CUpnpAVControlPoint is really 
        // used.
        //
        // In some cases CUPnPBrowsingSession is be instantiated with dummy
        // device uuid (KNullDesC8). In these cases we shouldn't leave nor
        // get the control point device.
        iCpDevice = iServer.ControlPoint().Device( aUuid );
        if ( !iCpDevice ) 
            {
            User::Leave( KErrNotFound );
            }
        }
    }



// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ActionResponseL
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ActionResponseL(CUpnpAction* aAction)
    {
    __ASSERT( aAction->SessionId()==iIPSessionId, __FILE__, __LINE__ );

    if (aAction->Name().Compare(KGetSearchCapabilities) == 0)
        {
        CdsSearchCapabilitiesResponse(
            aAction->Error(),
            aAction->ArgumentValue( KSearchCaps ) );
        }
    else if (aAction->Name().Compare(KBrowse) == 0)
        {
        const TDesC8& numberReturned = aAction->ArgumentValue( 
                KNumberReturned );
        TLex8 returnedLex( numberReturned );
        TInt numberReturnedInt;
        User::LeaveIfError( returnedLex.Val( numberReturnedInt ) );
        
        const TDesC8& totalmatches = aAction->ArgumentValue( KTotalMatches );
        TLex8 matchesLex( totalmatches );
        TInt totalMatchesInt;
        User::LeaveIfError( matchesLex.Val( totalMatchesInt ) );
   
        CdsBrowseResponseL(
            aAction->Error(),
            aAction->ArgumentValue( KBrowseFlag ),
            aAction->ArgumentValue( KResult ),
            numberReturnedInt,
            totalMatchesInt,
            aAction->ArgumentValue( KUpdateID ) );
        }
    else if (aAction->Name().Compare(KSearch) == 0)
        {		
        const TDesC8& numberReturned = aAction->ArgumentValue( 
                KNumberReturned );
        TLex8 returnedLex( numberReturned );
        TInt numberReturnedInt;
        User::LeaveIfError( returnedLex.Val( numberReturnedInt ) );

        const TDesC8& totalmatches = aAction->ArgumentValue( KTotalMatches );
        TLex8 matchesLex( totalmatches );
        TInt totalMatchesInt;
        User::LeaveIfError( matchesLex.Val( totalMatchesInt ) );

        CdsSearchResponse(
            aAction->Error(),
            aAction->ArgumentValue( KResult ),
            numberReturnedInt,
            totalMatchesInt,
            aAction->ArgumentValue( KUpdateID )
            );
        }
    else if (aAction->Name().Compare(KDestroyObject) == 0)
        {		
        CdsDestroyObjectResponse(
            aAction->Error() );
        }
    else if (aAction->Name().Compare(KCreateObject) == 0)
        {
        CdsCreateObjectResponse(
            aAction->Error(),
            aAction->ArgumentValue(KObjectID), 
            aAction->ArgumentValue(KResult)
            );
        }
    else
        {
        __LOG( "response not expected, ignore" );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::StateUpdatedL
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::StateUpdatedL(CUpnpService* /*aService*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::HttpResponseL
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::HttpResponseL(CUpnpHttpMessage* /*aMessage*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDiscoveredL
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDiscoveredL(CUpnpDevice* /*aDevice*/)
    {
    // No implementation required        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedL
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedL(CUpnpDevice* /*aDevice*/)
    {
    // No implementation required        
    }



// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSearchCapabilitiesResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSearchCapabilitiesResponse(
    TInt aErr,
    const TDesC8& aSearchCaps)
    {
    __LOG1( "CUPnPBrowsingSession::CdsSearchCapabilitiesResponse: %d",
        aErr );

    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            if(  aSearchCaps != KNullDesC8 )
                {
                delete iRespBuf; iRespBuf = NULL;
                iRespBuf = aSearchCaps.Alloc();
                if( iRespBuf )
                    {
                    TPckgBuf<TInt> resp1( aSearchCaps.Length() );
                    iActionMessage->Write( 1, resp1 );
                    }
                else
                    {
                    TPckgBuf<TInt> resp1( 0 );
                    iActionMessage->Write( 1, resp1 );
                    }
                
                iActionMessage->Complete(
                    EAVControllerGetSearchCapabilitiesSizeCompleted );      
                delete iActionMessage; iActionMessage = NULL;
                }
            else
                {
                TPckgBuf<TInt> resp1( 0 );
                iActionMessage->Write( 1, resp1 );

                iActionMessage->Complete(
                    EAVControllerGetSearchCapabilitiesSizeCompleted );
                delete iActionMessage; iActionMessage = NULL;
                }    
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            }        
        }
    else
        {
        __LOG( "CdsSearchCapabilitiesResponse - no msg" );
        }
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsBrowseResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsBrowseResponseL(
    TInt aErr,
    const TDesC8&  aBrowseFlag,
    const TDesC8&  aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8&  aUpdateID )
    {
    __LOG1( "CUPnPBrowsingSession::CdsBrowseResponseL: %d", aErr );

    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    if( iActionMessage )
        {
        if (  aBrowseFlag.CompareF( KBrowseMetadata ) == 0 && aReturned == 0 
                && aErr != KErrCouldNotConnect && aErr != KErrHostUnreach )
            {
            aErr = ENoSuchObject;  //the file not exist;
            }
        
        if ( aErr != KErrCouldNotConnect && aErr != KErrHostUnreach )
            {
            aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );
            __LOG1( "CUPnPBrowsingSession::CdsBrowseResponseL:001 %d", 
                    aErr );
            }    
    
        if( aErr == KErrNone )
            {
            if( aResult != KNullDesC8 )
                {
                if( iInternalState == EDestroyObject )
                    {
                    TRAP( aErr, CheckAndSendDestroyObjectActionL( aResult) );
                    if( aErr )    
                        {
                        iInternalState = ENone;
                        iActionMessage->Complete( aErr );
                        delete iActionMessage; iActionMessage = NULL;
                        }
                    }    
                else // Browse
                    {
                    delete iRespBuf; iRespBuf = NULL;
                    delete iRespBuf2; iRespBuf2 = NULL;
                    iRespBuf = aResult.Alloc();
                    iRespBuf2 = aUpdateID.Alloc();
                         
                    if( iRespBuf && iRespBuf2 )
                        {
                        TUpnpAVBrowseRespParams params;
                        TPckg<TUpnpAVBrowseRespParams> resp2( params );
                        params.iMatches = aReturned ;
                        params.iTotalCount = aMatches;
                        params.iResponseSize = aResult.Length();
                        params.iUpdateIdSize = aUpdateID.Length();
                        iActionMessage->Write( 2, resp2 );
                        iActionMessage->Complete( 
                            EAVControllerGetBrowseResponseSizeCompleted );
                        }
                    else
                        {
                        delete iRespBuf; iRespBuf = NULL;
                        delete iRespBuf2; iRespBuf2 = NULL;
                        iActionMessage->Complete( KErrNoMemory );
                        }
                    iInternalState = ENone;
                    delete iActionMessage; iActionMessage = NULL;      
                    }    
                }
            else
                {
                if( iInternalState == EBrowse )
                    {
                    TUpnpAVBrowseRespParams params;
                    TPckg<TUpnpAVBrowseRespParams> resp2( params );
                    params.iMatches = 0;
                    params.iTotalCount = 0;
                    params.iResponseSize = 0;
                    params.iUpdateIdSize = 0;
                    iActionMessage->Write( 2, resp2 );

                    iInternalState = ENone;
                    iActionMessage->Complete(
                        EAVControllerGetBrowseResponseSizeCompleted );
                    delete iActionMessage; iActionMessage = NULL;
                    }
                else
                    {
                    iInternalState = ENone;
                    iActionMessage->Complete( aErr );
                    delete iActionMessage; iActionMessage = NULL;
                    }        
                }    
            }
        else
            {
            iInternalState = ENone;
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            if ( KErrCouldNotConnect == aErr || KErrHostUnreach == aErr )
                {
                iServer.DeviceDisappearedL( iDevice->Uuid() );
                }
            }            
        }
    else
        {
        __LOG( "CdsBrowseResponseL - no msg" );
        }    
    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsSearchResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsSearchResponse(
    TInt aErr,
    const TDesC8& aResult,
    TInt aReturned,
    TInt aMatches,
    const TDesC8& aUpdateID )
    {
    __LOG1( "CUPnPBrowsingSession::CdsSearchResponse: %d", aErr );

    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            if(  aResult != KNullDesC8 )
                {
                delete iRespBuf; iRespBuf = NULL;
                delete iRespBuf2; iRespBuf2 = NULL;
                iRespBuf = aResult.Alloc();
                iRespBuf2 = aUpdateID.Alloc();

                if( iRespBuf && iRespBuf2 )
                    {
                    TUpnpAVBrowseRespParams params;
                    TPckg<TUpnpAVBrowseRespParams> resp2( params );
                    params.iMatches = aReturned ;
                    params.iTotalCount = aMatches;
                    params.iResponseSize = aResult.Length();
                    params.iUpdateIdSize = aUpdateID.Length();
                    iActionMessage->Write( 2, resp2 );                    
                    iActionMessage->Complete(
                         EAVControllerGetSearchResponseSizeCompleted );
                    }
                else
                    {
                    delete iRespBuf; iRespBuf = NULL;
                    delete iRespBuf2; iRespBuf2 = NULL;
                    iActionMessage->Complete( KErrNoMemory );
                    }
                delete iActionMessage; iActionMessage = NULL;       
                }
            else
                {
                TUpnpAVBrowseRespParams params;
                TPckg<TUpnpAVBrowseRespParams> resp2( params );
                params.iMatches = 0;
                params.iTotalCount = 0;
                params.iResponseSize = 0;
                params.iUpdateIdSize = 0;
                iActionMessage->Write( 2, resp2 );                    

                iActionMessage->Complete(
                    EAVControllerGetSearchResponseSizeCompleted );
                delete iActionMessage; iActionMessage = NULL;       
                }    
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;       
            }                                     
        }
    else
        {
        __LOG( "CdsSearchResponse - no msg" );
        }        
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsDestroyObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsDestroyObjectResponse(
    TInt aErr )
    {
    __LOG1( "CUPnPBrowsingSession::CdsDestroyObjectResponse: %d", aErr );

    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;

    iInternalState = ENone;
    
    if( iActionMessage )
        {
        aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
            EUPnPContentDirectoryError );    
        
        if( aErr == KErrNone )
            {
            iActionMessage->Complete( EAVControllerDeleteObjectCompleted );
            delete iActionMessage; iActionMessage = NULL;
            }
        else
            {
            iActionMessage->Complete( aErr );
            delete iActionMessage; iActionMessage = NULL;
            }            
        }
    else
        {
        __LOG( "CdsDestroyObjectResponse - no msg" );
        }      
    }





// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CdsCreateObjectResponse
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CdsCreateObjectResponse(
    TInt aErr,
    const TDesC8& aObjectID, 
    const TDesC8& /*aResult*/ )
    {
    __LOG1( "CUPnPBrowsingSession::CdsCreateObjectResponse: %d" , aErr );
    
    iServer.Dispatcher().UnRegister( iIPSessionId );
    iIPSessionId = KErrNotFound;
    
    aErr = UPnPAVErrorHandler::ConvertToSymbianErrorCode( aErr,
        EUPnPContentDirectoryError );

    if( aErr == KErrNone )
        {
        if( iActionMessage )
            {
            HBufC8* objectID = HBufC8::New( aObjectID.Length() );
            if( objectID )
                {
                objectID->Des().Copy( aObjectID );
                iActionMessage->Write( 1, *objectID );
                iActionMessage->Complete(
                    EAVControllerCreateContainerCompleted );
                delete objectID;                
                }
            else
                {
                iActionMessage->Write( 1, KNullDesC8 );
                iActionMessage->Complete( KErrNoMemory );
                }    
            delete iActionMessage; iActionMessage = NULL;
            }
        iInternalState = ENone;                                
        }
    else
        {
        // Create container failed
        iInternalState = ENone;
        iActionMessage->Complete( aErr );
        delete iActionMessage; iActionMessage = NULL;                    
        }                                       
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedL(
    CUpnpAVDeviceExtended& aDevice )
    {
    __LOG( "CUPnPBrowsingSession::DeviceDisappearedL" );

    if( aDevice.Local() )
        {
        delete iLocalMediaServerUuid; iLocalMediaServerUuid = NULL; 
        }
    else if( iDeviceMessage ) // Target device
        {
        iDeviceMessage->Complete( KErrNone );
        delete iDeviceMessage; iDeviceMessage = NULL;
        }     
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SetLocalMSUuidL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::SetLocalMSUuidL( const TDesC8& aUuid )
    {
    HBufC8* tmp = aUuid.AllocL();
    delete iLocalMediaServerUuid;
    iLocalMediaServerUuid = tmp; 
    }
 
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::SessionId
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
TInt CUPnPBrowsingSession::SessionId() const
    {
    return iSessionId;
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::Uuid
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
const TDesC8& CUPnPBrowsingSession::Uuid() const
    {
    if( iDevice )
        {
        return iDevice->Uuid();
        }
    else
        {
        return KNullDesC8;
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetBrowseResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetBrowseResponseSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetBrowseResponseSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();
        
    CUpnpAVBrowseRequest* tmpRequest = CUpnpAVBrowseRequest::NewLC();
    
    ReadBrowseReqFromMessageL( aMessage, 1, tmpRequest );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            iCpDevice, KContentDirectory, KBrowse );
    
    TBuf8<KMaxIntLength> startingIndexStr;
    startingIndexStr.Num(tmpRequest->StartIndex());
    TBuf8<KMaxIntLength> requestedCountStr;
    requestedCountStr.Num(tmpRequest->RequestedCount());
    action->SetArgumentL( KObjectID, tmpRequest->Id() );
    if( tmpRequest->BrowseFlag() == MUPnPAVBrowsingSession::EDirectChildren )
        {
        action->SetArgumentL( KBrowseFlag, KDirectChildren ); 
        }
    else
        {
        action->SetArgumentL( KBrowseFlag, KMetaData ); 
        }
    action->SetArgumentL( KFilter, tmpRequest->Filter() ); 
    action->SetArgumentL( KStartingIndex, startingIndexStr ); 
    action->SetArgumentL( KRequestedCount, requestedCountStr ); 
    action->SetArgumentL( 
            UpnpAVCPStrings::KSortCriteria, 
            tmpRequest->SortCriteria() ); 

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionId = action->SessionId();
    CleanupStack::PopAndDestroy( tmpRequest );
   
    // Register
    iInternalState = EBrowse;
    iServer.Dispatcher().RegisterL( iIPSessionId, *this );
    iActionMessage = new (ELeave) RMessage2( aMessage );    
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetBrowseResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetBrowseResponseSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetBrowseResponseSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;         
        }
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetBrowseResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetBrowseResponseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetBrowseResponseL" );
    
   __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    iIPSessionId = KErrNotFound;
    if( iRespBuf && iRespBuf2 )
        {
        aMessage.WriteL( 1, *iRespBuf );
        aMessage.WriteL( 2, *iRespBuf2 );
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        aMessage.Complete( EAVControllerGetBrowseResponseCompleted );    
        }
    else
        {
        //Memory allocaton failed
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        User::Leave( KErrNoMemory );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchResponseSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchResponseSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();
    
    CUpnpAVBrowseRequest* tmpRequest = CUpnpAVBrowseRequest::NewLC();
    
    ReadBrowseReqFromMessageL( aMessage, 1, tmpRequest );

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            iCpDevice, KContentDirectory, KSearch );
    
    TBuf8<KMaxIntLength> startingIndexStr;
    startingIndexStr.Num(tmpRequest->StartIndex());
    TBuf8<KMaxIntLength> requestedCountStr;
    requestedCountStr.Num(tmpRequest->RequestedCount());
    action->SetArgumentL( KContainerID, tmpRequest->Id() );
    action->SetArgumentL( KSearchCriteria, tmpRequest->SearchCriteria() ); 
    action->SetArgumentL( KFilter, tmpRequest->Filter() ); 
    action->SetArgumentL( KStartingIndex, startingIndexStr ); 
    action->SetArgumentL( KRequestedCount, requestedCountStr ); 
    action->SetArgumentL( 
            UpnpAVCPStrings::KSortCriteria, 
            tmpRequest->SortCriteria() ); 

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionId = action->SessionId();
    CleanupStack::PopAndDestroy( tmpRequest );

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionId, *this );
    iActionMessage = new (ELeave) RMessage2( aMessage );      
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetSearchResponseSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetSearchResponseSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetSearchResponseSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;               
        }
    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchResponseL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchResponseL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchResponseL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    iIPSessionId = KErrNotFound;
    if( iRespBuf && iRespBuf2 )
        {
        aMessage.WriteL( 1, *iRespBuf );
        aMessage.WriteL( 2, *iRespBuf2 );
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        aMessage.Complete( EAVControllerGetSearchResponseCompleted );    
        }
    else
        {
        //Memory allocaton failed
        delete iRespBuf; iRespBuf = NULL;
        delete iRespBuf2; iRespBuf2 = NULL;
        User::Leave( KErrNoMemory );
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchCapabitiesSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchCapabitiesSizeL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchCapabitiesSizeL" );
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );

    ResetL();

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            iCpDevice, KContentDirectory, KGetSearchCapabilities );
    
    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionId = action->SessionId();

    // Register
    iServer.Dispatcher().RegisterL( iIPSessionId, *this );
    iActionMessage = new (ELeave) RMessage2( aMessage );    
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL()
    {
    __LOG( "CUPnPBrowsingSession::CancelGetSearchCapabitiesSizeL" );
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;                   
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::GetSearchCapabitiesL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::GetSearchCapabitiesL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::GetSearchCapabitiesL" );
    
    iIPSessionId = KErrNotFound;
    
    aMessage.WriteL( 1, *iRespBuf );
    aMessage.Complete( EAVControllerGetSearchCapabilitiesCompleted );    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CreateContainerL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CreateContainerL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::CreateContainerL" );       
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );
    
    ResetL();

    // Title
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* tempTitle = HBufC8::NewLC( len );
    TPtr8 ptr( tempTitle->Des() );
    aMessage.ReadL( 1, ptr );

    // Container ID
    len = aMessage.GetDesMaxLength( 2 );
    HBufC8* tempId = HBufC8::NewLC( len );
    ptr.Set( tempId->Des() );
    aMessage.ReadL( 2, ptr );
    
    if( iDevice->DlnaCompatible() && !iDevice->CreateChildContainer()
        && *tempId != KContainerIdAny )
        {
        // The device is DLNA compatible and does not support creation
        // of a child container
        User::Leave( KErrNotSupported );
        }
            
    // Container type
    MUPnPAVBrowsingSession::TContainerType type =
            (MUPnPAVBrowsingSession::TContainerType)aMessage.Int3();

    // Create a container object
    CUpnpContainer* tmpContainer = CUpnpContainer::NewL();
    CleanupStack::PushL( tmpContainer );
    
    // Set the title and the parent ID
    tmpContainer->SetTitleL( *tempTitle );
    tmpContainer->SetParentIdL( *tempId );
    
    // Set the object type
    if( type == MUPnPAVBrowsingSession::EPlaylistContainer )
        {
        tmpContainer->SetObjectClassL( KClassPlaylist() );
        }
    else
        {
        tmpContainer->SetObjectClassL( KClassStorage() );
        }

    HBufC8* xmlDoc = CUPnPXMLParser::ContainerToXmlLC( *tmpContainer );  

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            iCpDevice, KContentDirectory, KCreateObject );
    
    action->SetArgumentL( KContainerID, *tempId );
    action->SetArgumentL( KElements, *xmlDoc );

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionId = action->SessionId();

    CleanupStack::PopAndDestroy( xmlDoc );
    CleanupStack::PopAndDestroy( tmpContainer );
    CleanupStack::PopAndDestroy( tempId );
    CleanupStack::PopAndDestroy( tempTitle );
   
    // Register
    iServer.Dispatcher().RegisterL( iIPSessionId, *this );
    iInternalState = ECreateContainer; 
    iActionMessage = new (ELeave) RMessage2( aMessage );         
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelCreateContainerL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelCreateContainerL()
    {
    __LOG( "CUPnPBrowsingSession::CancelCreateContainerL" );       
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );
        delete iActionMessage; iActionMessage = NULL;        
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeleteObjectL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeleteObjectL( const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::DeleteObjectL" );       
    
    __ASSERTD( !iActionMessage, __FILE__, __LINE__ );
    
    ResetL();
    
    TInt len = aMessage.GetDesMaxLength( 1 );
    HBufC8* tempId = HBufC8::NewLC( len );
    TPtr8 ptr( tempId->Des() );
    aMessage.ReadL( 1, ptr );
    
    CleanupStack::Pop( tempId );
    delete iItemId;
    iItemId = tempId;

    CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
            iCpDevice, KContentDirectory, KBrowse );
    
    action->SetArgumentL( KObjectID, *iItemId );
    action->SetArgumentL( KBrowseFlag, KMetaData ); 
    action->SetArgumentL( KFilter, KFilterCommon ); 
    action->SetArgumentL( KStartingIndex, KDefaultStartingIndex ); 
    action->SetArgumentL( KRequestedCount, KDefaultRequestedCount ); 

    iServer.ControlPoint().SendL( action );
    CleanupStack::Pop( action );
    if (action->SessionId() < 0) User::Leave( action->SessionId() );
    iIPSessionId = action->SessionId();

    // Register
    iInternalState = EDestroyObject;
    iServer.Dispatcher().RegisterL( iIPSessionId, *this );
    iActionMessage = new (ELeave) RMessage2( aMessage );   
    }
        
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelDeleteObjectL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelDeleteObjectL()
    {
    __LOG( "CUPnPBrowsingSession::CancelDeleteObjectL" );       
    
    if( iActionMessage )
        {
        iServer.Dispatcher().UnRegister( iIPSessionId );
        iActionMessage->Complete( KErrCancel );    
        delete iActionMessage; iActionMessage = NULL;
        }
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::DeviceDisappearedRequestL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::DeviceDisappearedRequestL(
    const RMessage2& aMessage )
    {
    __LOG( "CUPnPBrowsingSession::DeviceDisappearedRequestL" );       
    
    __ASSERTD( !iDeviceMessage, __FILE__, __LINE__ );
    
    iDeviceMessage = new (ELeave) RMessage2( aMessage );
    }
    
// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CancelDeviceDisappearedRequestL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CancelDeviceDisappearedRequestL()
    {
    __LOG( "CUPnPBrowsingSession::CancelDeviceDisappearedRequestL" );       
    
    if( iDeviceMessage )
        {
        iDeviceMessage->Complete( KErrCancel );    
        delete iDeviceMessage; iDeviceMessage = NULL;    
        }
    }


// --------------------------------------------------------------------------
// CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL(
    const TDesC8& aResponse )
    {
    __LOG( "CUPnPBrowsingSession::CheckAndSendDestroyObjectActionL" );
    
    CUPnPXMLParser* parser = CUPnPXMLParser::NewL();
    CleanupStack::PushL( parser );
    
    RPointerArray<CUpnpObject> array;
    CleanupResetAndDestroyPushL( array );
    
    parser->ParseResultDataL( array, aResponse );
    
    if( array.Count() == KExpectedCount )
        {
        if( array[ 0 ]->Restricted() )
            {
            User::Leave( KErrArgument );
            }
        else
            {
            // Not restricted, ok to destroy
            CUpnpAction* action = iServer.ControlPoint().CreateActionLC( 
                    iCpDevice, KContentDirectory, KDestroyObject );
            
            action->SetArgumentL( KObjectID, *iItemId );

            iServer.ControlPoint().SendL( action );
            CleanupStack::Pop( action );
            if (action->SessionId() < 0) 
                {
                User::Leave( action->SessionId() );   
                }
            iIPSessionId = action->SessionId();
            iServer.Dispatcher().RegisterL( iIPSessionId, *this );
            }    
        }
    else
        {
        User::Leave( KErrGeneral );
        }    
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( parser );    
    }



// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ResetL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ResetL()
    {
    __LOG( "CUPnPBrowsingSession::ResetL" );
    
    iIPSessionId = KErrNotFound;
    
    if( !iServer.DeviceRepository().IsWlanActive() )    
        {
        __LOG( "Reset - disconnected" );
        User::Leave( KErrDisconnected );
        }    
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadObjFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadObjFromMessageL( const RMessage2& aMessage, 
    TInt aSlot, CUpnpObject* aObj ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aObj;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }    

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadReqFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadReqFromMessageL( const RMessage2& aMessage, 
    TInt aSlot, CUpnpAVRequest* aReq ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aReq;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadBrowseReqFromMessageL
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
void CUPnPBrowsingSession::ReadBrowseReqFromMessageL(
    const RMessage2& aMessage, TInt aSlot, CUpnpAVBrowseRequest* aReq ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aReq;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }

// --------------------------------------------------------------------------
// CUPnPBrowsingSession::ReadBufFromMessageLC
// See upnpbrowsingsession.h
// --------------------------------------------------------------------------
HBufC8* CUPnPBrowsingSession::ReadBufFromMessageLC(
    const RMessage2& aMessage, TInt aSlot ) 
    {
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );        
    return buf;
    }    
    
// End of file
