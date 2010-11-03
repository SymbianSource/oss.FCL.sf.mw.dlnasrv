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
* Description:  Factory for UPnPSharingAlgorithm
*
*/


#ifndef C_UPNP_SHARING_ALGORITHM_FACTORY_H
#define C_UPNP_SHARING_ALGORITHM_FACTORY_H

#include <e32std.h>
#include <e32base.h>

// Forward declarations
class MUpnpSharingAlgorithm;

/**
 * Factory for UPnPSharingAlgorithm
 *
 * @lib upnpsharingalgorithm.lib
 * @since S60 5.2
 */
class CUPnPSharingAlgorithmFactory : public CBase
    {

public:

    /**
     * Creates a new instance of UpnpSharingAlgorithm
     *
     * @since S60 5.2
     * @return  pointer of type MUpnpSharingAlgorithm class
     */
    IMPORT_C static MUpnpSharingAlgorithm* NewSharingApiL();

    };

#endif // C_UPNP_SHARING_ALGORITHM_FACTORY_H
// End of file
