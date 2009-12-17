/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Utility for working with files in upnp context 
*
*/






// INCLUDE FILES
// System
#include <e32std.h>
#include <upnpelement.h>
#include <DRMCommon.h>              // OEM-DRM
#include <caf/content.h>                // CAF (WM-DRM check)
#include <caf/agent.h>                  // CAF (WM-DRM check)
#include <pathinfo.h>               // PathInfo
#include <upnpitem.h>
#include <upnpattribute.h>          // for getting resource protocol info
#include <upnpdlnaprotocolinfo.h>   // for resolving object mimetypE

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"        // FindAttributeByName
#include "upnpconstantdefs.h"       // KAttributeSize
#include "upnpinternalfileutility.h"

// upnpframework / internal api's
#include "upnpsettingsengine.h"     // get target drive
#include "upnpcommonutils.h"        // for FileExtensionByMimeTypeL

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
//_LIT( KWmDrmAgentName,     "PV WMA CAF Agent" );
//_LIT( K60WmDrmAgentName,   "WM DRM Agent" );
//const TInt64 KPhoneMemoryThreshold    = 1048576; //1MB
_LIT( KTempPrefix, "upnpfwtemp");


// --------------------------------------------------------------------------
// UPnPFileUtility::GetSelectedDownloadLocationL
//---------------------------------------------------------------------------
EXPORT_C HBufC* UPnPInternalFileUtility::GetSelectedDownloadLocationL()
    {
    __LOG( "UPnPFileUtility::GetSelectedDownloadLocationL" );

    HBufC* copyLocation = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( copyLocation );
    TBool copyLocationIsPhoneMemory = 0;
    TPtr copyLocationPtr( copyLocation->Des() );
    
    CUPnPSettingsEngine* settingsEngine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( settingsEngine );
    TRAPD( error, settingsEngine->GetCopyLocationL( 
        copyLocationPtr, copyLocationIsPhoneMemory ) )
    
    // Something wrong in getting copy location. Default to 
    // phone memory.
    if( error != KErrNone )
        {
        TPtrC phoneDrive( PathInfo::PhoneMemoryRootPath() );
        copyLocation->Des().Append( phoneDrive );
        }
    CleanupStack::PopAndDestroy( settingsEngine );
    CleanupStack::Pop( copyLocation );
    return copyLocation; // transfer ownership
    }


// --------------------------------------------------------------------------
// UPnPFileUtility::CreateTmpFileNameL
//---------------------------------------------------------------------------
EXPORT_C HBufC* UPnPInternalFileUtility::CreateTmpFileNameL( const CUpnpItem& aItem )
    {
    __LOG( "UPnPFileUtility::CreateTmpFileName" );

    HBufC* tempfilename = NULL;
    HBufC* fileExt = NULL;
    
    // Get file extension        
    const CUpnpElement& res =
        UPnPItemUtility::ResourceFromItemL( aItem );        
    const CUpnpAttribute* attr = UPnPItemUtility
        ::FindAttributeByName( res, KAttributeProtocolInfo );
        if ( attr != 0 )
        {
        CUpnpDlnaProtocolInfo* pInfo =
            CUpnpDlnaProtocolInfo::NewL( attr->Value() );
        CleanupStack::PushL( pInfo );
        fileExt = UPnPCommonUtils::FileExtensionByMimeTypeL(
            pInfo->ThirdField() );
        CleanupStack::PopAndDestroy( pInfo );
        pInfo = NULL;
        }

    // If file extension exist create whole file name.
    if( fileExt )
        {
        CleanupStack::PushL( fileExt );
        tempfilename = HBufC::NewL( 
            KTempPrefix().Length() + 
            aItem.Id().Length() + 
            fileExt->Length() );
        CleanupStack::PushL( tempfilename );
        // Add prefix
        tempfilename->Des().Append( KTempPrefix ); 
        // Add item name. Convert 8 to 16 and replace illeagal characters.
        HBufC8* tmpItemName8 = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            aItem.Id() );
        CleanupStack::PushL( tmpItemName8 );
        HBufC* itemname = HBufC::NewL( tmpItemName8->Length() );
        CleanupStack::PushL( itemname );
        itemname->Des().Copy( *tmpItemName8 );
        tempfilename->Des().Append( *itemname );
        CleanupStack::PopAndDestroy( itemname );
        CleanupStack::PopAndDestroy( tmpItemName8 );
        // Add file extension
        tempfilename->Des().Append( *fileExt );

        CleanupStack::Pop( tempfilename );
        CleanupStack::PopAndDestroy( fileExt );
        }
    else // Create without file extension
        {
        tempfilename = HBufC::NewL( 
            KTempPrefix().Length() + aItem.Id().Length() );
        CleanupStack::PushL( tempfilename );
        // Add prefix
        tempfilename->Des().Append( KTempPrefix ); 
        // Add item name. Convert 8 to 16 and replace illeagal characters.
        HBufC8* tmpItemName8 = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            aItem.Id() );
        CleanupStack::PushL( tmpItemName8 );
        HBufC* itemname = HBufC::NewL( tmpItemName8->Length() );
        CleanupStack::PushL( itemname );
        itemname->Des().Copy( *tmpItemName8 );
        tempfilename->Des().Append( *itemname );
        CleanupStack::PopAndDestroy( itemname );
        CleanupStack::PopAndDestroy( tmpItemName8 );

        CleanupStack::Pop( tempfilename );
        }

    return tempfilename; //transfer ownership
    }
