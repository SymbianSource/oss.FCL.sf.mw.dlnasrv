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
* Description:      file sharing engine active object class defition
 *
*/







#ifndef __UPNPPROGRESSWATCHER_H__
#define __UPNPPROGRESSWATCHER_H__


// Include Files
#include <e32base.h>
#include <e32property.h>
#include "upnpcontentserverclient.h"
#include "upnpfilesharingengineobserver.h"

// FORWARD DECLARATIONS


// CLASS DECLARATION
/**
 * Active object class for observing sharing progress
 * @since S60 3.1
 */
NONSHARABLE_CLASS( CUpnpProgressWatcher ): public CActive
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aEngine File sharing engine
     */
    static CUpnpProgressWatcher* NewL( CUPnPFileSharingEngine& aEngine );

    /**
     * Destructor.
     */
    virtual ~CUpnpProgressWatcher();

public: // New functions

    /**
     * Starts listening property
     * @since S60 3.1
     */
    void StartL();

    /**
     * Stops listening property
     * @since S60 3.1
     */
    void Stop();

protected:

    /**
     * Function is called when active request is ready
     */
    virtual void RunL();

    /**
     * Cancels active request
     */
    virtual void DoCancel();

private:

    /**
     * C++ default constructor.
     * @param aEngine File sharing engine
     */
    CUpnpProgressWatcher( CUPnPFileSharingEngine& aEngine );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Determine if the sharing completed note needs to be shown
     * @since S60 3.1
     * @param aProgress Progress information struct
     */
    void SetShowCompleteNote( const TUpnpProgress& aProgress );

    /**
     * Determine if there is sharing ongoing
     * @since S60 3.1
     * @param aProgress Progress information struct
     * @return ETrue if there is sharing ongoing for at least one media type
     */
    TBool SharingOngoing( const TUpnpProgress& aProgress );

    /**
     * Handle Errors
     * @since S60 3.1
     * @param aProgress Progress information struct
     */
    void HandleErrorL( const TUpnpProgress& aProgress );

private: // data

    /**
     * file sharing engine
     */
    CUPnPFileSharingEngine& iEngine;

    /**
     * property subscribed
     */
    RProperty iProgressInfo;

    /**
     * Indicates if is is needed to show note about sharing end 
     */
    TBool iShowCompleteNote;

    };


#endif  // __UPNPPROGRESSWATCHER_H__

// End of file
