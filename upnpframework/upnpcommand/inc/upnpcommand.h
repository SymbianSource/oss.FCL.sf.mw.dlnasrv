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
* Description:  UpnpCommand plugin interface
*
*/


#ifndef UPNP_COMMAND_H
#define UPNP_COMMAND_H

// INCLUDES
#include <ecom/ecom.h>                       // REComSession
#include <e32std.h>                     // RLibrary
#include "upnpcommandcons.h"

// FORWARD DECLARATION
class MUpnpCommandCallback;

// CONSTANTS


// CLASS DEFINITION

/**
* UpnpCommand ECom plugin interface definition.
*/
class CUpnpCommand : public CBase
    {
    public: // constructing / destructing

        /**
         * the DtorKey offset
         * to pass in REComSession::CreateImplementationL
         */
        static inline TInt32 DtorKeyOffset()
            {
            return _FOFF( CUpnpCommand, iDtor_ID_Key );
            }

        /**
         * Sets the command identity.
         *
         * @param aCommandId (UpnpCommand::TUpnpCommandId) the command ID
         */
        virtual void SetCommandIdL(
            UpnpCommand::TUpnpCommandId aCommandId ) = 0;

        /**
         * Sets the observer.
         *
         * @param aCallback The callback interface
         */
        virtual void SetObserver( MUpnpCommandCallback* aCallback ) = 0;

        /**
         * Destructor.
         */
        virtual ~CUpnpCommand();

    public: // Method for querying the availablity

        /**
         * Checks if this command is available for execution.
         *
         * @return ETrue if command is available and can be executed
         */
        virtual TBool IsAvailableL() = 0;

        /**
         * Checks if given command is available for execution.
         *
         * @param aCommandId (UpnpCommand::TUpnpCommandId) the command ID
         * @return ETrue if command is available and can be executed
         */
        virtual TBool IsAvailableL( UpnpCommand::TUpnpCommandId aCommandId ) = 0;

    public: // Business logic methods

        /**
         * Allocates the Upnp Framework resources.
         */
        virtual void AllocateResourcesL() = 0;

        /**
         * Releases the Upnp Framework resources. Stops command execution if
         * it is ongoing.
         */
        virtual void ReleaseResources() = 0;

        /**
         * Executes the command. If Upnp Framework resources are not yet
         * allocated, they are allocated.
         */
        virtual void ExecuteL() = 0;

        /**
         * Sets a parameter.
         *
         * Leaves if the given param type or value is not valid.
         *
         * @param aParamType parameter category
         * @param aParamValue (const TDesC&) parameter value
         */
        virtual void SetParameterL(
                                UpnpCommand::TUpnpParameterType aParamType,
                                const TDesC& aParamValue ) = 0;

        /**
         * Returns a parameter, either set by client or returned
         * as an out parameter after command execution
         *
         * @param aParamType (UpnpCommand::TUpnpParameterType)
         * @return value of the parameter
         */
        virtual const TDesC& Parameter(
            UpnpCommand::TUpnpParameterType aParamType ) = 0;

        /**
         * Resets all parameter values
         */
        virtual void ResetParameters() = 0;

        /**
         * Pushes one file into the file pipe.
         *
         * @param aParamvalue (const TDesC&) parameter value
         */
        virtual void PushFileL( const TDesC& aParam ) = 0;

        /**
         * number of files in the file pipe
         *
         * @return number of files in the pipe
         */
        virtual TInt FileCount() = 0;

        /**
         * Returns a file in the file pipe
         *
         * @param aIndex index of the file in pipe
         * @return the file reference
         */
        virtual const TDesC& File( TInt aIndex ) = 0;

        /**
         * Resets files in the file pipe
         */
        virtual void ResetFiles() = 0;

        /**
         * Returns the state of the command
         *
         * @return UpnpCommand::TUpnpCommandState the state
         */
        virtual UpnpCommand::TUpnpCommandState State() = 0;

    private: // Private data members

        /**
         * ECom instance identifier key.
         */
        TUid iDtor_ID_Key;

    };


// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
inline CUpnpCommand::~CUpnpCommand()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

#endif // UPNP_COMMAND_H

// End of File
