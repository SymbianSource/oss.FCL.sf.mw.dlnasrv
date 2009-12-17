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






#ifndef UPNPINTERNALFILEUTILITY_H
#define UPNPINTERNALFILEUTILITY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpElement;
class CUpnpItem;

// CLASS DECLARATION

/**
* Utility for working with local files in upnp context. 
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class UPnPInternalFileUtility
    {

public: // Business logic methods
    /**
     * Create temporary file name for file to be download.
     * Format is upnpfwtemp<item_id><file extension>
     * The following characters are replaced with an underscore: 
     * >, <, ", \, /, *, |, * : and ?
     *
     * @return file name. Ownership transferred.
     */
    IMPORT_C static HBufC* CreateTmpFileNameL( const CUpnpItem& aItem );
    
    /**
     * Get selected download location from settings engine.
     *
     * @return download location. Ownership transferred.
     */
    IMPORT_C static HBufC* GetSelectedDownloadLocationL();
    
    };



#endif  // UPNPINTERNALFILEUTILITY_H

// End of File
