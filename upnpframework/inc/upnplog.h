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
* Description:  log/assert facility of upnp framework
*
*/


// **************************************************************************
// *
// *  HOW TO USE THIS LOGGING FACILITY
// *
// * 1. enable log facility in the code.
// *     _LIT( KComponentLogfile, "component.txt");
// *     #include "upnplog.h"
// *
// * 2. have flogger in mmp-file
// *     DEBUGLIBRARY FLOGGER.LIB
// *
// * 3. use the log interface in the source
// *     __LOG( char* strz )
// *     __LOG1( char* strz, p1 )
// *     __LOG2( char* strz, p1, p2 )
// *     __LOG3( char* strz, p1, p2, p3 )
// *     __LOG8( const TDesC8& msg )
// *     __LOG16( const TDesC16& msg )
// *     __ASSERTD( test, file, line )
// *     __ASSERT( test, file, line )
// *     __PANICD( file, line )
// *     __PANIC( file, line )
// *
// * 4. The default logging/assertion modes are following:
// *     DEBUG build:    log to console, assertion mode on.
// *     RELEASE build:  log turned off, assertion mode off.
// *     To override this preset, you can use following macros:
// *     MACRO __UPNP_LOG_FILE          (logging to file)
// *     MACRO __UPNP_LOG_CONSOLE       (logging to console)
// *     MACRO __UPNP_LOG_OFF           (logging turned off in DEBUG build)
// *     MACRO __UPNP_DEBUG_ASSERT      (enable D-panic and D-assert)
// *     MACRO __UPNP_DEBUG_ASSERT_OFF  (disable D-panic and D-assert)
// *     in case of using logging in RELEASE build, remember to switch
// *     LIBRARY FLOGGER.LIB instead of DEBUGLIBRARY.
// *
// **************************************************************************


#ifndef __UPNPLOG_H__
#define __UPNPLOG_H__


#include <e32std.h>


// in DEBUG build activate the default modes
#ifdef _DEBUG

    #if !( defined(__UPNP_LOG_FILE) || \
        defined(__UPNP_LOG_CONSOLE) || \
        defined(__UPNP_LOG_OFF) )
        // activate default logging mode
        #define __UPNP_LOG_CONSOLE
    #endif

    #if !( defined(__UPNP_DEBUG_ASSERT) || \
        defined(__UPNP_DEBUG_ASSERT_OFF) )
        // activate debug assertion mode
        #define __UPNP_DEBUG_ASSERT
    #endif

#endif //_DEBUG


// this dummy function is to avoid compiler warning #177-D:
// variable was declared but never referenced
inline void __Dummy_KComponentLogfile()
    {
    TUint16 c = KComponentLogfile().Size();
    c = 0;
    }

//
// **************************************************************************
// LOGGING MACROS
// **************************************************************************
//

#if defined(__UPNP_LOG_FILE)

    #include <flogger.h>

    // Subdirectory under c:\logs -directory
    _LIT( KLogDir, "upnpframework" );

    #define __LOG( strz ) Print16(_L(strz))
    #define __LOG1( strz,p1 ) Print16(_L(strz),p1)
    #define __LOG2( strz,p1,p2 ) Print16(_L(strz),p1,p2)
    #define __LOG3( strz,p1,p2,p3 ) \
        Print16(_L(strz),p1,p2,p3)
    #define __LOG8( msg ) Print8( _L8( "%S" ), &msg )
    #define __LOG16( msg ) Print16( _L16( "%S" ), &msg )
    #define __LOG8_1( strz,p1 ) Print8(_L8(strz),p1)

    // Writes a log text to a file using file logger
    inline void Print16( const TRefByValue<const TDesC> aFmt, ... )
        {
        VA_LIST list;
        VA_START(list,aFmt);
        RFileLogger::WriteFormat( KLogDir, KComponentLogfile,
            EFileLoggingModeAppend, aFmt, list );
        VA_END( list );
        }    

        // Writes a log text to a file using file logger
    inline void Print8( const TRefByValue<const TDesC8> aFmt, ... )
        {
        VA_LIST list;
        VA_START(list,aFmt);
        RFileLogger::WriteFormat( KLogDir, KComponentLogfile,
            EFileLoggingModeAppend, aFmt, list );
        VA_END( list );
        }

#elif defined(__UPNP_LOG_CONSOLE)

    #include <f32file.h>

    #define __LOG( strz ) RDebug::Print(_L(strz))
    #define __LOG1( strz,p1 ) RDebug::Print(_L(strz),p1)
    #define __LOG2( strz,p1,p2 ) RDebug::Print(_L(strz),p1,p2)
    #define __LOG3( strz,p1,p2,p3 ) \
        RDebug::Print(_L(strz),p1,p2,p3)
    #define __LOG8( msg ) Debug8(msg)
    #define __LOG16( msg ) RDebug::Print(msg)
    #define __LOG8_1( strz,p1 ) RDebug::Printf(strz, p1)

    inline void Debug8( const TDesC8& aMsg )
        {
        __Dummy_KComponentLogfile();
        RDebug::RawPrint( aMsg );
        }

#else // __UPNP_LOG_OFF

    #define __LOG( strz ) __Dummy_KComponentLogfile();
    #define __LOG1( fmt,p1 ) __Dummy_KComponentLogfile();
    #define __LOG2( fmt,p1,p2 ) __Dummy_KComponentLogfile();
    #define __LOG3( fmt,p1,p2,p3 ) __Dummy_KComponentLogfile();
    #define __LOG8( msg ) __Dummy_KComponentLogfile();
    #define __LOG16( msg ) __Dummy_KComponentLogfile();
    #define __LOG8_1( strz,p1 ) __Dummy_KComponentLogfile();

#endif // __UPNP_LOG_FILE || __UPNP_LOG_CONSOLE || __UPNP_LOG_OFF


//
// **************************************************************************
// ASSERTION MACROS
// **************************************************************************
//

#define __ASSERT( test, file, line ) \
    if( !( test ) ) \
        { \
        __LOG2( "Assertion failed: %s %d", \
            file, line ); \
        User::Panic( _L(file), line ); \
        }

#define __PANIC( file, line ) \
    { \
    __LOG2( "Panic: %s %d", \
        file, line ); \
    User::Panic( _L(file), line ) \
    }

#ifdef __UPNP_DEBUG_ASSERT

    #define __ASSERTD( test, file, line ) \
        if( !( test ) ) \
            { \
            __LOG2( "Assertion failed: %s %d", \
                file, line ); \
            User::Panic( _L(file), line ); \
            }

    #define __PANICD( file, line ) \
        { \
        __LOG2( "Panic: %s %d", \
            file, line ); \
        User::Panic( _L(file), line ); \
        }

#else // __UPNP_DEBUG_ASSERT_OFF

    #define __ASSERTD( test, file, line )
    #define __PANICD( file, line )

#endif // __UPNP_DEBUG_ASSERT || __UPNP_DEBUG_ASSERT_OFF



#endif // __UPNPLOG_H__

