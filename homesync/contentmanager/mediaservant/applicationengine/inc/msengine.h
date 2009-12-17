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
* Description:  file sharing engine class definition
*
*/



#ifndef __MSENGINE_H__
#define __MSENGINE_H__


//  Include Files
#include <e32base.h>
#include "contentmanager.h"
#include "cmbaselistitem.h"

// FORWARD DECLARATIONS
class CMSEngineAO;
class MMSEngineObserver;
class CCmBaseListitem;
class CCmSqlPropertyCollector;

// CLASS DECLARATION

/**
 *  Media Servant engine class for Media Servant application
 *
 *  @lib msengine.lib
 *
 *  @since S60 5.1
 */
class CMSEngine : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CMSEngine* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CMSEngine* NewLC();

    /**
     * Destructor.
     */
    virtual ~CMSEngine();

    /**
     * Scans media servers on the network
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    IMPORT_C TInt ScanMediaServersL();

    /**
     * Start store & fill operations
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    IMPORT_C TInt ExecuteSyncL();

    /**
     * Preprocesses files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    IMPORT_C TInt ExecuteStorePreProcessingL();

    /**
     * Preprocesses files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    IMPORT_C TInt ExecuteFillPreProcessingL();

    /**
     * Cancels current operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    IMPORT_C void StopOperation();

    /**
     * Sets observer for active object
     *
     * @since S60 5.1
     * @param MMSEngineObserver*, observer class
     * @return None
     */
    IMPORT_C void SetObserver( MMSEngineObserver* aObserver );

    /**
     * Gets fill rules
     *
     * @since S60 5.1
     * @param None
     * @return pointer to fill rule container object
     */
    IMPORT_C CCmFillRuleContainer* FillRulesL();
    /**
     * Get filled files size
     * @since S60 5.1
     * @param aSize reference to filled files size
     * @param aID id of fill rule
     */
    IMPORT_C void GetFilledFilesSize( TInt64& aSize, const TInt aID );

    /**
     * Sets Fill rules
     *
     * @since S60 5.1
     * @param aFillRules reference to fill rule container object
     * @return None
     */
    IMPORT_C void SetFillRulesL( CCmFillRuleContainer* aFillRules );

    /**
     * Gets store rules
     *
     * @since S60 5.1
     * @param None
     * @return pointer to store rule container object
     */
    IMPORT_C CCmStoreRuleContainer* StoreRulesL();

    /**
     * Sets Store rules
     *
     * @since S60 5.1
     * @param aStoreRules reference to store rule container object
     * @return None
     */
    IMPORT_C void SetStoreRulesL( CCmStoreRuleContainer* aStoreRules );

    /**
     * Reads server state
     *
     * @since S60 5.1
     * @param aState, server state
     * @return None
     */
    IMPORT_C void ServerState( TCmServerState& aState );

    /**
     * Gets service state
     *
     * @since S60 5.1
     * @param aService, service type
     * @return TCmServiceState, service state
     */
    IMPORT_C TCmServiceState ServiceState( const TCmService aService );

    /**
     * Sets service state
     *
     * @since S60 5.1
     *
     * @param aService, service type
     * @param aState, service state
     * @return TInt, error
     */
    IMPORT_C TInt SetServiceState( const TCmService aService,
                                   TCmServiceState aState );

    /**
     * Gets media servers
     *
     * @since S60 5.1
     * @param None
     * @return RPointerArray*, mediaservers
     */
    IMPORT_C RPointerArray<CCmMediaServerFull>* GetMediaServersL();

    /**
     * Sets media servers
     *
     * @since S60 5.1
     * @param aMediaServers, mediaservers
     * @return KErrNone if successfull
     */
    IMPORT_C TInt SetMediaServersL(
                    RPointerArray<CCmMediaServerFull>& aMediaServers );

    /**
     * Deletes filled files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    IMPORT_C TInt DeleteFilledFilesL();

    /**
     * Gets store list items
     *
     * @since S60 5.1
     * @param aRuleName, name of the rule.
     * @return RPointerArray*, store list items
     */
    IMPORT_C RPointerArray<CCmBaseListItem>* GetStoreListItemsL(
                                                      TDesC8& aRuleName);

    /**
     * Gets fill list items
     *
     * @since S60 5.1
     * @param aRuleName, name of the rule.
     * @return RPointerArray*, fill list items
     */
    IMPORT_C RPointerArray<CCmBaseListItem>* GetFillListItemsL(
                                                      TDesC8& aRuleName);

    /**
     * Returns item count for a list of a service (fill or store)
     *
     * @since S60 5.1
     * @param aService defines the service
     * @param aListName list name
     * @param aCount, item count
     * @param aSize, size in MBs
     * @return item count (fill case in bytes, store case number of items)
     */
    IMPORT_C TInt GetItemCountsL( const TCmService aService,
                                  const TDesC8& aListName,
                                  TInt& aCount,
                                  TInt64& aSize ) const;

    /**
     * Preprocesses a fill list
     *
     * @since S60 5.1
     * @param aListName, list name to be preprocessed
     * @return TInt, error code
     */
    IMPORT_C TInt PreProcessFillListL( const TDesC8& aListName ) const;

    /**
     * Updates fill lists
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    IMPORT_C void UpdateFillListsL() const;

    /**
     * Deletes metadata collected from given servers
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    IMPORT_C void DeleteMetadataL() const;

    /**
     * Gets app wizard state information.
     *
     * @since S60 5.1
     * @param aErrorCode defines operation success information
     * @return ETrue if application wizard first run has been executed.
     */
    IMPORT_C TBool GetAppWizardState( TInt& aErrorCode );

    /**
     * Sets app wizard state information.
     *
     * @since S60 5.1
     * @param None
     * @return KErrNone if successfull.
     */
    IMPORT_C TInt SetAppWizardState();

    /**
     * Gets last sync time
     *
     * @since S60 5.1
     * @param None
     * @return last sync time
     */
    IMPORT_C TTime LastSyncTime() const;

    /**
     * Sets fill file statuses
     *
     * @since S60 5.1
     * @param aItems reference to item array containing statuses
     * @return None         
     */
    IMPORT_C void SetFillFileStatusesL(
                        const RPointerArray<CCmBaseListItem>* aItems );

    /**
     * Get transfer item count and time for a service
     *
     * @since S60 5.1
     * @param aService defines the service
     * @param aItemCount returns count
     * @param aTimeInSec returns time
     * @return None
     */
    IMPORT_C void GetTransferInfo( const TCmService aService,
                                   TInt& aItemCount,
                                   TInt& aTimeInSec ) const;

    /**
     * Deletes media servers from db ( servers with deleted status )
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    IMPORT_C void DeleteDeletedMediaserversL();

    /**
     * Sets drives
     *
     * @since S60 5.1    
     * @param aDrives, drive info array
     * @return None
     */
    IMPORT_C void SetDrivesL( RPointerArray<CCmDriveInfo> aDrives );

    /**
     * Gets drives
     *
     * @since S60 5.1
     * @param aDrives, drives on return
     * @return None
     */
    IMPORT_C void GetDrivesL( RPointerArray<CCmDriveInfo>& aDrives );

    /**
     * Gets drive selection info
     *
     * @since S60 5.1
     * @param None
     * @return TBool, ETrue if drive(s) selected, EFalse otherwise
     */
    IMPORT_C TBool DriveSelectionStatus();

    /**
     * Gets metadata
     *
     * @since S60 5.1
     * @param aCollector, group of metadata categories
     * @return None
     */
    IMPORT_C void GetFilteredMetadataL(
                        CCmSqlPropertyCollector*& aCollector );
    
private:

    /**
     * Opens Content Manager session
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    TInt OpenServerSession();

    /**
     * Closes Content Manager session
     *
     * @since S60 5.1
     * @param None
     * @return None         
     */
    TInt CloseServerSession();
    
    /**
     * Performs the first phase of two phase construction.
     */
    CMSEngine();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

private:

    /**
     * Active object for asynchronous file sharing
     */
    CMSEngineAO*                    iAO;        // owned
    /**
     * mediaserver session handle
     */
    RContentManager                 iCMServer;  // owned
    };

#endif  // __MSENGINE_H__

// End of file

