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
* Description:
*
*/

#include <es_sock.h>
#include <es_enum.h>
#include <upnpsettingsengine.h>
#include <upnpavcontrollerfactory.h>
#include <upnpavcontroller.h>
#include <upnpavdevice.h>
#include <upnpavdevicelist.h>
#include <upnprenderingstatemachine.h>
#include <upnpavrenderingsession.h>
#include <upnpvolumestatemachine.h>
#include <upnpitemresolverfactory.h>
#include <upnpitemresolver.h>
#include <upnpconnectionmonitor.h>

#include "exampleappengine_p.h"
#include "trace.h"

/*!
    /class ExampleAppEnginePrivate
    /brief Implements interface to Symbian side DLNA APIs.
*/

/*!
    C++ constructor.
*/
ExampleAppEnginePrivate::ExampleAppEnginePrivate():
    q_ptr(0),
    mSettingsEngine(0),
    mAVRenderingSession(0),
    mRenderingStateMachine(0),
    mVolumeStateMachine(0),
    mAVController(0),
    mIap(0),
    mIapName(""),
    mDevices(0),
    mDevice(0),
    mPlaybackState(ExampleAppEngine::PlaybackStateStopped),
    mItemResolver(0)
{
    FUNC_LOG    

    TRAP_IGNORE(mSettingsEngine = CUPnPSettingsEngine::NewL());
    
    TRAP_IGNORE(mDevices = CUpnpAVDeviceList::NewL());
    
    TRAP_IGNORE(mConnectionMonitor = CUPnPConnectionMonitor::NewL(0));
    mConnectionMonitor->SetObserver(*this);
}

/*!
    C++ destructor.
*/
ExampleAppEnginePrivate::~ExampleAppEnginePrivate()
{
    FUNC_LOG
    
    delete mItemResolver;
    
    delete mDevices;
    
    delete mSettingsEngine;
    
    if (mAVRenderingSession)
    {
        if (mAVController)
        {
            mAVController->StopRenderingSession(*mAVRenderingSession);
        }
    }
    
    delete mRenderingStateMachine;
    
    delete mVolumeStateMachine;
    
    if (mAVController)
    {
        mAVController->Release();
    }
    
    delete mConnectionMonitor;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::construct()
{
    FUNC_LOG

    resolveIapL();
    resolveIapNameL();
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEnginePrivate::getConnectedIap() const
{
    FUNC_LOG
    
    return mIap;
}

/*!
    description
    
    /a
    /return
*/
QString ExampleAppEnginePrivate::getConnectedIapName() const
{
    FUNC_LOG
    
    return mIapName;
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEnginePrivate::getPlaybackState() const
{
    FUNC_LOG
    
    return mPlaybackState;
}

/*!
    description
    
    /a
    /return
*/
bool ExampleAppEnginePrivate::isSeekSupported() const
{
    FUNC_LOG

    // TODO: return real value when seek is implemented
    
    return false;
}

/*!
    description
    
    /a
    /return
*/
bool ExampleAppEnginePrivate::isPauseSupported() const
{
    FUNC_LOG
    
    bool isSupported(false);
    
    if (mDevice)
    {
        isSupported = mDevice->PauseCapability();
    }
    
    return isSupported;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::searchRenderingDevices()
{
    FUNC_LOG
    
    if (mIap)
    {
        // first create av controller
        if (!mAVController)
        {
            TRAP_IGNORE(mAVController = UPnPAVControllerFactory::NewUPnPAVControllerL());
            if (!mAVController)
            {
                return;
            }
            mAVController->SetDeviceObserver(*this);
        }
        
        // check if devices have been found
        TRAP_IGNORE(searchRenderingDevicesL());
    }    
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::prepareRenderingDevice(const QString &uuid)
{
    FUNC_LOG
    
    TRAP_IGNORE(prepareRenderingDeviceL(uuid));    
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEnginePrivate::initFile(const QString& file)
{
    FUNC_LOG
    
    TRAPD(err, initFileL(file));
    
    return err;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::play()
{
    FUNC_LOG

    TRAP_IGNORE(playL());
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::pause()
{
    FUNC_LOG
    
    TRAP_IGNORE(pauseL());
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::stop()
{
    FUNC_LOG
    
    TRAP_IGNORE(stopL());
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::volumeUp()
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::volumeDown()
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::rew()
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::ff()
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::UPnPDeviceDiscovered(const CUpnpAVDevice& aDevice)
{
    FUNC_LOG
    
    TRAP_IGNORE(upnpDeviceDiscoveredL(aDevice));
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::UPnPDeviceDisappeared(const CUpnpAVDevice& aDevice)
{
    FUNC_LOG

    // search device from list
    CUpnpAVDevice *device(NULL);
    int count(mDevices->Count());
    int index(0);
    for (int i = 0; i < count; i++)
    {
        device = (*mDevices)[i];
        if (device->Uuid() == aDevice.Uuid())
        {
            index = i;
            break;
        }
        device = NULL;
    }
    
    // remove if device was found
    if (device)
    {
        mDevices->Remove(index);
        
        if (device->Uuid() == mDevice->Uuid())
        {
            stopRenderingSession();
        }
        
        QString name = asString8(device->FriendlyName());
        QString uuid = asString8(device->Uuid());        
        emit q_ptr->renderingDeviceDisappeared(name, uuid);
        
        delete device;
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::WLANConnectionLost()
{
    FUNC_LOG

    // no implementation required
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::VolumeResult(TInt aError,
    TInt aVolumeLevel,
    TBool aActionResponse)
{
    FUNC_LOG

    if (mVolumeStateMachine)
    {
        mVolumeStateMachine->VolumeResult(aError, aVolumeLevel, aActionResponse);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::MuteResult(TInt aError,
    TBool aMute,
    TBool aActionResponse)
{
    FUNC_LOG

    if (mVolumeStateMachine)
    {
        mVolumeStateMachine->MuteResult(aError, aMute, aActionResponse);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::InteractOperationComplete(TInt aError,
    TUPnPAVInteractOperation aOperation)
{
    FUNC_LOG

    if (mRenderingStateMachine)
    {
        mRenderingStateMachine->InteractOperationComplete(aError, aOperation);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::PositionInfoResult(TInt aError,
    const TDesC8& aTrackPosition,
    const TDesC8& aTrackLength)
{
    FUNC_LOG

    if (mRenderingStateMachine)
    {
        mRenderingStateMachine->PositionInfoResult(aError, aTrackPosition, aTrackLength);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::SetURIResult(TInt aError)
{
    FUNC_LOG

    if (mRenderingStateMachine)
    {
        mRenderingStateMachine->SetURIResult(aError);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::SetNextURIResult(TInt aError)
{
    FUNC_LOG
    
    if (mRenderingStateMachine)
    {
        mRenderingStateMachine->SetNextURIResult(aError);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::MediaRendererDisappeared(TUPnPDeviceDisconnectedReason /*aReason*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::RendererSyncReady(TInt /*aError*/, Upnp::TState /*aState*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::RenderingStateChanged(TInt aError,
    Upnp::TState aState,
    TBool /*aUserOriented*/,
    TInt /*aStateParam*/)
{
    FUNC_LOG

    int state(ExampleAppEngine::PlaybackStateStopped);
    
    if (aError == KErrNone)
    {
        switch (aState)
        {
            case Upnp::EBuffering:
            {
                state = ExampleAppEngine::PlaybackStateBuffering;
                break;
            }
            case Upnp::EPlaying:
            {
                state = ExampleAppEngine::PlaybackStatePlaying;
                break;
            }
            case Upnp::EPaused:
            {
                state = ExampleAppEngine::PlaybackStatePaused;
                break;
            }
            default:
            {
                // no actions, stopped state is used
                break;
            }
        }
    }
    
    mPlaybackState = state;
    
    emit q_ptr->stateChanged(mPlaybackState);
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::PositionSync(TInt /*aError*/,
    Upnp::TPositionMode /*aMode*/,
    TInt /*aDuration*/,
    TInt /*aPosition*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::VolumeSyncReady(TInt /*aError*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::VolumeChanged(TInt /*aError*/,
    TInt /*aVolume*/,
    TBool /*aUserOriented*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::MuteChanged(TInt /*aError*/,
    TBool /*aMuteState*/,
    TBool /*aUserOriented*/)
{
    FUNC_LOG
    
    // TODO: implement
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::ResolveComplete(const MUPnPItemResolver& /*aResolver*/,
    TInt aError)
{
    FUNC_LOG
    
    ERROR(aError, "Error while resolving an item");
    
    emit q_ptr->initComplete(aError);
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::ConnectionLost(TBool /*aUserOriented*/)
{
    FUNC_LOG

    stopRenderingSession();
    
    mIap = 0;
    mIapName = "Not connected";

    emit q_ptr->iapUpdated(mIapName);
    emit q_ptr->iapUpdated(mIap);
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::ConnectionCreated(TInt /*aConnectionId*/)
{
    FUNC_LOG

    TRAP_IGNORE(resolveIapL());
    TRAP_IGNORE(resolveIapNameL());

    emit q_ptr->iapUpdated(mIapName);
    emit q_ptr->iapUpdated(mIap);
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::searchRenderingDevicesL()
{
    FUNC_LOG
    
    emit q_ptr->renderingDeviceSearchStarted();

    CUpnpAVDeviceList *deviceList = mAVController->GetMediaRenderersL();
    int count(deviceList->Count());
    for (int i = count - 1; i >= 0; i--)
    {
        CUpnpAVDevice *device = (*deviceList)[i];
        upnpDeviceDiscoveredL(*device);
    }
    delete deviceList;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::upnpDeviceDiscoveredL(const CUpnpAVDevice& aDevice)
{
    FUNC_LOG

    if (aDevice.DeviceType() == CUpnpAVDevice::EMediaRenderer)
    {
        CUpnpAVDevice *device(0);
        int count(mDevices->Count());
        for (int i = 0; i < count; i++)
        {
            device = (*mDevices)[i];
            if (device->Uuid() == aDevice.Uuid())
            {
                // found
                break;
            }
            device = 0;
        }
        
        if (!device)
        {
            // create new device
            device = CUpnpAVDevice::NewL(aDevice);
            mDevices->AppendDeviceL(*device);
            
            QString name = asString8(device->FriendlyName());
            QString uuid = asString8(device->Uuid());

            INFO_2("New rendering device found: Name = %s, Uuid = %s",
                name.utf16(), uuid.utf16());
            
            emit q_ptr->renderingDeviceFound(name, uuid);
        }
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::prepareRenderingDeviceL(const QString &uuid)
{
    FUNC_LOG
    
    // get current uuid
    QString currentUuid;
    if (mDevice)
    {
        currentUuid = asString8(mDevice->Uuid());
    }
    
    // check if uuid is different than the requsted one
    if (currentUuid != uuid)
    {
        // search the rendering device
        CUpnpAVDevice *device = 0;
        int count(mDevices->Count());
        for (int i = 0; i < count; i++)
        {
            device = (*mDevices)[i];
            QString deviceUuid = asString8(device->Uuid());
            if (deviceUuid == uuid)
            {
                // device found
                break;
            }
            device = 0;
        }
        
        stopRenderingSession();
        
        mDevice = device;
        if (mDevice)
        {            
            startRenderingSessionL(*mDevice);
        }
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::startRenderingSessionL(const CUpnpAVDevice &device)
{
    FUNC_LOG

    // start new rendering session
    mAVRenderingSession = &mAVController->StartRenderingSessionL(device);
    mAVRenderingSession->SetObserver(*this);
    
    // start new rendering state machine
    mRenderingStateMachine = CUpnpRenderingStateMachine::NewL(*mAVRenderingSession);
    mRenderingStateMachine->SetObserver(*this);
    mRenderingStateMachine->SyncL();
    
    // start new volume state machine
    mVolumeStateMachine = CUpnpVolumeStateMachine::NewL(*mAVRenderingSession);
    mVolumeStateMachine->SetObserver(*this);
    mVolumeStateMachine->SyncL();
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::stopRenderingSession()
{
    FUNC_LOG
    
    // stop ongoing playback
    if (mPlaybackState != ExampleAppEngine::PlaybackStateStopped)
    {
        stop();
        RenderingStateChanged(KErrNone, Upnp::EStopped, EFalse, 0);
    }
    
    // release rendering state machine
    delete mRenderingStateMachine;
    mRenderingStateMachine = 0;
    
    // release volume state machine
    delete mVolumeStateMachine;
    mVolumeStateMachine = 0;

    // stop and release rendering session
    if (mAVController && mAVRenderingSession)
    {
        mAVController->StopRenderingSession(*mAVRenderingSession);            
        mAVRenderingSession = 0;
    }
    
    mDevice = 0;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::initFileL(const QString &file)
{
    FUNC_LOG
    
    delete mItemResolver;
    mItemResolver = 0;
    
    if (mDevice)
    {
        TPtrC filePath(file.utf16(), file.length());
        TUPnPSelectDefaultResource selector;
        mItemResolver =
            UPnPItemResolverFactory::NewLocalItemResolverL(
                filePath, *mAVController, selector);
        mItemResolver->ResolveL(*this, mDevice);
    }
    else
    {
        // rendering device has not been selected
        User::Leave(KErrNotReady);
    }
}

/*!
    description
    
    /a
    /return
*/
bool ExampleAppEnginePrivate::isReadyForPlayback() const
{
    FUNC_LOG
    
    bool isReady(false);
    
    if (mDevice &&           // device is selected
        mAVRenderingSession &&      // av rendering session is created
        mRenderingStateMachine &&   // rendering state machine is created
        mVolumeStateMachine)        // volume state machine is created
    {
        isReady = true;
    }
    
    return isReady;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::playL()
{
    FUNC_LOG
    
    if (isReadyForPlayback())
    {
        mRenderingStateMachine->CommandL(Upnp::EPlay, 0, &mItemResolver->Item());
    }
    else
    {
        User::Leave(KErrNotReady);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::pauseL()
{
    FUNC_LOG
    
    if (isReadyForPlayback())
    {
        // double check that the rendering device supports pause capability
        if (mDevice->PauseCapability())
        {
            mRenderingStateMachine->CommandL(Upnp::EPause);
        }
        else
        {
            User::Leave(KErrNotSupported);
        }
    }
    else
    {
        User::Leave(KErrNotReady);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::stopL()
{
    FUNC_LOG
    
    if (isReadyForPlayback())
    {
        mRenderingStateMachine->CommandL(Upnp::EStop);
    }
    else
    {
        User::Leave(KErrNotReady);
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::resolveIapL()
{
    FUNC_LOG
    
    // code below could be optimized so that connections to socket server
    // are only made once
    RSocketServ socketServ;
    int err = socketServ.Connect();
    if (err == KErrNone)
    {
        RConnection connection;
        err = connection.Open(socketServ);
        if (err == KErrNone)
        {
            uint connectionCount(0);
            err = connection.EnumerateConnections(connectionCount);
            if (err == KErrNone &&
            connectionCount == 1)
            {
                // One active connection - find it and try using it
                TPckgBuf<TConnectionInfo> connectionInfo;
                for (int i = 1; i <= connectionCount; ++i)
                {
                    if (connection.GetConnectionInfo(i, connectionInfo) == KErrNone)
                    {
                        // resolve iap id and name
                        mIap = connectionInfo().iIapId;
                    }
                }
                connection.Close();
            }
        }
        socketServ.Close();
    }
    
    mSettingsEngine->SetAccessPoint(mIap);
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEnginePrivate::resolveIapNameL()
{
    FUNC_LOG
    
    HBufC* iapName(NULL);
    TRAP_IGNORE(iapName = CUPnPSettingsEngine::GetCurrentIapNameL(mIap));
    if (iapName)
    {
        mIapName = asString(*iapName);
        delete iapName;
    }
}

/*!
    description
    
    /a
    /return
*/
QString ExampleAppEnginePrivate::asString(const TDesC &desc) const
{
    return QString::fromUtf16(desc.Ptr(), desc.Length());
}

/*!
    description
    
    /a
    /return
*/
QString ExampleAppEnginePrivate::asString8(const TDesC8 &desc) const
{
    return QString::fromUtf8((char*)desc.Ptr(), desc.Length());
}

// End of file
