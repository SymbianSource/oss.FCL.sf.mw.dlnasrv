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
* Description:      Implementation of common utility class
*
*/






#include <f32file.h>
#include <bautils.h>
#include <eikenv.h>
#include <AknUtils.h>
#include <commdb.h>
#include <WlanCdbCols.h>
#include <wlanmgmtclient.h>
#include <pathinfo.h>

#include "cmcommonutils.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CmCommonUtils::LoadResourceFileL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CmCommonUtils::LoadResourceFileL( const TDesC& aFilePath,
                                                CEikonEnv& eikonEnv )
    {
    LOG(_L("[CmCommonUtils]\t CmCommonUtils::LoadResourceFileL"));
        
    TInt resFileOffset( 0 );
            
    RFs& fileSession = eikonEnv.FsSession();

    // Load resource file
    TFileName rscFileName( aFilePath );
    TInt err = CompleteWithAppPath( rscFileName );
    if ( err != KErrNone )
        {
        User::LeaveIfError( err);
        }

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }
    
    TRAP( err, resFileOffset = eikonEnv.AddResourceFileL( rscFileName ) );
    if ( err != KErrNone )
        {
        // try memory card drive
        rscFileName.Copy( PathInfo::MemoryCardRootPath() );
        rscFileName.Delete( 2, 2 ); // remove '//'
        rscFileName.Append( aFilePath );
        resFileOffset = eikonEnv.AddResourceFileL( rscFileName );
        }
    
    return resFileOffset;    
    }

// ---------------------------------------------------------------------------
// CmCommonUtils::SetWlanScanL
// ---------------------------------------------------------------------------
//
EXPORT_C void CmCommonUtils::SetWlanScanL( const TInt aInterval )
    {
    LOG(_L("[CmCommonUtils]\t CmCommonUtils::SetWlanScanL"));

#ifndef __WINS__

    // open commsdb
    CCommsDatabase* commsDb = CCommsDatabase::NewL();
    CleanupStack::PushL( commsDb );

    // open wlan table
    CCommsDbTableView* view = commsDb->OpenViewMatchingUintLC
        (
        TPtrC( WLAN_DEVICE_SETTINGS ),
        TPtrC( WLAN_DEVICE_SETTINGS_TYPE ), KWlanUserSettings
        );

    User::LeaveIfError( view->GotoFirstRecord() );
    User::LeaveIfError( view->UpdateRecord() );

    // set scan interval
    view->WriteUintL( TPtrC( WLAN_BG_SCAN_INTERVAL ), aInterval );

    view->WriteBoolL( TPtrC( WLAN_ALLOW_RADIO_MEASUREMENTS ), ETrue );
    view->WriteBoolL( TPtrC( WLAN_POWER_MODE ), EFalse );
    User::LeaveIfError( view->PutRecordChanges() );

    CleanupStack::PopAndDestroy( view );
    CleanupStack::PopAndDestroy( commsDb );

    CWlanMgmtClient* wlanMgmtClient = CWlanMgmtClient::NewL();
    CleanupStack::PushL( wlanMgmtClient );
    // notify about changes
    wlanMgmtClient->NotifyChangedSettings();
    CleanupStack::PopAndDestroy( wlanMgmtClient );
#endif
    }
    
// End of file
