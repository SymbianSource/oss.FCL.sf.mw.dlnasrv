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
* Description:      content fetching engine active object class defition
*
*/






#ifndef __UPNPCONTENTREQUESTAO_H__
#define __UPNPCONTENTREQUESTAO_H__


// Include Files
#include <e32base.h>
#include <badesca.h>  //for CDesCArray
#include "upnpcontentserverclient.h"

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;


// CLASS DECLARATION
/**
 *  Active object class for querying shared content from upnpcontentserver
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CUpnpContentRequestAO ): public CActive
    {

public: // Constructors and destructor

    /**
     * C++ default constructor.
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aContentServer Handle to upnpcontentserver
     */
    CUpnpContentRequestAO(
        CUPnPFileSharingEngine& aEngine, 
        RUpnpContentServerClient& aContentServer );

    /**
     * Destructor.
     */
    virtual ~CUpnpContentRequestAO();

public: // new functions

    /**
     * Request playlist or album names for UI from server, asynchronous
     * @since S60 3.1
     * @param aType Type of selections to request
     */
    void RequestSelectionContentL( UpnpContentServer::TUpnpMediaType aType );

    /**
     * Get playlist or album names for UI after request, synchronous
     * @since S60 3.1
     * @param aArray Array for albums / playlist names
     */
    void GetSelectionContentL( CDesCArray& aArray );

    /**
     * Request selected items for UI from server, asynchronous
     * @since S60 3.1
     * @param aType Type of selections to request
     */
    void RequestSelectionIndexesL(
        UpnpContentServer::TUpnpMediaType aType );

    /**
     * Get the selected items for UI after request, synchronous
     * @since S60 3.1
     * @param aCurrentSelection Array for selection indexes
     */
    void GetSelectionIndexesL( CArrayFix<TInt>& aCurrentSelection );

private: // from CActive

    /**
     * Function is called when active request is ready
     * @since S60 3.1
     */
    virtual void RunL();

    /**
     * Cancels active request
     * @since S60 3.1
     */
    virtual void DoCancel();

private: // data

    /**
     * file sharing engine
     */
    CUPnPFileSharingEngine& iEngine;

    /**
     * Content server session handle
     */
    RUpnpContentServerClient& iContentServer;

    /**
     * Current state
     */
    enum TState
        {
        ENone = 0,
        ERequestContent,
        ERequestIndexes
        };
    TState iState;

    };

#endif  // __UPNPCONTENTREQUESTAO_H__

// End of file
