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
* Description:      a base class for browsing and rendering sessoin observers
*
*/






#ifndef M_UPNPAVSESSIONOBSERVERBASE_H
#define M_UPNPAVSESSIONOBSERVERBASE_H

/**
* Defines the response interface for the general operations of the
* UPnP AV Controller.
*
* @since Series 60 3.1
*/
class MUPnPAVSessionObserverBase
    {
    
public:

    enum TUPnPDeviceDisconnectedReason
        {
        EDisconnected = 100,
        EWLANLost
        };
    
public:

    };

#endif // M_UPNPAVSESSIONOBSERVERBASE_H

// End of File
