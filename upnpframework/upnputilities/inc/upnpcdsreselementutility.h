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
* Description:  Utility class for UPnP contentdirectory res (resource)
*                element related actions.
*
*/


#ifndef UPNPCDSRESELEMENTUTILITY_H
#define UPNPCDSRESELEMENTUTILITY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpElement;

// CLASS DECLARATION

/**
 * CUpnpCdsResElementUtility provides utility functions for dealing with UPnP 
 * res (resource) elements. 
 *
 * @lib upnputilities.lib
 * @since Series 60 3.1
 */
class UpnpCdsResElementUtility
    {
public: // New functions

    /**
     * Adds a res tag to an existing UPnP item. Res tag has always 
     * protocolinfo and size parameters. Image files will have resolution 
     * parameter and audio&video files will have duration parameter filled.
     * 
     * @since Series 60 3.1
     * @param aItem UPnP item that needs res element.
     * @param aFilename The filename of the resource. Full path included.
     */
    IMPORT_C static void AddResElementL(
        CUpnpItem& aItem,
        const TDesC& aFilename );

    /**
     * Checks if the provided uri is absolute uri
     *
     * @since Series 60 3.1
     * @param aUri uri
     * @return ETrue if the uri is absolute
     */
    IMPORT_C static TBool IsUriAbsolute( const TDesC8& aUri );    

    };

#endif // UPNPCDSRESELEMENTUTILITY_H  
            
// End of File
