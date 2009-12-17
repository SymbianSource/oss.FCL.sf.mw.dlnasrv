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
* Description:  Logging facility for UPnP framework
*
*/


// ***************************************************
// * How to use UPnP framework logging facility
// * 1. define KComponentLogfile
// *     _LIT( KComponentLogfile, "AVControlServer.txt");
// * 2. if you want, define one of these macros:
// *     __FILE_LOGGING__     (logging to file)
// *     __CONSOLE_LOGGING__  (logging to console)
// * 3. have flogger in mmp-file
// *     DEBUGLIBRARY FLOGGER.LIB
// * 4. now you can use the log interface
// *     __LOG( char* strz )
// *     __LOG1( char* strz, p1 )
// *     __LOG2( char* strz, p1, p2 )
// *     __LOG3( char* strz, p1, p2, p3 )
// *     __LOG8( const TDesC8& msg )
// *     __LOG16( const TDesC16& msg )
// ***************************************************


#ifndef __UPNPLOGGING_H__
#define __UPNPLOGGING_H__


#include <e32std.h>


#ifdef _DEBUG
  // activate default logging mode in debug builds
  #define __CONSOLE_LOGGING__
  //#define __FILE_LOGGING__
#endif //_DEBUG



#if defined(__CONSOLE_LOGGING__)

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
        RDebug::RawPrint( aMsg );
        }

    inline void __Dummy_KComponentLogfile()
        {
        // this dummy function is to avoid compiler warning #177-D:
        // variable was declared but never referenced
        TUint16 c = KComponentLogfile()[0];
        c = 0;
        }

#elif defined(__FILE_LOGGING__)

    #include <flogger.h>

    // Subdirectory under c:\logs -directory
    _LIT(KLogDir, "upnpframework");

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

#else // !(__CONSOLE_LOGGING__ | __FILE_LOGGING__)

    #define __LOG( strz )
    #define __LOG1( fmt,p1 )
    #define __LOG2( fmt,p1,p2 )
    #define __LOG3( fmt,p1,p2,p3 )
    #define __LOG8( msg )
    #define __LOG16( msg )
    #define __LOG8_1( strz,p1 )

#endif // __CONSOLE_LOGGING__ | __FILE_LOGGING__


#endif // __UPNPLOGGING_H__

