/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  upnpsharingalgorithm interface
*
*/

#ifndef M_UPNP_SHARING_ALGORITHM_H
#define M_UPNP_SHARING_ALGORITHM_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpCdsLiteObjectArray;

/**
 *  Defines the upnp sharing algorithm interface
 *
 *  @lib upnpsharingalgorithm.lib
 *  @since S60 5.2
 */
class MUpnpSharingAlgorithm
    {

public:

    /**
     * Shares the given file using the local M-DMS. Does not share the same
     * file twice. Checks the validity of the parameter, existance of the
     * file and drm protection. Leaves with KErrArgument if the given 
     * parameter is invalid or if the file does not exist. Leaves with 
     * KErrAccessDenied if the file is drm protected. Leaves normally if 
     * out-of-memory.
     *
     * @since S60 5.2
     * @param aFileName, file to be shared
     */
    virtual void ShareFileL( const TFileName &aFileName ) = 0;

    /**
     * Unshares the given file using the local M-DMS. Does not do anything if
     * the file is not shared. Leaves with KErrCorrupt if the component's
     * internal object array is corrupted. Leaves normally if out-of-memory.
     *
     * @since S60 5.2
     * @param aFileName, file to be unshared
     */
    virtual void UnshareFileL( const TFileName &aFileName ) = 0;

    /**
     * Reads the shared content from UPnPSharingalgoritm
     *
     * @since S60 5.2
     * @return a reference to the shared content array in UPnPSharingalgoritm.
     */
    virtual CUpnpCdsLiteObjectArray& ReadSharedContent() = 0;

    /**
     * Deletes the object and frees its resources.
     *
     * @since S60 5.2
     */
    virtual void Close() = 0;

    /**
     * Check if item is shared based on item filename
     *
     * @since S60 5.2
     * @param aFileName, file to be checked
     * @return TBool, ETrue if item is shared, EFalse otherwise
     */
    virtual TBool IsFileSharedL( const TFileName &aFileName ) = 0;
    };

#endif /* M_UPNP_SHARING_ALGORITHM_H */

//  End of File

