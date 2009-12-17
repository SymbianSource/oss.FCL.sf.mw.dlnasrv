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
* Description:      File tranfer session
*
*/






#ifndef C_UPNPFILEUPLOADSESSIONIMPL_H
#define C_UPNPFILEUPLOADSESSIONIMPL_H

// INCLUDES
#include <e32base.h>
#include "upnpfileuploadsession.h"
#include "upnpavdeviceobserver.h"

#include "tupnpfiletransferevent.h"

// FORWARD DECLARATIONS
class RUPnPAVControllerClient;
class CUpnpAVDevice;
class MUPnPFileTransferSessionObserver;

// CLASS DECLARATION
/**
 *  AV Controller file upload session client-side implementation.
 *
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CUPnPFileUploadSessionImpl ) :
                                    public CActive, 
                                    public MUPnPFileUploadSession,
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
    static CUPnPFileUploadSessionImpl* NewL(
        RUPnPAVControllerClient& aServer, const CUpnpAVDevice& aDevice );
    
    /**
     * Destructor
     */
    ~CUPnPFileUploadSessionImpl();

private:

    /**
     * Private constructor
     *
     * @param aServer a handle to AV Controller server
     */
    CUPnPFileUploadSessionImpl( RUPnPAVControllerClient& aServer );

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
     * See upnpfileuploadsession.h
     */
    void StartUploadL( const TDesC& aFilePath, TInt aKey );

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
     * Filetransfer observer
     *
     * Not owned
     */        
    MUPnPFileTransferSessionObserver*   iObserver;
    
    /**
     * AV Controller server client handle
     *
     * Not owned
     */        
    RUPnPAVControllerClient&            iServer;
    
    /**
     * Source device
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
     * Modifiable pointer to data the buffer
     */        
    TPtr8                               iBufferPtr;

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

#endif // C_UPNPFILEUPLOADSESSIONIMPL_H

// End of file
