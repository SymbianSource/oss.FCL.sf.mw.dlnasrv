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
* Description:      The factory for CMSettings
*
*/






#include "cmsettingsfactory.h"
#include "cmsettingsengine.h"
#include "cmsettings.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
EXPORT_C MCmSettings* CCmSettingsFactory::NewCmSettingsEngineL()
    {
    return CCmSettingsEngine::NewL();
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//    
EXPORT_C MCmSettings* CCmSettingsFactory::NewCmSettingsEngineLC()
    {
    return CCmSettingsEngine::NewLC();
    }

// end of file
