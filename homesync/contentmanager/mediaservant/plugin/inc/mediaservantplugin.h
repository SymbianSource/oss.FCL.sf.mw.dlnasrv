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
* Description:  CMediaservantPlugin class definition
*
*/


#ifndef C_MEDIASERVANTPLUGIN_H
#define C_MEDIASERVANTPLUGIN_H

// INCLUDES
#include <gulicon.h>
#include <upnpplugininterface.h>
#include "mspropertywatcher.h"

// FORWARD DECLARATIONS
class CEikonEnv;
class CMSEngine;

/**
 * MediaServant UPnP plugin class
 *
 * @Since S60 5.1
 */
class CMediaservantPlugin : public CUPnPPluginInterface,
                            public MMSPropertyObserver
    {
    public:
        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aPluginObserver observer for plugins
         * @return pointer to CMediaservantPlugin
         */
        static CMediaservantPlugin* NewL(
                        MUPnPPluginObserver* aPluginObserver);

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aPluginObserver observer for plugins
         * @return pointer to CMediaservantPlugin
         */
        static CMediaservantPlugin* NewLC(
                        MUPnPPluginObserver* aPluginObserver);                        

        /**
         * Informs to ECom that it has been destroyed.
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        virtual ~CMediaservantPlugin();

    protected:

// From base class CUPnPPluginInterface
        
        /**
         * From CUPnPPluginInterface
         * See base class definition
         */
        const CGulIcon& GetIcon();

        /**
         * From CUPnPPluginInterface
         * See base class definition
         */
        const TDesC& GetTitle();

        /**
         * From CUPnPPluginInterface
         * See base class definition
         */
        const TDesC& GetSecondaryText();

        /**
         * From CUPnPPluginInterface
         * See base class definition
         */
        void ExecuteL();

        /**
         * From CUPnPPluginInterface
         * See base class definition
         */
        TBool RequiresUpnpConfiguration();

// From base class MMSPropertyObserver

        /**
         * From MMSPropertyObserver
         * See base class definition
         */
        void PropertyChangedL( TCmService aService,
                               TInt aTotalItems,
                               TInt aProcessedItems,
                               TInt aItemsToBeProcessed );
                               
    private:

        /**
         * Performs the second phase construction.
         *
         * @since S60 5.1
         * @param aPluginObserver observer for plugins
         * @return None
         */
        void ConstructL( MUPnPPluginObserver* aPluginObserver );

        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aEcomArguments, Ecom arguments
         * @return None
         */
        CMediaservantPlugin( TAny* aEcomArguments );

        /**
         * Load secondary text to plugin
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void LoadPluginTextL();

    private:
        /**
         * Required in all plugins which implement CUPnPPluginInterface
         */
        MUPnPPluginObserver*            iPluginObserver;

        /**
         * Plugin primary text
         */
        HBufC*                          iTitle;             // owned
        /**
         * Plugin secondary text
         */
        HBufC*                          iSecondaryText;     // owned
        /**
         * Flag for indication that IAP setup is needed
         */
        TBool                           iRequiresUpnpConf;
        /**
         * Plugin icon
         */
        CGulIcon*                       iIcon;              // owned
        /**
         * Resource file offset
         */
        TInt                            iResFileOffset;
        /**
         * Control environment for res file handling
         */
        CEikonEnv*                      iCoeEnv;
        /**
         * Progress watcher
         */
        CMSPropertyWatcher*             iMSPropertyWatcher; // owned
        /**
         * Application engine for text loading
         */
        CMSEngine*                      iMSEngine;          // owned
    };


#endif // C_MEDIASERVANTPLUGIN_H

