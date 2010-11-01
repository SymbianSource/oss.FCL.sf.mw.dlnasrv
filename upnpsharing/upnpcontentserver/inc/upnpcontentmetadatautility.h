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
* Description:      Metadata utility class definition
 *
*/






#ifndef __UPNPCONTENTMETADATAUTILITY_H__
#define __UPNPCONTENTMETADATAUTILITY_H__


// INCLUDES
// System
#include <e32base.h>
#include <MCLFOperationObserver.h>
#include <MCLFItem.h>
#include <badesca.h>
// Internal
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpcontentserverdefs.h"
#include "upnpcontentserverhandler.h"

// FORWARD DECLARATIONS
class MCLFContentListingEngine;
class MCLFItemListModel;
class TClfMediaType;

using namespace UpnpContentServer;

// CLASS DECLARATION

/**
 *  Class to collect files from phone to share.
 *  This class collects the media files for Upnp content server to share
 *  The files are obtained using Content Listing Framework.
 *
 *  @since S60 3.1
 */
class CUpnpContentMetadataUtility : public CBase,
                                public MCLFOperationObserver
    {
    
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUpnpContentMetadataUtility* NewL();

    /**
     * Destructor.
     */
    virtual ~CUpnpContentMetadataUtility();

public: // New functions

    /**
     * Returns music model
     * @since S60 5.2
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel* MusicFiles();

    /**
     * Returns image model
     * @since S60 5.2
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel* ImageFiles();

    /**
     * Returns video model
     * @since S60 5.2
     * @return MCLFItemListModel reference to model
     */
    const MCLFItemListModel* VideoFiles();

protected: // From MCLFOperationObserver

    /**
     * Abstract method to get list model operation events. This method is
     * called when an event is received.
     * @since S60 3.1
     * @param aOperationEvent Operation event code of the event
     * @param aError System wide error code if the operation did not
     *        succeed.
     */
    void HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                TInt aError );

private:

    /**
     * C++ default constructor.
     */
    CUpnpContentMetadataUtility();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Refreshes the selected model information
     * @since S60 5.2
     * @param aCLFMediaType CLF media type
     */
    void RefreshModelL( TCLFMediaType aCLFMediaType );

    /**
     * Wait for refresh operation to complete
     * @since S60 5.2
     */
    void WaitForRefreshToComplete();

private:    // Data
    // Content listing engine (owned)
    MCLFContentListingEngine* iEngine;

    // Content listing model for music (owned)
    MCLFItemListModel* iMusicModel;

    // Content listing model for images (owned)
    MCLFItemListModel* iImageModel;

    // Content listing model for videos (owned)
    MCLFItemListModel* iVideoModel;

    // Waiter for Refresh processes (owned)
    CActiveSchedulerWait* iWait;

    };

#endif      // __UPNPCONTENTMETADATAUTILITY_H__

// End of File
