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
* Description:      Abstract factory for CMSettings
*
*/







#ifndef C_CMSETTINGSFACTORY_H
#define C_CMSETTINGSFACTORY_H

#include <e32std.h>
#include <e32base.h>

/* Forward declarations. */
class MCmSettings;

/**
 *  Abstract factory for CMSettings
 *
 *  @lib cmsettings.lib
 *  @since S60 v3.1
 */
class CCmSettingsFactory : public CBase
    {

public:

    /**
     * Creates a new instance of CSSettings
     * @return  pointer to CCmSettingsEngine class
     */
    IMPORT_C static MCmSettings* NewCmSettingsEngineL();

    /**
     * Creates a new instance of CSSettings. Instance is left in
     * cleanup stack.
     * @return  pointer to CCmSettingsEngine class
     */
    IMPORT_C static MCmSettings* NewCmSettingsEngineLC();

    };

#endif // C_CMSETTINGSFACTORY_H
