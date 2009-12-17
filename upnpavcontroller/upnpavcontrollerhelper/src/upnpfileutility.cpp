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

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"        // FindAttributeByName
#include "upnpconstantdefs.h"       // KAttributeSize
#include "upnpfileutility.h"

// upnpframework / internal api's
#include "upnpsettingsengine.h"     // get target drive

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KWmDrmAgentName,     "PV WMA CAF Agent" );
_LIT( K60WmDrmAgentName,   "WM DRM Agent" );
const TInt64 KPhoneMemoryThreshold    = 1048576; //1MB


// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UPnPFileUtility::IsFileDrmProtectedL
//---------------------------------------------------------------------------
EXPORT_C TBool UPnPFileUtility::IsFileProtectedL(
    const TDesC& aFilePath )
    {
    __LOG( "FileUtility:IsFileDrmProtectedL( aFileName )" );
    TBool isProtected = ETrue;

    // Check if the file is OEM-DRM protected
    DRMCommon* drmCommon = DRMCommon::NewL();
    CleanupStack::PushL( drmCommon );
    User::LeaveIfError( drmCommon->Connect() );
    User::LeaveIfError( drmCommon->IsProtectedFile(
        aFilePath, isProtected ) );
    CleanupStack::PopAndDestroy( drmCommon );
    drmCommon = NULL;

    // If not OEM-DRM protected, check if the file is WM-DRM protected
    if( !isProtected )
        {
        CContent* content = CContent::NewLC( aFilePath );
        TAgent agent = content->Agent();

        // Check if .wma file
        if( agent.Name().CompareF( KWmDrmAgentName ) == 0 ||
            agent.Name().CompareF( K60WmDrmAgentName ) == 0 )
            {
            __LOG( "FileUtility:IsFileDrmProtectedL: .wma file " );
            CData* data = CData::NewL( (TVirtualPathPtr) aFilePath,
                EPeek, EContentShareReadOnly );
            data->GetAttribute( EIsProtected, isProtected );
            delete data;
            }
        CleanupStack::PopAndDestroy( content );
        content = NULL;
        }
    return isProtected;
    }


// --------------------------------------------------------------------------
// UPnPFileUtility::FitsInMemory
// see UPnPFileUtility.h
// --------------------------------------------------------------------------
EXPORT_C TBool UPnPFileUtility::FitsInMemory(
    const CUpnpElement& aResource )
    {
    TBool result( ETrue );
    
    //it will always return ETrue if Leave
    TRAP_IGNORE( result = FitsInMemoryL( aResource ) );
    return result;
    }


// --------------------------------------------------------------------------
// UPnPFileUtility::FitsInMemory
// see UPnPFileUtility.h
// --------------------------------------------------------------------------
TBool UPnPFileUtility::FitsInMemoryL( const CUpnpElement& aResource )
    {
    __LOG( "UPnPFileUtility::FitsInMemoryL" );
    // 1. get object size
    TInt fileSize = -1;
    const CUpnpAttribute* attr =
        UPnPItemUtility::FindAttributeByName( aResource, KAttributeSize );

    //if "file size" not found, straightly go for downloading
    if( !attr )   
        {
        __LOG( "UPnPFileUtility::size not found" );
        return ETrue;
        }

    // 2. convert size to integer
    TLex8 sizeconvert( attr->Value() );
    sizeconvert.Val( fileSize );

    if( fileSize < 0)
        {
        __LOG1( "UPnPFileUtility::file size %d", fileSize );
        return ETrue;
        }
           
    // 3. check the copy target configuration
    CUPnPSettingsEngine* engine = NULL;
    engine = CUPnPSettingsEngine::NewL();
    CleanupStack::PushL( engine );
    
    HBufC* location = HBufC::NewLC( KMaxFileName );
    TPtr locationPtr( location->Des() );
    TBool isPhoneMemory;
    
    engine->GetCopyLocationL( locationPtr, isPhoneMemory );
        
    // 4. check bytes available in the drive
    RFs fileServer;
    CleanupClosePushL( fileServer );
    User::LeaveIfError( fileServer.Connect() );

    TVolumeInfo freeVol;

    User::LeaveIfError( fileServer.SetSessionPath( locationPtr ) );
    User::LeaveIfError( fileServer.Volume( freeVol ) );

    CleanupStack::PopAndDestroy(); // close fileServer;
    CleanupStack::PopAndDestroy(); // delete location
    CleanupStack::PopAndDestroy( engine ); // not needed anymore

    __LOG1( "UPnPFileUtility::free space: %d", freeVol.iFree );

    TInt64 freeSize = 0;

    // 5. result
    // With phone memory a threshold is used. 
    if( isPhoneMemory )
        {
        freeSize = freeVol.iFree - KPhoneMemoryThreshold;
        }
    else
        {
        freeSize = freeVol.iFree;
        }

    if( freeSize >= fileSize )
        {
        __LOG( "UPnPFileUtility::FitsInMemoryL -TRUE" );
        return ETrue;
        }
    else
        {
        __LOG( "UPnPFileUtility::FitsInMemoryL -FALSE" );
        return EFalse;
        }
    }
