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
* Description:  Manages upnp download
*
*/



#ifndef __CMFMDOWNLOADMNGR_H
#define __CMFMDOWNLOADMNGR_H

#include <e32base.h>
#include <f32file.h>
#include "upnpfiletransfersessionobserver.h"

/* Forward declarations. */
class MUPnPAVController;
class CUpnpAVDevice;
class MCmFmTransferObserver;

// CLASS DECLARATION
 /**
  *  Class represents methods to communication with AV Controller API.
  *
  *  @lib cmfillmanager.lib
  *
  *  @since S60 5.1
  */
NONSHARABLE_CLASS( CCmFmDownloadMngr ): public CBase,
                          public MUPnPFileTransferSessionObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aAVController, av controller
     * @param aObserver, action observer
     * @param aDevice, device
     * @param aDownloadId, id for the download manager
     * @return  pointer to CCmFmDownloadMngr class
     */
    static CCmFmDownloadMngr* NewL( MUPnPAVController* aAVController, 
        MCmFmTransferObserver* aObserver, const CUpnpAVDevice& aDevice,
        const TInt aDownloadId  );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aAVController, av controller
     * @param aObserver, action observer
     * @param aDevice, device      
     * @param aDownloadId, id for the download manager
     * @return  pointer to CCmFmDownloadMngr class
     */
    static CCmFmDownloadMngr* NewLC( MUPnPAVController* aAVController, 
        MCmFmTransferObserver* aObserver, const CUpnpAVDevice& aDevice,
        const TInt aDownloadId  );

    /**
     * Destructor.
     */
    virtual ~CCmFmDownloadMngr();

public: 
        
    /**
    * Starts to fetch defined file
    *
    * @since S60 5.1
    * @param aResElement, pointer to av controller
    * @param aItem, item to be transferred
    * @param aFile, file to be downloaded
    * @param aInternalItemId, internal item index
    * @return None
    */ 
    void FetchFileL( const CUpnpElement& aResElement, 
        const CUpnpItem& aItem, RFile& aFile, const TInt64 aInternalItemId );
    
    /**
    * Cancel UPnP operation
    *
    * @since S60 5.1
    * @param None
    * @return None
    */    
    void CancelOperation();

protected: 
    
// From base class MUPnPFileTransferSessionObserver

    /**
     * From MUPnPFileTransferSessionObserver
     * @see base class definition
     */
    void TransferStarted( TInt aKey, TInt aStatus );
 
    /**
     * From MUPnPFileTransferSessionObserver
     * @see base class definition
     */
     void TransferCompleted( TInt aKey, TInt aStatus,
                             const TDesC& aFilePath );
    
    /**
     * From MUPnPFileTransferSessionObserver
     * @see base class definition
     */
    void TransferProgress( TInt aKey, TInt aBytes, TInt aTotalBytes );

    /**
     * From MUPnPFileTransferSessionObserver
     * @see base class definition
     */        
    void MediaServerDisappeared( TUPnPDeviceDisconnectedReason aReason );
                           
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aAVController, av controller
     * @param aObserver, transfer observer
     * @param aDownloadId, download manager id
     */
    CCmFmDownloadMngr( MUPnPAVController* aAVController, 
        MCmFmTransferObserver* aObserver, const TInt aDownloadId );

    /**
     * Performs the second phase construction.
     * @since S60 5.1
     * @param aDevice, UPnP server
     */
    void ConstructL( const CUpnpAVDevice& aDevice );
    

private:
    
    /**
     * AV Controller
     */
    MUPnPAVController* iAVController;                   // Not owned
    
    /**
     * File transfer Observer
     */
    MCmFmTransferObserver* iObserver;                   // Not owned
    
    /**
     * Download manager id. Proxy identifies managers with this id
     */
    TInt iDownloadId;
    
    /**
     * Download session
     */
    MUPnPFileDownloadSession* iDownloadSession;         // Not owned

    /**
     * Size of transferred file
     */
    TInt64 iFileSize;
    
    };

#endif //  __CMFMDOWNLOADMNGR_H
