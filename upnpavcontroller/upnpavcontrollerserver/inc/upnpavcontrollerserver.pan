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
* Description:     AvController server panic codes
*
*/





#ifndef __AVControllerServer_PAN__
#define __AVControllerServer_PAN__

/** Panic Category */

/** MessageHandler panic codes */
enum TAVControllerServerPanic
    {
    EAVControllerServerBadRequest = 1,
    EAVControllerServerBadDescriptor = 2,
    EAVControllerServerSrvCreateServer = 3,
    EAVControllerServerMainSchedulerError = 4,
    EAVControllerServerCreateTrapCleanup = 5,
    EAVControllerServerSrvSessCreateTimer = 6,
	EAVControllerServerReqAlreadyPending = 7,		//	A request is already pending
	EAVControllerServerDifferencesInIap = 8
    };

#endif // __AVControllerServer_PAN__
