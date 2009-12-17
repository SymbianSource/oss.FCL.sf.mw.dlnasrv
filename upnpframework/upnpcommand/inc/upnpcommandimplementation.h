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
* Description:  Header file for the UpnpCommand plugin implementation.
*
*/


#ifndef UPNP_COMMAND_IMPLEMENTATION_H
#define UPNP_COMMAND_IMPLEMENTATION_H

// INCLUDES
#include "upnpcommandcons.h"
#include "upnpcommand.h"
#include "upnptaskhandler.h"

// FORWARD DECLARATIONS
class CUpnpFilePipe;
class CUpnpCommandParameters;
class CUpnpTask;
class CUpnpNoteHandler;
class MUpnpCommandCallback;

/**
* This class provides an easy to use ECom plugin interface to access the
* features of the Upnp Framework.
*/
class CUpnpCommandImplementation : public CUpnpCommand,
                                   public MUpnpTaskHandler
    {
    public: // Instantiation methods

        /**
         * Creates an instance of UpnpCommand implementation. Upnp Fw
         * resources are not allowated yet at this point.
         *
         * @return instance of this class.
         */
        static CUpnpCommandImplementation* NewL();

    public: // Business logic methods, from CUpnpCommand

        /**
         * Checks if the command is available for execution.
         */
        TBool IsAvailableL();

        /**
         * Checks if given command is available for execution.
         */
        TBool IsAvailableL( UpnpCommand::TUpnpCommandId aCommandId );

        /**
         * Allocates the Upnp Framework resources.
         */
        void AllocateResourcesL();

        /**
         * Releases the Upnp Framework resources. Stops command execution if
         * it is ongoing.
         */
        void ReleaseResources();

        /**
         * Executes the command. If Upnp Framework resources are not yet
         * allocated, they are allocated.
         */
        void ExecuteL();

        /**
         * Sets a parameter.
         *
         * @param aParamType the parameter category
         * @param aParamValue value of the parameter
         */
        void SetParameterL( UpnpCommand::TUpnpParameterType aParamType,
                            const TDesC& aParamValue );

        /**
         * Returns a parameter
         *
         * @param aParamType (UpnpCommand::TUpnpParameterType)
         * @return value of the parameter
         */
        const TDesC& Parameter(
            UpnpCommand::TUpnpParameterType aParamType );

        /**
         * Resets parameteres.
         */
        void ResetParameters();

        /**
         * Pushes one file into the file pipe.
         *
         * @param aFilename (const TDesC&) parameter value
         */
        void PushFileL( const TDesC& aFilename );

        /**
         * number of files in the file pipe
         *
         * @return number of files in the pipe
         */
        TInt FileCount();

        /**
         * Returns a file in the file pipe
         *
         * @param aIndex index of the file in pipe
         * @return the file reference
         */
        const TDesC& File( TInt aIndex );

        /**
         * Resets all files in the pipe
         */
        void ResetFiles();

        /**
         * Returns the state of the command.
         *
         * @return UpnpCommand::TUpnpCommandState the state
         */
        UpnpCommand::TUpnpCommandState State();

    public: // From MUpnpTaskHandler

        /**
         * Destroys the ongoing task.
         */
        void DestroyTask();

    public: // Methods for UpnpCommand internal use, from CUpnpCommand

        /**
         * Sets the command ID. Leaves with KErrNotSupported if the given
         * is not supported.
         *
         * @param aCommandId (UpnpCommand::TUpnpCommandId) the command ID
         */
        void SetCommandIdL( UpnpCommand::TUpnpCommandId aCommandId );

        /**
         * Sets the observer.
         *
         * @param aCallback the callback interface
         */
        void SetObserver( MUpnpCommandCallback* aCallback );

    private: // Private construction methods

        /**
         * Perform the first phase of two phase construction.
         */
        CUpnpCommandImplementation();

        /**
         * Destructor.
         */
        virtual ~CUpnpCommandImplementation();

        /**
         * Perform the second phase of two phase construction.
         */
        void ConstructL();

    private: // methods for own use

        TBool IsUpnpConfiguredL();

    private: // Data members

        /**
         * The Id of the command.
         */
        UpnpCommand::TUpnpCommandId iCommandId;

        /**
         * The state of the command.
         */
        UpnpCommand::TUpnpCommandState iState;

        /**
         * Pointer to the file pipe. Owned.
         */
        CUpnpFilePipe* iFilePipe;

        /**
         * Pointer to the command parameters. Owned.
         */
        CUpnpCommandParameters* iParameters;

        /**
         * Pointer to the note handler. Owned.
         */
        CUpnpNoteHandler* iNoteHandler;

        /**
         * Pointer to the command task. Owned.
         */
        CUpnpTask* iTask;

        /**
         * A boolean value indicating the state of the DRM note showing.
         * The note is shown only once per session.
         */
        TBool iDrmNoteShown;

    };

#endif // UPNP_COMMAND_IMPLEMENTATION_H

// End of File