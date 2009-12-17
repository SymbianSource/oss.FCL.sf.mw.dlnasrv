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
* Description:  Media Server notifier
*
*/


#ifndef C_CUPNPMEDIASERVERNOTIFIER_H
#define C_CUPNPMEDIASERVERNOTIFIER_H

// INCLUDES
#include "upnpmediaserverclient.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class MUpnpMediaServerObserver;

// CLASS DECLARATION

/**
*  Class wrapps asynchronous communication with Media Server Engine
*  in scope of eventing. New RUpnpMediaServerClient session is opened
*  for each CUpnpMediaServerNotifier object.
*  Events are automatically subscribed during object
*  construction and unsubscribed during object destruction.
*  Event observer must be derived from MUpnpMediaServerObserver.
*
*  @since Series 60 3.1
*/
class CUpnpMediaServerNotifier : public CActive
    {
public:

    enum TNotifierAction
        {
        ENoAction = 1,
        ESubscribe,
        EListenEvent,
        EGetEventBody
        };

    /**
    * Two-phased constructor. Subscribes events during construction phase.
    * @since Series 60 3.1
    * @param aObserver event observer
    * @return notifier object
    */
    IMPORT_C static CUpnpMediaServerNotifier* NewL( MUpnpMediaServerObserver* aObserver );

    /**
    * Two-phased constructor. Subscribes events during construction phase.
    * @since Series 60 3.1
    * @param aObserver event observer
    * @return notifier object
    */
    IMPORT_C static CUpnpMediaServerNotifier* NewLC( MUpnpMediaServerObserver* aObserver );

    /**
    * Destructor. Usubscribes
    */
	virtual ~CUpnpMediaServerNotifier();

private:
    /**
    * Cancellation of ongoing asynchronous operation (from CActive)
    */
	void DoCancel();

    /**
    * RunL (from CActive)
    */
	void RunL();

    /**
    * RunError (from CActive)
    */
    TInt RunError(TInt aError);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Constructor.
    */
    CUpnpMediaServerNotifier( MUpnpMediaServerObserver* aObserver );

private:
    //Upnp Media Server Client
	RUpnpMediaServerClient iMediaServerClient;
    //Pending action, state of active object
    TNotifierAction  iPendingAction;
    //Media Server observer
    MUpnpMediaServerObserver* iObserver;

    //Size of buffer for response to be allocated by client
    TInt            iRespBufSize;
    TPckg<TInt>     iRespBufSizePkg;

    //Buffer for response
    HBufC8*         iReceiveBuffer;
    TPtr8           iReceiveBufferPtr;
    };

#endif // C_CUPNPMEDIASERVERNOTIFIER_H
