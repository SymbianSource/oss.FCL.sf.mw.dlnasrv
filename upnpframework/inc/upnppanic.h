/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic facility for UPnP framework
*
*/


// ***************************************************
// * How to use UPnP framework panic facility
// * 1. define KModuleName and KComponentLogfile (at least in _DEBUG)
// *     _LIT( KModuleName, "upnpplaybacksession");
// *     _LIT( KComponentLogfile, "AVControlServer.txt");
// * 2. if you like, define __UPNP_DEBUG_PANIC__
// *     by default this is only active in debug builds
// * 3. use the panic interface
// *     __ASSERTD(check, line) to assert id debug builds, panic if fail
// *     __ASSERT(check, line) to assert and panic if fail
// *     __PANICD(line) to panic instantly in debug builds
// *     __PANIC(line) to panic instantly
// *     panics will use module name and line number (use __LINE__)
// *     and create an entry to module logfile, if logging is enabled
// ***************************************************


#ifndef __UPNPPANIC_H__
#define __UPNPPANIC_H__


#include <e32std.h>
#include "upnplogging.h"

#ifdef _DEBUG
  // activate panic in debug builds
  #define __UPNP_DEBUG_PANIC__
#endif //_DEBUG


// define panics that work in all builds
#define __ASSERT( check, line ) \
    if(!(check)) \
        { \
        __LOG2( "Assertion failed: %S %d", \
            &KModuleName, line ); \
        User::Panic( KModuleName, line ); \
        }

#define __PANIC(line) \
    __LOG2( "Panic: %S %d", \
        &KModuleName, line ); \
    User::Panic( KModuleName, line )

#ifdef __UPNP_DEBUG_PANIC__

    // panics that work only in debug  builds
    #define __ASSERTD( check, line ) \
        if(!(check)) \
            { \
            __LOG2( "Assertion failed: %S %d", \
                &KModuleName, line ); \
            User::Panic( KModuleName, line ); \
            }

    #define __PANICD( line ) \
        __LOG2( "Panic: %S %d", \
            &KModuleName, line ); \
        User::Panic( KModuleName, line )

#else // !__UPNP_DEBUG_PANIC__

    // these are optimized away
    #define __ASSERTD( check, line )
    #define __PANICD( line )

#endif // __UPNP_DEBUG_PANIC__


#endif // __UPNPPANIC_H__
