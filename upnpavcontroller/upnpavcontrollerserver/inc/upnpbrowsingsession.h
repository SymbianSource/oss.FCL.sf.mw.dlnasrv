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
* Description:      implementation for session towards a media server
*
*/



#ifndef C_UPNPBROWSINGSESSION_H
#define C_UPNPBROWSINGSESSION_H

// INCLUDE FILES
#include <e32base.h>
#include "upnpavcontroller.h"
#include <upnpmediaserverobserver.h>
#include "upnpavbrowsingsession.h"

#include "upnpavcontrolpointobserver.h"


// FORWARD DECLARATIONS
class CUpnpObject;
class CUpnpDevice;
class CUpnpAVRequest;
class CUpnpAVBrowseRequest;
class CUpnpAVDeviceExtended;
class CUpnpAVControllerServer;

/**
 * Implements server side browsing session functionality. Implements
 * browsing, searching, copying etc.  
 *
 * @since S60 v3.1
 */
class CUPnPBrowsingSession : public CBase,
                             private MUpnpAVControlPointObserver
    {

private:

    /**
     * Defines the internal state of browsing session
     */
    enum TInternalBrowseState
        {
        ENone,
        EBrowse,
        EDestroyObject,
        ECreateContainer,
        };


public:

    /**
     * Static 1st phase constructor
     *
     * @param aControlPoint AV Control Point reference
     * @param aClient S60 Media Server session reference
     * @param aDispatcher observer callback dispatcher reference
     * @param aSessionId session id
     * @param aUuid device Uuid
     */
    static CUPnPBrowsingSession* NewL
        (
        CUpnpAVControllerServer& aServer,
        TInt aSessionId,   
        const TDesC8& aUuid
        );
    
    /**
     * Destructor
     */
    virtual ~CUPnPBrowsingSession();
    
private:

    /**
     * Private constructor
     *
     * @param aControlPoint AV Control Point reference
     * @param aClient S60 Media Server session reference
     * @param aDispatcher observer callback dispatcher reference
     * @param aSessionId session id
     */
    CUPnPBrowsingSession
        (
        CUpnpAVControllerServer& aServer,
        TInt aSessionId
        );    
    
    /**
     * 2ns phase constructor
     */
    void ConstructL( const TDesC8& aUuid );

private: // From MUpnpAVControlPointObserver

    void ActionResponseL(CUpnpAction* aAction );
    void StateUpdatedL(CUpnpService* aService);
    void HttpResponseL(CUpnpHttpMessage* aMessage);
    void DeviceDiscoveredL(CUpnpDevice* aDevice);
    void DeviceDisappearedL(CUpnpDevice* aDevice);

private: // network event handling

    /**
     * Handles response for ContentDirectory SearchCapabilities command
     */
    void CdsSearchCapabilitiesResponse(
        TInt aErr,
        const TDesC8& aSearchCaps );

    /**
     * Handles response for ContentDirectory Browse command
     */
    void CdsBrowseResponseL(
        TInt aErr,
        const TDesC8&  aBrowseFlag,
        const TDesC8&  aResult,
        TInt aReturned,
        TInt aMatches,
        const TDesC8& aUpdateID );

    /**
     * Handles response for ContentDirectory Search command
     */
    void CdsSearchResponse(
        TInt aErr,
        const TDesC8& aResult,
        TInt aReturned,
        TInt aMatches,
        const TDesC8& aUpdateID );

    /**
     * Handles response for ContentDirectory DestroyObject command
     */
    void CdsDestroyObjectResponse(
        TInt aErr );

    /**
     * Handles response for ContentDirectory CreateObject command
     */
    void CdsCreateObjectResponse(
        TInt aErr,
        const TDesC8& aObjectID, 
        const TDesC8& aResult );

public: // New functions

    /**
     * Handles UPnP device disappears.
     *
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );

    /**
     * Sets local (S60) Media Server Uuid
     *
     * @param aUuid device Uuid
     */
    void SetLocalMSUuidL( const TDesC8& aUuid );

    /**
     * Returns Session Id
     *
     * @return session id
     */
    TInt SessionId() const;
    
    /**
     * Returns device Uuid 
     *
     * @return device Uuid
     */
    const TDesC8& Uuid() const;
    
public:

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetBrowseResponseSizeL( const RMessage2& aMessage );  

    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetBrowseResponseSizeL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetBrowseResponseL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchResponseSizeL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetSearchResponseSizeL();

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchResponseL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchCapabitiesSizeL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetSearchCapabitiesSizeL();     
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void GetSearchCapabitiesL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CreateContainerL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelCreateContainerL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void DeleteObjectL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDeleteObjectL();
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void DeviceDisappearedRequestL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDeviceDisappearedRequestL();
                                   
private:


    /**
     * Parses browse response and checks if the object can be deleted. If
     * supported, sends destroyobject-action.
     *
     * @param aResponse browse response
     */
    void CheckAndSendDestroyObjectActionL( const TDesC8& aResponse );    
    

    /**
     * Resets internal state
     */
    void ResetL();
        

    /**
     * Reads an object from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aObj UPnP object
     */
    void ReadObjFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpObject* aObj );
        
    /**
     * Reads a request from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aReq request
     */
    void ReadReqFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpAVRequest* aReq );
        
    /**
     * Reads a browse request from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @param aReq browse request
     */
    void ReadBrowseReqFromMessageL( const RMessage2& aMessage, TInt aSlot,
        CUpnpAVBrowseRequest* aReq );

    /**
     * Reads a buffer from a message
     *
     * @param aMessage client/server message
     * @param aSlot message slot number
     * @return a buffer (heap descriptor)
     */
    HBufC8* ReadBufFromMessageLC( const RMessage2& aMessage, TInt aSlot );


private:

    CUpnpAVControllerServer&    iServer;
    
    TInt                        iSessionId;

    TInt                        iInstanceId;
    
    TInt                        iIPSessionId;

    TInternalBrowseState        iInternalState;
    
    RMessage2*                  iActionMessage; // Own
    
    RMessage2*                  iDeviceMessage; // Own
    
    CUpnpAVDeviceExtended*      iDevice; // Own

    const CUpnpDevice*          iCpDevice; // Not own.    
    
    HBufC8*                     iLocalMediaServerUuid; // Own
    
    HBufC8*                     iRespBuf; // Own
    
    HBufC8*                     iRespBuf2; // Own  
        
    HBufC8*                     iItemId; // Own
    
    };

#endif // C_UPNPBROWSINGSESSION_H
