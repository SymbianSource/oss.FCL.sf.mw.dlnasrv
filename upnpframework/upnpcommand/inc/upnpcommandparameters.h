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
* Description:  Header file for the CUpnpCommandParameters class.
*
*/


#ifndef UPNP_COMMAND_PARAMETERS_H
#define UPNP_COMMAND_PARAMETERS_H

// INCLUDES
#include <e32base.h>
#include "upnpcommandcons.h"

// FORWARD DECLARATIONS
class MUpnpCommandCallback;

/**
* This class works as a storage class, storing parameter values used in
* UpnpCommand component.
*/
class CUpnpCommandParameters : public CBase
    {
    public: // Instantiation methods

        /**
         * Creates an instance of the implementation.
         *
         * @return instance of this class.
         */
        static CUpnpCommandParameters* NewL();

        /**
         * Destructor.
         */
        virtual ~CUpnpCommandParameters();

    public: // Business logic methods

        /**
         * Sets the callback interface
         */
        void SetObserver( MUpnpCommandCallback* aCallback );

        /**
         * The callback interface
         */
        MUpnpCommandCallback* Observer();

        /**
         * Sets a parameter
         *
         * @param aParamType the parameter category (see upnpcommandcons.h)
         * @param aParamValue value to be set for the parameter
         */
        void SetL( UpnpCommand::TUpnpParameterType aParamType,
            const TDesC& aParamValue );

        /**
         * Returns a parameter
         *
         * @param aParamType the parameter category (see upnpcommandcons.h)
         * @return the parameter value
         */
        const TDesC& Get( UpnpCommand::TUpnpParameterType aParamType );

        /**
         * Clears all parameters
         */
        void Reset();

    private: // Private construction methods

        /**
         * Perform the first phase of two phase construction.
         */
        CUpnpCommandParameters();

    private: // Data members

        /**
         * Stores the collection name parameter. Owned.
         */
        HBufC* iCollectionName;

        /**
         * Pointer to the command callback interface. Not owned.
         */
        MUpnpCommandCallback* iCallback;

    };

#endif // UPNP_COMMAND_PARAMETERS_H

// End of File