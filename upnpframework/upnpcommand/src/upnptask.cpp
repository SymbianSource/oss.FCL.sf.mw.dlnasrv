/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for CUpnpTask base class.
*
*/


// INCLUDE FILES
// upnpframework / command api
#include "upnpcommand.h"
#include "upnpcommandcallback.h"

// command internal
#include "upnptask.h"
#include "upnptaskhandler.h"
#include "upnpcommandparameters.h"
#include "upnpfilepipe.h"
#include "upnpnotehandler.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"



// --------------------------------------------------------------------------
// CUpnpTask::CUpnpTask
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpTask::CUpnpTask()
    {
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpTask::~CUpnpTask()
    {
    }

// --------------------------------------------------------------------------
// CUpnpTask::CommandEvent
// Notifies the client of an asynchronous command related event
// --------------------------------------------------------------------------
void CUpnpTask::CommandEvent(
    UpnpCommand::TUpnpCommandEvent aEventType,
    TInt aStatus,
    TBool aKillTask )
    {
    __ASSERTD( iParameters!=0, __FILE__, __LINE__ );

    // the callback interface
    MUpnpCommandCallback* callback = NULL;
    if( iParameters &&
        iParameters->Observer() )
        {
        callback = iParameters->Observer();
        }

    // kill the task if instructed
    if ( aKillTask )
        {
        TaskHandler()->DestroyTask();
        }

    // notify
    if ( callback )
        {
        callback->CommandEvent( aEventType, aStatus );
        }
    }

// --------------------------------------------------------------------------
// CUpnpTask::SetTaskHandlerL
// Sets the pointer to the task handler.
// --------------------------------------------------------------------------
void CUpnpTask::SetTaskHandlerL( MUpnpTaskHandler* aTaskHandler )
    {
    // Check parameter
    if( !aTaskHandler )
        {
        User::Leave( KErrArgument );
        }

    iTaskHandler = aTaskHandler;
    }

// --------------------------------------------------------------------------
// CUpnpTask::TaskHandler
// Returns the pointer to the task handler.
// --------------------------------------------------------------------------
MUpnpTaskHandler* CUpnpTask::TaskHandler()
    {
    return iTaskHandler;
    }

// --------------------------------------------------------------------------
// CUpnpTask::SetCommandParametersL
// Sets the pointer to the parameters.
// --------------------------------------------------------------------------
void CUpnpTask::SetCommandParametersL( CUpnpCommandParameters* aParameters )
    {
    // Check parameter
    if( !aParameters )
        {
        User::Leave( KErrArgument );
        }

    iParameters = aParameters;
    }

// --------------------------------------------------------------------------
// CUpnpTask::CommandParameters
// Returns the pointer to the parameters.
// --------------------------------------------------------------------------
CUpnpCommandParameters* CUpnpTask::CommandParameters()
    {
    return iParameters;
    }

// --------------------------------------------------------------------------
// CUpnpTask::SetFilePipeL
// Sets the pointer to the file pipe.
// --------------------------------------------------------------------------
void CUpnpTask::SetFilePipeL( CUpnpFilePipe* aFilePipe )
    {
    // Check parameter
    if( !aFilePipe )
        {
        User::Leave( KErrArgument );
        }

    iFilePipe = aFilePipe;
    }

// --------------------------------------------------------------------------
// CUpnpTask::FilePipeL
// Returns the pointer to the file pipe.
// --------------------------------------------------------------------------
CUpnpFilePipe* CUpnpTask::FilePipe()
    {
    return iFilePipe;
    }

// --------------------------------------------------------------------------
// CUpnpTask::SetNoteHandlerL
// Sets the pointer to the note handler.
// --------------------------------------------------------------------------
void CUpnpTask::SetNoteHandlerL( CUpnpNoteHandler* aNoteHandler )
    {
    // Check parameter
    if( !aNoteHandler )
        {
        User::Leave( KErrArgument );
        }

    iNoteHandler = aNoteHandler;
    }

// --------------------------------------------------------------------------
// CUpnpTask::NoteHandler
// Returns the pointer to the note handler.
// --------------------------------------------------------------------------
CUpnpNoteHandler* CUpnpTask::NoteHandler()
    {
    return iNoteHandler;
    }

    
  

// End of File
