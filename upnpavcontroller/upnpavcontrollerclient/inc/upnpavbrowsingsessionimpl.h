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
* Description:      implements a session towards a media server
*
*/






#ifndef C_UPNPAVBROWSINGSESSIONIMPL_H
#define C_UPNPAVBROWSINGSESSIONIMPL_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>

#include "upnpavbrowsingsession.h"
#include "upnpavdeviceobserver.h"
#include "upnpavbrowserespparams.h"

// FORWARD DECLARATIONS
class CUpnpAVDevice;
class MUPnPAVBrowsingSessionObserver;
class RUPnPAVControllerClient;
//class CUPnPXMLParser;
class CUPnPAVDeviceActive;

/**
 *  AV Controller browsing session implementation.
 *
 *  This class implements the browsing interface of AV Controller. Includes
 *  operations to search, browse, copy, create container, delete object and
 *  to get search capabilities.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVBrowsingSessionImpl :  public CActive,
                                    public MUPnPAVBrowsingSession,
                                    public MUPnPAVDeviceObserver
    {

private:

    /**
     * Defines internal state of operation
     */
    enum TPendingOperation
        {
        ENone = 0,
        EBrowseSize,
        ESearchSize,
        EGetSearchCapabilities,
        ECreateContainer,
        EDeleteObject,
        EStartMediaServer
        };

public:

    /**
     * Static constructor
     *
     * @param aServer handle to AV Controller server
     * @param aDevice target device
     * @return a new browsing session implementation instance
     */
    static CUPnPAVBrowsingSessionImpl* NewL( RUPnPAVControllerClient&
        aServer, const CUpnpAVDevice& aDevice );

    /**
     * Destructor
     */    
    virtual ~CUPnPAVBrowsingSessionImpl();
    
private:

    /**
     * 2nd phase construct
     */    
    CUPnPAVBrowsingSessionImpl( RUPnPAVControllerClient& aServer );
    
    /**
     * 2nd phase construct
     */        
    void ConstructL();
    
protected: // From CActive

    void RunL();
    
    void DoCancel();
    
    TInt RunError( TInt aError );         

protected: // From MUPnPAVBrowser

    /**
     * See upnpavbrowsingsession.h
     */
    void SetObserver( MUPnPAVBrowsingSessionObserver& aObserver);
     
    /**
     * See upnpavbrowsingsession.h
     */
    void RemoveObserver();

    /**
     * See upnpavbrowsingsession.h
     */
    MUPnPAVBrowsingSessionObserver* Observer() const;
    

    /**
     * See upnpavbrowsingsession.h
     */
    const CUpnpAVDevice& Device() const;

    /**
     * See upnpavbrowsingsession.h
     */
    void ReserveLocalMSServicesL(); 

    /**
     * See upnpavbrowsingsession.h
     */
    void CancelReserveLocalMSServicesL(); 

    /**
     * See upnpavbrowsingsession.h
     */
    void ReleaseLocalMSServicesL(); 
    
    /**
     * See upnpavbrowsingsession.h
     */
    void BrowseL(   const TDesC8& aId,
                    const TDesC8& aFilter,
                    TBrowseFlag aBrowseFlag,
                    TInt aStartIndex,
                    TInt aRequestedCount,
                    const TDesC8& aSortCriteria );                      

    /**
     * See upnpavbrowsingsession.h
     */
    void CancelBrowse();

    /**
     * See upnpavbrowsingsession.h
     */
    void SearchL(   const TDesC8& aId,
                    const TDesC8& aSearchCriteria,
                    const TDesC8& aFilter,
                    TInt aStartIndex,
                    TInt aRequestedCount,
                    const TDesC8& aSortCriteria );

    /**
     * See upnpavbrowsingsession.h
     */
    void CancelSearch();

    /**
     * See upnpavbrowsingsession.h
     */
    void GetSearchCapabilitiesL();

    /**
     * See upnpavbrowsingsession.h
     */
    void CreateContainerL( const TDesC8& aTitle, const TDesC8& aParentId,
        TContainerType aContainerType );
    
    /**
     * See upnpavbrowsingsession.h
     */
    void DeleteObjectL( const TDesC8& aId );

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
    
private: // Internal methods

    /**
     * Handles browse
     */    
    void BrowseSizeCompletedL();

    /**
     * Handles browse
     */        
    void BrowseResultCompletedL();
    
    /**
     * Handles search
     */    
    void SearchSizeCompletedL();

    /**
     * Handles search
     */        
    void SearchResultCompletedL();    

    /**
     * Handles search capabilities
     */        
    void SearchCapabilitiesCompletedL();
    
    /**
     * Handles create container
     */                    
    void CreateContainerCompletedL();
    
    /**
     * Handles delete object
     */                    
    void DeleteObjectCompletedL();

    /**
     * Handles media server start
     */                        
    void StartMediaServerCompleteL();

    /**
     * Reset
     */                            
    void ResetL();
    
private:

    MUPnPAVBrowsingSessionObserver* iObserver; // not owned
    
    RUPnPAVControllerClient&    iServer; // not owned
    
    CUpnpAVDevice*              iDevice; // owned
    
    // Buffer to pass data to server
    HBufC8*                     iBuffer; // owned
    TPtr8                       iBufferPtr;

    // Buffer to pass data to server
    HBufC8*                     iBuffer2; // owned
    TPtr8                       iBufferPtr2;

    // Buffer to pass data to server
    HBufC8*                     iBuffer3; // owned
    TPtr8                       iBufferPtr3;
    
    //CUPnPXMLParser*             iParser; // owned
    
    CUPnPAVDeviceActive*        iDeviceActive; // owned
    
    TInt                        iRespBufSize;           
    TPckg<TInt>                 iRespBufSizePkg;

//    TInt                        iRespBufSize;           
//    TPckg<TInt>                 iRespBufSizePkg;

    TUpnpAVBrowseRespParams         iRespParams;           
    TPckg<TUpnpAVBrowseRespParams>  iRespParamsPkg;
    
    TPendingOperation           iPendingOperation;
    
    TBool iAlive;
    
    TBool                       iMediaServerResourceReserved;       
            
    };


#endif // C_UPNPAVBROWSERIMPL_H
