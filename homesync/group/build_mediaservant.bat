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
rem Description: Build mediaservant
rem

echo on
echo Building !!

call bldmake bldfiles
call abld build armv5 >"%DRIVE%"\mediaservant_2.1\group\mediaservant_armv5.txt 2>&1
call abld build armv5 -c >"%DRIVE%"\mediaservant_2.1\group\mediaservant_armv5_check.txt 2>&1
cd "%DRIVE%"\mediaservant_2.1\install
call mediaservant_sis.bat
call mediaservant_stub_sis.bat
cd "%DRIVE%"\

echo Done !!!
echo off
