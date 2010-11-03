/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Container class for AV Transport events.    
*
*/

#ifndef C_UPNPAVTEVENT_INL
#define C_UPNPAVTEVENT_INL

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CONSTS

/**
 * Details of AVTransport event
 *
 * @lib upnpxmlparser.lib
 */
class CUPnPAVTEvent : public CBase
    {

public:

    /**
     * AV Transport state.
     */
    enum TTransportState
        {
        EIdle = 0,
        EStopped,
        EPlaying,
        ETransitioning,
        EPausedPlayback,
        EPausedRecording,
        ERecording,
        ENoMediaPresent
        };
 
private:    
    
    /**
     * Default constructor.
     */
    CUPnPAVTEvent();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
public:
    
    /**
     * 1st phase constructor.
     */
    static CUPnPAVTEvent* NewL();

    /**
     * Default destructor.
     */
    ~CUPnPAVTEvent();
    
    /**
     * Copy constructor.
     * 
     * @param event object to be copied.
     * @return a copy of event.
     */
    static CUPnPAVTEvent* CloneL( const CUPnPAVTEvent& event );
    
public: // Exported getters 
    
    /**
     * Return instance id
     * 
     * @return instance id
     */
    IMPORT_C TInt InstanceID() const;
    
    /**
     * Return mute state
     * 
     * @return mute
     */
    IMPORT_C TInt Mute() const;

    /**
     * Return volume
     * 
     * @return volume
     */
    IMPORT_C TInt Volume() const;

    /**
     * Return AV Transport state
     * 
     * @return transport state
     */
    IMPORT_C TTransportState TransportState() const;

    /**
     * Return AV Transport Uri
     * 
     * @return uri
     */
    IMPORT_C const TDesC8& TransportURI() const;

public: // Not exported setters  
    
    /**
     * Set instance id
     * 
     * @param instance id
     */
    void SetInstanceID( TInt aInstanceID );
        
    /**
     * Set mute
     * 
     * @param mute
     */
    void SetMute( TInt aMute );

    /**
     * Set volume
     * 
     * @param volume
     */
    void SetVolume( TInt aVolume );

    /**
     * Set AV Transport state
     * 
     * @param transport state
     */
    void SetTransportState( TTransportState aTransportState );
        
    /**
     * Set AV Transport state
     * 
     * @param stanport state (as a descriptor)
     */
    void SetTransportState( const TDesC8& aData );

    /**
     * Set AV Tranport Uri
     * 
     * @param Uri
     */
    void SetTransportURIL( const TDesC8& aTransportURI );
 
public:    
    
    /**
     * Reset the member variables.
     */
    void Reset();

private: // data
    
    TInt            iInstanceID;
    
    TInt            iMute;

    TInt            iVolume;
    
    TTransportState iTransportState;
    
    /**
     * Heap buffer for AV Transport Uri.
     * Own.
     */
    HBufC8*         iTransportURI;
    };

#endif // C_UPNPAVTEVENT

// End of File
