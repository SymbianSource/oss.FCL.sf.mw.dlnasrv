/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Configurator class for mediaserver icons
*
*/






#ifndef __UPNPICONCONFIGURATOR_H__
#define __UPNPICONCONFIGURATOR_H__


// Include Files
#include <e32base.h>
#include <upnpmediaserverclient.h>
#include "upnpcontentserverclient.h"
// FORWARD DECLARATIONS
class RUpnpMediaServerClient;
class CActiveSchedulerWait;
class CRepository;
class CUPnPFileSharingEngineAO;

// CLASS DECLARATION
/**
 *  Active object class for add icons
 *  Handles add icons.
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CUPnPIconConfigurator ): public CActive
    {

public: // Constructors and destructor

    /**
     * C++ default constructor.
     * @since S60 3.1
     * @param CUPnPFileSharingEngine*, pointer to file sharing engine
     */
    CUPnPIconConfigurator( 
        RUpnpMediaServerClient& aMediaServer, 
        CUPnPFileSharingEngineAO& aFileShareingAO );

    /**
     * Destructor.
     */
    virtual ~CUPnPIconConfigurator();

public: // thre icon configurator interface

    /**
     * Checks if icons need to be configured.
     * @return ETrue if icons are not yet configured, EFalse if they are
     */
    TBool NeedToConfigureIcons();

    /**
     * Configure icons for upnp mediaserver
     * Asynchronous method - wakes up given active object
     * when operation is completed.
     * @return error code
     */
    void ConfigureIconsL();

protected:

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

    /**
     * RunL error handler
     * @param aError Error code
     * return See CActive description
     * @since S60 3.1
     */
    TInt RunError(TInt aError);

private: // own methods

    /**
     * Removes all icons
     */
    void RemoveIcons();

    /**
     * Sets first icon
     */
    void AddIcon1L();

    /**
     * Sets second icon
     */
    void AddIcon2L();

    /**
     * Sets third icon
     */
    void AddIcon3L();

    /**
     * Sets fourth icon
     */
    void AddIcon4L();

    /**
     * Resolves the absolute icon path. The icon will be searched from
     * the same drive where this DLL is located.
     * @param relativePath relative path, without the drive, beginning with \
     * @param iconPath, output, receives absolute icon path.
     */
    void ResolveIconPath( const TDesC& relativePath, TFileName& iconPath );

    /**
     * Checks if icons are added (from cenrep)
     * @return true (configured) of false (icons not configured)
     */
    TBool CheckIconsAddedL();

public:

    /**
     * Marks icons configured to cenrep
     * @param aMark true (configured) or false (not configured)
     */
    static void MarkIconsAddedL( TBool aMark );

private: // data

    enum TConfiguratorState
        {
        EStateIdle, // ready
        EStateRemoving, // removing all currently existing icons 
                        // (only in debug mode)
        EStateAdding1, // adding first icon
        EStateAdding2, // adding second icon
        EStateAdding3, // adding third icon
        EStateAdding4 // adding fourth icon
        };

    /**
     * mediaserver session handle
     */
    RUpnpMediaServerClient& iMediaServer;

    /**
     * Indicates whether the starting sharing or stopping
     * sharing is ongoing.
     */
    TConfiguratorState iState;

    /**
     * icon
     */   
    CUpnpIcon* iIcon1;
    CUpnpIcon* iIcon2;
    CUpnpIcon* iIcon3;
    CUpnpIcon* iIcon4;
    
    /**
     * Sharing engine's active object
     */
    CUPnPFileSharingEngineAO& iFileShareEngineAO;
    
    };

#endif  // __UPNPICONCONFIGURATOR_H__

// End of file
