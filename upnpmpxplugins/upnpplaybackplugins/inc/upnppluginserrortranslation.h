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
* Description:      translate all plugins error code for music player
*
*/





#ifndef T_UPNPPLUGINSERRORTRANSLATION_H
#define T_UPNPPLUGINSERRORTRANSLATION_H

// INCLUDES
#include <e32std.h>
/**
 * Class for translate all plugins error code for music player
 *
 * @since S60 v3.1
 */
class TUpnpPluginsErrorTranslation
{
public:

    /**
     * Static function for translate error
     *
     * @since Series 60 3.1
     * @param aError the upnp plugins error code
     * @return a valid error code
     */
    static TInt ErrorTranslate( TInt aError );
};

#endif /*UPNPPLUGINSERRORTRANSLATION_H*/

