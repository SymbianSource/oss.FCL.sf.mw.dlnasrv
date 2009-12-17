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
* Description:  Source file for CUpnpCommandParameters class.
*
*/


// INCLUDE FILES
// upnpframework / command api
#include "upnpcommandcallback.h"        // MUpnpCommandCallback

// command internal
#include "upnpcommandparameters.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpCommandParameters::NewL
// Creates an instance of the implementation.
// --------------------------------------------------------------------------
//
CUpnpCommandParameters* CUpnpCommandParameters::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::NewL" );

    return new (ELeave) CUpnpCommandParameters;
    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::CUpnpCommandParameters
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpCommandParameters::CUpnpCommandParameters()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::Constructor" );

    iCollectionName = NULL;
    iCallback = NULL;
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpCommandParameters::~CUpnpCommandParameters()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::Destructor" );

    // Reset the parameters
    Reset();
    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::SetObserver
// Sets the observer.
// --------------------------------------------------------------------------
//
void CUpnpCommandParameters::SetObserver( MUpnpCommandCallback* aCallback )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::SetObserver" );

    // Parameter check
    __ASSERTD( aCallback!=0, __FILE__, __LINE__ );

    // Set the variable
    iCallback = aCallback;
    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::Observer
// Gets the observer.
// --------------------------------------------------------------------------
//
MUpnpCommandCallback* CUpnpCommandParameters::Observer()
    {
    return iCallback;
    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::SetL
// Sets a parameter
// --------------------------------------------------------------------------
//
void CUpnpCommandParameters::SetL(
    UpnpCommand::TUpnpParameterType aParamType,
    const TDesC& aParamValue )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::SetL" );

    switch( aParamType )
        {
        case UpnpCommand::EParamCollectionName:
            {
            // Parameter check
            if( aParamValue == KNullDesC )
                {
                User::Leave( KErrArgument );
                }
            // allocation
            HBufC* newCollectionName = aParamValue.AllocL();
            delete iCollectionName;
            iCollectionName = newCollectionName;
            newCollectionName = NULL;
            }
            break;
        case UpnpCommand::EParamMediaType: // flow through
        case UpnpCommand::EParamDeviceUuid: // flow through
        case UpnpCommand::EParamDeviceName:
            // do nothing
            break;
        default:
            __PANICD( __FILE__, __LINE__ );
        }

    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::Get
// Returns a parameter by given type
// --------------------------------------------------------------------------
//
const TDesC& CUpnpCommandParameters::Get(
    UpnpCommand::TUpnpParameterType aParamType )
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::Get" );

    switch( aParamType )
        {
        case UpnpCommand::EParamCollectionName:
            {
            if( iCollectionName )
                {
                return *iCollectionName;
                }
            }
            break;
        case UpnpCommand::EParamMediaType: // flow through
        case UpnpCommand::EParamDeviceUuid: // flow through
        case UpnpCommand::EParamDeviceName:
            return KNullDesC;
        default:
            __PANICD( __FILE__, __LINE__ );
        }

    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CUpnpCommandParameters::Reset
// Resets variables.
// --------------------------------------------------------------------------
//
void CUpnpCommandParameters::Reset()
    {
    __LOG( "[UpnpCommand]\t CUpnpCommandParameters::Reset" );

    // Delete/reset the variables
    delete iCollectionName;
    iCollectionName = NULL;
    }

// End of File
