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


#ifndef EXAMPLEAPPUI_H_
#define EXAMPLEAPPUI_H_

#include <qmainwindow.h>

class QGroupBox;
class QLabel;
class QPushButton;
class QComboBox;
class ExampleAppEngine;

class ExampleAppUi : public QMainWindow
{
    Q_OBJECT
    
public:
    ExampleAppUi();
    virtual ~ExampleAppUi();
    
    void construct();
    
private:
    void createUi();
    QLayout *createApSelectionUi();
    QLayout *createRendererSelectionUi();
    QLayout *createFileSelectionUi();
    QLayout *createPlaybackUi();
    void createEngine();
    
private slots:
    void updateState(int newState);
    void selectFile();
    void deviceSearchStarted();
    void addRenderingDevice(const QString &name, const QString &uuid);
    void removeRenderingDevice(const QString &name, const QString &uuid);
    void selectRenderingDevice(int index);
    void enablePlayback(int result = 0);
    void disablePlayback();
    void enableFileSelection();
    void disableFileSelection();
    void enableRenderingDeviceSelection(int iap = 0);
    void disableRenderingDeviceSelection();
    
private:
    enum
    {
        RewButton,
        PlayButton,
        PauseButton,
        StopButton,
        FfButton
    };
    
private:
    // owned
    ExampleAppEngine *mEngine;
    
    // not owned
    QLabel *mSelectedRenderer;
    QLabel *mSelectedFileLabel;
    QLabel *mPlaybackStatus;
    QComboBox *mRenderingDevices;
    QPushButton *mPlayButton;
    QPushButton *mPauseButton;
    QPushButton *mStopButton;
    QPushButton *mRewButton;
    QPushButton *mFfButton;
    QPushButton *mSelectFileButton;
    QPushButton *mSearchDevicesButton;
    QString mSelectedFile;
};

#endif /* EXAMPLEAPPUI_H_ */
