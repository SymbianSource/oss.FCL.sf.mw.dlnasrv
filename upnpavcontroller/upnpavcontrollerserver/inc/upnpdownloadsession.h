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







#ifndef C_UPNPDOWNLOADSESSION_H
#define C_UPNPDOWNLOADSESSION_H

// INCLUDE FILES
#include <e32base.h>
#include "upnpavcontroller.h"
#include "upnpfiletransfersessionbase.h"
#include "httptransferobserver.h"
#include "tupnpfiletransferevent.h"

class CHttpDownloader;
class CUpnpDlnaProtocolInfo;

/**
 * Implements AV Controller server side download session
 *
 * @since S60 v3.2
 */
class CUPnPDownloadSession :    public CUPnPFileTransferSessionBase,
                                public MHttpTransferObserver
    {
public:

    /**
     * Static 1st phase constructor
     *
     * @param aSessionId session id
     * @param aUuid device Uuid
     */
    static CUPnPDownloadSession* NewL( CUpnpAVControllerServer& aServer,
        TInt aSessionId, const TDesC8& aUuid );
    
    /**
     * Destructor
     */
    virtual ~CUPnPDownloadSession();
    
private:

    /**
     * Private constructor
     *
     * @param aSessionId session id
     */
    CUPnPDownloadSession( CUpnpAVControllerServer& aServer,
        TInt aSessionId );    
    
    /**
     * 2ns phase constructor
     */
    void ConstructL( const TDesC8& aUuid );

private: // From MHttpTransferObserver
    
    /**
     * See httptransferobserver.h
     */
    void TransferProgress( TAny* aKey, TInt aBytes, TInt aTotalBytes );

    /**
     * See httptransferobserver.h
     */
    void ReadyForTransferL( TAny* aKey );

    /**
     * See httptransferobserver.h
     */
    void TransferCompleted( TAny* aKey, TInt aStatus );
     
public: // New functions

    /**
     * See upnpavcontrollerimpl.h
     */
    void StartDownloadL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void StartDownloadFHL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelDownloadL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelAllDownloadsL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void StartTrackingDownloadProgressL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void StopTrackingDownloadProgressL( const RMessage2& aMessage );

    /**
     * See upnpavcontrollerimpl.h
     */
    void GetDownloadEventL( const RMessage2& aMessage );
    
    /**
     * See upnpavcontrollerimpl.h
     */
    void CancelGetDownloadEventL( const RMessage2& aMessage );

    /**
     * Handles UPnP device disappears.
     *
     * @param aDevice Device that disappeared.
     */
    void DeviceDisappearedL( CUpnpAVDeviceExtended& aDevice );

private: // New functions

    void SetHeadersL( CUpnpDlnaProtocolInfo& aInfo, TAny* aKey );
    
private:
    
    CHttpDownloader* iDownloader; // Own
      
    };

#endif // C_UPNPDOWNLOADSESSION_H
