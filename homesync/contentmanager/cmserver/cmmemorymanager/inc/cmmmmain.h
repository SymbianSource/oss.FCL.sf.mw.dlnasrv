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
* Description:  Main class in the Memory manager component
*
*/


#ifndef C_CMMMMAIN_H
#define C_CMMMMAIN_H

#include <e32base.h>
#include <badesca.h> // CDesCArray
#include <f32file.h> // RFs
#include "cmcommon.h" // TCmListItemStatus

// Forward declarations
class MCmMmObserver;
class MMPXHarvesterUtility;
class CCmMmQuotaListener;
class CCmMmShrinker;
class CCmDmMain;
class MMPXCollectionHelper;

/**
 *  CCmMmMain class
 *  Memory manager
 *
 *  @lib cmmemorymanager.lib
 *  @since S60 v3.0
 */
class CCmMmMain : public CBase
    {

public: // Symbian constructors and C++ destructor

    /**
     * Creates new CCmMmMain class
     *
     * @since S60 v3.0
     * @param aDbManager
     * @return pointer to CCmMmMain object
     */
    IMPORT_C static CCmMmMain* NewL( CCmDmMain& aDbManager );

    /**
     * Creates new CCmMmMain class and leaves the instance
     * in the cleanup stack
     *
     * @since S60 v3.0
     * @param aDbManager
     * @return  pointer to CCmMmMain object
     */
    IMPORT_C static CCmMmMain* NewLC( CCmDmMain& aDbManager );

    /**
     * @since S60 v3.0
     *
     * C++ destructor
     */
    IMPORT_C virtual ~CCmMmMain();

public: // New methods

    /**
     * Cancels deletion and shrinking
     *
     * @since S60 3.0
     */
    IMPORT_C void Cancel();

    /**
     * Deletes files from array
     *
     * @since S60 3.0
     * @param aFiles
     */
    IMPORT_C void DeleteFilesL( CDesCArray& aFiles );

    /**
     * Sets observer
     *
     * @since S60 3.0
     * @param aObserver
     */
    IMPORT_C void SetObserver( MCmMmObserver& aObserver );

    /**
     * Sets quotalistener on/off
     *
     * @since S60 3.0
     * @param aState
     */
    IMPORT_C void SetQuotaListenerStateL( TBool aState );

    /**
     * Shrinks files from array
     *
     * @since S60 3.0
     * @param aFiles
     */
    IMPORT_C void ShrinkImagesL( CDesCArray& aFiles );

    /**
     * Tells MMC state
     *
     * @since S60 3.0
     * @param aFileServer
     * @param aDriveNumber
     * @return ETrue if Drive is usable and else EFalse
     */
    static TBool CCmMmMain::DriveState( 
        const RFs& aFileServer, 
        TInt aDriveNumber );

private: // Constructors

    /**
     * C++ constructor
     *
     * @since S60 3.0
     * @param aObserver
     */
    CCmMmMain( CCmDmMain& aDbManager );

    /**
     * Second-phase constructor
     *
     * @since S60 3.0
     */
    void ConstructL();

private: // New methods

    /**
     * Static callback method for deletion
     *
     * @since S60 3.0
     * @param aMm
     */
    static TInt BackgroundDeleteL( TAny* aMm );

    /**
     * Deletes files in background
     *
     * @since S60 3.0
     * @return ETrue if Drive is usable and else EFalse
     */
    TInt DoBackgroundDeleteL();

    /**
     * Delete the collection hepler
     *
     * @since S60 3.0
     */
    void CloseMpxCollectionHelper();

private: // data
    TInt                                iDeleteIndex;
    RFs                                 iFileSession;
    TBool                               iQuotaListenerState;            
    CCmDmMain&                          iDbManager;
    CIdle*                              iDeleter;               // owned
    CDesCArray*                         iFilesToDelete;         // owned
    CDesCArray*                         iFilesToShrink;         // owned   
    MMPXHarvesterUtility*               iHarvester;             // owned
    MCmMmObserver*                      iObserver;              // now owned
    CCmMmShrinker*                      iShrinker;              // owned
    RPointerArray<CCmMmQuotaListener>   iQuotaListeners;        // owned
    MMPXCollectionHelper*               iMpxCollectionHelper;   // owned

    };

#endif // C_CMMMMAIN_H
