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
* Description:  Content Manager server's client header
*
*/



#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "cmcommon.h"

// FORWARD DECLARATIONS
class CCmFillRuleContainer;
class CCmStoreRuleContainer;
class CCmMediaServerFull;
class CCmBaseListItem;
class CCmActive;
class CCmDriveInfo;
class CCmSqlPropertyCollector;
class RDesReadStream;

/**
*  Content Manager session
*
*  @lib cmclient.lib
*  @since S60 3.1
*/
class RContentManager : public RSessionBase
    {
public:

    /**
     * Session constructor.
     */
     IMPORT_C RContentManager();

    /**
     * Creates a connection to server. If server is not started,
     * method will start the server first and then create a new session.
     *
     * @since S60 3.1
     * @return TInt indicating the success of the call.
     */
    IMPORT_C TInt Connect();

    /**
     * Closes connection to server
     *
     * @since S60 3.1
     * @return TInt indicating the success of the call.
     */
    IMPORT_C TInt Close();

    /**
     * Cancels all ongoing async operations
     *
     * @since S60 3.1
     */
    IMPORT_C void Cancel();

    /**
     * Returns the current state of the Content Manager
     *
     * @since S60 3.1
     * @return TCmServerState state of the manager
     */
    IMPORT_C TCmServerState ServerState() const;


    /**
     * Sets state of a service
     *
     * @since S60 3.1
     * @param aService, defines the service
     * @param aNewState, service state to set¨
     * @return TInt, status of the operation
     */
    IMPORT_C TInt SetServiceState( const TCmService aService,
                                   const TCmServiceState aNewState );

    /**
     * Gets service state
     *
     * @since S60 3.1
     * @param aService, defining the requested service
     * @return TCmServiceState, service state
     */
    IMPORT_C TCmServiceState ServiceState( const TCmService aService ) const;

    /**
     * Execute a service
     *
     * @since S60 3.1
     * @param aService defines the service
     * @param aStatus asynchronous request status
     * @return TInt, KErrNone if successfull
     */
    IMPORT_C TInt ExecuteService( const TCmService aService,
                                  TRequestStatus& aStatus );

    /**
     * Sets media server information to database
     *
     * @since S60 3.1
     * @param aServers New media server information.
     * @return TInt, status of the operation
     */
    IMPORT_C TInt SetMediaserversL(
            RPointerArray<CCmMediaServerFull>& aServers );

    /**
     * Deletes media servers from db ( servers with deleted status )
     *
     * @since S60 3.1
     * @return None
     */
    IMPORT_C void DeleteDeletedMediaserversL();

    /**
     * Gets media server information from database (ownership to caller)
     *
     * @since S60 3.1
     * @return RPointerArray<CCmMediaServerFull>,
     *         media server information array
     */
    IMPORT_C RPointerArray<CCmMediaServerFull>* MediaserversL() const;

    /**
     * Gets current item count for a list of a service (fill or store)
     *
     * @since S60 3.1
     * @param aService defines the service
     * @param aListName list name
     * @param aCount returns count
     * @param aSize returns size
     * @return TInt, KErrNone if successfull
     */
    IMPORT_C TInt GetItemCountsL( const TCmService aService,
                                  const TDesC8& aListName,
                                  TInt& aCount,
                                  TInt64& aSize ) const;
    /**
     * Gets filled files size
     *
     * @since S60 3.1
     * @param aSize returns size
     * @param aID id of fill rule
     */
    IMPORT_C void GetFilledFilesSize( TInt64& aSize, const TInt aID ) const;

    /**
     * Gets filtered metadata items
     *
     * @since S60 3.2
     * @param aService, defines the service
     * @param aCollector, metadata collector
     * @param aStatus, asynchronous request status
     * @return TInt, error code
     */
    IMPORT_C TInt GetFilteredMetadataL( const TCmService aService,
                                        CCmSqlPropertyCollector*& aCollector,
                                        TRequestStatus& aStatus );

    /**
     * Updating fill lists
     *
     * @since S60 3.1
     * @param aStatus asynchronous request status
     */
    IMPORT_C void UpdateFillListsL( TRequestStatus& aStatus );

    /**
     * Deleting metadata
     *
     * @since S60 3.1
     * @param aStatus asynchronous request status
     */
    IMPORT_C void DeleteMetadataL( TRequestStatus& aStatus );

    /**
     * Returns the app wizard execution information.
     *
     * @since S60 3.1
     * @return TBool, ETrue if app wizard has been executed at startup
     */
    IMPORT_C TBool AppWizardState() const;

    /**
     * Sets the app wizard execution information.
     *
     * @since S60 3.1
     * @return TInt, KErrNone if successfull
     */
    IMPORT_C TInt SetAppWizardState();

    /**
     * Gets last sync time
     *
     * @since S60 3.1
     * @return TTime, last sync time
     */
    IMPORT_C TTime LastSyncTime() const;

    /**
     * Sets fill file statuses
     *
     * @param aItems, reference to item array containing statuses
     * @since S60 3.1
     */
    IMPORT_C void SetFillFileStatusesL(
            const RPointerArray<CCmBaseListItem>& aItems );

    /**
     * Get transfer item count and time for a service
     *
     * @param aService defines the service
     * @param aItemCount, item count value
     * @param aTimeInSec, transfer time value
     * @since S60 3.1
     */
    IMPORT_C void GetTransferInfo( const TCmService aService,
                                   TInt& aItemCount,
                                   TInt& aTimeInSec ) const;

    /**
     * Sets Fill rules
     *
     * @since S60 3.1
     * @param aFillRules, reference to fill rule container object
     */
    IMPORT_C void SetFillRulesL( const CCmFillRuleContainer& aFillRules );

    /**
     * Gets fill rules
     *
     * @since S60 3.1
     * @return CCmFillRuleContainer, pointer to fill rule container
     *                               object (ownership to caller)
     */
    IMPORT_C CCmFillRuleContainer* FillRulesL();

    /**
     * Gets files of the list
     *
     * @since S60 3.1
     * @param aListName, list name
     * @param aService, fill or store service
     * @return RPointerArray<CCmBaseListItem, pointer to fill list item
     *                                        array (ownership to caller)
     */
    IMPORT_C RPointerArray<CCmBaseListItem>*
                ListItemsL( const TDesC8& aListName, TCmService aService );

    /**
     * Preprocesses fill list
     *
     * @since S60 3.1
     * @param aListName fill list name
     * @return TInt, error code
     */
    IMPORT_C TInt PreProcessFillList( const TDesC8& aListName,
                                      TRequestStatus& aStatus );

    /**
     * Sets store rules
     *
     * @since S60 3.1
     * @param aStoreRules reference to store rule container object
     */
    IMPORT_C void SetStoreRulesL(
            const CCmStoreRuleContainer& aStoreRules );

    /**
     * Gets store rules
     *
     * @since S60 3.1
     * @return CCmStoreRuleContainer, pointer to store rule container object
     *                                (ownership to caller)
     */
    IMPORT_C CCmStoreRuleContainer* StoreRulesL();

    /**
     * Sets drives
     * @since S60 3.1
     * @param aDrive, drive info array
     */
    IMPORT_C void SetDrivesL( RPointerArray<CCmDriveInfo> aDrives );

    /**
     * Gets drives
     * @since S60 3.1
     * @param aDrives, drives on return
     */
    IMPORT_C void GetDrivesL( RPointerArray<CCmDriveInfo>& aDrives );

    /**
     * Returns drive selection status
     * @since S60 3.1
     * @return TBool, ETrue if set, EFalse otherwise
     */
    IMPORT_C TBool DriveSelectionStatus();

    /**
     * Informs server that application will be shut down
     * @since S60 3.1
     */
    IMPORT_C void ApplicationExit();
   
    /**
     * Informs client when server operation is complete
     *
     * @since S60 3.1
     * @param aErr
     * @return TInt, KErrNone if successfull
     */
    TInt OperationCompletedL( TInt aErr );

private:

    /**
     * Copy items from server side collector to client collector
     * @Since s60 3.1
     * @param aStream, dataStream from server
     */
    void CopyItemsL( RDesReadStream& aStream );

private: // data

    /**
     * active object
     */
    CCmActive* iAo;                                 // owned

    /**
     * target array for metadata items
     */
    CCmSqlPropertyCollector** iTargetContainer;     // not owned

    /**
     * client's request status
     */
    TRequestStatus* iClientRequest;

    /**
     * indicator for user cancelling
     */
    TBool iCanceled;

    /**
     * data buffer
     */
    CBufFlat* iBuffer;                              // owned

    /**
     * buffer to be sent to server size
     */
    TPtrC8 iBufferPointer;
    };


#endif // CONTENTMANAGER_H

// End of File
