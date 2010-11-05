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

#include <qlabel.h>
#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlayoutitem.h>
#include <qfiledialog.h>
#include <qaction.h>

#include "exampleappui.h"
#include "exampleappengine.h"
#include "trace.h"

/*!
    \class ExampleAppUi
    \brief Implements the application UI for Dlnasrv example application.

    The application provides a simple UI for selecting a renderer device
    (from a connected WLAN AP, if none is connected that needs to be done
    outside the application UI) and selecting a file to be rendered and
    control buttons for controlling the playback on the renderer device.

    So the first step is to have a WLAN connection active. Only then the
    renderer search and selection can be done.

    The next step is to select a rendering device. When rendering device
    search is initiated, the underlying UPnP AV control server is queried
    for existing renderers. Also all new renderers appearing in the AP
    get signalled with renderingDeviceFound()

    When a renderer is selected, the next step is to select a file to be
    pushed to the renderer. When selecting a file, it gets published in
    the push server as a DLNA item. When that is done, the controls for
    playback are enabled and the renderer can be controlled with those.
*/

/*!
    ExampleAppUi constructor.
 */
ExampleAppUi::ExampleAppUi():
    mEngine(0),
    mSelectedRenderer(0),
    mSelectedFileLabel(0),
    mPlaybackStatus(0),
    mRenderingDevices(0),
    mPauseButton(0),
    mStopButton(0),
    mRewButton(0),
    mFfButton(0),
    mSelectFileButton(0),
    mSearchDevicesButton(0),
    mSelectedFile("")
{
    FUNC_LOG
}

/*!
    ExampleAppUi destructor.
*/
ExampleAppUi::~ExampleAppUi()
{
    FUNC_LOG

    delete mEngine;
}

/*!
    Creates application engine and the UI. Also the state of the WLAN AP
    connection gets queried and updated on the UI.
 */
void ExampleAppUi::construct()
{
    FUNC_LOG

    createEngine();
    createUi();
}

/*!
    Creates the UI components.
 */
void ExampleAppUi::createUi()
{
    FUNC_LOG

    // create main view
    QWidget *view = new QWidget;
    setCentralWidget(view);
    
    // create main view layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    view->setLayout(mainLayout);
    
    // create accesspoint selection
    QLayout *layout = createApSelectionUi();
    mainLayout->addLayout(layout);
    mainLayout->addSpacing(40);
    
    // create renderer selection
    layout = createRendererSelectionUi();
    mainLayout->addLayout(layout);
    mainLayout->addSpacing(40);
    
    // create file selection
    layout = createFileSelectionUi();
    mainLayout->addLayout(layout);
    mainLayout->addSpacing(40);
    
    // create playback ui
    layout = createPlaybackUi();
    mainLayout->addLayout(layout);
    mainLayout->addSpacing(40);
}

/*!
    Creates the UI for showing and selecting WLAN AP connection status.
 */
QLayout *ExampleAppUi::createApSelectionUi()
{
    FUNC_LOG

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QLabel *titleLabel = new QLabel(tr("Access point:"));
    QLabel *statusLabel = new QLabel(tr("Not connected"));    

    // connect signals
    connect(mEngine, SIGNAL(iapUpdated(const QString &)), statusLabel, SLOT(setText(const QString &)));
    
    // add widgets to top layout
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(statusLabel);
    
    // add sub layouts to main layout
    layout->addLayout(topLayout);
    
    // update iap
    QString iapName = mEngine->getIapName();
    if (iapName.length())
    {
        statusLabel->setText(iapName);
    }
    
    return layout;
}

/*!
    Creates the UI for showing and selecting renderer device.
 */
QLayout *ExampleAppUi::createRendererSelectionUi()
{
    FUNC_LOG

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QVBoxLayout *bottomLayout = new QVBoxLayout;
    QLabel *titleLabel = new QLabel(tr("Rendering device:"));
    QLabel *statusLabel = new QLabel(tr("Not connected"));
    QPushButton *searchButton = new QPushButton(tr("Search for new devices"));
    QComboBox *selectionComboBox = new QComboBox;
    
    // connect signals
    connect(searchButton, SIGNAL(clicked()), mEngine, SLOT(searchRenderingDevices()));
    connect(selectionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectRenderingDevice(int)));
    connect(mEngine, SIGNAL(iapUpdated(int)), this, SLOT(enableRenderingDeviceSelection(int)));
    
    // add widgets to top layout
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(statusLabel);
    
    // add widgets to bottom layout
    bottomLayout->addWidget(searchButton);
    bottomLayout->addWidget(selectionComboBox);
    
    // add sub layouts to main layout
    layout->addLayout(topLayout);
    layout->addLayout(bottomLayout);
    
    // store rendering devices
    mRenderingDevices = selectionComboBox;
    mSelectedRenderer = statusLabel;
    mSearchDevicesButton = searchButton;
    
    // disable search button if iap has not been defined
    if (!mEngine->getIap())
    {
        disableRenderingDeviceSelection();
    }
    
    return layout;
}

/*!
    Creates the UI for showing and selecting media file to be rendered.
 */
QLayout *ExampleAppUi::createFileSelectionUi()
{
    FUNC_LOG

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QVBoxLayout *bottomLayout = new QVBoxLayout;    
    QLabel *titleLabel = new QLabel(tr("Selected file:"));
    QLabel *statusLabel = new QLabel(tr("None"));
    QPushButton *selectButton = new QPushButton(tr("Select media file"));
    
    // connect signals
    connect(selectButton, SIGNAL(clicked()), this, SLOT(selectFile()));

    // add widgets to top layout
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(statusLabel);
    
    // add widgets to bottom layout
    bottomLayout->addWidget(selectButton);
    
    // add sub layouts to main layout
    layout->addLayout(topLayout);
    layout->addLayout(bottomLayout);
    
    // store status label and select file button
    mSelectedFileLabel = statusLabel;
    mSelectFileButton = selectButton;
    
    // disable select file button by default
    disableFileSelection();

    return layout;
}

/*!
    Creates the push buttons for controlling the playback.
 */
QLayout *ExampleAppUi::createPlaybackUi()
{
    FUNC_LOG

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *topLayout = new QHBoxLayout;
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    QLabel *titleLabel = new QLabel(tr("Playback status:"));
    QLabel *statusLabel = new QLabel(tr("Stopped"));
    QPushButton *rewButton = new QPushButton(tr("Rew"));    
    QPushButton *playButton = new QPushButton(tr("Play"));    
    QPushButton *pauseButton = new QPushButton(tr("Pause"));    
    QPushButton *stopButton = new QPushButton(tr("Stop"));    
    QPushButton *ffButton = new QPushButton(tr("Ff"));    
    
    // connect signals
    connect(rewButton, SIGNAL(clicked()), mEngine, SLOT(rew()));
    connect(playButton, SIGNAL(clicked()), mEngine, SLOT(play()));
    connect(pauseButton, SIGNAL(clicked()), mEngine, SLOT(pause()));
    connect(stopButton, SIGNAL(clicked()), mEngine, SLOT(stop()));
    connect(ffButton, SIGNAL(clicked()), mEngine, SLOT(ff()));

    // add widgets to top layout
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(statusLabel);
    
    // add widgets to bottom layout
    bottomLayout->addWidget(rewButton);
    bottomLayout->addWidget(playButton);
    bottomLayout->addWidget(pauseButton);
    bottomLayout->addWidget(stopButton);
    bottomLayout->addWidget(ffButton);
    
    // add sub layouts to main layout
    layout->addLayout(topLayout);
    layout->addLayout(bottomLayout);
    
    // store status label
    mPlaybackStatus = statusLabel;
    mPlayButton = playButton;
    mPauseButton = pauseButton;
    mStopButton = stopButton;
    mRewButton = rewButton;
    mFfButton = ffButton;
    
    // disable all buttons by default
    disablePlayback();

    return layout;
}

/*!
    Creates the application engine and resolves the current connection status.
 */
void ExampleAppUi::createEngine()
{
    FUNC_LOG

    mEngine = new ExampleAppEngine;
    mEngine->construct();
    
    // connect signals
    connect(mEngine, SIGNAL(stateChanged(int)), this, SLOT(updateState(int)));
    connect(mEngine, SIGNAL(renderingDeviceSearchStarted()), this, SLOT(deviceSearchStarted()));
    connect(mEngine, SIGNAL(renderingDeviceFound(const QString &, const QString &)), this, SLOT(addRenderingDevice(const QString &, const QString &)));
    connect(mEngine, SIGNAL(renderingDeviceDisappeared(const QString &, const QString &)), this, SLOT(removeRenderingDevice(const QString &, const QString &)));
    connect(mEngine, SIGNAL(initComplete(int)), this, SLOT(enablePlayback(int)));
}

/*!
    This function gets called by the engine, when the renderer changes its state.
 */
void ExampleAppUi::updateState(int newState)
{
    FUNC_LOG

    switch (newState)
    {
        case ExampleAppEngine::PlaybackStateBuffering:
        {
            mPlaybackStatus->setText(tr("Buffering"));
            break;
        }
        case ExampleAppEngine::PlaybackStatePlaying:
        {
            mPlaybackStatus->setText(tr("Playing"));
            break;
        }
        case ExampleAppEngine::PlaybackStatePaused:
        {
            mPlaybackStatus->setText(tr("Paused"));
            break;
        }
        case ExampleAppEngine::PlaybackStateStopped:
            // fall through
        default:
        {
            mPlaybackStatus->setText(tr("Stopped"));
            break;
        }
    }
    
    // update playback controls
    enablePlayback();
}

/*!
    Launches QFile dialog for selecting a media file from the local
    file system. Then the file is sent to the engine for sharing, i.e.
    it will get resolved as an URI on the UPnP push server.
 */
void ExampleAppUi::selectFile()
{
    FUNC_LOG
    
    mSelectedFile = QFileDialog::getOpenFileName(this,
        tr("Open file"),
        tr("e:\\"),
        tr("Media Files (*.mp3 *.jpg *.mp4)"));
    if (mSelectedFile.length())
    {
        // convert all '/' to '\' since symbian does not understand '/' 
        // characters in the file path
        mSelectedFile.replace("/", "\\");
        
        // update text to ui
        QFileInfo fileInfo(mSelectedFile);
        // use only 16 characters to fit text on screen 
        // since otherwise qt will panic with bad alloc
        mSelectedFileLabel->setText(fileInfo.fileName().left(16));
       
        // resolve and init file for rendering
        int err = mEngine->initFile(mSelectedFile);
        if (err != 0)
        {
            ERROR_1(err, "Failed to init file %s", mSelectedFile.utf16());
            
            mSelectedFile = "";
            
            mSelectedFileLabel->setText(tr("Could not init"));

            disablePlayback();
        }
    }
    else
    {
        mSelectedFileLabel->setText(tr("Not selected"));

        disablePlayback();
    }
}

/*!
    Shows searching string on UI during search if no renderers are found yet.
*/
void ExampleAppUi::deviceSearchStarted()
{
    FUNC_LOG
    
    if (!mRenderingDevices->count())
    {
        mSelectedRenderer->setText(tr("Searching"));
    }
}

/*!
    Adds a renderer to the UI's model.
 */
void ExampleAppUi::addRenderingDevice(const QString &name, const QString &uuid)
{
    FUNC_LOG
    
    mRenderingDevices->addItem(name, QVariant(uuid));
}

/*!
    Removes a renderer from the UI's model.
 */
void ExampleAppUi::removeRenderingDevice(const QString &/*name*/, const QString &uuid)
{
    FUNC_LOG

    int count(mRenderingDevices->count());
    for (int i = 0; i < count; i++)
    {
        QVariant userData(mRenderingDevices->itemData(i));
        if (userData.toString() == uuid)
        {
            mRenderingDevices->removeItem(i);
            break;
        }
    }
}

/*!
    Set the renderer selected from the UI as the current renderer on the engine side.
*/
void ExampleAppUi::selectRenderingDevice(int index)
{
    FUNC_LOG
    
    // use only 16 characters to fit on screen otherwise qt will panic with bad alloc
    mSelectedRenderer->setText(mRenderingDevices->itemText(index).left(16));
    
    QVariant userData(mRenderingDevices->itemData(index));
    mEngine->prepareRenderingDevice(userData.toString());
    
    enablePlayback();
    enableFileSelection();
}

/*!
    Enables/disables the pushbuttons for renderer control according to the
    current state.
*/
void ExampleAppUi::enablePlayback(int result)
{
    FUNC_LOG
    
    if (result == 0 &&
        mSelectedFile.length())
    {
        switch (mEngine->getPlaybackState())
        {
            case ExampleAppEngine::PlaybackStateBuffering:
            {
                // disable all controls when buffering
                disablePlayback();
                break;
            }
            case ExampleAppEngine::PlaybackStatePlaying:
            {
                mRewButton->setEnabled(mEngine->isSeekSupported());
                mPlayButton->setDisabled(true);
                mPauseButton->setEnabled(mEngine->isPauseSupported());
                mStopButton->setEnabled(true);
                mRewButton->setEnabled(mEngine->isSeekSupported());
                break;
            }
            case ExampleAppEngine::PlaybackStatePaused:
            {
                mRewButton->setEnabled(mEngine->isSeekSupported());
                mPlayButton->setEnabled(true);
                mPauseButton->setDisabled(true);
                mStopButton->setEnabled(true);
                mRewButton->setEnabled(mEngine->isSeekSupported());
                break;
            }
            case ExampleAppEngine::PlaybackStateStopped:
            {
                mRewButton->setDisabled(true);
                mPlayButton->setEnabled(true);
                mPauseButton->setDisabled(true);
                mStopButton->setDisabled(true);
                mRewButton->setDisabled(true);
                break;
            }
            default:
            {
                // invalid state, disable all buttons
                disablePlayback();
                break;
            }
        }
    }
}

/*!
    Disables all renderer control pushbuttons.
*/
void ExampleAppUi::disablePlayback()
{
    FUNC_LOG

    mPlayButton->setDisabled(true);
    mPauseButton->setDisabled(true);
    mStopButton->setDisabled(true);
    mRewButton->setDisabled(true);
    mFfButton->setDisabled(true);
}

/*!
    Enables file selection button. Called when a renderer is selected.
*/
void ExampleAppUi::enableFileSelection()
{
    FUNC_LOG
    
    mSelectFileButton->setEnabled(true);
}

/*!
    Disables file selection.
*/
void ExampleAppUi::disableFileSelection()
{
    FUNC_LOG

    mSelectFileButton->setDisabled(true);
}

/*!
    Enables the rendering device selection pushbutton. Called when a WLAN AP is in use.
*/
void ExampleAppUi::enableRenderingDeviceSelection(int iap)
{
    FUNC_LOG
    
    if (iap)
    {
        mSearchDevicesButton->setEnabled(true);
        mRenderingDevices->setEnabled(true);
    }
}

/*!
    Disables the rendering device selection pushbutton.
*/
void ExampleAppUi::disableRenderingDeviceSelection()
{
    FUNC_LOG
    
    mSearchDevicesButton->setDisabled(true);
    mRenderingDevices->setDisabled(true);
}

// End of file
