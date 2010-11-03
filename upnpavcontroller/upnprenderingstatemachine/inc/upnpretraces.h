/*
* Copyright (c) 2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  upnprenderingstatemachine trace helper functions
*
*/

#ifndef C_UPNPRETRACES_H
#define C_UPNPRETRACES_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CONSTANTS

// FUNCTIONS


    /**
     * Name of a state, for logging purposes
     * @param aState: see Upnp::TState
     * @return 16-bit zero-terminated string. Use %s in descriptor format.
     */
    const TUint16* StateName( TInt aState );

    /**
     * Name of a command, for logging purposes
     * @param aCommand: see Upnp::TCommand
     * @return 16-bit zero-terminated string. Use %s in descriptor format.
     */
    const TUint16* CommandName( TInt aCommand );

    /**
     * Name of a property, for logging purposes
     * @param aProperty: see Upnp::TProperty
     * @return 16-bit zero-terminated string. Use %s in descriptor format.
     */
    const TUint16* PropertyName( TInt aProperty );

    /**
     * Name of a interact operation, for logging purposes
     * @param aInteractOperation: see TUPnPAVInteractOperation
     * @return 16-bit zero-terminated string. Use %s in descriptor format.
     */
    const TUint16* InteractOperationName( TInt aInteractOperation );

#endif // C_UPNPRETRACES_H

