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

#include "exampleappengine.h"
#include "exampleappengine_p.h"
#include "trace.h"

/*!
    /class ExampleAppEngine
    /brief Implements the playback engine and DLNA interface for the UI.
*/

/*!
    C++ constructor.
*/
ExampleAppEngine::ExampleAppEngine(): d_ptr(new ExampleAppEnginePrivate())
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->q_ptr = this;
    }
}

/*!
    C++ destructor.
*/
ExampleAppEngine::~ExampleAppEngine()
{
    FUNC_LOG
    
    delete d_ptr;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::construct()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->construct();
    }
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEngine::getIap() const
{
    FUNC_LOG
    
    int iap(0);
    
    if (d_ptr)
    {
        iap = d_ptr->getConnectedIap();
    }
    
    return iap;
}

/*!
    description
    
    /a
    /return
*/
QString ExampleAppEngine::getIapName() const
{
    FUNC_LOG
    
    QString iapName;
    
    if (d_ptr)
    {
        iapName = d_ptr->getConnectedIapName();
    }
    
    return iapName;
}

/*!
    description
    
    /a
    /return
*/
bool ExampleAppEngine::isSeekSupported() const
{
    FUNC_LOG
    
    bool isSupported(false);
    
    if (d_ptr)
    {
        isSupported = d_ptr->isSeekSupported();
    }
    
    return isSupported;
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEngine::getPlaybackState() const
{
    FUNC_LOG
    
    int state(ExampleAppEngine::PlaybackStateStopped);
    
    if (d_ptr)
    {
        state = d_ptr->getPlaybackState();
    }
    
    return state;
}

/*!
    description
    
    /a
    /return
*/
bool ExampleAppEngine::isPauseSupported() const
{
    FUNC_LOG
    
    bool isSupported(false);
    
    if (d_ptr)
    {
        isSupported = d_ptr->isPauseSupported();
    }
    
    return isSupported;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::searchRenderingDevices()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->searchRenderingDevices();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::prepareRenderingDevice(const QString &uuid)
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->prepareRenderingDevice(uuid);
    }
}

/*!
    description
    
    /a
    /return
*/
int ExampleAppEngine::initFile(const QString& file)
{
    FUNC_LOG
    
    int err(-1);
    
    if (d_ptr)
    {
        err = d_ptr->initFile(file);
    }
    
    return err;
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::play()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->play();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::pause()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->pause();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::stop()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->stop();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::volumeUp()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->volumeUp();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::volumeDown()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->volumeDown();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::rew()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->rew();
    }
}

/*!
    description
    
    /a
    /return
*/
void ExampleAppEngine::ff()
{
    FUNC_LOG
    
    if (d_ptr)
    {
        d_ptr->ff();
    }
}

// End of file
