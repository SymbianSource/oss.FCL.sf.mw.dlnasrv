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
* Description:  Implementation of UPnP GStreamer wrapper
*
*/

// INCLUDES
#include <bautils.h>

#include <gst/app/gstappsink.h>

#include <gst/app/gstappsink.h>
#include <gst/app/gstappbuffer.h>

#include <gst/gst.h>
#include <gst/gstinfo.h>

#include "upnpgstwrapper.h"

_LIT( KComponentLogfile, "upnpgstwrapper.txt");
#include "upnplog.h"

#define __FUNC_LOG __LOG8_1( "%s", __PRETTY_FUNCTION__ );

_LIT( KAlreadyInUseErrorMsg, "GST already in use (call Stop first)" );
_LIT( KUnknownErrorErrorMsg, "Unknown error" );
_LIT( KOutOfMemoryErrorMsg, "Out of memory" );

const char* const KMimeTypeVideo = "video/";
const char* const KMimeTypeAudio = "audio/";
const char* const KDemuxerName = "demux";

class CUpnpGstWrapperPimpl : CBase
    {
    
public: // construction / destruction
       
    /**
     * Static NewL
     *
     * @param none
     * @return instance to CUpnpGstWrapperPimpl
     */
    static CUpnpGstWrapperPimpl* NewL();    
    
    /**
     * Destructor
     */
    ~CUpnpGstWrapperPimpl();
    
private:
    
    /**
     * Constructor
     */
    CUpnpGstWrapperPimpl();
    
    /**
     * 2nd phrase Constructor
     */
    void ConstructL();

public:
    
    inline void SetPipeline( GstElement* aPipeline );
    
    inline GstElement* Pipeline(); 
    
    inline void SetDemuxer( GstElement* aDemuxer );
    
    inline GstElement* Demuxer();
    
    inline void InitAppSinkL();
    
    inline TBool PullBufferL( TPtr8& aPointer,TInt aMaxBytesLength, RBuf8* aBuf );
    
    inline TInt TranscodedBytes();
    
public: //callbacks
    
    /**
     * A function to receive any message occured within the pipeline
     * Note that the function will be called in the same thread context as
     * the posting object.
     * 
     * @param GstBus* a bus to create the watch for
     * @param GstMessage* a message created within the pipeline
     * @param gponter the user data that has been given, 
     *        when registering the handler
     * @return GstBusSyncReply indicates whether the message is also added
     *         into async queue (after this callback) or dropped
     */
    static GstBusSyncReply SyncBusCallback( GstBus* aBus, GstMessage* aMsg, 
            gpointer aData );
    
    /**
     * A function to receive callback from demuxer whenever source pad is
     * added into it
     * 
     * @param GstElement* an element (demuxer) where the source pad was added 
     * @param GstPad* a souce pad that was added
     * @param gponter the user data that has been given, 
     *        when registering the handler
     */
    static void DemuxPadAddedCallback( GstElement* aElement, GstPad* aPad,
            gpointer aData );
    
    /**
     * Internal log handler.
     * Implements GStreamer's log handler callback.
     * Ouputs GStreamer debug traces if DEBUG_ENABLE flag is enabled in
     * GStreamer.
     */    
    static void LogCallback( GstDebugCategory *category, GstDebugLevel level,
            const gchar *file, const gchar *function, gint line, 
            GObject *object, GstDebugMessage *message, gpointer data );

private:
    
    GstElement*                            iPipeline;
    GstElement*                            iDemuxer;        
    
    TInt                                   iBytesOffSet;
    
    GstElement*                            iAppsink;

    GstElement*                            iElement;
    
    GstBuffer*                             iGstBuffer;
    
    };

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::NewL
//---------------------------------------------------------------------------
CUpnpGstWrapperPimpl* CUpnpGstWrapperPimpl::NewL()
    {    
    __FUNC_LOG;
    
    CUpnpGstWrapperPimpl* self = new( ELeave ) CUpnpGstWrapperPimpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );  
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::~CUpnpGstWrapperPimpl
//---------------------------------------------------------------------------
CUpnpGstWrapperPimpl::~CUpnpGstWrapperPimpl()
    {  
    __FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::CUpnpGstWrapperPimpl
//---------------------------------------------------------------------------
CUpnpGstWrapperPimpl::CUpnpGstWrapperPimpl()
    {    
    __FUNC_LOG;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::ConstructL
//---------------------------------------------------------------------------
void CUpnpGstWrapperPimpl::ConstructL()
    {    
    __FUNC_LOG;
    }

void CUpnpGstWrapperPimpl::SetPipeline( GstElement* aPipeline )
    {
    __FUNC_LOG;
    
    iPipeline = aPipeline;
    }

GstElement* CUpnpGstWrapperPimpl::Pipeline()
    {
    __FUNC_LOG;
    return iPipeline;
    }

void CUpnpGstWrapperPimpl::SetDemuxer( GstElement* aDemuxer )
    {    
    __FUNC_LOG;
    
    iDemuxer = aDemuxer;
    }

GstElement* CUpnpGstWrapperPimpl::Demuxer()
    {
    __FUNC_LOG;
    
    return iDemuxer;
    }

void CUpnpGstWrapperPimpl::InitAppSinkL()
    {
    iAppsink = gst_bin_get_by_name (
            GST_BIN ( iPipeline ), "sinkbuffer");
  
    if( !iAppsink )
        User::Leave( KErrNotFound);
    
    g_object_set (
             G_OBJECT (iAppsink), "emit-signals", TRUE, "sync", FALSE, NULL);       
    gst_object_unref (iAppsink); 
    }
    
TBool CUpnpGstWrapperPimpl::PullBufferL( TPtr8& aPointer,TInt aMaxBytesLength, RBuf8* aBuf )
    {
    guint size;     
     
    if( iGstBuffer )
        {
        delete [] GST_BUFFER_DATA(iGstBuffer);
        GST_BUFFER_DATA(iGstBuffer) = NULL;
        }
    
    iGstBuffer = gst_app_sink_pull_buffer( GST_APP_SINK ( iAppsink  ));
    
    if( iGstBuffer )
        {
        size = GST_BUFFER_SIZE( iGstBuffer );

        iBytesOffSet += size;
        
        if( aBuf && ( size + aBuf->Length() ) > aBuf->MaxLength() )
            {
            HBufC8* tmp(NULL);
            TInt len(aBuf->Length());
            if( len > 0 )
                {
                tmp = aBuf->AllocLC();
                }
            aBuf->Close();
            aBuf->CreateL(size+len);
            if( tmp )
                {
                aBuf->Append(*tmp);
                CleanupStack::PopAndDestroy(tmp);
                }
            
            aBuf->Append( (TUint8*)GST_BUFFER_DATA(iGstBuffer),size );
            }
        else
            {                       
            __ASSERT( (aMaxBytesLength >= size), __FILE__, __LINE__ );
            aPointer.Set((TUint8*)GST_BUFFER_DATA(iGstBuffer),size,aMaxBytesLength);
            }
        }
    return ( iGstBuffer ) ? EFalse : ETrue;
    }

TInt CUpnpGstWrapperPimpl::TranscodedBytes()
    {
    return iBytesOffSet;
    }
    
// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::SyncBusCallback
//---------------------------------------------------------------------------
GstBusSyncReply CUpnpGstWrapperPimpl::SyncBusCallback( GstBus* /*aBus*/, 
        GstMessage* aMsg, gpointer aData )
    {
    __FUNC_LOG;
    
    GstMessageType messageType = GST_MESSAGE_TYPE( aMsg );    
    
    if( messageType == GST_MESSAGE_EOS ||
        messageType == GST_MESSAGE_ERROR )
        {
        __ASSERT( aData, __FILE__, __LINE__ );      
        
        CUpnpGstWrapper* parent = static_cast<CUpnpGstWrapper*>( aData );
        Gst::TEvent event = Gst::EUnknown;
    
        parent->iMutex.Wait();
        
        switch( messageType ) 
            {
            case GST_MESSAGE_EOS:
                {        
                event = Gst::EEOS;  
                break;
                }
            case GST_MESSAGE_ERROR: 
                {
                GError* err = NULL;
                
                char* debug;
                gst_message_parse_error( aMsg, &err, &debug );
                g_free( debug );   
                
                if( err )
                    {
                    TRAP_IGNORE( parent->SetErrorMsg( 
                            parent->ConvertCharToDescL(
                            err->message ) ) );
                    g_error_free( err );
                    err = NULL;
                    }
                event = Gst::EError;        
                break;
                }
            default:
                {
                __ASSERT( EFalse, __FILE__, __LINE__ );
                }
            }

        parent->iEventQueue.Append( event );        
        
        __ASSERT( parent->IsActive(), __FILE__, __LINE__ );
        if( parent->iStatus == KRequestPending )
            {
            TRequestStatus* status = &parent->iStatus;
            RThread thread;
            TInt err = thread.Open( parent->iClientThreadId );
            __ASSERT( !err, __FILE__, __LINE__ );                        
                
            thread.RequestComplete( status, KErrNone );            
            thread.Close();
            }
        
        parent->iMutex.Signal();

        gst_message_unref( aMsg );
        }

    return GST_BUS_DROP; //do not add anything into async queue
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::DemuxPadAddedCallback
//---------------------------------------------------------------------------
void CUpnpGstWrapperPimpl::DemuxPadAddedCallback( GstElement* /*aElement*/, 
        GstPad* aPad, gpointer aData )
    {
    __FUNC_LOG;    
    __ASSERT( aData, __FILE__, __LINE__ );       
    
    CUpnpGstWrapper* parent = static_cast<CUpnpGstWrapper*>( aData );
    
    GstCaps* caps = gst_pad_get_caps( aPad );
    char* str = gst_caps_to_string( caps );  
    Gst::TEvent event = Gst::EUnknown;
    
    parent->iMutex.Wait();
    
    if( g_str_has_prefix( str, KMimeTypeVideo ) )
        {
        delete parent->iVideoInfo; parent->iVideoInfo = NULL;
        TRAPD( err, parent->iVideoInfo = parent->ConvertCharToDescL( str ) );
        if( !err )
            {
            event = Gst::EVideoStreamInfoAvailable;
            }
        else
            {            
            event = Gst::EError;
            TRAP_IGNORE( parent->SetErrorMsgL( KOutOfMemoryErrorMsg ) );
            }
        }
    else if( g_str_has_prefix( str, KMimeTypeAudio ) )
        {
        delete parent->iAudioInfo; parent->iAudioInfo = NULL;
        TRAPD( err, parent->iAudioInfo = parent->ConvertCharToDescL( str ) );
        if( !err )
            {
            event = Gst::EAudioStreamInfoAvailable;
            }
        else
            {            
            event = Gst::EError;
            TRAP_IGNORE( parent->SetErrorMsgL( KOutOfMemoryErrorMsg ) );
            }        
        }
    else
        {
        __ASSERT( EFalse, __FILE__, __LINE__ );
        }
    
    parent->iEventQueue.Append( event );        
    
    __ASSERT( parent->IsActive(), __FILE__, __LINE__ );
    if( parent->iStatus == KRequestPending )
        {
        TRequestStatus* status = &parent->iStatus;
        RThread thread;
        TInt err = thread.Open( parent->iClientThreadId );
        __ASSERT( !err, __FILE__, __LINE__ );                        
            
        thread.RequestComplete( status, KErrNone );            
        thread.Close();
        }
    
    parent->iMutex.Signal();
    
    g_free(str);
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapperPimpl::LogCallback
//---------------------------------------------------------------------------
void CUpnpGstWrapperPimpl::LogCallback( GstDebugCategory* category, 
        GstDebugLevel level, const char* file, const char* function, 
        gint line, GObject* /*object*/, GstDebugMessage* message, 
        gpointer /*data*/ )
    {  
    if (level > gst_debug_category_get_threshold (category))
        {
        return;    
        }    
    
#ifndef __UPNP_LOG_FILE    
    const char* const KGstLogFmt = "%s %20s %s:%d:%s %s\n";
    RDebug::Printf( KGstLogFmt,
            gst_debug_level_get_name (level),
            gst_debug_category_get_name (category), file, line, function,
            gst_debug_message_get (message) );
#else
    _LIT8( KGstLogFmtDesc, "%s %20s %s:%d:%s %s\n" );
    RFileLogger::WriteFormat( KLogDir, KComponentLogfile,
            EFileLoggingModeAppend, KGstLogFmtDesc, 
            gst_debug_level_get_name (level),
            gst_debug_category_get_name (category), file, line, function,
            gst_debug_message_get (message) );
#endif    
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::GetInstanceL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C CUpnpGstWrapper* CUpnpGstWrapper::GetInstanceL()
    {
    CUpnpGstWrapper* singleton = static_cast<CUpnpGstWrapper*>( Dll::Tls() );
    if ( singleton == 0 )
        {
        // singleton must be created first
        singleton = new ( ELeave ) CUpnpGstWrapper();
        CleanupStack::PushL( singleton );
        singleton->ConstructL();
        User::LeaveIfError( Dll::SetTls( singleton ) );
        CleanupStack::Pop( singleton );
        }    
    singleton->iSingletonRefCount++;

    return singleton;
    }
	
// --------------------------------------------------------------------------
// CUpnpGstWrapper::Close
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C void CUpnpGstWrapper::Close()
    {
    if(--iSingletonRefCount == 0)
        {
        Dll::FreeTls();
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::~CUpnpGstWrapper
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C CUpnpGstWrapper::~CUpnpGstWrapper()
    {    
    __FUNC_LOG;

    Stop();

    Cancel();
    
    gst_deinit();
    
    delete iErrorMsg;
    delete iVideoInfo;
    delete iAudioInfo;
    
    iEventQueue.Close();    
    iMutex.Close();
    iObservers.Close();
    
    delete iPimpl;
    delete iPipeline;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::CUpnpGstWrapper
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
CUpnpGstWrapper::CUpnpGstWrapper()
    : CActive( EPriorityStandard )
    , iDuration( KErrNotFound )
    {    
    __FUNC_LOG;    
    
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::ConstructL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
void CUpnpGstWrapper::ConstructL()
    {
    __FUNC_LOG;      
    
    iPimpl = CUpnpGstWrapperPimpl::NewL();
    
    iMutex.CreateLocal();
    
    RThread thread;
    iClientThreadId = thread.Id(); 
    
    /*
     * Install our own log handler
     * FIXME: After this there is two log handlers:
     *        
     *          Default: Uses glib's g_printerr() that prints 
     *                   RDebug::RawPrints ONLY in full udeb mode
     *          Our own: See behaviour in LogCallback function below
     *          
     *        We should somehow get rid of the default one...
     */                    
    gst_debug_add_log_function( CUpnpGstWrapperPimpl::LogCallback, this );
    
    __LOG( "Initializing GStreamer..." );
    gst_init( NULL, NULL );    
    __LOG( "GStreamer initialization done!" );     
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::SetObserverL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C void CUpnpGstWrapper::SetObserverL( 
        MUpnpGstWrapperObserver& aObserver )
    {
    if( iObservers.Find( &aObserver ) < 0 )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::RemoveObserverL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C void CUpnpGstWrapper::RemoveObserver( 
        MUpnpGstWrapperObserver& aObserver )
    {
    TInt index = iObservers.Find( &aObserver );
    if( index >= 0 )
        {
        iObservers.Remove( index );
        }    
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::StartL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C void CUpnpGstWrapper::StartL()
    {
    __FUNC_LOG;
    
    if( !iPipeline )
        {
        SetErrorMsgL( KAlreadyInUseErrorMsg );        
        User::Leave( KErrNotReady );  
        }

    //Create pipeline according to the input description
    char* pipelineStr = ConvertDescToCharL( *iPipeline );

    GError* err = NULL;
    /**
     * Create a new pipeline based on command line syntax. 
     * Please note that you might get a return value that is not NULL even 
     * though the error is set. In this case there was a recoverable 
     * parsing error and you can try to play the pipeline
     */
    __LOG( "Parsing GStreamer pipeline..." );
    GstElement* pipeline = gst_parse_launch( pipelineStr, &err );
    __LOG( "GStreamer pipeline parsing done!" );
    User::Free( pipelineStr );
    
    if( err )
        {    
        SetErrorMsg( ConvertCharToDescL( err->message ) );
        g_error_free( err );
        err = NULL;
        User::Leave( KErrGeneral );
        }
    
    iPimpl->SetPipeline( pipeline );
	
    //TODO: Hardcoded demuxer name 'KDemuxerName -> should be fetched somehow 
    //dynamically ??
    GstElement* demuxer = gst_bin_get_by_name( GST_BIN( pipeline ), 
            KDemuxerName );
         
    //If a demuxer is found, create 'pad-added' callback; else do nothing 
    if( demuxer )
        {
        g_signal_connect( demuxer, "pad-added", 
                G_CALLBACK( CUpnpGstWrapperPimpl::DemuxPadAddedCallback ), 
                this );
        iPimpl->SetDemuxer( demuxer );
        }     

    GstBus* bus = NULL;
    bus = gst_pipeline_get_bus( GST_PIPELINE( pipeline ) );
    if( !bus )
        {
        //unknown error
        User::Leave(KErrGeneral);	
        }	
    
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();     
        }
    
    gst_bus_set_sync_handler( bus, CUpnpGstWrapperPimpl::SyncBusCallback, this );
    
    gst_object_unref( bus );

    gst_element_set_state( pipeline, GST_STATE_PLAYING );       
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::Stop
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C void CUpnpGstWrapper::Stop()
    {    
    __FUNC_LOG;
    GstElement* demuxer = iPimpl->Demuxer();
    if( demuxer )
        {
        gst_object_unref( GST_OBJECT( demuxer ) );
        iPimpl->SetDemuxer( NULL );
        }    
    
    GstElement* pipeline = iPimpl->Pipeline();
    if( pipeline )
        {
        gst_element_set_state( pipeline, GST_STATE_NULL );
        gst_object_unref( GST_OBJECT( pipeline ) );
        iPimpl->SetPipeline( NULL );
        }
    //Cancel();    
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::ErrorMsg
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C const TDesC& CUpnpGstWrapper::ErrorMsg()
    {
    __FUNC_LOG;
    
    if( iErrorMsg )
        {
        return *iErrorMsg;
        }
    else
        {
        return KUnknownErrorErrorMsg();
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::FeatureListL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C CDesCArray* CUpnpGstWrapper::FeatureListL()
    {
    __FUNC_LOG;
    
    CDesCArray* array = new (ELeave) CDesCArrayFlat( 5 );
    CleanupStack::PushL( array );

    GList* features = gst_registry_get_feature_list( 
            gst_registry_get_default(), GST_TYPE_ELEMENT_FACTORY );
    while( features )
        {
        GstPluginFeature* feature = (GstPluginFeature*)features->data;        
        HBufC* tempBuf = ConvertCharToDescL( gst_plugin_feature_get_name( 
                feature ) );
        CleanupStack::PushL( tempBuf );
        array->AppendL( *tempBuf );
        CleanupStack::PopAndDestroy( tempBuf );
        features = features->next;
        }

    CleanupStack::Pop( array );
    return array;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::VideoInfo
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C const TDesC& CUpnpGstWrapper::VideoInfo()
    {
    __FUNC_LOG;
    
    if( iVideoInfo )
        {
        return *iVideoInfo;
        }
    else
        {
        return KNullDesC;
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::AudioInfo
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C const TDesC& CUpnpGstWrapper::AudioInfo()
    {
    __FUNC_LOG;
    
    if( iAudioInfo )
        {
        return *iAudioInfo;
        }
    else
        {
        return KNullDesC;
        }
    }
// --------------------------------------------------------------------------
// CUpnpGstWrapper::Position
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C TInt64 CUpnpGstWrapper::Position()
    {
    __FUNC_LOG;
    
    GstFormat fmt = GST_FORMAT_TIME;
    TInt64 position = KErrNotFound;
    
    //fetch position from pipeline (not from the demuxer)
    GstElement* pipeline = iPimpl->Pipeline();
    
    if( pipeline )
        {
        gst_element_query_position( pipeline, &fmt, &position );
        }
    
    return position;
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::Duration
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
EXPORT_C TInt64 CUpnpGstWrapper::Duration()
    {
    __FUNC_LOG;
     
     if( iDuration < 0 )
         {
         GstFormat fmt = GST_FORMAT_TIME;
         if( iPimpl->Demuxer() )
             {
             //fetch duration from demuxer
             gst_element_query_duration( iPimpl->Demuxer(), &fmt, &iDuration );
             } 
         else if( iPimpl->Pipeline() )
             {
             //backup plan: not so good choice as demuxer
             gst_element_query_position( iPimpl->Pipeline(), &fmt, &iDuration );
             }
         }
     
     return iDuration;
    }

EXPORT_C TAny* CUpnpGstWrapper::PipelinePtr()
    {
    return iPimpl->Pipeline();
    }

EXPORT_C void CUpnpGstWrapper::InitAppsinkL()
    {
    iPimpl->InitAppSinkL();
    }
   
EXPORT_C TBool CUpnpGstWrapper::PullBufferL( TPtr8& aPointer,TInt aMaxBytesLength, RBuf8* aBuf )
    {
    return iPimpl->PullBufferL(aPointer,aMaxBytesLength,aBuf);
    }
   
EXPORT_C TInt CUpnpGstWrapper::TranscodedBytes()
    {
    return iPimpl->TranscodedBytes();
    }
       
EXPORT_C void CUpnpGstWrapper::SetTranscodedFileSize( TInt aSize )
    {
    iFileSize = aSize;
    }

EXPORT_C TInt CUpnpGstWrapper::TranscodedFileSize()
    {
    return iFileSize;
    }

EXPORT_C void CUpnpGstWrapper::SetPipelineL( const TDesC8& aPipeline )
    {
    delete iPipeline;
    iPipeline = NULL;
    iPipeline = aPipeline.AllocL();
    }
     

// --------------------------------------------------------------------------
// CUpnpGstWrapper::SetErrorMsgL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
void CUpnpGstWrapper::SetErrorMsgL( const TDesC& aErrorMsg )
    {
    __FUNC_LOG;     
    
    delete iErrorMsg; iErrorMsg = NULL;
    iErrorMsg = aErrorMsg.AllocL();
    
    __LOG1( "CUpnpGstWrapper::SetErrorMsgL Error desc: %S", iErrorMsg );
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::SetErrorMsg
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
void CUpnpGstWrapper::SetErrorMsg( HBufC* const aErrorMsg )
    {
    delete iErrorMsg; iErrorMsg = NULL;
    iErrorMsg = aErrorMsg;
    
    __LOG1( "CUpnpGstWrapper::SetErrorMsg Error desc: %S", iErrorMsg );
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::ConvertCharToDescL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
HBufC* CUpnpGstWrapper::ConvertCharToDescL( const char* aString )
    {
    __FUNC_LOG;
    
    HBufC* desc = NULL;
    TPtrC8 tempPtr8( (TUint8*)(aString) );
    desc = HBufC::NewL( tempPtr8.Length() );
    desc->Des().Copy( tempPtr8 );
    
    return desc; 
    }


// --------------------------------------------------------------------------
// CUpnpGstWrapper::ConvertDescToCharL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
char* CUpnpGstWrapper::ConvertDescToCharL( const TDesC8& aDescriptor )
    {
    __FUNC_LOG;
    
    TUint length = aDescriptor.Length();
    char* string = (char*)User::AllocL( length + 1 );
    Mem::Copy((char*)string, aDescriptor.Ptr(), length );
    string[length] = 0; //null terminated
    return string;
    }

void CUpnpGstWrapper::SendEventToObsevers( Gst::TEvent aEvent )
    {
    __FUNC_LOG;
    
    TInt observerCount = iObservers.Count();
    for( TInt i = 0; i < observerCount; i++ )
        {
        iObservers[i]->HandleGstEvent( aEvent );
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::RunL
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
void CUpnpGstWrapper::RunL()
    {    
    __FUNC_LOG;
    if( iStatus.Int() >= KErrNone )
        {
        TBool completed = EFalse;   
        
        iMutex.Wait();
        
        TInt eventQueueCount = iEventQueue.Count();
        for( TInt i = 0; i < eventQueueCount; i++ )
            {
            Gst::TEvent event = iEventQueue[i];
            SendEventToObsevers( event );
            if( event == Gst::EEOS || event == Gst::EError )
                {
                __LOG1( "CUpnpGstWrapper::RunL Completed: %d (2=Error, 3=EOS)",
                        event);
                completed = ETrue;
                }
            }
        iEventQueue.Reset();
        
        if( !completed )
            {        
            iStatus = KRequestPending;
            SetActive();
            }        
        iMutex.Signal();
        }
    }

// --------------------------------------------------------------------------
// CUpnpGstWrapper::DoCancel
// See upnpgstwrapper.h
//---------------------------------------------------------------------------
void CUpnpGstWrapper::DoCancel()
    {  
    __FUNC_LOG;
    }

// end of file
