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
* Description:  Media Server file transfer event list
*
*/


#ifndef C_UPNPFILETRANSFEREVENTLIST_H
#define C_UPNPFILETRANSFEREVENTLIST_H

// INCLUDES
#include "upnpobject.h"
#include "upnpmediaservereventlist.h"

#include <e32base.h>
#include <s32mem.h>










// FORWARD DECLARATIONS
class CUpnpMediaServerEventList;
class CUpnpMediaServerEvent;


// CLASS DECLARATION

/**
*  CUpnpFileTransferEventList class
*
*  @since Series 60 3.1
*/
class CUpnpFileTransferEventList : public CUpnpMediaServerEventList
    {
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpFileTransferEventList* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpFileTransferEventList* NewLC();

    /**
     * Internalizes CUpnpFileTransferEventList class object
     */
    virtual void InternalizeSubTypeL( RReadStream& aStream );

    /**
     * Destructor.
     */
    ~CUpnpFileTransferEventList();

private:

    /**
     * Constructor.
     */
    CUpnpFileTransferEventList();
    };

#endif      // C_UPNPFILETRANSFEREVENTLIST_H

// End of File
