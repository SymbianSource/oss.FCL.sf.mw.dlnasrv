/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Observer interface for item resolver interface
*
*/






#ifndef UPNPITEMRESOLVEROBSERVER_H
#define UPNPITEMRESOLVEROBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MUPnPItemResolver;

// CLASS DECLARATION

/**
* Observer for the item resolver class.
* MUPnPItemResolver service informs using this interface that resolve
* operation has been completed and (if succesful) an item is ready
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class MUPnPItemResolverObserver
    {

public:

    /**
     * indicates that operation is complete and results are available
     * (if operation was succesful) after this operation, browsing
     * session will not be used anymore.
     * @param aResolver the resolver object that produced the event
     * @param aError KErrNone if operation was succesful
     */
    virtual void ResolveComplete(
        const MUPnPItemResolver& aResolver,
        TInt aError ) = 0;

    };


#endif  // UPNPITEMRESOLVEROBSERVER_H

// End of File
