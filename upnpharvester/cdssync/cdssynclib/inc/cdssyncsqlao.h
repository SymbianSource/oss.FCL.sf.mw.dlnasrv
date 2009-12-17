/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Cds Sync Active Object header
*
*/






#ifndef CDSSYNCSQLAO_H
#define CDSSYNCSQLAO_H

// FORWARD DECLARATIONS
class MCmSqlMain;
class CCdsSyncImpl;
class CCmSqlBaseItem;
class CCmSqlGenericItem;

/**
 * CCdsSyncSqlAo class
 *
 * @lib cdssync.lib
 *
 * @since S60 v3.1
 */
class CCdsSyncSqlAo : public CActive 
    {         
public:
    // active object states
    enum TCdsSyncSqlAoState 
        {
        ECdsSyncSqlAoIdle = 0,
        ECdsSyncSqlAoAdding,
        ECdsSyncSqlAoDeleting,
        ECdsSyncSqlAoInitializing    
        };
        
    /**
     * Two-phased constructor.
     *
     * @param aSqlDb, Database handler instance
     * @param aSync, CdsSync implementation instance
     * @param aItemsToAdd, array of items to be added
     * @param aItemsToAdd, array of items to be deleted
     * @param aAddGranularity, count of items added at a time
     */
    static CCdsSyncSqlAo* NewL( MCmSqlMain& aSqlDb,
                                CCdsSyncImpl& aSync,
                                RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
                                RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
                                TInt aAddGranularity );

    /**
     * Two-phased constructor.
     *
     * @param aSqlDb, Database handler instance
     * @param aSync, CdsSync implementation instance
     * @param aItemsToAdd, array of items to be added
     * @param aItemsToAdd, array of items to be deleted
     * @param aAddGranularity, count of items added at a time     
     */
    static CCdsSyncSqlAo* NewLC( 
        MCmSqlMain& aSqlDb,
        CCdsSyncImpl& aSync,
        RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
        RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
        TInt aAddGranularity );

    /**
     * Destructor.
     */
    virtual ~CCdsSyncSqlAo();
 
    /**
     * NotifyItemsAddedL
     *
     * @since S60 v3.1
     * @param aSourceDataComplete, indicates if all source data is added     
     */
    void NotifyItemsAddedL( TBool aSourceDataComplete = EFalse );

    /**
     * Activate
     *
     * @since S60 v3.1
     */
    void Activate();
 
 protected:
 
// From base class CActive

    /**
     * From CActive
     * See base class definition
     */       
    void RunL();

    /**
     * From CActive
     * See base class definition
     */
    void DoCancel();
        
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @param aSqlDb, Database handler instance
     * @param aSync, CdsSync implementation instance
     * @param aItemsToAdd, array of items to be added
     * @param aItemsToAdd, array of items to be deleted
     * @param aAddGranularity, count of items added at a time     
     */   
    CCdsSyncSqlAo( MCmSqlMain& aSqlDb,
                   CCdsSyncImpl& aSync,
                   RPointerArray<CCmSqlGenericItem>& aItemsToAdd,
                   RPointerArray<CCmSqlBaseItem>& aItemsToDelete,
                   TInt aAddGranularity );

    /**
     * Performs the second phase construction.
     */  
    void ConstructL();    

    /**
     * Checks metadata amouts and removes overhead items
     *
     * @since S60 v5.1
     */        
    void IfOverLimitDeleteMediaItems();                  
        
public:

    /**
     * Active object state 
     */
    TCdsSyncSqlAoState                  iState;
  
private:

    /**
     * Database handler
     */  
    MCmSqlMain&                         iSqlDb;

    /**
     * Cds sync implementation reference
     */
    CCdsSyncImpl&                       iSync;
    
    /**
     * Array of items to be added
     */
    RPointerArray<CCmSqlGenericItem>&   iItemsToAdd;
    
    /**
     * Array of items to be deleted
     */
    RPointerArray<CCmSqlBaseItem>&      iItemsToDelete;

    /**
     * Amount of files to be added at a time
     */
    TInt                                iAddGranularity;
    /**
     * Flag for indicating that source data is ready
     */
    TBool                               iSourceDataComplete;

    /**
     * Count of items to be added
     */
    TInt                                iItemsInAddition;        

#ifdef _DEBUG
    /**
     * Time stamps for logging
     */
    TTime iOperationTime;
#endif // _DEBUG

};

#endif // CDSSYNCSQLAO_H
