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
* Description:  Helper for upnp SHOW command
*
*/


// INCLUDE FILES
#include <upnpcommandobserver.h>
#include "upnpcommandcallbackadapter.h"

// --------------------------------------------------------------------------
// CUpnpCommandCallbackAdapter::CUpnpCommandCallbackAdapter
// Constructor
// --------------------------------------------------------------------------
//
CUpnpCommandCallbackAdapter::CUpnpCommandCallbackAdapter(
    MUpnpCommandObserver* aObserver )
    {
    iObserver = aObserver;
    }

// --------------------------------------------------------------------------
// CUpnpCommandCallbackAdapter::~CUpnpCommandCallbackAdapter
// Destructor
// --------------------------------------------------------------------------
//
CUpnpCommandCallbackAdapter::~CUpnpCommandCallbackAdapter()
    {
    }

// --------------------------------------------------------------------------
// CUpnpCommandCallbackAdapter::CommandEvent
// Indicates that the command has been completed.
// --------------------------------------------------------------------------
//
void CUpnpCommandCallbackAdapter::CommandEvent(
    UpnpCommand::TUpnpCommandEvent aEventType,
    TInt aStatusCode )
    {
    // If there is an observer, forward the callback
    if ( iObserver )
        {
        if ( aEventType == UpnpCommand::EEventComplete )
            {
            iObserver->CommandComplete( aStatusCode );
            }
        else if ( aEventType == UpnpCommand::EEventProgress && aStatusCode > 0 )
            {
            // Convert progress events to CommandComplete methods of the observer
            // ONLY if the progress code is a positive value. Therefore it does not
            // get mixed up with real CommandComplete notifications with either
            // KErrNone or an error value.
            iObserver->CommandComplete( aStatusCode );
            }
        else
            {
            // eat the event
            }
        }
    }

// End of File
