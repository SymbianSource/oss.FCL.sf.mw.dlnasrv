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
* Description:  Content Manager server executable header file
*
*/



#ifndef CMSERVER_H
#define CMSERVER_H

//  Includes
#include <mdesession.h>
#include "upnpavdeviceobserver.h"
#include <f32file.h>

#include "cmmmobserver.h"
#include "cmserviceobserver.h"

// Forward declarations
class MUPnPAVController;
class CCmMediaServerFull;
class CCmDriveInfo;
class CCmFillListItem;
class CCmSqlPropertyCollector;
class CCmDmMain;
class MCmFmManager;
class MCmSmManager;
class CCmMmMain;
class MCmMdhClient;
class MCmSettings;
class CCmScheduler;
class CCmFillRuleContainer;
class CCmSqlPropertyContainer;


// CLASS DECLARATION

/**
 *  Content Manager server Core class
 *
 *  @since S60 3.1
 */
NONSHARABLE_CLASS ( CCmServer ) : public CPolicyServer,
                                  public MCmServiceObserver,
                                  public MUPnPAVDeviceObserver,
                                  public MCmMmObserver,
                                  public MMdESessionObserver
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCmServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmServer();

public:

    /**
     * Decrements server sessions
     * @Since S60 3.1
     */
    void DecrementSessions();

    /**
     * Server state
     * @Since S60 3.1
     * @return TCmServerState, server state
     */
    TCmServerState ServerState() const;

    /**
     * Sets asynchronous message
     * @Since S60 3.1
     * @param aMessage, asynchronous message
     */
    void SetCurrentAsyncMsg( const RMessage2& aMessage );

    /**
     * Executes service
     * @Since S60 3.1
     * @param aService, service to be executed
     */
    void ExecuteServiceL( TCmService aService );

    /**
     * Executes service
     * @Since S60 3.1
     * @param aService, service to be executed
     * @param aOptParam1, optional parameter 1
     * @param aOptParam2, optional parameter 2
     */
        void ExecuteServiceL( TCmService aService,
                              TInt aOptParam1,
                              TAny* aOptParam2 );

    /**
     * Sets service state
     * @Since S60 3.1
     * @param aService, Service
     * @param aState, service state
     */
    void SetServiceStateL( TCmService aService, TCmServiceState aState );

    /**
     * Cancels current service
     * @Since S60 3.1
     */
    void CancelServiceL();

    /**
     * Returns propertycontainer
     * @since S60 3.1
     * @return CCmSqlPropertyContainer, property container
     */
    CCmSqlPropertyContainer* PropertyContainer() const;

    /**
     * Returns propertycollector
     * @since S60 3.2
     * @return CCmSqlPropertyCollector, property containers
     */
    CCmSqlPropertyCollector* PropertyCollector() const;

    /**
     * Returns mediaserver array
     * @Since S60 3.1
     * @return RPointerArray<CCmMediaServerFull>, server array
     */
    RPointerArray<CCmMediaServerFull>& MediaServerArray();

    /**
     * Returns fill rule container
     * @Since S60 3.1
     * @return CCmFillRuleContainer, fill rule container
     */
    CCmFillRuleContainer* FillRuleContainerL() const;

    /**
     * Drive status
     * @since S60 3.1
     * @return TBool, ETrue if set, EFalse otherwise
     */
    TBool DriveSelectionStatus();

    /**
     * Sets drive status
     * @since S60 3.1
     * @param aDriveSelectionStatus, Drive status
     */
    void SetDriveSelectionStatus( const TBool aDriveSelectionStatus );

    /**
     * Set async observer flag
     * @since S60 3.1
     * @param aState, ETrue if application initiated
     *                EFalse if scheduler initiated
     */
    void SetAsyncRequestStateL( TBool aState );

protected:

// From base class MCmAVDeviceObserver

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */
    void UPnPDeviceDiscovered(const CUpnpAVDevice& aDevice);

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */
    void UPnPDeviceDisappeared(const CUpnpAVDevice& aDevice);

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */
    void WLANConnectionLost();

// From base class MCmMmObserver

    /**
     * From MCmMmObserver
     * See base class definition
     */
    void DeleteCompleteL( TInt aErr );

    /**
     * From MCmMmObserver
     * See base class definition
     */
    void ShrinkCompleteL( TInt aErr );

// From base class MMdESessionObserver

    /**
     * From MMdESessionObserver
     * See base class definition
     */
    void HandleSessionOpened(CMdESession& aSession, TInt aError);

    /**
     * From MMdESessionObserver
     * See base class definition
     */
    void HandleSessionError(CMdESession& aSession, TInt aError);

// From base class CPolicyServer

    /**
     * From CPolicyServer
     * See base class definition
     */
    CSession2* NewSessionL( const TVersion& aVersion,
                            const RMessage2& aMessage ) const;

private:
    /**
     * Creates SQL about files to be deleted
     * 
     * @since S60 5.1
     * @param aItems, item list
     * @param aDrives, drivinfo
     * @param aFilesToDelete, Sql clause array
     */
    void CreateFileDeleteSQLL( RPointerArray<CCmFillListItem>& aItems,
                        RPointerArray<CCmDriveInfo>& aDrives,
                        CDesCArrayFlat* aFilesToDelete );
private:

    /**
     * C++ default constructor.
     */
     CCmServer();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Creates AV controller and starts discovery timer
     * @since S60 3.1
     */
    void CreateAvcAndTimerL();

    /**
     * Checks if server is not ready
     * @since S60 3.1
     */
    void LeaveIfNotReadyL();

    /**
     * Deletes next fill rule
     * @since S60 3.1
     */
    void DeleteUnselectedFillFilesL();

    /**
     * Starts store process
     * @since S60 3.1
     */
    void ExecuteStoreL();

    /**
     * Starts fill process
     * @since S60 3.1
     */
    void ExecuteFillL();

    /**
     * Gets status by group id
     * @since S60 3.1
     * @param aListId, list id
     * @param aStatus, list item status
     * @return TInt, error code
     */
    TInt GetStatusByGroupId( TUint aListId, TCmListItemStatus& aStatus );

    /**
     * Checks server and adds it to db
     * @since S60 3.1
     * @param aDevice, media server
     */
    void CheckServerL( const CUpnpAVDevice& aDevice );

    /**
     * Deletes filled files
     * @Since S60 3.1
     */
    void DeleteFilledFilesL();

    /**
     * Device discovery timer
     * @Since S60 3.1
     * @param TAny, caller class pointer
     * @return TInt, returns always 0
     */
    static TInt SearchTimerCompletedL( TAny* aNy );

    /**
     * Called when service is executed
     * @Since S60 3.1
     * @param TCmService, completed service
     * @param TInt, error code
     */
    void ServiceExecuted( TCmService aService, TInt aErrCode );

    /**
     * Updates fill list priorities
     * @Since S60 3.1
     */
    void UpdateFillPriorities();

    /**
     * Append drive letter
     * @since S60 3.1
     * @param aFs, file server
     * @param aDriveNumber, drive number
     */
    HBufC* ResolveAndAppendRootPathL( RFs& aFs, const TInt aDriveNumber );

    /**
     * Updates drive quota when files are deleted
     * @since S60 3.1
     * @param aDrives, drive info
     * @param aItem, deleted item
     */
    void UpdateDriveQuota( RPointerArray<CCmDriveInfo>& aDrives,
                           CCmFillListItem& aItem );

    /**
     * Starts service when timer elapses
     * @Since S60 3.1
     * @return TInt, returns always 0
     */
    TInt DoSearchTimerCompletedL();

    /**
     * Deletes property container
     * @Since S60 3.1
     */
    void DeletePropertyContainer();
    
    /**
     * Deletes items from database by FillListId
     * @Since S60 3.1
     */
    void DeleteItemsByFillListId( TInt aFillListId );
        
    /**
     * Sort function that arranges pointers by id
     * @since S60 5.1
     * @param aItem1, item to be sorted
     * @param aItem2, item to be sorted
     */
    static TInt CompareById( const CCmFillListItem& aItem1,
                             const CCmFillListItem& aItem2 );

private:

    /**
     * AV controller
     */
    MUPnPAVController*          iAvController;          // owned
    /**
     * Database manager
     */
    CCmDmMain*                  iDbManager;             // owned
    /**
     * Fill manager
     */
    MCmFmManager*               iFillManager;           // owned
    /**
     * Store manager
     */
    MCmSmManager*               iStoreManager;          // owned
    /**
     * Memory manager
     */
    CCmMmMain*                  iMemoryManager;         // owned
    /**
     * Metadata harvester
     */
    MCmMdhClient*               iMdhClient;             // owned
    /**
     * Settings engine
     */
    MCmSettings*                iSettings;              // owned
    /**
     * Scheduler
     */
    CCmScheduler*               iScheduler;             // owned
    /**
     * Server state
     */
    TCmServerState              iServerState;
    /**
     * Current asyncronous message holder
     */
    RMessage2                   iCurrentAsyncMsg;
    /**
     * Device discovery timer
     */
    CPeriodic*                  iDeviceDiscoveryTimer;  // owned
    /**
     * Fill rule container
     */
    CCmFillRuleContainer*       iFillRuleContainer;     // owned
    /**
     * Item container
     */
    CCmSqlPropertyContainer*    iPropertyContainer;     // owned
    /**
     * Mediaserver array
     */
    RPointerArray<CCmMediaServerFull> iMediaServers;    // owned
    /**
     * Session counter
     */
    TInt*                       iSessionCount;
    /**
     * Fill rule index
     */
    TInt                        iFillRuleIndex;
    /**
     * Indicator for delete process
     */
    TBool                       iDeletingDeletedItems;
    /**
     * Asynchronous request status
     */
    TBool                       iAsyncRequestOn;
    /**
     * Store execution indicator
     */
    TBool                       iExecuteStoreAfterDelete;
    /**
     * Memory manager status
     */
    TBool                       iMmState;
    /**
     * MDE session
     */
    CMdESession*                iSession;               // owned
    /**
     * Fill list collector
     */
    CCmSqlPropertyCollector*    iPropertyCollector;     // not owned
    /**
     * status flag for drive setting
     */
    TBool                       iDriveSelectionStatus;
    };

#endif // CMSERVER_H

// End of File
