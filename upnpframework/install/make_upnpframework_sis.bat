rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:   Bat for creating and signing upnpframework sis-files
rem

makesis cenrep.pkg
signsis cenrep.sis cenrep.sisx rd.cer rd-key.pem

makesis upnpframework.pkg
signsis upnpframework.sis upnpframework.sisx  rd.cer rd-key.pem
