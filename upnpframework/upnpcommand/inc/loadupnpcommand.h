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
* Description:  helper function for loading CUpnpCommand plugin
*
*/


#include <e32base.h>
#include <ecom/ecom.h>          // REComSession
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "upnpcommand.h"

/**
 * -------------------------------------------------------------------------
 * LoadUpnpCommandL
 * A helper inline function to load the upnp plugin.
 * The method guarantees the correct instance is loaded in a secure way.
 * If plugin can not be located, leaves with KErrNotSupported
 *
 * @param aCommandId the identity of command to load
 * @param aCalback callback interface pointer (may be left NULL)
 * -------------------------------------------------------------------------
 */
inline CUpnpCommand* LoadUpnpCommandL(
    UpnpCommand::TUpnpCommandId aCommandId,
    MUpnpCommandCallback* aCallback = 0 )
    {
    CUpnpCommand* command = NULL;

    // Create resolver parameters to pin point the implementation
    TEComResolverParams resolverParams;
    _LIT8( KCommandDataType, "UpnpCommand" );
    resolverParams.SetDataType( KCommandDataType );
    resolverParams.SetWildcardMatch( EFalse );

    RImplInfoPtrArray implArray;
   CleanupResetAndDestroyPushL( implArray );

    // Find implementation for the interface
    const TUid implIFUid = {0x200075DB};
    REComSession::ListImplementationsL( implIFUid, implArray );
    for( TInt i=0; i<implArray.Count() && command==0; ++i )
        {
        CImplementationInformation* implInfo = implArray[i];

        if( implInfo->VendorId() == VID_DEFAULT && implInfo->RomBased() )
            {
            TAny* impl = REComSession::CreateImplementationL(
                implIFUid,
                CUpnpCommand::DtorKeyOffset(),
                resolverParams );

            // Cast the object to correct type before returning
            command = REINTERPRET_CAST( CUpnpCommand*, impl );

            // Set the command ID
            CleanupStack::PushL( command );
            command->SetCommandIdL( aCommandId );
            if ( aCallback )
                {
                command->SetObserver( aCallback );
                }
            CleanupStack::Pop( command );
            }
        }
    CleanupStack::PopAndDestroy(); // empties implArray

    if ( command == 0 )
        {
        User::Leave( KErrNotSupported );
        }

    // Return the object
    return command;
    }

// End of File
