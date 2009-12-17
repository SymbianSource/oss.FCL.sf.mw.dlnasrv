/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Functions to notify Media Gallery of new files.
*
*/


#ifndef UPNPGALLERYNOTIFIER_H
#define UPNPGALLERYNOTIFIER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
* Functions to notify Media Gallery of new files.
*
* @lib upnputilities.lib
* @since S60 3.1
*
*/
class UpnpGalleryNotifier
    {

public:

    /**
     * Notify CLF about the update in the file system (file added 
     * or removed).
     *
     * @since S60 3.1
     */
    IMPORT_C static void NotifyMediaGalleryL();

    };

#endif  // UPNPGALLERYNOTIFIER_H

// End of File
