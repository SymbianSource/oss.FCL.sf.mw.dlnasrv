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
* Description:      CM Settings class
*
*/







#ifndef M_CCMSETTINGS_H
#define M_CCMSETTINGS_H

// INCLUDES
#include <e32base.h>
#include "cmcommon.h"

// FORWARD DECLARATIONS
class CRepository;
class CCmDriveInfo;
class RFs;

/**
 *  Defines the cmsettings interface
 *
 *  This class defines the cmsettings interface. Provides methods to
 *  execute fill specific services
 *
 *  @lib cmsettings.lib
 *  @since S60 v3.2
 */
class MCmSettings
    {

public: // New functions

    /**
     * Gets the status for the Content Manager service
     *
     * @since S60 3.1
     * @param aService indicates service
     * @param aState state enum is returned here
     * @return status of the operation
     */
    virtual TInt GetServiceState( const TCmService aService,
                                   TCmServiceState& aState ) const = 0;

     /**
     * Sets Content Manager service state
     *
     * @since S60 3.1
     * @param aState TCmServerState to indicate state
     * @return status of the operation
     */
    virtual TInt SetServiceState( const TCmService aService,
                                   const TCmServiceState aState ) = 0;

    /**
     * Gets defined MMC information.
     *
     * @since S60 3.1
     * @param aName MMC card's name (max length KMaxFileName)
     * @param aId MMC car's unique id
     * @return status of the operation
     */
//    virtual TInt GetMmcInformation( TDes& aName, TUint& aId ) const = 0;


    /**
     * Sets defined MMC information.
     *
     * @since S60 3.1
     * @param aName MMC card's name (max length KMaxFileName)
     * @param aId MMC car's unique id
     * @return status of the operation
     */
//    virtual TInt SetMmcInformation( const TDesC& aName, const TUint aId ) = 0;


    /**
     * Gets defined IAP information.
     *
     * @since S60 3.1
     * @param aIap access point info is stored here
     * @return status of the operation
     */
   virtual TInt GetIapL( TInt& aIap ) const = 0;

    /**
     * Gets next runtime information for the specified service.
     *
     * @since S60 3.1
     * @param aService content manager service
     * @param aRuntime the next date and time service is run.
     * @return status of the operation
     */
    virtual TInt GetNextRuntime( const TCmService aService,
                                  TTime& aRuntime ) const = 0;


    /**
     * Sets next runtime information for the specified service.
     *
     * @since S60 3.1
     * @param aService content manager service
     * @param aNextTime the next date and time service is run.
     * @return status of the operation
     */
    virtual TInt SetNextRuntime( const TCmService aService,
                                  const TTime aRuntime ) = 0;


    /**
     * Gets device discovery delay
     *
     * @since S60 3.1
     * @param aDelay device discovery delay
     * @return status of the operation
     */
    virtual TInt GetDiscoveryDelay( TInt& aDelay ) const = 0;


    /**
     * Sets device discovery delay
     *
     * @since S60 3.1
     * @param aDelay device discovery delay
     * @return status of the operation
     */
    virtual TInt SetDiscoveryDelay( const TInt aDelay ) = 0;


    /**
     * Gets automatic sync settings
     *
     * @since S60 3.1
     * @param aAutoSync automatic sync settings
     * @return status of the operation
     */
    virtual TInt GetAutoSync( TBool& aAutoSync ) const = 0;


    /**
     * Sets automatic sync settings
     *
     * @since S60 3.1
     * @param aAutoSync automatic sync settings
     * @return status of the operation
     */
    virtual TInt SetAutoSync( const TBool aAutoSync ) = 0;

     /**
     * Gets memory manager status
     *
     * @since S60 3.1
     * @param aMmStatus memory manager status
     * @return status of the operation
     */
    virtual TInt GetMemoryManagerStatus( TBool& aMmStatus ) const = 0;

     /**
     * Sets memory manager status
     *
     * @since S60 3.1
     * @param aMmStatus memory manager status
     * @return status of the operation
     */
    virtual TInt SetMemoryManagerStatus( const TBool aMmStatus ) = 0;

    /**
     * Gets search item count
     *
     * @since S60 3.1
     * @param aItemCount result item count
     * @return status of the operation
     */
    virtual TInt GetSearchCount( TInt& aItemCount ) const = 0;

    /**
     * Gets add item count
     *
     * @since S60 3.1
     * @param aItemCount result item count
     * @return status of the operation
     */
    virtual TInt GetAddCount( TInt& aItemCount ) const = 0;

    /**
     * ???
     *
     * @since S60 3.1
     * @param aAppWizardState ??
     * @return TInt ???
     */
    virtual TInt GetAppWizardInformation( TInt& aAppWizardState ) const = 0;

    /**
     * ???
     *
     * @since S60 3.1
     * @return TInt ???
     */
    virtual TInt SetAppWizardInformation() = 0;

    /**
     * ???
     *
     * @since S60 3.1
     * @param aFs ??
     * @param aDrives ??
     */
    virtual void DriveListL( RFs& aFs,
                              RPointerArray<CCmDriveInfo>& aDrives ) const = 0;

    /**
     * Deletes the object
     *
     * @since S60 3.1
     */
	virtual void Close() = 0;

    };


#endif // M_CCMSETTINGS_H
