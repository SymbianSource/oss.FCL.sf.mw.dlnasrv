/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  pluginloader which loads home network ECom plugins
*
*/


#ifndef C_UPNPPLUGINLOADER_H
#define C_UPNPPLUGINLOADER_H

// INCLUDE FILES
// System
#include <e32cmn.h>
#include <e32base.h>
#include <gulicon.h>

// upnpframework / home media extension api
#include <upnpplugininterface.h>

class MUPnPPluginLoaderObserver;

/**
 * Loads all the ECom plugins which implement the CUPnPPluginInterface
 * interface
 *
 * @since S60 3.2
 */
NONSHARABLE_CLASS( CUPnPPluginLoader ) : public MUPnPPluginObserver,
                                         public CBase
    {
    
public: // 1st phase constructor

    /**
     * Returns a pointer to an instance of the CUPnPPluginLoader class
     *
     * @since S60 3.2
     * @param aLoaderObserver reference to the MUPnPPluginLoaderObserver
     * @return Pointer to an instance of the CUPnPPluginLoader class
     */
    IMPORT_C static CUPnPPluginLoader* NewL( MUPnPPluginLoaderObserver& 
        aLoaderObserver );

protected: // 2nd phase constructor

    /**
     * Default constructor
     *
     * @since S60 3.2
     * @param aLoaderObserver reference to the MUPnPPluginLoaderObserver
     */
    CUPnPPluginLoader( MUPnPPluginLoaderObserver& aLoaderObserver );

public: // Destructor

    /**
     * Destructor
     *
     * @since S60 3.2
     */
    IMPORT_C virtual ~CUPnPPluginLoader();

public:

    /**
     * This method creates all ECOM plugins which implement 0x200075DB
     * interface (CUPnPPluginInterface).
     *
     * @since S60 3.2
     * @return Reference to the plugin array. CUPnPMainDialog uses this
     *         reference to draw the UI.
     */
    IMPORT_C const RPointerArray<CUPnPPluginInterface>& CreatePluginsL();

    /**
     * Deletes plugin from iPluginArray which index corresponds the
     * parameter.
     *
     * @since S60 3.2
     * @param aPluginIndex index of plugin to be deleted
     */
    IMPORT_C void DeletePlugin( TInt aPluginIndex );

protected: // From MUPnPPluginObserver

    /**
     * Plugins call this method when they update.
     *
     * @since S60 3.2
     * @param aEvent event from plugin
     */
    void ExtensionEvent( const TExtensionEvent& aEvent );

private:

    /**
     * Pointer array which contains all the plugins
     */
    RPointerArray<CUPnPPluginInterface> iPluginArray;

    /**
     * Pointer to CUPnPPluginLoaderObserver
     * Not own.
     */
    MUPnPPluginLoaderObserver* iLoaderObserver;
    
    };


#endif // C_UPNPPLUGINLOADER_H

// end of file
