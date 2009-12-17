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
* Description:      Definition of drive info class
*
*/






#ifndef C_CCMDRIVEINFO_H
#define C_CCMDRIVEINFO_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  Drive info class
 *
 *  Provides info about MCs drives
 *
 *  @lib cmcommon.lib
 *  @since S60 3.1
 */
 class CCmDriveInfo : public CBase
    {
    
    public:
    
        /**
         * Creates new CCmDriveInfo class.
         * @param None
         * @return  pointer to CCmDriveInfo class
         */    
        IMPORT_C static CCmDriveInfo* NewL();
       
        /**
         * Creates new CCmDriveInfo class.
         * @param None
         * @return  pointer to CCmDriveInfo class
         */        
        IMPORT_C static CCmDriveInfo* NewLC();
        
        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCmDriveInfo();
            
    public:
        /**
         * Sets drive number
         * @since S60 3.1
         * @param aDriveNumber, drive number
         */
        IMPORT_C void SetDriveNumber( const TInt aDriveNumber );

        /**
         * Gets drive number
         * @since S60 3.1
         * @return TInt, drive number
         */
        IMPORT_C TInt DriveNumber() const;
        
        /**
         * Sets drive type
         * @since S60 3.1
         * @param aDriveType, drive type
         */
        IMPORT_C void SetDriveType( const TUint aDriveType );

        /**
         * Gets drive type
         * @since S60 3.1
         * @return TUint, drive type
         */
        IMPORT_C TUint DriveType() const;
        
        /**
         * Sets drive name
         * @since S60 3.1
         * @param aDriveNumber, drive name
         */
        IMPORT_C void SetDriveNameL( const TDesC& aDriveName ); 

        /**
         * Gets drive name
         * @since S60 3.1
         * @return TDesC*, drive name
         */
        IMPORT_C TDesC& DriveName() const;
                
        /**
         * Sets drive size
         * @since S60 3.1
         * @param aDriveSize, drive size
         */
        IMPORT_C void SetDriveSize( const TInt64 aDriveSize );

        /**
         * Gets drive size
         * @since S60 3.1
         * @return TInt64, drive size
         */
        IMPORT_C TInt64 DriveSize() const;

        /**
         * Sets drive quota
         * @since S60 3.1
         * @param aDriveQuota, drive quota
         */        
        IMPORT_C void SetDriveQuota( const TInt64 aDriveQuota );

        /**
         * Gets drive quota
         * @since S60 3.1
         * @return drive quota
         */        
        IMPORT_C TInt64 DriveQuota() const;
 
        /**
         * Sets used drive quota
         * @since S60 3.1
         * @param aUsedDriveQuota, used drive quota
         */        
        IMPORT_C void SetUsedDriveQuota( const TInt64 aUsedDriveQuota );

        /**
         * Gets used drive quota
         * @since S60 3.1
         * @return used drive quota
         */        
        IMPORT_C TInt64 UsedDriveQuota() const;
                
        /**
         * Sets drive id
         * @since S60 3.1
         * @param aDriveId, drive id
         */
        IMPORT_C void SetDriveId( const TUint aDriveId );

        /**
         * Gets drive Id
         * @since S60 3.1
         * @return TInt, drive id
         */
        IMPORT_C TUint DriveId() const;

        /**
         * Sets drive status
         * @since S60 3.1
         * @param aActive, ETrue if drive selected
         * @return None
         */        
        IMPORT_C void SetStatus( const TBool aActive );
        
        /**
         * Gets drive status
         * @since S60 3.1
         * @return ETrue if drive selected
         */        
        IMPORT_C TBool Status() const;
        
        /**
        * Externalizes container information to stream.
        * Leaves in case of errors.
        * @since Series 60 3.1
        * @param reference to RWriteStream
        * @return none
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalizes container information from stream.
        * Leaves in case of errors.
        * @since Series 60 3.1
        * @param reference to RReadStream
        * @return none
        */
        IMPORT_C void InternalizeL( RReadStream& aStream );        
        
    private:
    
        /**
         * CCmDriveInfo.
         */        
        CCmDriveInfo();
            
        /**
         * ConstructL.
         */
        void ConstructL();
            
    private:
    
        /**
         * drive number
         */
        TInt                    iDriveNumber;
        
        /**
         * drive type
         */
        TUint                   iDriveType;
        
        /**
         * drive name
         * owned
         */
        HBufC*                  iDriveName; 
        
        /**
         * used drive space
         */
        TInt64                  iSize;

        /**
         * drive capasity
         */        
        TInt64                  iQuota;
        
        /**
         * used drive capasity
         */        
        TInt64                  iUsedQuota;        
        
        /**
         * drive id
         */
        TUint                   iUniqueId;
        
        /**
         * drive active
         */
         TBool                  iActive;
                 
    };
 
#endif // C_CCMDRIVEINFO_H