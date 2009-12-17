/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  constants and enumerations used in UpnpCommand API
*
*/


#ifndef UPNP_COMMAND_CONS_H
#define UPNP_COMMAND_CONS_H

namespace UpnpCommand
    {

    // ENUMS

    /**
     * The command ids
     */
    enum TUpnpCommandId
        {
        ECommandUndefined = 100,
        ECommandShow =      101,   // Renders an image or a video file
        ECommandCopy =      102,   // Copies file(s) or a collection (playlist/album)
        ECommandMove =      103,   // Moves file(s) or a collection (playlist/album)
        ECommandBrowse =    104,   // Browses the Upnp home network
        ECommandSetup =     105,   // Runs the Upnp Fw's setup wizard
        ECommandLast               // for boundary checking
        };

    /**
     * Defines the parameter types used to control command behaviour and to get
     * parameter output from the command
     */
    enum TUpnpParameterType
        {
        EParamCollectionName = 201,    // Name of collection (playlist/album)
        EParamMediaType = 202,         // "m" (music), "i" (image) or "v" (video)
        EParamDeviceUuid = 203,        // UUID of target device
        EParamDeviceName = 204         // Name of target device
        };

    /**
     * States of UpnpCommand
     */
    enum TUpnpCommandState
        {
        EStateIdle =      301,     // Upnp Fw resources not allocated
        EStateAllocated = 302,     // Upnp Fw resources allocated
        EStateExecuting = 303      // Upnp command execution ongoing
        };

    /**
     * Callback event types from UpnpCommand API
     */
    enum TUpnpCommandEvent
        {
        EEventComplete =  401,  // Indicates that async command has been completed
        EEventProgress =  402   // Progress event during execution of a command
        };

    // constant statuses for EEventProgress
    const TInt KUpnpCommandStatusStartPlayVideo = 6; // video ready to be played

    } // namespace UpnpCommand

#endif // UPNP_COMMAND_CONS_H

// End of File