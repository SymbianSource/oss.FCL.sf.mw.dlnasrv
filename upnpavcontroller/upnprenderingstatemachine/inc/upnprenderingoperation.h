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
* Description:  A rendering operation modeled as a class
*
*/

#ifndef C_UPNPRENDERINGOPERATION_H
#define C_UPNPRENDERINGOPERATION_H

// INCLUDES
#include <e32base.h>
#include "upnprenderingstatemachineconstants.h"

// FORWARD DECLARATIONS

// CONSTANTS

/**
 * Abstract class that represents an internal operation on the
 * rendering state machine
 */
NONSHARABLE_CLASS( TUpnpRenderingOperation )
    {
    public: // construction/destruction

        // constructor with command, int param and an untyped pointed
        inline TUpnpRenderingOperation( Upnp::TCommand aCmd,
            TInt aParam = 0, const TAny* aData = NULL );

        // constructor
        inline TUpnpRenderingOperation( Upnp::TCommand aCmd,
            const TAny* aData );

    public: // assignment

        /**
         * operator set
         */
        inline TUpnpRenderingOperation operator=( 
            Upnp::TCommand aCmd );
        /**
         * operator set
         */
        inline TUpnpRenderingOperation operator=( 
            const TUpnpRenderingOperation& aOther );

    public: // comparison

        /**
         * operator equals
		 *
         * @return boolean
         */
        inline TBool operator==( Upnp::TCommand aCmd ) const;

        /**
         * operator not equal
		 *
         * @return boolean
         */
        inline TBool operator!=( Upnp::TCommand aCmd ) const;

        /**
         * operator equals
		 *
         * @return boolean
         */
        inline TBool operator==( 
            const TUpnpRenderingOperation& aOther ) const;

        /**
         * operator not equal
		 *
         * @return boolean
         */
        inline TBool operator!=( 
            const TUpnpRenderingOperation& aOther ) const;

    public: // convenience methods

        /**
         * IsValid
         * 
         * @return boolean
         */
        inline TBool IsValid() const;

        /**
         * IsUserOriented
         * 
         * @return boolean
         */
        inline TBool IsUserOriented() const;

    public: // member access

        // the command id (const)
        inline const Upnp::TCommand& Command() const;

        // any command-related parameter (const)
        inline TInt& Param();

        // any command-related parameter
        inline const TInt& Param() const;

        // any command-related pointer data (const)
        inline const TAny* Data() const;

        // command user-oriented or not
        inline void SetUserOriented( TBool aUserOriented );

    private: // members

        // the command ID
        Upnp::TCommand iCmd;

        // any parameter
        TInt iParam;

        // any data
        const TAny* iData;

        // whether this was by user's request
        TBool iUserOriented;
    };

#include "upnprenderingoperation.inl"

#endif // C_UPNPRENDERINGOPERATION_H

