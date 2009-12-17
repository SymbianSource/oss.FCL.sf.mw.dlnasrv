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
* Description:      File tranfer session implementation
*
*/






#ifndef C_UPNPFILEDOWNLOADSESSIONIMPL_H
#define C_UPNPFILEDOWNLOADSESSIONIMPL_H

// EXTERNAL INCLUDES
#include <e32base.h>

// INTERNAL INCLUDES
#include "upnpfiledownloadsession.h"
#include "upnpavdeviceobserver.h"
#include "tupnpfiletransferevent.h"

// FORWARD DECLARATIONS
class RUPnPAVControllerClient;
class CUpnpAVDevice;
class MUPnPFileTransferSessionObserver;

// CLASS DECLARATION
/**
 *  AV Controller file download session clint-side implementation.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CUPnPFileDownloadSessionImpl ) :
                                        public CActive, 
                                        public MUPnPFileDownloadSession,
                                        public MUPnPAVDeviceObserver
    {
public:

    /**
     * 1st phase construct
     *
     * @param aServer a handle to AV Controller server
     * @param aDevice target device
     * @return new instance
     */
    static CUPnPFileDownloadSessionImpl* NewL(
        RUPnPAVControllerClient& aServer, const CUpnpAVDevice& aDevice );

    /**
     * Destructor
     */
    ~CUPnPFileDownloadSessionImpl();

private:

    /**
     * Private constructor
     *
     * @param aServer a handle to AV Controller server
     */
    CUPnPFileDownloadSessionImpl( RUPnPAVControllerClient& aServer );

    /**
     * 2nd phase constructor
     */
    void ConstructL();

private: // From CActive

    /**
     * See e32base.h
     */        
    void RunL();
    
    /**
     * See e32base.h
     */        
    void DoCancel();
    
    /**
     * See e32base.h
     */        
    TInt RunError( TInt aError );         

public: // From MUPnPFileUploadSession

    /**
     * See upnpfiledownloadsession.h
     */
    void StartDownloadL( const CUpnpItem& aItem, TInt aKey );

    /**
     * See upnpfiledownloadsession.h
     */
    void StartDownloadL( const CUpnpElement& aResElement,
        const CUpnpItem& aItem, RFile& aFile, TInt aKey );

public: // From MUPnPFileTransferSession

    /**
     * See upnpfiletransfersession.h
     */
    void SetObserver( MUPnPFileTransferSessionObserver& aObserver );

    /**
     * See upnpfiletransfersession.h
     */
    void RemoveObserver();
    
    /**
     * See upnpfiletransfersession.h
     */
    MUPnPFileTransferSessionObserver* Observer() const;   

    /**
     * See upnpfiletransfersession.h
     */
    void StartTrackingProgressL( TInt aKey );
    
    /**
     * See upnpfiletransfersession.h
     */
    void CancelTransfer( TInt aKey );
    
    /**
     * See upnpfiletransfersession.h
     */
    void CancelAllTransfers();

public: // From MUPnPAVDeviceObserver

    /**
     * See upnpavdeviceobserver.h
     */
    void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

    /**
     * See upnpavdeviceobserver.h
     */
    void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );

    /**
     * See upnpavdeviceobserver.h
     */
    void WLANConnectionLost();
    
public: // New methods

    /**
     * Returns device
     *
     * @return device
     */        
    const CUpnpAVDevice& Device() const;

private:

    /**
     * Reset
     */        
    void ResetL();

private:

    /**
     * Filetransfer session observer
     *
     * Not owned
     */        
    MUPnPFileTransferSessionObserver*   iObserver;
    
    /**
     * AV Controller client handle
     */        
    RUPnPAVControllerClient&            iServer;
    
    /**
     * Target device
     *
     * Owned
     */        
    CUpnpAVDevice*                      iDevice;
    
    /**
     * Buffer to pass data to the server process
     *
     * Owned
     */        
    HBufC8*                             iBuffer;
    
    /**
     * Modifiable pointer to the data buffer
     */        
    TPtr8                               iBufferPtr;

    /**
     * Buffer to pass data to the server process
     *
     * Owned
     */        
    HBufC*                              iBuffer2;
    
    /**
     * Modifiable pointer to the data buffer
     */        
    TPtr                                iBufferPtr2;
    
    /**
     * Flag to store the state of WLAN
     */        
    TBool                               iAlive;

    /**
     * Filetransfer event. Used to receive events from the server process
     */        
    TUpnpFileTransferEvent              iEvent;           
    
    /**
     * Package buffer used in client-server comms
     */        
    TPckg<TUpnpFileTransferEvent>       iEventPkg;
    
    };

#endif // C_UPNPFILEDOWNLOADSESSIONIMPL_H

// End of file

