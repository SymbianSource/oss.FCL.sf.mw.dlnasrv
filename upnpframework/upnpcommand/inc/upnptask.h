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
* Description:  Header file for the CUpnpTask class.
*
*/


#ifndef UPNP_TASK_H
#define UPNP_TASK_H

// INCLUDES
#include <e32base.h>
#include "upnpcommandcons.h"

// FORWARD DECLARATIONS
class CUpnpCommandParameters;
class CUpnpFilePipe;
class CUpnpNoteHandler;
class MUpnpTaskHandler;


/**
* This class defines the UpnpTask base class used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpTask : public CBase
    {
    public: // Methods for constructing

        /**
         * Destructor
         *
         * @since S60 3.2
         */
        virtual ~CUpnpTask();

        /**
         * Default constructor
         *
         * @since S60 3.2
         */
        CUpnpTask();

    public: // Methods for sub classes to implement

        /**
         * Allocates the Upnp Fw resources.
         *
         * @since S60 3.2
         */
        virtual void AllocateResourcesL() = 0;

        /**
         * Executes the task.
         *
         * @since S60 3.2
         */
        virtual void ExecuteL() = 0;

    public: // Methods for sub class use

        /**
         * Notifies the client for a general command event
         *
         * @param aEventType see upnpcommandcons.h
         * @param aStatus (TInt) status related to the event type
         * @param aKillTask if true, kills the task (typically error condition)
         */
        void CommandEvent(
            UpnpCommand::TUpnpCommandEvent aEventType,
            TInt aStatus,
            TBool aKillTask = EFalse );

        /**
         * Sets the pointer to the task handler. The ownership of the
         * pointer is not transfered.
         *
         * Leaves with KErrArgument, if the provided pointer is invalid.
         *
         * @since S60 3.2
         * @param aTaskHandler (MUpnpTaskHandler*) the pointer to the task
         *        handler
         */
        void SetTaskHandlerL( MUpnpTaskHandler* aTaskHandler );

        /**
         * Returns the pointer to the task handler. The ownership of the
         * pointer is not transfered.
         *
         * @since S60 3.2
         * @return (MUpnpTaskHandler*) the pointer to the task handler
         */
        MUpnpTaskHandler* TaskHandler();

        /**
         * Sets the pointer to the command parameters. The ownership of the
         * pointer is not transfered.
         *
         * Leaves with KErrArgument, if the provided pointer is invalid.
         *
         * @since S60 3.2
         * @param aCommandParameters (CUpnpCommandParameters*) the pointer to
         *        the command parameters
         */
        void SetCommandParametersL(
                            CUpnpCommandParameters* aCommandParameters );

        /**
         * Returns the pointer to the parameters. The ownership of the
         * pointer is not transfered.
         *
         * @since S60 3.2
         * @return (CUpnpCommandParameters*) the pointer to the parameters
         */
        CUpnpCommandParameters* CommandParameters();

        /**
         * Sets the pointer to the file pipe. The ownership of the pointer is
         * not transfered.
         *
         * Leaves with KErrArgument, if the provided pointer is invalid.
         *
         * @since S60 3.2
         * @param aFilePipe (CUpnpFilePipe*) the pointer to the file pipe
         */
        void SetFilePipeL( CUpnpFilePipe* aFilePipe );

        /**
         * Returns the pointer to the file pipe. The ownership of the
         * pointer is not transfered.
         *
         * @since S60 3.2
         * @return (CUpnpFilePipe*) the pointer to the file pipe
         */
        CUpnpFilePipe* FilePipe();

        /**
         * Sets the pointer to the note handler. The ownership of the pointer
         * is not transfered.
         *
         * Leaves with KErrArgument, if the provided pointer is invalid.
         *
         * @since S60 3.2
         * @param aNoteHandler (CUpnpNoteHandler*) the pointer to the note
         *        handler
         */
        void SetNoteHandlerL( CUpnpNoteHandler* aNoteHandler );

        /**
         * Returns the pointer to the note handler. The ownership of the
         * pointer is not transfered.
         *
         * @since S60 3.2
         * @return (CUpnpNoteHandler*) the pointer to the note handler
         */
        CUpnpNoteHandler* NoteHandler();


    private: // Private data members

        /**
         * Pointer to the class who instantiated the task. Not owned.
         */
        MUpnpTaskHandler* iTaskHandler;

        /**
         * Pointer to the parameters class of the UpnpCommand. Not owned.
         */
        CUpnpCommandParameters* iParameters;

        /**
         * Pointer to the file pipe class of the UpnpCommand. Not owned.
         */
        CUpnpFilePipe* iFilePipe;

        /**
         * Pointer to the note handler class of the UpnpCommand. Not owned.
         */
        CUpnpNoteHandler* iNoteHandler;
        
    };

#endif // UPNP_TASK_H

// End of File