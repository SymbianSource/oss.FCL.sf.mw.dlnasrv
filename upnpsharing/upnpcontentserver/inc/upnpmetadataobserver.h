/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      metadata observer class definition
*
*/






#ifndef __UPNPMETADATAOBSERVER_H__
#define __UPNPMETADATAOBSERVER_H__

// INCLUDE FILES
#include <e32base.h>

/**
 *  Callback class for receiving events from CupnpContentMetadataUtility
 *  @lib upnpcontentserver.lib
 *  @since S60 3.1
 */
class MUpnpMetadataObserver
    {
public:
    /**
     * Informs that CLF refresh has completed
     * @since S60 3.1
     */
    virtual void RefreshDoneL() = 0;

    /**
     * Virtual destructor
     */
    virtual ~MUpnpMetadataObserver() {};
    };

#endif //UPNPMETADATAOBSERVER

// End of File
