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
* Description:  Callback definitions for copy progress
*
*/


#ifndef M_UPNPPROGRESSDIALOGOBSERVER
#define M_UPNPPROGRESSDIALOGOBSERVER

// INCLUDES

// FORWARD DECLARATIONS

/**
* Defines the response interface for the progress dialog the
* UPnPFileTransferEngine.
*
* @since Series 60 3.2
*/
class MUPnPProgressDialogobserver
    {

    public:

        /**
         * Notifies the progress dialog is canceled
         *
         * @since Series 60 3.2
         * @param aError reason of exit
         * @return None
         */
        virtual void DialogDismissedL() = 0;

    };

#endif // MUPnPProgressDialogobserver

// End of File
