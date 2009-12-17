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
* Description:      Utility for working with files in upnp context
*
*/






#ifndef UPNPFILEUTILITY_H
#define UPNPFILEUTILITY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpElement;

// CLASS DECLARATION

/**
* Utility for working with local files in upnp context. The class
* contains static methods that can be used to check various things
* related to downloading files to local filesystem, or when playing
* local files via upnp.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class UPnPFileUtility
    {

public: // Business logic methods

    /**
     * Checks if given file content is protected.
     * Note that if a file is protected, it can not be copied to
     * a remote upnp mediaserver, nor it can be played via a remote
     * renderer. A client should use this interface before attempting
     * any of these operations for usability sake.
     * Currently supports checks for OEM-DRM and WM-DRM.
     * The method will leave if the file is not found, or if the
     * protection status can not be checked for some reason.
     *
     * @since S60 3.2
     * @param aFilePath full path to file
     * @return ETrue if file is protected 
     */
    IMPORT_C static TBool IsFileProtectedL(
        const TDesC& aFilePath );

    /**
     * Checks if the given remote object (and the given particular
     * resource) can fit in memory when downloaded using current
     * default copy target setting
     * @since S60 3.2
     *
     * @param aResource the resource to download
     * @return EFalse if the target drive does not have enough space
     */
    IMPORT_C static TBool FitsInMemory(
        const CUpnpElement& aResource );

private:
    
    /**
     * Checks if the given remote object (and the given particular
     * resource) can fit in memory when downloaded using current
     * default copy target setting
     * @since S60 3.2
     *
     * @param aResource the resource to download
     * @return EFalse if the target drive does not have enough space
     */
    static TBool FitsInMemoryL(
        const CUpnpElement& aResource );
    
    };



#endif  // UPNPFILEUTILITY_H

// End of File
