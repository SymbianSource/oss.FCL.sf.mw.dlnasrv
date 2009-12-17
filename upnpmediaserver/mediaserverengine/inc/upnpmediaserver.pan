/** @file
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#ifndef __MEDIASERVER_PAN__
#define __MEDIASERVER_PAN__

/** Panic Category */

/** MessageHandler panic codes */
enum TMediaServerPanic
    {
    EMediaServerBadRequest = 1,
    EMediaServerBadDescriptor = 2,
    EMediaServerSrvCreateServer = 3,
    EMediaServerMainSchedulerError = 4,
    EMediaServerCreateTrapCleanup = 5,
    EMediaServerSrvSessCreateTimer = 6,
	EMediaServerReqAlreadyPending = 7,		//	A request is already pending
	EMediaServerDifferencesInIap = 8
    };

#endif // __MEDIASERVER_PAN__
