/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Abstract factory for UPnP AV Controller
*
*/







#ifndef C_UPNPAVCONTROLLERFACTORY_H
#define C_UPNPAVCONTROLLERFACTORY_H

#include <e32std.h>

class MUPnPAVController;

/**
 *  Abstract factory for UPnP AV Controller
 *
 *  Abstract factory for UPnP AV Controller. Exports methods to instantiate
 *  the AV Controller.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class UPnPAVControllerFactory
    {

public:

    enum TAVControllerServerStatus
        {
        EStatusOffline = 0,
        EStatusActive
        };

public:

    /**
     * Creates a new instance of AV Controller
     *
     * @return instance of AV Controller
     */
    IMPORT_C static MUPnPAVController* NewUPnPAVControllerL();

    /**
     * Creates a new instance of AV Controller. Instance is left in
     * cleanup stack.
     *
     * @return instance of AV Controller
     */
    IMPORT_C static MUPnPAVController* NewUPnPAVControllerLC();

    /**
     * Return ETrue is some application is using the local Media Server
     * services via AV Controller server.
     *
     * @return EFalse/ETrue
     */
    IMPORT_C static TBool MSServicesInUse();

    /**
     * Returns the operational state of AV Controller Server
     *
     * @param aStatus operational state
     * @return system wide error code
     */
    IMPORT_C static TInt Status( TAVControllerServerStatus& aStatus );
    };

#endif // ? C_UPNPAVCONTROLLERFACTORY_H
