/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Common Client/Server header for CM Metadata Harvester
*
*/






#ifndef CMMDHCOMMON_H
#define CMMDHCOMMON_H

_LIT(KCmMdhServerExe,"cmmdh.exe"); ///<EXE name

// CONSTANTS

// Server name
_LIT(KCmMdhServerName,"CmMdhServer");

// Panic name
_LIT(KCmMdhServerPanic, "CmMdhServerPanic");

//const TUid KCmMetadataHarvesterUid3={0x0EFFEFEF};

/// A version must be specified when creating a session with the server
const TUint KCmMdhServerMajor = 1;
const TUint KCmMdhServerMinor = 0;
const TUint KCmMdhServerBuild = 0;

/// opcodes used in message passing between client and server
enum TCmMdhRequest
    {
    ECmMdhSearchMediaservers,
    ECmMdhHarvest,
    ECmMdhCancel
    };

/// Panic enums
enum TCmMdhClientPanics
    {
    ECmMdhClientNoSessionActive,
    ECmMdhClientBadRequest
    };

/// Panic enums
enum TCmMdhServerPanics
    {
    EDCmMdhServerIllegalParameter
    };

#endif
// end of file
