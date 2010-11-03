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
    In general this class calls the private implementation functions for
    a minimal implementation required to enable media rendering on a
    DLNA rendering device.

    For a real application implementing this functionality, more robust
    error handling should be applied, this version mostly relies on the
    function calls to return succesfully.
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
    Checks for connected IAP.
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
    Returns the id of the currently connected IAP.
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
    Returns the name of the currently connected IAP
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
    Returns a boolean describing whether the selected renderer
    supports seek functionality.
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
    Returns the playback state of the selected renderer.
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
    Returns a boolean describing whether the selected renderer
    supports pause functionality.
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
    Initiate renderer search. Each renderer found from the current
    IAP will cause a renderingDeviceFound() signal to be emitted
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
    Prepares the renderer for accepting further commands. When this function
    returns, there should be a rendering session opened to the renderer whose
    uuid was passed in /a uuid.

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
    Calls the private implementation of the engine, which will cause
    the /a file to be shared as a DLNA item on the local push server.
    When the file is ready to be shared, an initComplete() signal is
    emitted.
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
    Issues the play() command to the renderer.
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
    Issues the pause() command to the renderer.
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
    Issues the stop() command to the renderer.
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
    Issues the volumeUp() command to the renderer.
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
    Issues the volumeDown() command to the renderer.
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
    Issues the rew() command to the renderer.
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
    Issues the ff() command to the renderer.
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
