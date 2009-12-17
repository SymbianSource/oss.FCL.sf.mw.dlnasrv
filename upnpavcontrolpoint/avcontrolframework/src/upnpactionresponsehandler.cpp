/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CUpnpActionResponseHandler
*
*/


// INCLUDE FILES
#include "upnpactionresponsehandler.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpstring.h"

// CONSTANTS]
using namespace UpnpAVCPStrings;


// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::CUpnpActionResponseHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpActionResponseHandler::CUpnpActionResponseHandler( 
            MUpnpAVControlPointObserver& aAVCPObserver, CUpnpAVControlPoint& aAVCP )
    :iAVControlPointObserver( aAVCPObserver ), iAVCP( aAVCP )
    {
	}

// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpActionResponseHandler::ConstructL() 
	{
	}

// -----------------------------------------------------------------------------
// CSuperDir::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpActionResponseHandler* CUpnpActionResponseHandler::NewL( 
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP )
    {
	CUpnpActionResponseHandler* self = new (ELeave) CUpnpActionResponseHandler( 
	    aAVCPObserver, aAVCP );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// Destructor
CUpnpActionResponseHandler::~CUpnpActionResponseHandler()	
	{
	}
// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::ActionResponseReceived
// This is where action responses first came.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpActionResponseHandler::ActionResponseReceived(CUpnpAction* aAction)
    {
    CUpnpService& service = aAction->Service();
    if( service.ServiceType().Length() <= 0 )
        {
        return KErrArgument;
        }
    TInt err( KErrNone );
    if ( service.ServiceType().Match(KRenderingControl) != KErrNotFound )
        {
        err = RenderingControlResponse(aAction);
        }
    else if ( service.ServiceType().Match(KAVTransport) != KErrNotFound )
        {
        err = AVTransportResponse(aAction);
        }
    else if ( service.ServiceType().Match(KConnectionManager) != KErrNotFound )
        {
        err = ConnectionManagerResponse(aAction);
        }
    else if ( service.ServiceType().Match(KContentDirectory) != KErrNotFound )
        {
        err = ContentDirectoryResponse(aAction);
        }
    else
        {
        err = KErrNotSupported;
        }
    return err;
    }
// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::RenderingControlResponse
// This is where action responses first came.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpActionResponseHandler::RenderingControlResponse(CUpnpAction* aAction)
    {
	CUpnpService& service = aAction->Service();
	if( service.ServiceType().Length() <= 0 )
		{
		return KErrArgument;
		}
    const TDesC8& uuid = service.Device().Uuid();
    if (aAction->Name().Compare(KSetVolume) == 0)
        {
        iAVControlPointObserver.RcSetVolumeResponse(
             uuid,
             aAction->SessionId(),
             aAction->Error(),
             aAction->ArgumentValue( KInstanceID ),
             aAction->ArgumentValue( KChannel ),
             aAction->ArgumentValue( KDesiredVolume ) 
             );
        }		
    else if (aAction->Name().Compare(KGetVolume) == 0)
        {
        iAVControlPointObserver.RcVolumeResponse( 
             uuid,
             aAction->SessionId(),
             aAction->Error(),
             aAction->ArgumentValue( KInstanceID ),
             aAction->ArgumentValue( KChannel ),
             aAction->ArgumentValue( KCurrentVolume ) 
             );
        }    
    else if (aAction->Name().Compare(KSetMute) == 0)
        {
        iAVControlPointObserver.RcSetMuteResponse( 
             uuid,
             aAction->SessionId(),
             aAction->Error(),
             aAction->ArgumentValue( KInstanceID ),
             aAction->ArgumentValue( KChannel ),
             aAction->ArgumentValue( KDesiredMute ) 
             );
        }	
    else if (aAction->Name().Compare(KGetMute) == 0)
        {				
        iAVControlPointObserver.RcMuteResponse( 
             uuid,
             aAction->SessionId(),
             aAction->Error(),
             aAction->ArgumentValue( KInstanceID ),
             aAction->ArgumentValue( KChannel ),
             aAction->ArgumentValue( KCurrentMute ) 
             );
        }
    else
        {
        return KErrNotSupported;
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::AVTransportResponse
// This is where action responses fot AV transport service are handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpActionResponseHandler::AVTransportResponse(CUpnpAction* aAction)
    {
       
    CUpnpService& service = aAction->Service();
	if( service.ServiceType().Length() <= 0 )
		{
		return KErrArgument;
		}
    const TDesC8& uuid = service.Device().Uuid();
    if (aAction->Name().Compare(KSetAVTransportURI) == 0)
        {
        iAVControlPointObserver.AvtSetTransportUriResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KCurrentURI ),
            aAction->ArgumentValue( KCurrentURIMetaData )
            );
        }
    else if (aAction->Name().Compare(KSetNextAVTransportURI) == 0)
        {
        iAVControlPointObserver.AvtSetNextTransportUriResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KNextURI ),
            aAction->ArgumentValue( KNextURIMetaData )
            );
        }
    else if (aAction->Name().Compare(KGetMediaInfo) == 0)
        {
        iAVControlPointObserver.AvtMediaInfoResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KNrTracks ),
            aAction->ArgumentValue( KMediaDuration ),
            aAction->ArgumentValue( KCurrentURI ),
            aAction->ArgumentValue( KCurrentURIMetaData ),
            aAction->ArgumentValue( KNextURI ),
            aAction->ArgumentValue( KNextURIMetaData ),
            aAction->ArgumentValue( KPlayMedium ),
            aAction->ArgumentValue( KRecordMedium ),
            aAction->ArgumentValue( KWriteStatus )
            );
        }
    else if (aAction->Name().Compare(KGetTransportInfo) == 0)
        {			
        iAVControlPointObserver.AvtGetTransportInfoResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KCurrentTransportState ),
            aAction->ArgumentValue( KCurrentTransportStatus ),
            aAction->ArgumentValue( KCurrentSpeed )
            );
        }
    else if (aAction->Name().Compare(KGetPositionInfo) == 0)
        {
        iAVControlPointObserver.AvtPositionInfoResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KTrack ),
            aAction->ArgumentValue( KTrackDuration),
            aAction->ArgumentValue( KTrackMetaData ),
            aAction->ArgumentValue( KTrackURI ),
            aAction->ArgumentValue( KRelTime ),
            aAction->ArgumentValue( KAbsTime ),
            aAction->ArgumentValue( KRelCount ),
            aAction->ArgumentValue( KAbsCount )
            );
        }
    else if (aAction->Name().Compare(KGetDeviceCapabilities) == 0)
        {				
        iAVControlPointObserver.AvtDeviceCapabilitiesResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KPlayMedia ),
            aAction->ArgumentValue( KRecMedia ),
            aAction->ArgumentValue( KRecQualityMode )
            );
        }
    else if (aAction->Name().Compare(KGetTransportSettings) == 0)
        {				
        iAVControlPointObserver.AvtTransportSettingsResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KPlayMode ),
            aAction->ArgumentValue( KRecQualityMode )
            );
        }
    else if (aAction->Name().Compare(KStop) == 0)
        {				
        iAVControlPointObserver.AvtStopResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID )
            );
        }
    else if (aAction->Name().Compare(KPlay) == 0)
        {				
        iAVControlPointObserver.AvtPlayResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KSpeed )
            );
        }
    else if (aAction->Name().Compare(KPause) == 0)
        {		
        iAVControlPointObserver.AvtPauseResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID )
            );
        }
    else if (aAction->Name().Compare(KRecord) == 0)
        {		
        iAVControlPointObserver.AvtRecordResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID )
            );
        }
    else if (aAction->Name().Compare(KSeek) == 0)
        {	
        iAVControlPointObserver.AvtSeekResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KUnit ),
            aAction->ArgumentValue( KTarget )
            );
        }
    else if (aAction->Name().Compare(KNext) == 0)
        {
        iAVControlPointObserver.AvtNextResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID )
            );
        }
    else if (aAction->Name().Compare(KPrevious) == 0)
        {
        iAVControlPointObserver.AvtPreviousResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID )
            );
        }
    else if (aAction->Name().Compare(KSetPlayMode) == 0)
        {
        iAVControlPointObserver.AvtSetPlayModeResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KNewPlayMode )
            );
        }
    else if (aAction->Name().Compare(KSetRecordQualityMode) == 0)
        {
        iAVControlPointObserver.AvtSetRecordModeResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KNewRecordQualityMode )
            );
        }
    else if (aAction->Name().Compare(KGetCurrentTransportActions) == 0)
        {
        iAVControlPointObserver.AvtSetPlayModeResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KInstanceID ),
            aAction->ArgumentValue( KActions )
            );
        }
    else{
        return KErrNotSupported;
        }
    return KErrNone;
    }
// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::ConnectionManagerResponse
// This is where action responses for Connection Manager service are handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpActionResponseHandler::ConnectionManagerResponse(CUpnpAction* aAction)
    {
    CUpnpService& service = aAction->Service();
    if( service.ServiceType().Length() <= 0 )
        {
        return KErrArgument;
        }
    const TDesC8& uuid = service.Device().Uuid();
    if (aAction->Name().Compare( KGetProtocolInfo ) == 0)
        {
        iAVControlPointObserver.CmProtocolInfoResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KSource ), 
            aAction->ArgumentValue( KSink )
            );
        }
    
    else if (aAction->Name().Compare( KPrepareForConnection ) == 0)
        {
        TLex8 connectionLex1( aAction->ArgumentValue( KConnectionId ) );
        TInt connectionId;
        connectionLex1.Val( connectionId );			
        TLex8 transportLex1( aAction->ArgumentValue( KAVTransportId ) );
        TInt transportId;
        transportLex1.Val( transportId );			
        TLex8 rscLex3( aAction->ArgumentValue( KRcsID ) );
        TInt rscId;
        rscLex3.Val( rscId );
        iAVControlPointObserver.CmPrepareResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KRemoteProtocolInfo ),
            aAction->ArgumentValue( KPeerConnectionManager ),
            aAction->ArgumentValue( KPeerConnectionId ),
            aAction->ArgumentValue( KDirection ),
            connectionId,
            transportId,
            rscId
            );
        }
    else if (aAction->Name().Compare(KConnectionComplete) == 0)
        {
        TLex8 connectionLex1( aAction->ArgumentValue( KConnectionId ) );
        TInt connectionId;
        connectionLex1.Val( connectionId );

        iAVControlPointObserver.CmComplete(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            connectionId
            );
        }
    else if (aAction->Name().Compare(KGetCurrentConnectionIDs) == 0)
        {
        iAVControlPointObserver.CmCurrentConnections(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KConnectionIds )
            );
        }
    else if (aAction->Name().Compare( KGetCurrentConnectionInfo ) == 0)
        {	
        
        TLex8 rscLex3( aAction->ArgumentValue( KRcsID ) );
        TInt rscId;
        rscLex3.Val( rscId );			
        TLex8 transportLex1( aAction->ArgumentValue( KAVTransportId ) );
        TInt transportId;
        transportLex1.Val( transportId );			
        TLex8 peerLex( aAction->ArgumentValue( KPeerConnectionId ) );
        TInt peerId;
        peerLex.Val( peerId );
        
        iAVControlPointObserver.CmCurrentInfo(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            rscId, 
            transportId, 
            aAction->ArgumentValue( KProtocolInfo),
            aAction->ArgumentValue( KPeerConnectionManager), 
            peerId, 
            aAction->ArgumentValue( KDirection ), 
            aAction->ArgumentValue( KStatus )
            );
        }
    else
        {       
        return KErrNotSupported;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CUpnpActionResponseHandler::ContentDirectoryResponse
// This is where action responses for Content Directory service are handled.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
TInt CUpnpActionResponseHandler::ContentDirectoryResponse(CUpnpAction* aAction)
    {
    CUpnpService& service = aAction->Service();
	if( service.ServiceType().Length() <= 0 )
		{
		return KErrArgument;
		}
    const TDesC8& uuid = service.Device().Uuid();
    if (aAction->Name().Compare(KGetSearchCapabilities) == 0)
        {
        iAVControlPointObserver.CdsSearchCapabilitiesResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KSearchCaps )
            );
        }
    else if (aAction->Name().Compare(KGetSortCapabilities) == 0)
        {
        iAVControlPointObserver.CdsSortCapabilitiesResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KSortCaps )
            );
        }
    else if (aAction->Name().Compare(KGetSystemUpdateID) == 0)
        {	
        const TDesC8& systemUpdateId = aAction->ArgumentValue( KId );
        TLex8 updateidLex( systemUpdateId );
        TInt systemUpdate;
        updateidLex.Val( systemUpdate );
        
        iAVControlPointObserver.CdsSystemUpdateIdResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            systemUpdate
            );
        }
    else if (aAction->Name().Compare(KBrowse) == 0)
        {		
        const TDesC8& numberReturned = aAction->ArgumentValue( KNumberReturned );
        TLex8 returnedLex( numberReturned );
        TInt numberReturnedInt;
        returnedLex.Val( numberReturnedInt );
        
        const TDesC8& totalmatches = aAction->ArgumentValue( KTotalMatches );
        TLex8 matchesLex( totalmatches );
        TInt totalMatchesInt;
        matchesLex.Val( totalMatchesInt );
        
        const TDesC8& updateId = aAction->ArgumentValue( KUpdateID );
        TLex8 updateLex( updateId );
        TInt updateIdInt;
        updateLex.Val( updateIdInt );

        const TDesC8& startIndex = aAction->ArgumentValue( KStartingIndex );
        TLex8 indexLex( startIndex );
        TInt indexInt;
        indexLex.Val( indexInt );

        const TDesC8& requestCount = aAction->ArgumentValue( KRequestedCount );
        TLex8 requestLex( requestCount );
        TInt requestInt;
        requestLex.Val( requestInt );
        
        iAVControlPointObserver.CdsBrowseResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KObjectID ),
            aAction->ArgumentValue( KBrowseFlag ),
            aAction->ArgumentValue( KFilter ),
            indexInt,
            requestInt,
            aAction->ArgumentValue( KSortCriteria ),
            aAction->ArgumentValue( KResult ),
            numberReturnedInt,
            totalMatchesInt,
            aAction->ArgumentValue( KUpdateID )
                );
    }
    else if (aAction->Name().Compare(KSearch) == 0)
        {		

        const TDesC8& numberReturned = aAction->ArgumentValue( KNumberReturned );
        TLex8 returnedLex( numberReturned );
        TInt numberReturnedInt;
        returnedLex.Val( numberReturnedInt );
        
        const TDesC8& totalmatches = aAction->ArgumentValue( KTotalMatches );
        TLex8 matchesLex( totalmatches );
        TInt totalMatchesInt;
        matchesLex.Val( totalMatchesInt );
        
        const TDesC8& updateId = aAction->ArgumentValue( KUpdateID );
        TLex8 updateLex( updateId );
        TInt updateIdInt;
        updateLex.Val( updateIdInt );

        const TDesC8& startingIndex = aAction->ArgumentValue( KStartingIndex );
        TLex8 indexLex( startingIndex );
        TInt indexInt;
        indexLex.Val( indexInt );

        const TDesC8& reqCnt = aAction->ArgumentValue( KRequestedCount );
        TLex8 requestLex( reqCnt );
        TInt requestInt;
        requestLex.Val( requestInt );
        
        iAVControlPointObserver.CdsSearchResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KContainerID ),
            aAction->ArgumentValue( KSearchCriteria ),
            aAction->ArgumentValue( KFilter ),
            indexInt,
            requestInt,
            aAction->ArgumentValue( KSortCriteria ),
            aAction->ArgumentValue( KResult ),
            numberReturnedInt,
            totalMatchesInt,
            aAction->ArgumentValue( KUpdateID )
            );
        }
    else if (aAction->Name().Compare(KDestroyObject) == 0)
        {		
        iAVControlPointObserver.CdsDestroyObjectResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KObjectID )
            );
        }
    else if (aAction->Name().Compare(KUpdateObject) == 0)
        {	
        iAVControlPointObserver.CdsUpdateObjectResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KObjectID ),
            aAction->ArgumentValue( KCurrentTagValue ),
            aAction->ArgumentValue( KNewTagValue )
            );
        }
    else if (aAction->Name().Compare(KImportResource) == 0)
        {		
        iAVControlPointObserver.CdsImportResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KSourceURI ),
            aAction->ArgumentValue( KDestinationURI  ),
            aAction->ArgumentValue( KTransferID )
            );
        }
    else if (aAction->Name().Compare(KExportResource) == 0)
        {			
        iAVControlPointObserver.CdsExportResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KSourceURI ),
            aAction->ArgumentValue( KDestinationURI  ),
            aAction->ArgumentValue( KTransferID )
            );		
        }
    else if (aAction->Name().Compare(KStopTransferResource) == 0)
        {		
        iAVControlPointObserver.CdsStopTransferResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KTransferID )
            );		
        }
    else if (aAction->Name().Compare(KGetTransferProgress) == 0)
        {	
        iAVControlPointObserver.CdsCTransferProgressResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue( KTransferID ),
            aAction->ArgumentValue( KTransferStatus ),
            aAction->ArgumentValue( KTransferLength ),            
            aAction->ArgumentValue( KTransferTotal )
            );
        }
    else if (aAction->Name().Compare(KDeleteResource) == 0)
        {		
        iAVControlPointObserver.CdsDeleteResourceResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue(KResourceURI)
            );
        }
    else if (aAction->Name().Compare(KCreateReference) == 0)
        {
        iAVControlPointObserver.CdsCreateReferenceResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue(KContainerID), 
            aAction->ArgumentValue(KObjectID), 
            aAction->ArgumentValue(KNewID)
            );
        }
    else if (aAction->Name().Compare(KCreateObject) == 0)
        {
        iAVControlPointObserver.CdsCreateObjectResponse(
            uuid,
            aAction->SessionId(),
            aAction->Error(),
            aAction->ArgumentValue(KContainerID), 
            aAction->ArgumentValue(KElements), 
            aAction->ArgumentValue(KObjectID), 
            aAction->ArgumentValue(KResult)
            );
        }
    else
        {
        return KErrArgument;
        }
    return KErrNone;
    }

//End of file
