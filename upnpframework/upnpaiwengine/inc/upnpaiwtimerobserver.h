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
* Description:  This interface class defines the call back interface for the
*                UPnPAiwEngineTimer.
*
*/


#ifndef UPNP_AIW_TIMER_OBSERVER_H
#define UPNP_AIW_TIMER_OBSERVER_H

/**
* Defines the call back interface for the timer of the UPnP Aiw Engine.
*
* @since S60 3.0
*/
class MUPnPAiwTimerObserver
    {

    public:

        /**
         * Callback method for the UPnPAiWTimer.
         *
         * @since S60 3.0
         */
        virtual void TimerCallback() = 0;

    };

#endif // UPNP_AIW_TIMER_OBSERVER_H

// End of File
