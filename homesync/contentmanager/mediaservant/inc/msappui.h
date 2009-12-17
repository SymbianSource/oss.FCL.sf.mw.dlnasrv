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
* Description:  Definition for MediaServant appui class
*
*/


#ifndef C_CMSAPPUI_H
#define C_CMSAPPUI_H

// INCLUDES
#include <aknViewAppUi.h>
#include <GSPluginInterface.h>
#include <GSFWViewUIDs.h>
#include <GSPluginLoader.h>

#include "msparameteragent.h"
#include "msfillrulecontroller.h"

// FORWARD DECLARATIONS
class CMSEngine;
class CMSStoreListController;
class CCmDriveInfo;
class CMSGSInterface;

/**
 *  Application UI class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSAppUi ): public CAknViewAppUi
    {

    public:


        /**
         * Destructor.
         */
        ~CMSAppUi();

        /**
         * Changes active view according to parameters.
         * @since S60 3.1
         * @param aOldView current view
         * @param aNewView new view
         */
        void ChangeViewL(const TUid& aOldView, const TUid& aNewView);

        /**
         * Returns fill rule controller
         * @since S60 3.1
         * @return CMSFillRuleController, controller pointer
         */
        CMSFillRuleController* FillRuleController();

        /**
         * Returns store rule controller
         * @since S60 5.0
         * @return CCmStoreListController, pointer to controller
         */
        CMSStoreListController* StoreListController();

        /**
         * Refresh store and fill lists
         * @since S60 3.1
         */
        void RefreshListsL();

        /**
         * File server session
         * @since S60 3.1
         * return RFs, file server session
         */
        RFs& FsSession();

        /**
         * Returns msengine
         * @since S60 3.1
         * @return CMSEngine, pointer to engine
         */
        CMSEngine* MSEngine();

        /**
         * Reads memory info
         * @since S60 3.1
         * @param aCapasity, size of memory
         * @param aFreeSpace, free space of memory
         * @return TInt, error code
         */
        TInt ReadMMCInfoL( TInt64& aCapasity, TInt64& aFreeSpace );
        /**
         * Get Filled Files Size
         * @since S60 3.1
         * @param aSize, size of filled files
         * @param aID, fill rule ID
         */
        void GetFilledFilesSize( TInt64& aSize, const TInt aID );

        /**
         * Returns pointer to parameter agent
         * @since S60 3.1
         * @return CMSParameterAgent, parameter agent
         */
        CMSParameterAgent* ParameterAgent();

        /**
         * drive selection indicator
         * @since S60 3.1
         * @return TBool, ETrue if selected, EFalse otherwise
         */
        TBool DriveSelectedL();

        /**
         * drive selection indicator
         * @since S60 3.1
         * @return TBool, ETrue if selected, EFalse otherwise
         */
        CMSGSInterface* SettingsPlugin();

        /**
         * Compares IDs of two drives
         * @since S60 3.1
         * @return TBool, ETrue if match, EFalse otherwise
         */

        static TBool CompareIds( const CCmDriveInfo& aStoredDrive,
                                 const CCmDriveInfo& aDrive );

        /**
         * Start wizard
         * 
         * @since S60 5.1
         * @return TInt, wizard exit code
         */
        TInt RunWizardL();
        
protected:

// From base class CAknViewAppUi

        /**
         * From CAknViewAppUi
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );

private:
        /**
         * Performs the second phase construction.
         */
        void ConstructL();

        /**
         * Checks if selected memory is on the device
         * @since S60 3.1
         * @return TBool, ETrue if found, EFalse otherwise
         */
        TBool MemoryAttachedL();


        /**
         * Loads gs plugin ( settings view )
         * @since S60 3.1
         */
        void LoadSettingsPluginL();

       /**
         * Check memory selected status, if none of drives have been selected
         * the mass memory will be selected as default drive
         * @since S60 3.2
         */
        void CheckDrivesSelectedL();

private:

        /**
         * view which is currently active
         */
        TInt iActiveView;
        /** 
         * focus index of main view
         */        
        TInt iMainFocusIndex;
        /**
         * pointer to application engine
         */
        CMSEngine*                  iMSEngine;              // owned
        /**
         * parameter class
         */
        CMSParameterAgent*          iParameterAgent;        // owned
        /**
         * fill list controller
         */
        CMSFillRuleController*      iFillRuleController;    // owned
        /**
         * store list controller
         */
        CMSStoreListController*     iStoreListController;   // owned
        /**
         * GS plugin ( settings view )
         */
        CMSGSInterface*             iPlugin;                // owned
    };
#endif // C_CMSAPPUI_H

// End of File
