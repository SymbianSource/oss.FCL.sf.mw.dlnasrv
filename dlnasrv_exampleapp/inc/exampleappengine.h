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
#ifndef EXAMPLEAPPENGINE_H
#define EXAMPLEAPPENGINE_H

#include <qobject.h>

class QFile;
class ExampleAppEnginePrivate;

class ExampleAppEngine : public QObject
{
    Q_OBJECT

public:
    enum
    {
        PlaybackStateStopped,
        PlaybackStatePaused,
        PlaybackStateBuffering,
        PlaybackStatePlaying
    };
    
public:
    ExampleAppEngine();
    ~ExampleAppEngine();
    
public:
    void construct();
    int getIap() const;
    QString getIapName() const;
    bool isPauseSupported() const;
    bool isSeekSupported() const;
    int getPlaybackState() const;
    
public slots:
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
    
signals:
    void initComplete(int);
    void stateChanged(int);
    void renderingDeviceSearchStarted();
    void renderingDeviceFound(const QString &, const QString &);
    void renderingDeviceDisappeared(const QString &, const QString &);
    void iapUpdated(const QString &);
    void iapUpdated(int);
        
private: // data
    ExampleAppEnginePrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, ExampleAppEngine)
    Q_DISABLE_COPY(ExampleAppEngine)    
};

#endif // EXAMPLEAPPENGINE_H
