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
* Description:      File sharing observer class definition
*
*/






#ifndef __UPNPCONTENTSHARINGOBSERVER_H__
#define __UPNPCONTENTSHARINGOBSERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>

/**
 *  File sharing observer class definition
 *  @since S60 3.1
 */
class MUpnpContentSharingObserver
    {
public:

    /**
     * Completes the query about content to client
     * @since S60 3.1
     * @param aMarkedItems Array of items to select shared content
     */
    virtual void CompleteSelectionContentL(
        const CDesCArray& aMarkedItems ) = 0;

    /**
     * Virtual destructor
     */
    virtual ~MUpnpContentSharingObserver() {};
    };

#endif //UPNPCONTENTSHARINGOBSERVER

// End of File
