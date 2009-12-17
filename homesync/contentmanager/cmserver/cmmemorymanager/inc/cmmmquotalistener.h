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
* Description:  Quota listener class in the Store manager component
*
*/


#ifndef C_CMMMQUOTALISTENER_H
#define C_CMMMQUOTALISTENER_H

#include <e32base.h>
#include <f32file.h> // RFs

/* Forward declarations. */
class CAknGlobalProgressDialog;
class CCmDmMain;
class MMPXHarvesterUtility;

/**
 *  CCmMmQuotaListener class
 *  Part of Memory Manager
 *  @lib cmmemorymanager.lib
 *  @since S60 v3.0
 */
class CCmMmQuotaListener : public CActive
    {

public: // Symbian constructors and C++ destructor

    /**
     * Creates new CCmMmQuotaListener class and opens connection to database
     *
     * @since S60 v3.0
     * @param aDbManager
     * @param aDrive
     * @return pointer to CCmMmQuotaListener object
     */
    static CCmMmQuotaListener* NewL( CCmDmMain& aDbManager, TInt aDrive );

    /**
     * Creates new CCmMmQuotaListener class and leaves the instance
     * in the cleanup stack
     *
     * @since S60 v3.0
     * @param aDbManager
     * @param aDrive
     * @return  pointer to CCmMmQuotaListener object
     */
    static CCmMmQuotaListener* NewLC( CCmDmMain& aDbManager, TInt aDrive );

    /**
     * C++ destructor
     *
     * @since S60 v3.0
     */
    virtual ~CCmMmQuotaListener();

protected: // Functions from base class

    /**
     * From CActive
     * Cancels any outstanding operation
     *
     * @since S60 v3.0
     */
    void DoCancel();

    /**
     * From CActive
     * Callback function
     *
     * @since S60 v3.0
     */
    void RunL();
    
private:

    /**
     * Resolves drives root path
     *
     * @since S60 v3.0
     * @param aDriveNumber
     * @return Root path
     */
    HBufC* ResolveAndAppendRootPathL( const TInt aDriveNumber );  

private:

    /**
     * C++ constructor
     *
     * @since S60 v3.0
     * @param aDbManager
     * @param aDrive
     */
    CCmMmQuotaListener( CCmDmMain& aDbManager, TInt aDrive );

    /**
     * Second-phase constructor
     *
     * @since S60 v3.0
     */
    void ConstructL();

private: // New methods

    /**
     * ReadResourceL
     *
     * @since S60 v3.0
     * @param aResourceId
     * @return resource string
     */
    HBufC16* ReadResourceL( TInt aResourceId );

    /**
     * Waits for given time and after that issues NotifyDiskSpace request
     * to the fileserver and sets this object active.
     *
     * @since S60 v3.1
     * @param aTimeoutValue time to wait.
     */
    void RequestNotificationAfterTimeout( TInt aTimeoutValue );
    
    /**
     * Issues NotifyDiskSpace request to the fileserver and sets this object 
     * active.
     *
     * @since S60 v3.1
     */
    void RequestDiskSpaceNotification();

    /**
     * Asks a confirmation for delete operation from the user.
     *
     * @since S60 v3.1
     * @param aListName 
     * @param aListSize 
     * @return the status of the confirmation in a TRequestStatus
     */
    TRequestStatus DeleteConfirmationL( 
        const TDesC& aListName, 
        TUint32 aListSize );
    
    /**
     * Deletes the files in the array. Shows progressnote about delete 
     * operation progress.
     *
     * @since S60 v3.1
     * @param aArray 
     * @param aListName 
     * @return Total size of all deleted files together.
     */
    TUint32 DeleteFilesL( 
        const RPointerArray<CCmFillListItem>& aArray,
        const TDesC8& aListName );

    /**
     * Remove files from the end of the array so that the total amount of 
     * files in the list is no more than aDeleteLimit megabytes.
     *
     * @since S60 v3.1
     * @param aArray the array 
     * @param aDeleteLimit maximum amount of the filesizes in the array 
     *        together after this method call (in megabytes).
     * @return Total size of all files in the array together.
     */ 
    TUint32 CCmMmQuotaListener::RemoveExtraFiles( 
        RPointerArray<CCmFillListItem>& aArray, 
        TInt64 aDeleteLimit ) const;

private: // data
    TInt             iDrive;
    RFs              iFileSession;
    TInt64           iQuota;
    CCmDmMain&       iDbManager;    
    MMPXHarvesterUtility* iHarvester; // owned
    CAknGlobalProgressDialog* iProgressDialog; //owned
    };

#endif //  C_CMMMQUOTALISTENER_H
