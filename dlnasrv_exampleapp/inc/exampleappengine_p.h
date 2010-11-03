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
* Description: Dlna private interface definitions
*
*/

#ifndef EXAMPLEAPPENGINE_P_H
#define EXAMPLEAPPENGINE_P_H

#include <e32base.h>
#include <upnpavdeviceobserver.h>
#include <upnpavrenderingsessionobserver.h>
#include <upnprenderingstatemachineobserver.h>
#include <upnpvolumestatemachineobserver.h>
#include <upnpitemresolverobserver.h>
#include <upnpconnectionmonitorobserver.h>

#include "exampleappengine.h"

class CUPnPSettingsEngine;
class MUPnPAVRenderingSession;
class CUpnpRenderingStateMachine;
class CUpnpVolumeStateMachine;
class MUPnPAVController;
class CUpnpAVDeviceList;
class CUPnPConnectionMonitor;

class ExampleAppEnginePrivate : public QObject,
public MUPnPAVDeviceObserver,
public MUPnPAVRenderingSessionObserver,
public MUpnpRenderingStateMachineObserver,
public MUpnpVolumeStateMachineObserver,
public MUPnPItemResolverObserver,
public MUPnPConnectionMonitorObserver
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(ExampleAppEngine)

public:
    ExampleAppEnginePrivate();
    virtual ~ExampleAppEnginePrivate();
    
public:
    void construct();
    int getConnectedIap() const;
    QString getConnectedIapName() const;
    int getPlaybackState() const;
    bool isSeekSupported() const;
    bool isPauseSupported() const;
    void searchRenderingDevices();
    void prepareRenderingDevice(const QString &uuid);
    int initFile(const QString& file);
    void play();
    void pause();
    void stop();
    void volumeUp();
    void volumeDown();
    void rew();
    void ff();
    
protected:
    // from MUPnPAVDeviceObserver
    void UPnPDeviceDiscovered(const CUpnpAVDevice& aDevice);
    void UPnPDeviceDisappeared(const CUpnpAVDevice& aDevice);
    void WLANConnectionLost();
    
    // from MUPnPAVRenderingSessionObserver
    void VolumeResult(TInt aError, TInt aVolumeLevel, TBool aActionResponse);
    void MuteResult(TInt aError, TBool aMute, TBool aActionResponse);
    void InteractOperationComplete(TInt aError, TUPnPAVInteractOperation aOperation);
    void PositionInfoResult(TInt aError, const TDesC8& aTrackPosition, const TDesC8& aTrackLength);
    void SetURIResult(TInt aError);
    void SetNextURIResult(TInt aError);
    void MediaRendererDisappeared(TUPnPDeviceDisconnectedReason aReason);
    
    // from MUpnpRenderingStateMachineObserver
    void RendererSyncReady(TInt aError, Upnp::TState aState);
    void RenderingStateChanged(TInt aError, Upnp::TState aState, TBool aUserOriented, TInt aStateParam);
    void PositionSync(TInt aError, Upnp::TPositionMode aMode, TInt aDuration, TInt aPosition);
    
    // from MUpnpVolumeStateMachineObserver
    void VolumeSyncReady(TInt aError);
    void VolumeChanged(TInt aError, TInt aVolume, TBool aUserOriented);
    void MuteChanged(TInt aError, TBool aMuteState, TBool aUserOriented);

    // from MUPnPItemResolverObserver
    void ResolveComplete(const MUPnPItemResolver& aResolver, TInt aError);
    
    // from MUPnPConnectionMonitorObserver
    void ConnectionLost(TBool aUserOriented);
    void ConnectionCreated(TInt aConnectionId);
    
private:
    void searchRenderingDevicesL();
    void upnpDeviceDiscoveredL(const CUpnpAVDevice& aDevice);
    void prepareRenderingDeviceL(const QString &uuid);
    void startRenderingSessionL(const CUpnpAVDevice &device);
    void stopRenderingSession();
    void initFileL(const QString& file);
    bool isReadyForPlayback() const;
    void playL();
    void pauseL();
    void stopL();
    void resolveIapL();
    void resolveIapNameL();
    
    QString asString(const TDesC &desc) const;
    QString asString8(const TDesC8 &desc) const;

private:
    ExampleAppEngine *q_ptr;
    CUPnPSettingsEngine *mSettingsEngine;
    MUPnPAVRenderingSession *mAVRenderingSession;
    CUpnpRenderingStateMachine *mRenderingStateMachine;
    CUpnpVolumeStateMachine *mVolumeStateMachine;
    MUPnPAVController *mAVController;
    int mIap;
    QString mIapName;
    CUpnpAVDeviceList *mDevices;
    CUpnpAVDevice *mDevice;
    int mPlaybackState;
    MUPnPItemResolver *mItemResolver;
    CUPnPConnectionMonitor *mConnectionMonitor;
};

#endif // EXAMPLEAPPENGINE_P_H
