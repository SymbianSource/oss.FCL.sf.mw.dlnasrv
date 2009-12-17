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



#ifndef __CMFMDOWNLOADPROXY_H
#define __CMFMDOWNLOADPROXY_H

#include <e32base.h>

/* Forward declarations. */
class MUPnPAVController;

class CUpnpAVDevice;
class MCmFmTransferObserver;
class CCmFmDownloadMngr;

// CLASS DECLARATION
 
/**
 *  Class represents methods to communication with AV Controller API.
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmDownloadProxy ): public CBase
    {
 
private:
    
    // Struct containing indexed download session 
    // ( own session for each server )
    struct TCmFmDownloadSessionIndexes
        {
        TInt iDeviceIndex;  // Device index
        TInt iSessionIndex; // Session index
        };

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     */
    static CCmFmDownloadProxy* NewL( );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     */
    static CCmFmDownloadProxy* NewLC( );

    /**
     * Destructor.
     */
    virtual ~CCmFmDownloadProxy();

public: 
            
    /**
     * Starts to fetch defined file
     *
     * @since S60 5.1
     * @param aObserver, action observer
     * @param aAVController, pointer to av controller
     * @param aDevices, list of devices        
     * @param aResElement, pointer to av controller
     * @param aItem, item to be transferred
     * @param aFile, file reference
     * @param aInternalDeviceIndex, internal device index
     * @param aInternalItemId, internal item index
     * @return None
     */ 
    void FetchFileL( MCmFmTransferObserver* aObserver, 
        MUPnPAVController* aAVController, CUpnpAVDeviceList* aDevices, 
        const CUpnpElement& aResElement, 
        const CUpnpItem& aItem, RFile& aFile, const TInt aInternalDeviceIndex,
        const TInt64 aInternalItemId );
   
    /**
     * Closes download session
     *
     * @since S60 5.1
     * @param aSessionId, session to be closed
     * @return ETrue, if sessions left
     */    
    TBool CloseDownloadSession( const TInt aSessionId );
    
    /**
     * Cancel UPnP operation download
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void CancelOperation();
    
private:

    /**
     * Starts new download session if session to 
     * defined device doesn't already exist.
     *
     * @since S60 5.1
     * @param aObserver, action observer
     * @param aAVController, pointer to av controller
     * @param aDevices, list of devices       
     * @param aInternalDeviceIndex, internal device index
     * @return index of the used session
     */    
    TInt StartNewDownloadSessionIfNeededL( MCmFmTransferObserver* aObserver, 
        MUPnPAVController* aAVController, 
        CUpnpAVDeviceList* aDevices, 
        const TInt aInternalDeviceIndex );

    /**
     * Creates new download session
     *
     * @since S60 5.1
     * @param aObserver, action observer
     * @param aAVController, pointer to av controller
     * @param aDevices, list of devices       
     * @param aSessionCount, session count
     * @param aSessionIndex, ref to session index
     * @return index of the used session
     */        
    void CreateNewSessionL(  
        MCmFmTransferObserver* aObserver, MUPnPAVController* aAVController, 
        CUpnpAVDeviceList* aDevices, TInt aSessionCount, 
        TInt& aSessionIndex );                              
                           
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmFmDownloadProxy( );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:
    
    /**
     * Download sessions
     */
    RPointerArray<CCmFmDownloadMngr> iDownloadSessions;
    
    /**
     * Indexed array for download sessions
     */
    RArray<TCmFmDownloadSessionIndexes> iSessionIndexes;
    
    /**
     * Count of download session
     */
    TInt iDownloadSessionCount;
    
    };

#endif //  __CMFMDOWNLOADPROXY_H
