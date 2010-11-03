/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP GStreamer wrapper
*
*/

#ifndef C_UPNPGSTWRAPPER_H
#define C_UPNPGSTWRAPPER_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <e32debug.h>

#include "upnpgstwrapperobserver.h"

class CUpnpGstWrapperPimpl;

class CUpnpGstWrapper : public CActive
    {
    
public: // construction / destruction
    

    /**
     * Get wrapper instance.
     * Increased reference count by 1.
     *
     * @return none
     */
    IMPORT_C static CUpnpGstWrapper* GetInstanceL();
    
    /**
     * Decreses reference count by 1. 
     * If reference count decreses to zero, singleton will be deleted.
     *
     * @return none
     */
    IMPORT_C void Close();

private: // construction, private part
    
    /**
     * Constructor
     */
    CUpnpGstWrapper();
    
    /**
     * Destructor
     */
    IMPORT_C virtual ~CUpnpGstWrapper();
    
    /**
     * 2nd phrase Constructor
     */
    void ConstructL();

public:
    
    /**
     * Set observer
     *
     * @param MUpnpGstWrapperObserver observer to be set
     * @return none
     */
    IMPORT_C void SetObserverL( MUpnpGstWrapperObserver& aObserver );
    
    /**
     * Remove observer
     *
     * @param MUpnpGstWrapperObserver observer to be removed
     * @return none
     */
    IMPORT_C void RemoveObserver( MUpnpGstWrapperObserver& aObserver );
    
    /**
     * Start a pipeline; e.g. transcoding. This is used when client knows 
     * exactly what the pipeline is and the elements inside
     * 
     * @return none
     */
    IMPORT_C void StartL();
    
    /**
     * Stop the pipeline; e.g. transcoding
     * 
     * @param none
     * @return none
     */
    IMPORT_C void Stop();
    
    /**
     * Return the error description
     * 
     * @param None
     * @return A reference to the error description
     */
    IMPORT_C const TDesC& ErrorMsg();
	
    /**
     * Return list of currently supported GStreamer features
     * Note: Ownership of the array is passed to the caller
     * 
     * @param None
     * @return A pointer to descriptor array
     */
    IMPORT_C CDesCArray* FeatureListL();
    
    /**
     * Return the video info descriptor 
     * 
     * E.g.: 
     * video/x-h264, width=(int)1280, height=(int)720, 
     * framerate=(fraction)30000/1, codec_data=(buffer)0164401fffe10010276400
     * 1fac2b402802dd80b4078913501000528ee025cb0
     * 
     * @param None
     * @return A reference to video info descriptor
     */
    IMPORT_C const TDesC& VideoInfo();
    
    /**
     * Return the audio info descriptor 
     * 
     * E.g.: 
     * audio/mpeg, rate=(int)48000, channels=(int)1, 
     * channel-positions=(GstAudioChannelPosition)< 
     * GST_AUDIO_CHANNEL_POSITION_FRONT_MONO >, mpegversion(int)4, 
     * codec_data=(buffer)1188
     * 
     * @param None
     * @return A reference to audio info descriptor
     */
    IMPORT_C const TDesC& AudioInfo();
    
    /**
     * Return (transoding) position in nanoseconds.
     * 
     * @return Postion in nanoseconds; KErrNotFound if position not available
     */
    IMPORT_C TInt64 Position();
    
    /**
     * Return (transoding) duration in nanoseconds.
     * 
     * @return Duration in nanoseconds; KErrNotFound if duration not available 
     */
    IMPORT_C TInt64 Duration();
    
    /**
     * Return pointter to GStreamer pipeline.
     * 
     * @return Pointer to GStreamer pipeline.
     */
    IMPORT_C TAny* PipelinePtr();
    
	/**
	 * Initialises gstreamer appsink element named sinkbuffer if there is
	 */
    IMPORT_C void InitAppsinkL();
    
	/**
	 * pulls buffer from gstreamer appsink element
	 * 
	 * @return boolean true if end of transcoding
	 */
    IMPORT_C TBool PullBufferL( TPtr8& aPointer,TInt aMaxBytesLength, RBuf8* aBuf=NULL );
    
	/**
	 * returns count of bytes transcoded so far
	 */
    IMPORT_C TInt TranscodedBytes();

    /**
     * Setter
     */
    IMPORT_C void SetTranscodedFileSize( TInt aSize );
    
    /**
     * getter
     */
    IMPORT_C TInt TranscodedFileSize();
    
    /**
     * sets pipline has to be set before calling start()
     * 
     * @param aPipeline
     */
    IMPORT_C void SetPipelineL( const TDesC8& aPipeline );
        
    
private:
           
    /**
     * Set error message
     * 
     * @param TDesC& a reference to error message descriptor
     * @return none 
     */            
    void SetErrorMsgL( const TDesC& aErrorMsg );
    
    /**
     * Set error message
     * 
     * @param TDesC& a reference to error message descriptor
     * @return none 
     */            
    void SetErrorMsg( HBufC* const aErrorMsg );    
    
    /**
     * Convert char array to symbian descriptor
     * 
     * @param char* the reference of input string
     * @return HBufC* the pointer to the output 
     */    
    HBufC* ConvertCharToDescL( const char* aString );
    
    /**
     * Convert symbian description to char array
     * 
     * @param TDesC8& the reference of input string
     * @return char* the pointer to the output 
     */    
    char* ConvertDescToCharL( const TDesC8& aDescriptor );
    
    /**
     * Send an event to every registered observer.
     * 
     * @param Gst::TEvent evnet to be sent
     * @return None
     */        
    void SendEventToObsevers( Gst::TEvent aEvent );
    
private: // from CActive
    
    /**
     * @see e32base.h
     */
    void RunL();

    /**
     * @see e32base.h
     */
    void DoCancel();
    
private:
    
    CUpnpGstWrapperPimpl*                  iPimpl;    
    TThreadId                              iClientThreadId;
    HBufC*                                 iErrorMsg;       //owned
    HBufC*                                 iVideoInfo;      //owned
    HBufC*                                 iAudioInfo;      //owned
    RArray<Gst::TEvent>                    iEventQueue;
    RMutex                                 iMutex;
    TInt64                                 iDuration;
    TInt                                   iSingletonRefCount;
    RPointerArray<MUpnpGstWrapperObserver> iObservers;
    TInt                                   iFileSize;
    
    HBufC8*                                iPipeline;
    
private:
    
    friend class CUpnpGstWrapperPimpl;
    
    };


#endif // C_UPNPGSTWRAPPER_H

