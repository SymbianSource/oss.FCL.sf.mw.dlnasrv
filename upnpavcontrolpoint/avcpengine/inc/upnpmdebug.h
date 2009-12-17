/** @file
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Debug printing to a log file
*
*/


#ifndef C_UPNPMDEBUG_H
#define C_UPNPMDEBUG_H

// INCLUDES
#include <e32std.h>

#ifdef _DEBUG
#include <flogger.h>
#include "upnpmydebug.h"

// Take care that the correct upnpmydebug.h will be included
// In upnpmydebug.h in your module inc directory,
// define your application-specific configuration like this:
// ----------------------------------------------------------
// Debug file - debug output is disabled if the parent dir does not exist
// _LIT(KDebugFileName, "C:\\logs\\remotefileengine\\remotefileengine.txt");

// Replace the current debug file - if not defined appends to the file
#ifndef APPEND_TO_DEBUG_FILE
#define REPLACE_DEBUG_FILE
#endif

// Maximum formatted size resulting from a single DEBUG* call
#ifndef MAX_DEBUG_STRING_LENGTH
#define MAX_DEBUG_STRING_LENGTH 4096
#endif
// ----------------------------------------------------------


// FORWARD DECLARATIONS
static void DoOutput(TDesC8& aData);

static void DebugStringNarrowL(const char* aFmt, ...)
    {
    VA_LIST args;
    VA_START(args, aFmt);
    
    TPtrC8 fmt(reinterpret_cast<const unsigned char *>(aFmt));
    HBufC8* buf = HBufC8::NewLC(MAX_DEBUG_STRING_LENGTH);
    buf->Des().FormatList(fmt, args);
    buf->Des().Append('\n');
    DoOutput(*buf);
    CleanupStack::PopAndDestroy(buf);
       
    VA_END(args);
    }

static void DebugStringWideL(const char* aFmt, ...)
    {
    VA_LIST args;
    VA_START(args, aFmt);
    
    TPtrC8 fmt(reinterpret_cast<const unsigned char *>(aFmt));
    HBufC* fmt16 = HBufC::NewLC(fmt.Length());
    fmt16->Des().Copy(fmt);
    HBufC* buf = HBufC::NewLC(MAX_DEBUG_STRING_LENGTH);
    buf->Des().FormatList(*fmt16, args);
    buf->Des().Append('\n');
    HBufC8* buf8 = HBufC8::NewLC(buf->Length());
    buf8->Des().Copy(*buf);
    DoOutput(*buf8);
    CleanupStack::PopAndDestroy(3); // fmt16, buf, buf8

    VA_END(args);
    }

static void DebugBufferL(const TDesC8& aBuf)
    {
    DebugStringNarrowL("\"%S\"", &aBuf);
    }

static void DebugBufferL(const TDesC& aBuf)
    {
    DebugStringWideL("\"%S\"", &aBuf);
    }

static void DebugTimeL(const TTime& aTime)
    {
    TBuf<64> dateTimeString;
    _LIT(KDateString, "%E%D%X%N%Y %1 %2 %3");
    aTime.FormatL(dateTimeString, KDateString);
    DebugBufferL(dateTimeString);
    _LIT(KTimeString, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
    aTime.FormatL(dateTimeString, KTimeString);
    DebugBufferL(dateTimeString);
    }

static void DoOutput(TDesC8& aData)
    {
    RFileLogger::Write(KDebugDirName,
                       KDebugFileName,
                       EFileLoggingModeAppend,
                       aData);
    }
#endif    

// MACROS
// If you output one or more narrow descriptors by using '%S',
//    use DEBUGSTRING8
// else if you output one or more unicode descriptors by using '%S',
//    use DEBUGSTRING16
// else
//    use DEBUGSTRING
//
// Narrow and unicode cannot be mixed in a single DEBUGSTRINGx call.

#ifdef _DEBUG
#define DEBUGINIT() DebugInit()
#define DEBUGINITSUSPENDED() DebugInit(ETrue)
#define DEBUGENABLE() SetDebugEnabled(ETrue)
#define DEBUGDISABLE() SetDebugEnabled(EFalse)
#define DEBUGSUSPEND() SetDebugSuspended(ETrue)
#define DEBUGCONTINUE() SetDebugSuspended(EFalse)
#define DEBUGSTRING(x) DebugStringNarrowL x
#define DEBUGSTRING8(x) DebugStringNarrowL x
#define DEBUGSTRING16(x) DebugStringWideL x
#define DEBUGBUFFER(x) DebugBufferL x
#define DEBUGTIME(x) DebugTimeL x
#else
#define DEBUGINIT()
#define DEBUGINITSUSPENDED()
#define DEBUGENABLE()
#define DEBUGDISABLE()
#define DEBUGSUSPEND()
#define DEBUGCONTINUE()
#define DEBUGSTRING(x)
#define DEBUGSTRING8(x)
#define DEBUGSTRING16(x)
#define DEBUGBUFFER(x)
#define DEBUGTIME(x)
#endif

#endif // C_MUPNPDEBUG_H

// End of File
