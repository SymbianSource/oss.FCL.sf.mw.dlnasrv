# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: Dlna test app
TEMPLATE = app
TARGET = dlnasrv_exampleapp
LIBS += -lupnpavcontrollerclient \
    -lupnpsettingsengine \
    -linetprotutil \
    -lesocksvr \
    -lesock \
    -lupnpavcontrollerhelper \
    -lupnprenderingstatemachine \
    -lupnpconnmon
DEFINES += __FUNC_TRACE__
VPATH += ./inc \ 
    ./src
INCLUDEPATH += ../inc \
    $$MW_LAYER_SYSTEMINCLUDE \
    /epoc32/include
HEADERS = inc/tracedefs.h \
    inc/trace.h \
    inc/traceconfiguration.hrh \
    inc/exampleappengine_p.h \
    inc/exampleappengine.h \
    inc/exampleappui.h
SOURCES += src/exampleappengine_p.cpp \
    src/exampleappengine.cpp \
    src/exampleappui.cpp \
    main.cpp
symbian { 
    TARGET.CAPABILITY = \
# User Capabilities:    
    NetworkServices ReadUserData WriteUserData \
# System Capabilities:    
    ReadDeviceData WriteDeviceData
    TARGET.UID3 = 0xE0000000
}
