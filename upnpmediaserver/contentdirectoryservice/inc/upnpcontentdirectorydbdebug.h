/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Database for MediaServer
*
*/



#ifndef C_CUPNPCONTENTDIRECTORYDB_DEBUG_H
#define C_CUPNPCONTENTDIRECTORYDB_DEBUG_H

// Note that _DEBUG is automatically defined for debug builds

// To enable file logging: please comment following #ifdef line and 
// uncomment #ifdef TRUE
#ifdef _DEBUG
//#ifndef TRUE // to disable logging & debug in all builds
//#ifdef TRUE // to enable logging & debug in all builds

/****************************** LOGGING ON ***********************************/
// INCLUDES
#include <flogger.h>

// CONSTANTS
//_LIT(___KLogDir___, "ContentDirectoryDb");
_LIT(___KLogDir___, "ContentDirectoryDb");
_LIT(___KLogFile___, "ContentDirectoryDb.log");
_LIT(___KLogOpeningLine___, "INFO####ContentDirectoryDb.log file opened.");
_LIT(___KInfoFormatString___,"INFO#%S#%S#%d#%S");
_LIT(______KSqlComment______,"ERROR#%S#%S#%d#%S");
_LIT(KSqlComment,"SQL Command:");

// MACROS
/** Replaces the log file
*/
#define __LOG_OPEN()            RFileLogger::Write(___KLogDir___, ___KLogFile___, EFileLoggingModeOverwrite, ___KLogOpeningLine___)

/** For internal use (long comments) - do not use in the code 
*/
#define __LOG_STRING(string)            RFileLogger::Write(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, string);

/** For internal use (long comments) - do not use in the code 
*/
#define __LOG_ALL(longstring) \
    { \
    TInt ___iLogZ___ = 0; \
    TInt ___amountLogZ___ = 100; \
    while(___iLogZ___<longstring.Length()) \
        { \
        if(( longstring.Length()-___iLogZ___) < 100) \
            ___amountLogZ___ = longstring.Length()-___iLogZ___; \
        __LOG_STRING(longstring.Mid(___iLogZ___,___amountLogZ___)); \
        ___iLogZ___+=100; \
        } \
    } 
    
/** Logs error - this is intended to use in the code
* @param in_function: quoted string - name of function in which the called_function gets called, max 30 chars
* @param called_function:  quoted string - just called function, ussually it returns error_code, max 30 chars
* @param error_code: integer - error code returned by just called function
* @param comment:  quoted string - your comment, if longer than 50 chars will be expanded to the sequent line(s)
*/
#define __LOG(in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    if(error_code<0)\
    {\
        TPtrC ___comment_part___(___KComment___().Left(___comment_max_length___));\
        RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ______KSqlComment______, &___KInFunction___, &___KCalledFunction___, error_code, &___comment_part___);\
    }\
    else\
    {\
        TPtrC ___comment_part___(___KComment___().Left(___comment_max_length___));\
        RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ___KInfoFormatString___, &___KInFunction___, &___KCalledFunction___, error_code, &___comment_part___);\
    }\
    if(___KComment___().Length()>___comment_max_length___)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }

#define __LOG_INFO(in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    TPtrC ___comment_part___((___KComment___().Left(___comment_max_length___)));\
    RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ___KInfoFormatString___, &___KInFunction___, &___KCalledFunction___, error_code, &___comment_part___);\
    if(___KComment___().Length()>___comment_max_length___)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }

#define __LOG_ERROR(in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    TPtrC ___comment_part___(___KComment___().Left(___comment_max_length___));\
    RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ______KSqlComment______, &___KInFunction___, &___KCalledFunction___, error_code, &___comment_part___);\
    if(___KComment___().Length()>___comment_max_length___)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }

// file versions

#define __LOG_OPEN_FILE(file, string)           RFileLogger::Write(___KLogDir___, file, EFileLoggingModeOverwrite, string)

#define __LOG_TO_FILE(file,in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    if(error_code<0)\
    {\
        RFileLogger::WriteFormat(___KLogDir___, file, EFileLoggingModeAppend, ______KSqlComment______, &___KInFunction___, &___KCalledFunction___, error_code, &(___KComment___().Left(___comment_max_length___)));\
    }\
    else\
    {\
        RFileLogger::WriteFormat(___KLogDir___, file, EFileLoggingModeAppend, ___KInfoFormatString___, &___KInFunction___, &___KCalledFunction___, error_code, &(___KComment___().Left(___comment_max_length___)));\
    }\
    if(___KComment___().Length()>___comment_max_length___)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }

#define __LOG_INFO_TO_FILE(file,in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ___KInfoFormatString___, &___KInFunction___, &___KCalledFunction___, error_code, &(___KComment___().Left(___comment_max_length___)));\
    if(___KComment___().Length()>__comment_max_length__)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }

#define __LOG_ERROR_TO_FILE(file,in_function,called_function,error_code,comment) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    _LIT(___KComment___,comment);\
    TInt ___comment_max_length___ = 50;\
    RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ______KSqlComment______, &___KInFunction___, &___KCalledFunction___, error_code, &(___KComment___().Left(___comment_max_length___)));\
    if(___KComment___().Length()>___comment_max_length___)\
    {\
        TPtrC ___continuation___((___KComment___().Mid(___comment_max_length___)));\
        __LOG_ALL(___continuation___);\
    }\
    }


// module specific defines 

/** Logs sql Command
*/
#define __LOG_SQL_COMMAND(in_function,called_function,error_code,command) \
    {\
    _LIT(___KInFunction___,in_function);\
    _LIT(___KCalledFunction___,called_function);\
    if(error_code < 0 )\
    {\
        RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ______KSqlComment______, &___KInFunction___, &___KCalledFunction___, error_code, &KSqlComment);\
        __LOG_ALL(command);\
    }\
    else\
    {\
        RFileLogger::WriteFormat(___KLogDir___, ___KLogFile___, EFileLoggingModeAppend, ___KInfoFormatString___, &___KInFunction___, &___KCalledFunction___, error_code, &KSqlComment);\
        __LOG_ALL(command);\
    }\
    }


#else
/****************************** LOGGING OFF **********************************/

#define __LOG_OPEN()
#define __LOG_STRING(string)
#define __LOG_ALL(longstring)
#define __LOG(in_function,called_function,error_code,comment)
#define __LOG_INFO(in_function,called_function,error_code,comment)
#define __LOG_ERROR(in_function,called_function,error_code,comment)
#define __LOG_OPEN_FILE(file, string)
#define __LOG_TO_FILE(file,in_function,called_function,error_code,comment)
#define __LOG_INFO_TO_FILE(file,in_function,called_function,error_code,comment)
#define __LOG_ERROR_TO_FILE(file,in_function,called_function,error_code,comment)

// module specific defines defines 
/** Logs sql Command
*/
#define __LOG_SQL_COMMAND(in_function,called_function,error_code,command)

#endif // _DEBUG

#endif // C_CUPNPCONTENTDIRECTORYDB_DEBUG_H

// End of File
