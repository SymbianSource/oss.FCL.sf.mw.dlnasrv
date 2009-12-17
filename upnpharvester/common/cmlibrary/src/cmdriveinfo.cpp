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
* Description:      Implementation of drive info class
*
*/





#include <e32std.h>
#include <s32mem.h>
#include "cmdriveinfo.h"

// ================= MEMBER FUNCTIONS =======================
// --------------------------------------------------------------------------
// CCmDriveInfo::NewL
// --------------------------------------------------------------------------
EXPORT_C CCmDriveInfo* CCmDriveInfo::NewL()
    {
    CCmDriveInfo* self = CCmDriveInfo::NewLC();
    CleanupStack::Pop( self );
    return self;    
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::NewLC
// --------------------------------------------------------------------------
EXPORT_C CCmDriveInfo* CCmDriveInfo::NewLC()
    {
    CCmDriveInfo* self = new ( ELeave ) CCmDriveInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;     
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmDriveInfo::~CCmDriveInfo()
    {
    delete iDriveName;          
    }
                        
// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveNumber
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveNumber( const TInt aDriveNumber )
    {
    iDriveNumber = aDriveNumber;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveNumber
// --------------------------------------------------------------------------
EXPORT_C TInt CCmDriveInfo::DriveNumber() const
    {
    return iDriveNumber;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveType
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveType( const TUint aDriveType )
    {
    iDriveType = aDriveType;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveType
// --------------------------------------------------------------------------
EXPORT_C TUint CCmDriveInfo::DriveType() const
    {
    return iDriveType;
    }
        
// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveName
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveNameL( const TDesC& aDriveName )
    {
    delete iDriveName;
    iDriveName = NULL;        

    if( &aDriveName )
        {
        iDriveName = aDriveName.AllocL();
        }
    else
        {
        iDriveName = KNullDesC().AllocL();
        }      
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveName
// --------------------------------------------------------------------------
EXPORT_C TDesC& CCmDriveInfo::DriveName() const
    {
    return *iDriveName;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveSize
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveSize( const TInt64 aDriveSize )
    {
    iSize = aDriveSize;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveSize
// --------------------------------------------------------------------------
EXPORT_C TInt64 CCmDriveInfo::DriveSize() const
    {
    return iSize;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveSize
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveQuota( const TInt64 aDriveQuota )
    {
    iQuota = aDriveQuota;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveSize
// --------------------------------------------------------------------------
EXPORT_C TInt64 CCmDriveInfo::DriveQuota() const
    {
    return iQuota;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::SetUsedDriveSize
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetUsedDriveQuota( const TInt64 aUsedDriveQuota )
    {
    iUsedQuota = aUsedDriveQuota;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::UsedDriveQuota
// --------------------------------------------------------------------------
EXPORT_C TInt64 CCmDriveInfo::UsedDriveQuota() const
    {
    return iUsedQuota;
    }
        
// --------------------------------------------------------------------------
// CCmDriveInfo::SetDriveId
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetDriveId( const TUint aDriveId )
    {
    iUniqueId = aDriveId; 
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::DriveId
// --------------------------------------------------------------------------
EXPORT_C TUint CCmDriveInfo::DriveId() const
    {
    return iUniqueId;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::SetStatus
// --------------------------------------------------------------------------
EXPORT_C void CCmDriveInfo::SetStatus( const TBool aActive )
    {
    iActive = aActive;
    }

// --------------------------------------------------------------------------
// CCmDriveInfo::Status
// --------------------------------------------------------------------
EXPORT_C TBool CCmDriveInfo::Status() const
    {
    return iActive;
    }
// ---------------------------------------------------------------------------
// CCmDriveInfo::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDriveInfo::ExternalizeL( RWriteStream& aStream ) const
    {    
    aStream.WriteInt32L( iDriveNumber );
    aStream.WriteInt32L( iDriveType );    
    if ( iDriveName )
        {
        aStream.WriteInt32L( iDriveName->Length() );    
        aStream << *iDriveName;
        }
    else
        {
        aStream.WriteInt32L( 0 );    
        aStream << KNullDesC();
        }
    
    aStream.WriteUint32L( I64HIGH( iSize ) );
    aStream.WriteUint32L( I64LOW( iSize ) );
    
    aStream.WriteUint32L( I64HIGH( iQuota ) );
    aStream.WriteUint32L( I64LOW( iQuota ) );
    
    aStream.WriteUint32L( I64HIGH( iUsedQuota ) );
    aStream.WriteUint32L( I64LOW( iUsedQuota ) );
    
    aStream.WriteInt32L( iUniqueId );
    aStream.WriteInt32L( (TInt)iActive );                    
    }
        
// ---------------------------------------------------------------------------
// CCmDriveInfo::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmDriveInfo::InternalizeL( RReadStream& aStream )
    {       
    iDriveNumber = aStream.ReadInt32L();
    iDriveType = aStream.ReadInt32L();

    delete iDriveName;
    iDriveName = NULL;

    TInt bufLength = aStream.ReadInt32L();    
    iDriveName = HBufC::NewL( aStream, bufLength );
    
    iSize = MAKE_TINT64( aStream.ReadUint32L(), aStream.ReadUint32L() );
    iQuota = MAKE_TINT64( aStream.ReadUint32L(), aStream.ReadUint32L() );
    iUsedQuota = MAKE_TINT64( aStream.ReadUint32L(), aStream.ReadUint32L() );
    iUniqueId = aStream.ReadInt32L();
    iActive = (TBool)aStream.ReadInt32L();                            
    }
     
// --------------------------------------------------------------------------
// CCmDriveInfo::CCmDriveInfo
// --------------------------------------------------------------------------
CCmDriveInfo::CCmDriveInfo()
    {
    }
                
// --------------------------------------------------------------------------
// CCmDriveInfo::ConstructL
// --------------------------------------------------------------------------
void CCmDriveInfo::ConstructL()
    {
    iDriveName = KNullDesC().AllocL();
    }
    
// End of file
