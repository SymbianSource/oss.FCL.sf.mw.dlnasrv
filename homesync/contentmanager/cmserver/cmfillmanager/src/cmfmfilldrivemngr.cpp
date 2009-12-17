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
* Description:  Fill drive handling 
*
*/


#include <f32file.h>
#include "cmcommon.h"
#include "cmdriveinfo.h"
#include "cmfmfilldrivemngr.h"
#include "msdebug.h"    

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::NewL
// ---------------------------------------------------------------------------
// 
CCmFmFillDriveMngr* CCmFmFillDriveMngr::NewL( RFs& aFs, 
    RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::NewL() start"));    
    CCmFmFillDriveMngr* self = CCmFmFillDriveMngr::NewLC( aFs, aDrives );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmFillDriveMngr* CCmFmFillDriveMngr::NewLC( RFs& aFs, 
    RPointerArray<CCmDriveInfo>& aDrives )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::NewLC() start"));    
    CCmFmFillDriveMngr* self = new ( ELeave ) CCmFmFillDriveMngr( aFs, aDrives );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::~CCmFmFillDriveMngr
// ---------------------------------------------------------------------------
// 
CCmFmFillDriveMngr::~CCmFmFillDriveMngr()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::~CCmFmFillDriveMngr()"));
    }

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::CCmFmFillDriveMngr
// ---------------------------------------------------------------------------
// 
CCmFmFillDriveMngr::CCmFmFillDriveMngr( RFs& aFs, 
    RPointerArray<CCmDriveInfo>& aDrives )
    : iFsSession( aFs ), iDrives( aDrives )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::CCmFmFillDriveMngr()"));
    }

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmFillDriveMngr::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::ConstructL()"));    
    }    

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::CheckDriveId
// ---------------------------------------------------------------------------
// 
TCmFmStatus CCmFmFillDriveMngr::CheckDriveId()
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::CheckDriveId()"));
    
    TCmFmStatus status(ECmFmMMCCheckFailed);
    TVolumeInfo volInfo;    
    for( TInt i = iDrives.Count() - 1; i >= 0; i-- )
        {
        DoMountFileSystem( iFsSession, volInfo, iDrives[i]->DriveNumber() );
        if( volInfo.iUniqueID != iDrives[i]->DriveId() )
            {
            delete iDrives[i];
            iDrives.Remove(i);
            }
        }
    if( iDrives.Count() )
        {
        status = ECmFmMMCOk;
        }
    
    return status;       
    }

// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::CheckFreeDiscSpace
// ---------------------------------------------------------------------------
// 
TInt CCmFmFillDriveMngr::CheckFreeDiscSpace( TUint32 aFileSize,
                                             TUint& aDriveId, 
                                             TInt& aDriveNumber )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::CheckFreeDiscSpace()"));    
    
    aDriveNumber = KErrNotFound;
    TInt64 totalSize( 0 );
    TInt64 quota( 0 );
    TInt driveIndex( 0 );
    
    for( TInt i = 0; i < iDrives.Count(); i++ )
        {
        totalSize = iDrives[i]->UsedDriveQuota() + aFileSize;
        quota = iDrives[i]->DriveQuota();
        TRACE(Print(_L("[FILL MNGR]\t CCmFmFillDriveMngr Quota = \
            : %ld"), quota ));         
        if( totalSize >= quota )
            {
            TRACE(Print(_L("[FILL MNGR]\t QUOTA FULL drive number = %d"),
                iDrives[i]->DriveNumber()));
            }
        else
            {
            TInt error( KErrNone );
            TVolumeInfo volInfo;        
            TInt64 freespace( 0 );        
            
            error = DoMountFileSystem( iFsSession, volInfo, 
                iDrives[i]->DriveNumber() );
            
            if ( !error )
                {            
                freespace = volInfo.iFree;
                TRACE(Print(_L("[FILL MNGR]\t CCmFmFillDriveMngr freespace \
                    : %ld"), freespace ));
                if( freespace < ( KCmMmcLowMemory + aFileSize ) )
                    {
                    TRACE(Print(_L("[FILL MNGR]\t Disc full!!")));
                    }
                else
                    {
                    TRACE(Print(_L("[FILL MNGR]\t Drive number of the drive \
                    where to fill = %d"), iDrives[i]->DriveNumber() ));
                    aDriveNumber = iDrives[i]->DriveNumber();
                    aDriveId = iDrives[i]->DriveId();                    
                    driveIndex = i;
                    i = iDrives.Count();
                    }    
                }
            else
                {
                // Disc is not ready => Do not fill
                TRACE(Print(_L("[FILL MNGR]\t Disc is not ready!!")));
                }              
            }                    
        }
        
    return driveIndex;
    }       
    
// ---------------------------------------------------------------------------
// CCmFmFillDriveMngr::DoMountFileSystem
// ---------------------------------------------------------------------------
//
TInt CCmFmFillDriveMngr::DoMountFileSystem( RFs& aFileServer, 
                                            TVolumeInfo& aVolInfo, 
                                            TInt aDriveNumber )
    {
    LOG(_L("[FILL MNGR]\t CCmFmFillDriveMngr::CheckFreeDiscSpace()")); 
    
    TInt error( KErrNone );
    TFullName fsname;  
    error = aFileServer.FileSystemName( fsname, aDriveNumber );
    
    TRACE(Print(_L("[FILL MNGR]\t FileSystemName %d, %S"), 
        error, &fsname ));                    

    error = aFileServer.FileSystemName( fsname, aDriveNumber );
    
    TRACE(Print(_L("[FILL MNGR]\t FileSystemName %d, %S"), 
        error, &fsname ));

    error = aFileServer.Volume( aVolInfo, aDriveNumber );       

    TRACE(Print(_L("[FILL MNGR]\t CCmFmFillDriveMngr::volinfo error = %d"), 
        error )); 

    return error;
    }
        
// End of file


