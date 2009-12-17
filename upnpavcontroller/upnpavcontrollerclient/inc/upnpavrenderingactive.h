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
* Description:      AO for performing rendering commands
*
*/






#ifndef C_UPNPAVRENDERINGACTIVE_H
#define C_UPNPAVRENDERINGACTIVE_H

#include <e32std.h>
#include <e32base.h>
#include "upnpavrenderingsessionobserver.h"

class RUPnPAVControllerClient;
class CUpnpItem;
//class CUPnPAVRenderingSessionImpl;

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class CUPnPAVRenderingActive : public CActive
    {

private:

    /**
     * Enumeration to define the action
     */
    enum TPendingOperation
        {
        ENone = 0,
        ESetURI,
        ESetNextURI,
        EPlay,
        EStop,
        EPause,
        ESetVolume,
        EGetVolume,
        ESetMute,
        EGetMute,
        EPositionInfo,
        EStartMediaServer
        };

public:

    /**
     * Two-phased constructor.
     *
     * @param aServer session handle
     * @param aId session id
     * @return an instance
     */
    static CUPnPAVRenderingActive* NewL(
        RUPnPAVControllerClient& aServer, TInt aId );
    
    /**
     * Destructor
     */
    virtual ~CUPnPAVRenderingActive();
       
private:

    /**
     * Constructor
     */
    CUPnPAVRenderingActive( RUPnPAVControllerClient& aServer, TInt aId );
    
    /**
     * 2nd phase construct
     */
    void ConstructL();

protected: // From CActive

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

public: // New functions    

    /**
     * Sets Media observer instance
     *
     * @since Series 60 3.1      
     * @param aInstance MUPnPAVMediaObserver instance of the class that
     *        implements the IF
     * @return TInt status
     */
    void SetObserver( MUPnPAVRenderingSessionObserver& aObserver );

    /**
     * See upnpavrenderingsession.h
     */
    void RemoveObserver();

    /**
     * See upnpavrenderingsession.h
     */
    MUPnPAVRenderingSessionObserver* Observer() const;
        
    /**
     * See upnpavrenderingsession.h
     */
    void ReserveLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void CancelReserveLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void ReleaseLocalMSServicesL(); 

    /**
     * See upnpavrenderingsession.h
     */
    void SetURIL( const TDesC8& aURI, const CUpnpItem& aItem );

    /**
     * See upnpavrenderingsession.h
     */
    void SetNextURIL( const TDesC8& aURI, const CUpnpItem& aItem );

    /**
     * See upnpavrenderingsession.h
     */
    void PlayL();

    /**
     * See upnpavrenderingsession.h
     */
    void StopL();

    /**
     * See upnpavrenderingsession.h
     */
    void PauseL();

    /**
     * See upnpavrenderingsession.h
     */
    void SetVolumeL( TInt aVolumeLevel );

    /**
     * See upnpavrenderingsession.h
     */
    void GetVolumeL();

    /**
     * See upnpavrenderingsession.h
     */
    void SetMuteL( TBool aMute );

    /**
     * See upnpavrenderingsession.h
     */
    void GetMuteL();

    /**
     * See upnpavrenderingsession.h
     */
    void GetPositionInfoL();
        
private:

    /**
     * Reset method
     */
    void ResetL();
    
    /**
     * Set uri response handling
     */
    void SetURICompleteL();
    
    /**
     * Set next uri response handling
     */
    void SetNextURICompleteL();
    
    /**
     * Play response handling
     */
    void PlayCompleteL();
    
    /**
     * Stop response handling
     */
    void StopCompleteL();
    
    /**
     * Pause response handling
     */
    void PauseCompleteL();
    
    /**
     * Set volume response handling
     */
    void SetVolumeCompleteL();
    
    /**
     * Get volume response handling
     */
    void GetVolumeCompleteL();
    
    /**
     * Set mute response handling
     */
    void SetMuteCompleteL();
    
    /**
     * Get mute response handling
     */
    void GetMuteCompleteL();
    
    /**
     * Get position info response handling
     */
    void PositionInfoCompleteL();
    
    /**
     * Start media server response handling
     */
    void StartMediaServerCompleteL();    

private:

    RUPnPAVControllerClient&            iServer;

    TInt                                iId;

    // Buffer to pass data to server
    HBufC8*                             iBuffer; // owned
    TPtr8                               iBufferPtr;         

    HBufC8*                             iBuffer2; // owned
    TPtr8                               iBufferPtr2;         
    
    TInt                                iRespBufSize;           
    TPckg<TInt>                         iRespBufSizePkg;
    
    TBool                               iMediaServerResourceReserved;
    
    TPendingOperation                   iPendingOperation;
    
    MUPnPAVRenderingSessionObserver*    iObserver; // Not owned      
    
    };

#endif // C_UPNPAVRENDERINGACTIVE_H
