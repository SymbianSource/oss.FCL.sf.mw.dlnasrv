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
* Description:      File tranfer session
*
*/






#ifndef M_UPNPFILEDOWNLOADSESSION_H
#define M_UPNPFILEDOWNLOADSESSION_H

// EXTERNAL INCLUDES
#include <e32std.h>
#include <f32file.h>

// INTERNAL INCLUDES
#include "upnpfiletransfersession.h"

// FORWARD DECLARATIONS
class CUpnpElement;
class CUpnpItem;

// CLASS DECLARATION
/**
 *  AV Controller download session interface.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.2
 */
 class MUPnPFileDownloadSession : public MUPnPFileTransferSession
    {

public:

    /**
     * Start downloading an item. Original resource will be downloaded.
     *
     * @param aItem UPnP item to download
     * @param aKey identifies the download operation
     */        
    virtual void StartDownloadL( const CUpnpItem& aItem, TInt aKey ) = 0;

    /**
     * Start downloadin and item. Resource is selected by providing the
     * desired res-element and. The resource is downloaded to the provided
     * file.
     *
     * @param aResElement points to a resource in the item to download
     * @param aItem UPnP item to download
     * @param aFile file handle (download target)
     * @param aKey identifies the download operation
     */        
    virtual void StartDownloadL( const CUpnpElement& aResElement,
        const CUpnpItem& aItem, RFile& aFile, TInt aKey ) = 0;

protected:

    /**
     * Protected destructor
     */        
    virtual ~MUPnPFileDownloadSession() {};
    
    };


#endif // M_UPNPFILEDOWNLOADSESSION_H

// End of file

