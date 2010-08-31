/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      UPnP settings engine class implementation
*
*/







//  Include Files

#include <centralrepository.h>
#include <s32file.h>
#include <upnpstring.h>
#include <pathinfo.h> //PathInfo
#include <upnpmediaserversettings.h>

#include <cmmanager.h>   // RCmManager
#include <cmconnectionmethoddef.h>
#include <commdb.h> // CCommsDatabase
#include <cdbcols.h>
#include <WlanCdbCols.h>  // WLAN_SERVICE
#include <wlanmgmtcommon.h>

#include "upnpsettingsengine.h"
#include "upnpsettingsengine.hrh"
#include "upnpapplicationcrkeys.h"

#include "upnpsettings.h"

_LIT( KComponentLogfile, "upnpsettingsengine.txt");
#include "upnplog.h"

// CONSTANTS
_LIT8( KManufacturerName, "Nokia");
_LIT( KDummyIAP, "DummyIAP");

// upload directory
_LIT( KUploadDirectoryCDrive, "\\data\\Download\\Media\\" );
_LIT( KUploadDirectory, "\\Download\\Media\\" );
_LIT( KModelFile, "\\resource\\versions\\model.txt" );
_LIT8( KModel, "Nokia X" );
_LIT8( KManufacturerUrl, "http://www.nokia.com");
_LIT8( KModelUrl, "http://www.nseries.com");
const TInt KMaxNameLength = 128;
const TUint KUidWlanBearerType = 0x10281BB1;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::CUPnPSettingsEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPSettingsEngine::CUPnPSettingsEngine()
    {
    __LOG("CUPnPSettingsEngine::CUPnPSettingsEngine()");
    }


// --------------------------------------------------------------------------
// CUPnPSettingsEngine::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPSettingsEngine* CUPnPSettingsEngine::NewLC()
    {
    __LOG("CUPnPSettingsEngine::NewLC()");

    CUPnPSettingsEngine* self = new (ELeave) CUPnPSettingsEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPSettingsEngine* CUPnPSettingsEngine::NewL()
    {
    __LOG("CUPnPSettingsEngine::NewL()");

    CUPnPSettingsEngine* self = CUPnPSettingsEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPSettingsEngine::ConstructL()
    {
    iRepository = CRepository::NewL( KCRUidUPnPApplication );
    iUpnpRepository = CUpnpSettings::NewL(KCRUidUPnPStack);

    //set model, ManufacturerUrl, and ModelUrl into UpnpMediaServerSettings
    SetManufacturerParamsL();
    }


// Destructor
EXPORT_C CUPnPSettingsEngine::~CUPnPSettingsEngine()
    {
    __LOG("CUPnPSettingsEngine::~CUPnPSettingsEngine()");

    delete iRepository;
    iRepository = NULL;
    delete iUpnpRepository;
    iUpnpRepository = NULL;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetLocalFriendlyName
// Sets friendly name to Central Repository
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetLocalFriendlyName(
    const TDesC8& aFriendlyName ) const
    {
    __LOG("CUPnPSettingsEngine::SetLocalFriendlyName()");

    TBuf8<KMaxNameLength> name;
    name.Copy(aFriendlyName);
    TInt err( KErrNone );
    TRAP_IGNORE(
        CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
        CleanupStack::PushL(settings);
        err = settings->SetL( UpnpMediaServerSettings::EFriendlyName, name);
        CleanupStack::PopAndDestroy(settings);
        );
    return err;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetLocalFriendlyName
// Gets friendly name from Central Repository.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::GetLocalFriendlyName(
    TDes8& aFriendlyName ) const
    {
    __LOG("CUPnPSettingsEngine::GetLocalFriendlyName()");
    TInt err( KErrNone );
    TRAP(err, err = GetLocalFriendlyNameL( aFriendlyName ) );
    return err;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetLocalFriendlyNameL
// Gets friendly name from Central Repository.
// --------------------------------------------------------------------------
//
TInt CUPnPSettingsEngine::GetLocalFriendlyNameL(
    TDes8& aFriendlyName ) const
    {
    __LOG("CUPnPSettingsEngine::GetLocalFriendlyNameL()");
    
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );
    HBufC8 *tmp = settings->GetL( UpnpMediaServerSettings::EFriendlyName );
    
    //check if there is any name for the device
    if ( !tmp->Length() )
        {
        //no name available, try to get the device model name as the 
        //device name
        
        // Read device model
        delete tmp; tmp = NULL;
        
        //string ownership transferred
        tmp = settings->GetL( UpnpMediaServerSettings::
                              EModelName );
                              
        if ( tmp->Length() )
            {
            //if device model name is available
            aFriendlyName.Copy( *tmp );
            }
        else
            {
            // final try if reading model name fails, use manufacturer name
            // as device name
            aFriendlyName.Copy( KManufacturerName );
            }
        }
    else
        {
        aFriendlyName.Copy( *tmp );
        }
    delete tmp; tmp = NULL;
    CleanupStack::PopAndDestroy( settings );
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetAccessPoint
// Sets IAP to Central Repository
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetAccessPoint( const TInt aIAP )
    {
    TInt value = aIAP; // remove constness
    return iUpnpRepository->Set(CUpnpSettings::KUPnPStackIapId, value );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetAccessPoint
// Gets IAP from Central Repository.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::GetAccessPoint( TInt& aIAP )
    {
    return iUpnpRepository->Get(CUpnpSettings::KUPnPStackIapId, aIAP );
    }


// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetAccessPointSetting
// Sets IAP setting to Central Repository
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetAccessPointSetting(
    const TInt aIAPSetting )
    {
    return iRepository->Set( KUPnPAppAccessPointSetting, aIAPSetting );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetAccessPointSetting
// Gets IAP setting from Central Repository.
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::GetAccessPointSetting( TInt& aIAPSetting )
    {
    return iRepository->Get( KUPnPAppAccessPointSetting, aIAPSetting );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetWapId
// Sets iap wap id to Central Repository
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetWapId( const TInt aWapId )
    {
    return iRepository->Set( KUPnPAppWapId, aWapId );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetWapId
// Gets iap wap id from Central Repository
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::GetWapId( TInt& aWapId )
    {
    return iRepository->Get( KUPnPAppWapId, aWapId );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetFirstStart
// Gets information if this is the first start of home network
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::GetFirstStart( TInt& aFirstStart )
    {
    TInt err ( iRepository->Get( KUPnPAppFirstStart, aFirstStart ));
    if ( aFirstStart )
        {
        TRAP_IGNORE(
        RFs fs;
        err = fs.Connect();
        CleanupClosePushL( fs );
        TVolumeInfo volumeInfo;
        TDriveUnit driveUnit;
        TInt err = fs.Volume(volumeInfo, EDriveE);
        if( err == KErrNone && volumeInfo.iFree > 0)
            {
            driveUnit = TDriveUnit( PathInfo::MemoryCardRootPath() );
            }
        else
            {
            driveUnit = TDriveUnit( PathInfo::PhoneMemoryRootPath() );
            }

        CleanupStack::PopAndDestroy( &fs );
        SetCopyLocationL( (TDriveNumber)((TInt)driveUnit) ));
        }
    return err;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetFirstStart
// Sets information if this is the first start of home network
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetFirstStart( const TInt aFirstStart )
    {
    return iRepository->Set( KUPnPAppFirstStart, aFirstStart );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetManufacturerParamsL
// Sets the mandatory manufacturer settings
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetManufacturerParamsL() const
    {
    __LOG("CUPnPSettingsEngine::SetManufacturerParamsL()");
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );

    // Manufacturer
    InitializePropertyL( *settings, 
                         UpnpMediaServerSettings::EManufacturer, 
                         KManufacturerName );

    // Model name
    HBufC8* model = HBufC8::NewLC( KMaxFileName );
    TPtr8 modelPtr( model->Des() );
    TInt err = ReadDeviceModelL( modelPtr );
    if( err )
        {
        model->Des().Copy( KModel );
        }
    InitializePropertyL( *settings, 
                         UpnpMediaServerSettings::EModelName, 
                         *model );
    // Friendly name
    InitializePropertyL( *settings, 
                         UpnpMediaServerSettings::EFriendlyName, 
                         *model );
    CleanupStack::PopAndDestroy( model );

    // Manufacturer url
    InitializePropertyL( *settings, 
                         UpnpMediaServerSettings::EManufacturerUrl, 
                         KManufacturerUrl );

    // Model url
    InitializePropertyL( *settings, 
                         UpnpMediaServerSettings::EModelUrl, 
                         KModelUrl );

    CleanupStack::PopAndDestroy( settings );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::ReadDeviceModelL
// Get device model
// --------------------------------------------------------------------------
//
TInt CUPnPSettingsEngine::ReadDeviceModelL( TDes8& aModel ) const
    {
    __LOG("CUPnPSettingsEngine::ReadDeviceModel()");
    RFs fs;
    RFile file;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    HBufC*path = HBufC::NewLC( KMaxFileName );
    TParse parse;
    parse.Set( PathInfo::RomRootPath(), NULL, NULL );
    TPtrC romDrive = parse.Drive();
    
    path->Des().Append( romDrive );
    path->Des().Append( KModelFile() );
    
    TInt error ( file.Open(fs, *path, EFileRead ));
    if ( !error )
        {
        CleanupClosePushL( file );

        TFileText txtFile;
        txtFile.Set( file );
        HBufC* readBuf = HBufC::NewLC( KMaxFileName );
        txtFile.Seek( ESeekStart );
        TPtr bufPtr( readBuf->Des() );
        error = txtFile.Read( bufPtr ); // read one line
        if ( !error )
            {
            HBufC8* bufbuf = UpnpString::FromUnicodeL( *readBuf );
            CleanupStack::PushL( bufbuf );

            aModel = *bufbuf;
            CleanupStack::PopAndDestroy( bufbuf );
            }
        CleanupStack::PopAndDestroy( readBuf );
        CleanupStack::PopAndDestroy( &file ); // Close file
        }
    CleanupStack::PopAndDestroy( path );    
    CleanupStack::PopAndDestroy( &fs );

    return error;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetCopyLocationDriveL
// Gets the drive of the location for copied files
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPSettingsEngine::GetCopyLocationDriveL( 
    TDriveNumber& aDrive )
    const
    {
    __LOG("CUPnPSettingsEngine::GetCopyLocationDriveL()");

    HBufC* location = HBufC::NewL(KMaxFileName);
    CleanupStack::PushL( location );
    TBool isPhoneMem( EFalse );
    TPtr locationPtr( location->Des() );
    GetCopyLocationL( locationPtr, isPhoneMem );

    TDriveUnit driveUnit = TDriveUnit( locationPtr );

    aDrive = (TDriveNumber)((TInt)driveUnit);

    CleanupStack::PopAndDestroy( location );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetCopyLocationL
// Gets the location for copied files
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPSettingsEngine::GetCopyLocationL( TDes& aLocation,
                                                    TBool& aIsPhoneMemory )
    const
    {
    __LOG("CUPnPSettingsEngine::GetCopyLocationL()");

    // Get instance of Server Settings object
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );

    // Get the location setting from ServerSettings
    settings->Get( UpnpMediaServerSettings::EUploadDirectory, aLocation );

    // Check to see if drive is ready.
    TVolumeInfo volInfo;
    TDriveUnit driveUnit = TDriveUnit( aLocation );
    RFs fileServer;

    User::LeaveIfError( fileServer.Connect() );
    CleanupClosePushL( fileServer );
    TInt error = fileServer.Volume( volInfo, driveUnit );
    CleanupStack::PopAndDestroy( &fileServer );

    // check if memory is present
    // Revert to phone memory if location is not available
    // If the location is full no need to change the location. User will be
    // informed that memory is full and the location should be changed
    if( error == KErrNone )
        {
        TDriveUnit driveUnit = TDriveUnit( aLocation );    
        if( driveUnit == EDriveC )
            {
            aIsPhoneMemory = ETrue;
            }
        else
            {
            aIsPhoneMemory = EFalse;
            }
        }
    else
        {
        SetCopyLocationL( EDriveC );
        aIsPhoneMemory = ETrue;
        }

    CleanupStack::PopAndDestroy( settings );
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::SetCopyLocationL
// Sets the location for copied files
// --------------------------------------------------------------------------
//
EXPORT_C TInt CUPnPSettingsEngine::SetCopyLocationL( 
    const TDriveNumber aDrive )
    const
    {
    __LOG("CUPnPSettingsEngine::SetCopyLocationL()");
    TInt err( KErrNone );

    HBufC* path = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( path );

    CUpnpMediaServerSettings* settings =
        CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );

    TDriveUnit driveUnit = TDriveUnit( aDrive );
    path->Des().Append( driveUnit.Name() );
    if( aDrive == EDriveC )
        {
        path->Des().Append( KUploadDirectoryCDrive );
        }
    else
        {
        path->Des().Append( KUploadDirectory );
        }

    err = settings->Set( UpnpMediaServerSettings::EUploadDirectory, *path );

    CleanupStack::PopAndDestroy( settings );
    CleanupStack::PopAndDestroy( path );

    return err;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::GetCurrentIapNameL
// Get iap name
// --------------------------------------------------------------------------
//
EXPORT_C HBufC* CUPnPSettingsEngine::GetCurrentIapNameL( TInt aIapId )
    {
    __LOG("CUPnPSettingsEngine::GetCurrentIapNameL() begin");
    __LOG("read iap");
    
    HBufC* name = NULL;
    if( aIapId > EUPnPSettingsEngineIAPIdAlwaysAsk )
        {
        name = HBufC::NewLC( KMaxFileName );
        TPtr namePtr( name->Des() );
        CCommsDatabase* db = CCommsDatabase::NewL();
        CleanupStack::PushL( db );
        CCommsDbTableView* iapView = db->OpenViewMatchingUintLC(
            TPtrC(IAP),
            TPtrC(COMMDB_ID),
            aIapId );
        if( iapView )
            {
            TInt err = KErrNotFound;
            err = iapView->GotoFirstRecord();
            if ( !err )
                {
                iapView->ReadTextL(TPtrC(COMMDB_NAME), namePtr );
                }
            CleanupStack::PopAndDestroy( iapView );
            }
        CleanupStack::PopAndDestroy( db );
        CleanupStack::Pop( name );
        }
    
    if( !name )
        {
        User::LeaveIfError( KErrNotFound );
        }
    
    __LOG("CUPnPSettingsEngine::GetCurrentIapNameL() end");
    return name;
    }

//---------------------------------------------------------------------------
// CUPnPSettingsEngine::IsAccessPointValidL
// Checks if the given access point is valid.
//---------------------------------------------------------------------------
//
EXPORT_C TBool CUPnPSettingsEngine::IsAccessPointValidL( TInt aIapId )
    {
    if ( aIapId == EUPnPSettingsEngineIAPIdAlwaysAsk )
        {
        // IAP is always ask. Returns true automatically.
        return ETrue;
        }
    else if ( aIapId > EUPnPSettingsEngineIAPIdAlwaysAsk )
        {
        // Verifies that IAP exists in the comms db and it is valid.
        return IsWLANAccessPointValidL( aIapId );
        }
    else
        {
        // IAP is NONE or some bogus value. Not valid.
        return EFalse;
        }
    }

// -------------------------------------------------------------
// CUPnPSettingsEngine::GetWLANAccessPointsL
// Get wlan access points from commsdb
// -------------------------------------------------------------
//
EXPORT_C void CUPnPSettingsEngine::GetWLANAccessPointsL(
                                    CDesCArray* aNameArray,
                                    RArray<TInt64>& aIapIdArr )
    {
    __LOG("CUPnPSettingsEngine::GetWLANAccessPointsL() begin");
#ifdef __arm
    TUint32 bearerType;
#endif

    RCmManager cmm;
    cmm.OpenL();
    CleanupClosePushL ( cmm );

    RArray<TUint32> iapIdArr;    
    CleanupClosePushL ( iapIdArr );

    //get all the connections, exclude easy wlan
    cmm.ConnectionMethodL( iapIdArr, EFalse, EFalse, EFalse );

    __LOG1(" total %d", iapIdArr.Count() );

    //Get the iap names
    for(TInt i = 0; i < iapIdArr.Count(); i++ )
        {
#ifdef __arm

        bearerType = cmm.GetConnectionMethodInfoIntL(
                               iapIdArr[i], CMManager::ECmBearerType );

        __LOG1(" bearerType %d", bearerType);

        if( bearerType == KUidWlanBearerType )
            {
#endif
            __LOG1(" i: %d", i);
            aIapIdArr.AppendL( iapIdArr[i] );

            if( aNameArray )
                {
                HBufC* name = NULL;
                __LOG(" name ");

                // This can leave in WINSCW env, so catch and handle the
                // exception
                TRAPD( getConnectionMethodErr,
                       name = cmm.GetConnectionMethodInfoStringL(
                                   iapIdArr[i], CMManager::ECmName );
                );

                if( getConnectionMethodErr == KErrNone )
                    {
                    __LOG16( *name );
                    CleanupStack::PushL( name );
                    __LOG(" append name ");
                    aNameArray->AppendL( *name ); //ownership not transferred
                    CleanupStack::PopAndDestroy( name );
                    }
                else
                    {
                    // If failed to get the connection method info string,
                    // use a dummy IAP name
                    __LOG( " Failed to get IAP name! Using a dummy name. ");
                    aNameArray->AppendL( KDummyIAP );
                    }
                }
#ifdef __arm
            }
#endif

        }

    CleanupStack::PopAndDestroy( &iapIdArr );
    CleanupStack::PopAndDestroy( &cmm );

    __LOG1(" aIapIdArr count %d", aIapIdArr.Count() );  
    __LOG("CUPnPSettingsEngine::GetWLANAccessPointsL() end");

    }


// --------------------------------------------------------------------------
// CUPnPSettingsEngine::CreateAccessPointL
// Create AP modifying window
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPSettingsEngine::CreateAccessPointL()
    {
    }
    
//---------------------------------------------------------------------------
// CUPnPSettingsEngine::IsWLANAccessPointValidL
// Checks if the given access point is valid.
//---------------------------------------------------------------------------
//
TBool CUPnPSettingsEngine::IsWLANAccessPointValidL( TUint32 aIapId )
    {
    
    TBool ret = EFalse;
    
    RArray<TInt64> newArray;
    CleanupClosePushL( newArray );
    
    GetWLANAccessPointsL( NULL, newArray );
    
    for(TInt i = 0; i < newArray.Count(); i++ )
        {
        if( aIapId == newArray[i] )
            {
            ret = ETrue;
            i = newArray.Count(); //quit the loop
            }
        }
    CleanupStack::PopAndDestroy( &newArray ); 
    
    return ret;
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngine::InitializePropertyL
// Initializes a property if property has not yet been set
// --------------------------------------------------------------------------
//
void CUPnPSettingsEngine::InitializePropertyL( 
    CUpnpMediaServerSettings& aSettings, 
    TInt aKey, 
    const TDesC8& aValue ) const
    {
    HBufC8* currentValue( NULL );
    TRAPD( err, currentValue = aSettings.GetL( aKey ) );
    if( !err )
        {
        TInt currentLength = currentValue->Length();
        delete currentValue;
        currentValue = NULL;
        if( !currentLength )
            {
            User::LeaveIfError( aSettings.SetL( aKey, aValue ) );
            }
        }
    else if( err == KErrNotFound )
        {
        User::LeaveIfError( aSettings.SetL( aKey, aValue ) );
        }
    else
        {
        User::Leave( err );
        }
    }
    
// End of file
