/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media Server observer
*
*/


#ifndef C_CUPNPMEDIASERVEROBSERVER_H
#define C_CUPNPMEDIASERVEROBSERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CUpnpFileTransferEvent;
// CLASS DECLARATION

/**
*  Interface for Media Server events listeners
*
*  @since Series 60 3.1
*/
class MUpnpMediaServerObserver
    {
public:

    /**
    * Called when Content Directory file transfer is finished
    * either with success or failure.
    * See CUpnpFileTransferEvent class description for details.
    *
    * @since Series 60 3.1
    * @param aEvents Media Server event
    * @return notifier object
    */
    virtual void FileTransferEvent(CUpnpFileTransferEvent *aEvent) = 0;

    /**
    * Called in case of CUpnpMediaServerNotifier internal error.
    * Should be used for event handling recovery.
    *
    * @since Series 60 3.1
    * @param aObserver event observer
    * @return notifier object
    */
    virtual void NotifierError(TInt aError) = 0;
    };
#endif // C_CUPNPMEDIASERVEROBSERVER_H
