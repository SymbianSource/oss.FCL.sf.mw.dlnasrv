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
* Description:      CM Settings Engine implementation
*
*/






#include <e32base.h>
#include <e32const.h>
#include <bautils.h>
#include <f32file.h>
#include <centralrepository.h>
#ifdef __SERIES60_31__
#include <upnpnetworkaccess.h>
#else
#include "upnpsettingsengine.h"
#endif
#include "cmdriveinfo.h"
#include <driveinfo.h>

#include "msdebug.h"
#include "cmsettingsengine.h"
#include "contentmanagercrkeys.h"


// Constants

const TInt KTimeLength = 64;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CCmSettingsEngine::CCmSettingsEngine()
    {
    }


// ---------------------------------------------------------------------------
// Two-phase constructor
// ---------------------------------------------------------------------------
//
void CCmSettingsEngine::ConstructL()
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::ConstructL"));

    iRepository = CRepository::NewL( KCRUidCmContentManager );
    }


// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
//
CCmSettingsEngine* CCmSettingsEngine::NewL()
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::NewL"));

    CCmSettingsEngine* self = CCmSettingsEngine::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phase API constructor
// ---------------------------------------------------------------------------
//
CCmSettingsEngine* CCmSettingsEngine::NewLC()
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::NewLC"));

    CCmSettingsEngine* self = new( ELeave ) CCmSettingsEngine;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmSettingsEngine::~CCmSettingsEngine()
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::~CCmSettingsEngine"));

    delete iRepository;
    }


// ---------------------------------------------------------------------------
// Gets Content Manager service's state
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetServiceState(
    const TCmService aService,
    TCmServiceState& aState ) const
    {
    TInt error( KErrNone );
    TInt state( 0 );

    switch ( aService )
        {
        case ECmServiceContentManager:
            {
            error = iRepository->Get( KCmContentManagerState, state ) ;
            break;
            }
        case ECmServiceHarvest:
            {
            error = iRepository->Get( KCmHarvesterState, state ) ;
            break;
            }
        case ECmServiceFill:
            {
            error = iRepository->Get( KCmFillState, state ) ;
            break;
            }
        case ECmServiceStore:
            {
            error = iRepository->Get( KCmStoreState, state ) ;
            break;
            }
        case ECmServiceMemoryManager:
            {
            error = iRepository->Get( KCmMemoryManagerState, state ) ;
            break;
            }
        default:
            {
            LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetServiceState \
            service not found"));
            error = KErrNotFound;
            break;
            }
        }

    if ( !error )
        {
        TRACE(Print(_L("[CmSettinsEngine]\t CCmSettingsEngine::\
        GetServiceState state = %d"), state ));
        // converting the value back
        aState = (TCmServiceState) state;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// Set Content Manager service state
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetServiceState(
                                const TCmService aService,
                                const TCmServiceState aState )
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::SetServiceState"));

    TInt error( KErrNone );

    switch ( aService )
        {
        case ECmServiceContentManager:
            {
            error = iRepository->Set( KCmContentManagerState, (TInt)aState ) ;
            break;
            }
        case ECmServiceHarvest:
            {
            error = iRepository->Set( KCmHarvesterState, (TInt)aState ) ;
            break;
            }
        case ECmServiceFill:
            {
            error = iRepository->Set( KCmFillState, (TInt)aState ) ;
            break;
            }
        case ECmServiceStore:
            {
            error = iRepository->Set( KCmStoreState, (TInt)aState ) ;
            break;
            }
        case ECmServiceMemoryManager:
            {
            error = iRepository->Set( KCmMemoryManagerState, (TInt)aState ) ;
            break;
            }
        default:
            {
            error = KErrNotFound;
            break;
            }
        }

    return error;
    }

// ---------------------------------------------------------------------------
// Gets IAP information from Home Network's cenrep key
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetIapL( TInt& aIap ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetIapL"));

#ifdef __SERIES60_31__
    CUpnpNetworkAccess* access = CUpnpNetworkAccess::NewL( KCRUidUPnPStack );
    TInt error( access->Get( CUpnpNetworkAccess::KUPnPStackIapId, aIap ) );
    delete access;
#else
    CUPnPSettingsEngine* settings = CUPnPSettingsEngine::NewLC();
    TInt error( settings->GetAccessPoint( aIap ) );
    CleanupStack::PopAndDestroy( settings );
#endif
    return error;
    }

// ---------------------------------------------------------------------------
// Gets next start time information for specified service
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetNextRuntime( const TCmService aService,
                                                 TTime& aRuntime ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetNextRuntime"));

    TBuf<KTimeLength> time;
    TPtrC timePtr( time );
    TInt error( KErrNone );

    if ( aService == ECmServiceStore )
        {
        error = iRepository->Get( KCmNextStoreTime, time );
        }
    else // ECmServiceFill
        {
        error = iRepository->Get( KCmNextFillTime, time );
        }

    if ( !error )
        {
        // convert string to TInt64
        TInt64 timeInt;
        TLex lex( timePtr );
        lex.Val( timeInt );
        aRuntime = timeInt;
        }

    return error;
    }


// ---------------------------------------------------------------------------
// Sets next start time information for specified service
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetNextRuntime( const TCmService aService,
                                                 const TTime aNextTime )
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::SetNextRuntime"));

    TInt error( KErrNone );
    TBuf<KTimeLength> nextRunTime;
    nextRunTime.AppendNum( aNextTime.Int64() );

    if (aService == ECmServiceStore)
        {
        error = iRepository->Set( KCmNextStoreTime, nextRunTime );
        }
    else // ECmServiceFill
        {
        error = iRepository->Set( KCmNextFillTime, nextRunTime ) ;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// Gets discovery delay.
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetDiscoveryDelay( TInt& aDelay ) const
    {
    TInt error( KErrNone );
    
    error = iRepository->Get( KCmDiscoveryDelay, aDelay );
    TRACE(Print(_L("[CmSettinsEngine]\t CCmSettingsEngine::\
    GetDiscoveryDelay delay = %d"), aDelay));
    
    return error;
    }


// ---------------------------------------------------------------------------
// Sets discovery delay
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetDiscoveryDelay( const TInt aDelay )
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::SetDiscoveryDelay"));

    return iRepository->Set( KCmDiscoveryDelay, aDelay );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetAutoSync( TBool& aAutoSync ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetAutoSync"));

    TInt error( KErrNotFound );

    TInt autoSyncInt;
    error = iRepository->Get( KCmAutoSync, autoSyncInt );

    aAutoSync = (TBool) autoSyncInt;
    return error;
    }

// ---------------------------------------------------------------------------
// Sets synchronization state
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetAutoSync( const TBool aAutoSync )
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::SetAutoSync"));

    return iRepository->Set( KCmAutoSync, (TInt)aAutoSync ) ;
    }

// ---------------------------------------------------------------------------
// Gets memory manager's status
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetMemoryManagerStatus(
    TBool& aMmStatus ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetMemoryManagerStatus"));

    TInt mmInt;

    TInt error = iRepository->Get( KCmMemoryManagerState, mmInt );

    if ( !error )
        {
        aMmStatus = (TBool) mmInt;
        }

    return error;
    }

// ---------------------------------------------------------------------------
// Sets memory manager's status
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetMemoryManagerStatus(
    const TBool aMmStatus )
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::SetMemoryManagerStatus"));

    return iRepository->Set( KCmMemoryManagerState, (TInt)aMmStatus ) ;
    }

// ---------------------------------------------------------------------------
// Gets item count for search action
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetSearchCount( TInt& aItemCount ) const
    {
    TInt error( KErrNone );
    error = iRepository->Get( KCmSearchCount, aItemCount );
    TRACE(Print(_L("[CmSettinsEngine]\t CCmSettingsEngine::\
    GetSearchCount count = %d"), aItemCount));
    
    return error;
    }

// ---------------------------------------------------------------------------
// Gets item count for database add operation
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetAddCount( TInt& aAddCount ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::GetAddCount"));

    return iRepository->Get( KCmAddCount, aAddCount );
    }


// ---------------------------------------------------------------------------
// Gets app wizard execution status info.
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::GetAppWizardInformation(
    TInt& aAppWizardState ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::\
    GetAppWizardInformation"));

    return iRepository->Get( KCmAppWizardState, aAppWizardState );
    }

// ---------------------------------------------------------------------------
// Sets app wizard execution status info.
// ---------------------------------------------------------------------------
//
TInt CCmSettingsEngine::SetAppWizardInformation()
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::\
    SetAppWizardInformation"));


    // application wizard run -> set status 1
    return iRepository->Set( KCmAppWizardState, 1 ) ;
    }


// ---------------------------------------------------------------------------
// Gets information about MCs drives
// ---------------------------------------------------------------------------
//
void CCmSettingsEngine::DriveListL(
    RFs& aFs,
    RPointerArray<CCmDriveInfo>& aDrives ) const
    {
    LOG(_L("[CmSettinsEngine]\t CCmSettingsEngine::DriveListL"));


    TDriveList list;
    TDriveInfo info;

    aFs.DriveList( list );
    
    TInt drive( KErrNotFound );                                                         
    TInt error = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage,
                                             drive );
    TRACE( Print(_L("[CmSettinsEngine]\t GetDefaultDrive error = %d, drive = %d"),
    error, drive ) );

    
    for( TInt driveNumber = 0 ; driveNumber <= EDriveZ ; driveNumber++ )
        {
        if( list[driveNumber] ) // is drive available
            {
            TUint drvStatus( 0 );
            aFs.Drive( info, driveNumber );
            
            TRACE( Print(_L("[CmSettinsEngine]\t drivenumber = %d"),
                      driveNumber ) );
            User::LeaveIfError(DriveInfo::GetDriveStatus( aFs,
                                                          driveNumber,
                                                          drvStatus ) );                                                                      
                                                          
            TRACE( Print(_L("[CmSettinsEngine]\t removable drive = %d"),
                      drvStatus & DriveInfo::EDriveRemovable ) );

            TInt removableStatus = drvStatus & DriveInfo::EDriveRemovable;
            // Check if drive is internal hard disc or memory card
            if( 
                ((!error && driveNumber == drive ) && !removableStatus ) ||
                ( removableStatus && ( info.iType != EMediaNotPresent ) ) )
                {

                TRACE( Print(_L("[CmSettinsEngine]\t info.iType = %d"),
                       info.iType ) );
                TRACE( Print(_L("[CmSettinsEngine]\t info.iDriveAtt = %d"),
                       info.iDriveAtt ) );                       

                HBufC* memoryCardName = NULL;
                CCmDriveInfo* driveInfo = CCmDriveInfo::NewLC();
                TVolumeInfo volInfo;
                aFs.Volume( volInfo, driveNumber );

                TRACE( Print(_L("[CmSettinsEngine]\t volInfo.iName = %S"),
                       &volInfo.iName ) );

                if ( volInfo.iName.Compare( KNullDesC() ) )
                    {
                    memoryCardName = volInfo.iName.AllocLC();
                    driveInfo->SetDriveNameL( *memoryCardName );
                    }
                else
                    {
                    driveInfo->SetDriveNameL( KNullDesC() );
                    }
                // collect drive information
                driveInfo->SetDriveNumber( driveNumber );
                driveInfo->SetDriveType( removableStatus );
                driveInfo->SetDriveSize( volInfo.iSize );
                driveInfo->SetDriveId( volInfo.iUniqueID );

                aDrives.AppendL( driveInfo );

                if ( memoryCardName )
                    {
                    CleanupStack::PopAndDestroy( memoryCardName );
                    }
                CleanupStack::Pop( driveInfo );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Deletes the object
// ---------------------------------------------------------------------------
//
void CCmSettingsEngine::Close()
    {
    delete this;
	}
	
// End of file
