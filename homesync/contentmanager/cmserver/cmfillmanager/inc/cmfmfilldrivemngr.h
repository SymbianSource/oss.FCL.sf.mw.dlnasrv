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
* Description:  Fill drive manager class
*
*/



#ifndef __CMFMFILLDRIVEMNGR_H
#define __CMFMFILLDRIVEMNGR_H

#include <e32base.h>
#include "cmfmcommon.h"

// Forward declarations
class CCmDriveInfo;

/**
 *  CCmFmFillDriveMngr class
 *  Part of Fill manager. CCmFmFillDriveMngr manages
 *  driveId checking, which is executed before every fill
 *  operation.
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmFillDriveMngr ): public CBase
    {

public:

    /**     
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFs, File server session
     * @param aDrives, dirve array
     * @return  pointer to CCmFmFillDriveMngr class
     */
    static CCmFmFillDriveMngr* NewL( RFs& aFs, 
        RPointerArray<CCmDriveInfo>& aDrives );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFs, File server session
     * @param aDrives, dirve array
     * @return  pointer to CCmFmFillDriveMngr class
     */
    static CCmFmFillDriveMngr* NewLC( RFs& aFs, 
        RPointerArray<CCmDriveInfo>& aDrives );

    /**
     * Destructor.
     */
    virtual ~CCmFmFillDriveMngr();
    
public: 
    
    /**
     * Checking Drive Id
     *
     * @since S60 5.1
     * @param None
     * @return TCmFmStatus status of the drive id check
     */    
    TCmFmStatus CheckDriveId();

    /**
     * Checkking if the Drive has a capasity for saving the file
     *
     * @since S60 5.1
     * @param aFileSize, size of the file
     * @param aDriveId, drive id
     * @param aDriveNumber, drive number ( -1 if no space left )
     * @return index to drive array
     */        
    TInt CheckFreeDiscSpace( TUint32 aFileSize, TUint& aDriveId, 
                             TInt& aDriveNumber );                           
    
private:

    /**
     * Mounts file system
     *
     * @since S60 5.1
     * @param aFileServer, file server session
     * @param aVolInfo, drive info
     * @param aDriveNumber, drive number
     * @return error code
     */  
    TInt DoMountFileSystem( RFs& aFileServer, TVolumeInfo& aVolInfo, 
        TInt aDriveNumber );
    
    /**
     * Selects drive to be used
     *
     * @since S60 5.1
     * @param None
     * @return Drive number
     */    
    TInt SelectDrive();

private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aFs, file server
     * @param aDrives, drive array
     */
    CCmFmFillDriveMngr( RFs& aFs, 
        RPointerArray<CCmDriveInfo>& aDrives );

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private:
    
    /**
     * File server session
     */ 
    RFs iFsSession;
    
    /**
     * Drive info array
     */ 
    RPointerArray<CCmDriveInfo>& iDrives;
    
    };

#endif //  __CMFMFILLDRIVEMNGR_H
