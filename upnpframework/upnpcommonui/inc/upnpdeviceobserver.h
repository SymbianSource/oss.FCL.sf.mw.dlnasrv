/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CommonUI internal renderer device observer interface
*
*/


#ifndef M_UPNPDEVICEOBSERVER_H
#define M_UPNPDEVICEOBSERVER_H

// INCLUDES

// FORWARD DECLARATIONS

/**
* Defines the response interface for the image play in the
* UPnP CommonUI.
*
* @since Series 60 3.1
*/
class MUPnPDeviceObserver
    {

    public:

        /**
         * Notifies the dialog which should exit
         *
         * @since Series 60 3.1
         * @param aError reason of exit
         * @return None
         */
        virtual void DeviceDisappeared( TInt aError ) = 0;

    };

#endif // MUPnPDeviceObserver

// End of File
