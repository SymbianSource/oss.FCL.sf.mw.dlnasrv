/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*/


#ifndef TRACEDEFS_H
#define TRACEDEFS_H


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
//

/**
* Trace prefixes for macros with component name.
*/
#define _PREFIX_TRACE( a ) TPtrC( (const TText*) L"[DLNAEXAMPLEAPP]: " L##a )
#define _PREFIX_TRACE_2( a, b ) TPtrC( (const TText*) L"[DLNAEXAMPLEAPP]: " L##a L##b )
#define _PREFIX_TRACE8( a ) (const char*)( "[DLNAEXAMPLEAPP] " ##a )

/**
* Files for traces and event log
*/
#define TRACE_FILE "c:\\logs\\dlnaexampleapp\\dlnaexampleapp_trace.txt"
#define LOG_FILE "c:\\logs\\dlnaexampleapp\\dlnaexampleapp_log.txt"

#endif // TRACEDEFS_H
