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
* Description:      CM Settings Engine main class
*
*/







#ifndef C_CCMSETTINGSENGINE_H
#define C_CCMSETTINGSENGINE_H

// INCLUDES
#include <e32base.h>
#include "cmcommon.h"
#include "cmsettings.h"

// FORWARD DECLARATIONS
class CRepository;
class CCmDriveInfo;
class RFs;

/**
 *  Settings engine class for the cenrep keys
 *
 *  Provides interface to get persistent settings information
 *  for different ContentManager modules.
 *
 *  @lib cmsettingsengine
 *  @since S60 3.1
 */
class CCmSettingsEngine : public CBase,
                          public MCmSettings
    {

public: // Constructors and destructor

    // 2-phase constructor
    static CCmSettingsEngine* NewL();

    // 2-phase constructor
    static CCmSettingsEngine* NewLC();

    // Destructor
    virtual ~CCmSettingsEngine();

public: // New functions

    /**
     * Gets the status for the Content Manager service
     *
     * @since S60 3.1
     * @param aService indicates service
     * @param aState state enum is returned here
     * @return status of the operation
     */
    TInt GetServiceState( const TCmService aService,
                                   TCmServiceState& aState ) const;

     /**
     * Sets Content Manager service state
     *
     * @since S60 3.1
     * @param aState TCmServerState to indicate state
     * @return status of the operation
     */
    TInt SetServiceState( const TCmService aService,
                                   const TCmServiceState aState );

    /**
     * Gets defined IAP information.
     *
     * @since S60 3.1
     * @param aIap access point info is stored here
     * @return status of the operation
     */
   TInt GetIapL( TInt& aIap ) const;

    /**
     * Gets next runtime information for the specified service.
     *
     * @since S60 3.1
     * @param aService content manager service
     * @param aRuntime the next date and time service is run.
     * @return status of the operation
     */
    TInt GetNextRuntime( const TCmService aService,
                                  TTime& aRuntime ) const;


    /**
     * Sets next runtime information for the specified service.
     *
     * @since S60 3.1
     * @param aService content manager service
     * @param aNextTime the next date and time service is run.
     * @return status of the operation
     */
    TInt SetNextRuntime( const TCmService aService,
                                  const TTime aRuntime );


    /**
     * Gets device discovery delay
     *
     * @since S60 3.1
     * @param aDelay device discovery delay
     * @return status of the operation
     */
    TInt GetDiscoveryDelay( TInt& aDelay ) const;


    /**
     * Sets device discovery delay
     *
     * @since S60 3.1
     * @param aDelay device discovery delay
     * @return status of the operation
     */
    TInt SetDiscoveryDelay( const TInt aDelay );


    /**
     * Gets automatic sync settings
     *
     * @since S60 3.1
     * @param aAutoSync automatic sync settings
     * @return status of the operation
     */
    TInt GetAutoSync( TBool& aAutoSync ) const;


    /**
     * Sets automatic sync settings
     *
     * @since S60 3.1
     * @param aAutoSync automatic sync settings
     * @return status of the operation
     */
    TInt SetAutoSync( const TBool aAutoSync );

     /**
     * Gets memory manager status
     *
     * @since S60 3.1
     * @param aMmStatus memory manager status
     * @return status of the operation
     */
    TInt GetMemoryManagerStatus( TBool& aMmStatus ) const;

     /**
     * Sets memory manager status
     *
     * @since S60 3.1
     * @param aMmStatus memory manager status
     * @return status of the operation
     */
    TInt SetMemoryManagerStatus( const TBool aMmStatus );

    /**
     * Gets search item count
     *
     * @since S60 3.1
     * @param aItemCount result item count
     * @return status of the operation
     */
    TInt GetSearchCount( TInt& aItemCount ) const;

    /**
     * Gets add item count
     *
     * @since S60 3.1
     * @param aItemCount result item count
     * @return status of the operation
     */
    TInt GetAddCount( TInt& aItemCount ) const;

    /**
     * ???
     *
     * @since S60 3.1
     * @param aAppWizardState ??
     * @return TInt ???
     */
    TInt GetAppWizardInformation( TInt& aAppWizardState ) const;

    /**
     * ???
     *
     * @since S60 3.1
     * @return TInt ???
     */
    TInt SetAppWizardInformation();

    /**
     * ???
     *
     * @since S60 3.1
     * @param aFs ??
     * @param aDrives ??
     */
    void DriveListL( RFs& aFs,
                              RPointerArray<CCmDriveInfo>& aDrives ) const;

    /**
     * Deletes the object
     *
     * @since S60 3.1
     */
	void Close();

private:
    CCmSettingsEngine();

    void ConstructL();

private: // data

    /**
     * Central Repository access
     */
    CRepository*                    iRepository;        // owned

    };


#endif // C_CCMSETTINGSENGINE_H
