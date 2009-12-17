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


// INCLUDE FILES
#include <f32file.h>
#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include "upnpgallerynotifier.h"

// CONSTANTS
// none

// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UpnpGalleryNotifier::NotifyMediaGalleryL();
// Notify CLF about the update in the file system (file added or
// removed).
//---------------------------------------------------------------------------
//
EXPORT_C void UpnpGalleryNotifier::NotifyMediaGalleryL()
    {
    MCLFContentListingEngine* engine = 
        ContentListingFactory::NewContentListingEngineLC();  
    
    // Do the nofity
    engine->UpdateItemsL();
    
    CleanupStack::PopAndDestroy(); // Do not check pointer (M-class!)
    }

//  End of File
