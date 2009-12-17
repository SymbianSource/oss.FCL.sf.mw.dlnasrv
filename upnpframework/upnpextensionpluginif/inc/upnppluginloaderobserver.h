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
* Description:  pluginloader observer
*
*/


#ifndef C_UPNPPLUGINLOADEROBSERVER_H
#define C_UPNPPLUGINLOADEROBSERVER_H


/**
 * Implemented by CUPnPMainDialog
 *
 * @since S60 3.2
 */
class MUPnPPluginLoaderObserver
    {
public:

    /**
     * Called by UPnPPluginLoader when plugin(s) has been updated
     *
     * @since S60 3.2
     */
    virtual void PluginsUpdated()=0;
    
    };

#endif // C_UPNPPLUGINLOADEROBSERVER_H

// end of file


