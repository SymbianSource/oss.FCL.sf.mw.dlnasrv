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






#ifndef __UPNPAPPSHARINGENGINEOBSERVER_H__
#define __UPNPAPPSHARINGENGINEOBSERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include "upnpcontentserverclient.h"

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;


// CONSTANTS


/**
 *  File sharing observer definition
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
class MUPnPFileSharingEngineObserver
    {

public:    

    /**
     * Virtual destructor
     */
    virtual ~MUPnPFileSharingEngineObserver() {};

    /**
     * Function informs when file sharing was enabled or disabled
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aError, error code
     * @param aPhase ETrue if current phase is sharing activation,
     * otherwise EFalse
     */
    virtual void HandleSharingStatus( 
        CUPnPFileSharingEngine& /*aEngine*/, 
        TInt /*aError*/, 
        TBool /*aPhase*/ ) {}

    /**
     * Informs about status of the sharing progress
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aProgress Data contains information about sharing\
 progress
     */                                          
    virtual void HandleSharingProgress(
        CUPnPFileSharingEngine& /*aEngine*/, 
        const RArray<TUpnpProgressInfo>& /*aProgress*/ ) {} 

    /**
     * Function informs when file sharing is done
     * @param aEngine File sharing engine
     * @param aError, error code
     */
    virtual void HandleSharingDone( 
        CUPnPFileSharingEngine& /*aEngine*/, TInt /*aError*/ ) {}

    /**
     * callback for CUPnPFileSharingEngine::RequestSelectionIndexesL
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aError Error code
     */
    virtual void HandleSelectionIndexes( 
        CUPnPFileSharingEngine& /*aEngine*/, TInt /*aError*/ ) {}

    /**
     * callback for CUPnPFileSharingEngine::RequestSelectionContentL
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aError Error code
     */
    virtual void HandleSelectionContent( 
        CUPnPFileSharingEngine& /*aEngine*/, TInt /*aError*/ ) {}

    /**
     * Callback to inform if connection has been lost
     * @since S60 3.1
     * @param aEngine File sharing engine
     */ 
    virtual void HandleSharingConnectionLost( 
        CUPnPFileSharingEngine& /*aEngine*/ ) {}

    };
    
#endif //__UPNPAPPSHARINGENGINEOBSERVER_H__

// End of File
