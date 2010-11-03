rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off
cls
echo.
echo ========================================================================
echo CREATING THE UPGRADE SIS FILE FOR DLNA SERVICES
echo ========================================================================
echo.
echo Verifying that the required PKG files can be found...

IF NOT EXIST .\pkg\dlnasrv_cenrep_keys.pkg goto sis_pkg_error
IF NOT EXIST .\pkg\dlnasrv.pkg goto sis_pkg_error

echo [OK]

echo.
echo Deleting the old SIS, SISX and PKG files (if any) from this directory...

IF EXIST .\dlnasrv_cenrep_keys.pkg del .\dlnasrv_cenrep_keys.pkg /F /Q
IF EXIST .\dlnasrv.pkg del .\dlnasrv.pkg /F /Q

echo [OK]

echo.
echo Copying PKG files...

xcopy .\pkg\dlnasrv_cenrep_keys.pkg .
xcopy .\pkg\dlnasrv_udeb.pkg .

echo [OK]

echo.
echo Generating cenrep SIS file...

makesis dlnasrv_cenrep_keys.pkg

echo.
echo [OK]

echo.
echo Generating dlnasrv SIS file...

makesis dlnasrv_udeb.pkg

echo.
echo [OK]

echo.
echo DONE.

goto end


REM ==========================================================================
REM Error notes
REM ==========================================================================

:sis_pkg_error
echo.
echo ERROR: One or more required PKG files missing!
goto echo_failed

:echo_failed
echo.
echo FAILED.
echo.

:end

REM Delete temporary files
IF EXIST .\dlnasrv_cenrep_keys.pkg del .\dlnasrv_cenrep_keys.pkg /F /Q
IF EXIST .\dlnasrv_udeb.pkg del .\dlnasrv_udeb.pkg /F /Q

@echo on
