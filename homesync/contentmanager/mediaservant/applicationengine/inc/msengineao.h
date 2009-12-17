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
* Description:  file sharing engine active object class definition
*
*/



#ifndef __MSENGINEAO_H__
#define __MSENGINEAO_H__


// Include Files
#include "contentmanager.h"

// FORWARD DECLARATIONS
class CMSEngine;
class MMSEngineObserver;


// CLASS DECLARATION
/**
 * Active object class for MSEngine
 *
 * @lib msengine.lib
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSEngineAO ): public CActive
    {
private:
    // Active object states
    enum TMSEngineAOState
        {
        EMSEngineStateNone = 0,
        EMSEngineHarvest,
        EMSEngineFill,
        EMSEngineStore,
        EMSEngineDeleteFiles,
        EMSEnginePreprocess,
        EMSEngineGetMetadata,
        EMSEnginePreprocessList,
        EMSEngineDeleteMetadata,
        EMSUpdateFillLists
        };

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param CMSEngine*, pointer to mediaservant engine.
     * @return 5.1
     */
    static CMSEngineAO* NewL( CMSEngine* aEngine );

    /**
     * Destructor.
     */
    virtual ~CMSEngineAO();

    /**
     * Scans media Servers
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    TInt ScanMediaServersL();

    /**
     * Start fill operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void ExecuteFillL();

    /**
     * Start store operation
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    TInt ExecuteStoreL();

    /**
     * Sets observer for active object
     *
     * @since S60 5.1
     * @param MMSEngineObserver*, pointer to the observer object.
     * @return None
     */
    IMPORT_C void SetObserver(MMSEngineObserver* aObserver);

    /**
     * Cancels current operation
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void StopOperation();

    /**
     * Informs server that application is shutting down
     *
     * @since S60 5.1
     * @param None
     * @return None         
     */
    void ApplicationExit();

    /**
     * Deletes filled files
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    TInt DeleteFilledFilesL();

    /**
     * Preprocesses file information for store.
     *
     * @since S60 5.1
     * @param None
     * @return None         
     */
    TInt ExecuteStorePreProcessingL();

    /**
     * Preprocesses file information for fill.
     *
     * @since S60 5.1
     * @param None
     * @return None         
     */
    TInt ExecuteFillPreProcessingL();

    /**
     * Gets metadata
     *
     * @since S60 5.1
     * @param aCollector, group of metadata categories
     * @return None
     */
    void GetFilteredMetadataL( CCmSqlPropertyCollector*& aCollector );

    /**
     * Preprocesses one fill list.
     *
     * @since S60 5.1
     * @param aListName, list name to be preprocessed
     * @return TInt, error code
     */
    TInt PreProcessFillListL( const TDesC8& aListName );

    /**
     * Update fill lists.
     *
     * @since S60 5.1
     * @param None
     * @return None         
     */
    void UpdateFillListsL();

    /**
     * Deletes metadata collected from given servers.
     *
     * @since S60 5.1
     * @param aServerList, list of servers
     * @return None
     */
    void DeleteMetadataL();
        
protected:
    
// From base class CActive

    /**
     * From CActive
     * See base class definition
     */
    virtual void RunL();

    /**
     * From CActive
     * See base class definition
     */
    virtual void DoCancel();

private:

    /**
     * C++ default constructor.
     * @param aEngine*, pointer to mediaservant engine
     */
    CMSEngineAO( CMSEngine* aEngine );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Opens Content Manager session
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    TInt OpenServerSession();

    /**
     * Closes Content Manager session
     *
     * @since S60 5.1
     * @param None
     * @return TInt, error code
     */
    TInt CloseServerSession();

private:

    /**
     * file sharing engine pointer
     */
    CMSEngine*                  iEngine;            // not owned

    /**
     * mediaserver session handle
     */
    RContentManager             iContentManager;    // owned

    /**
     * engine observer
     */
    MMSEngineObserver*          iObserver;          // not owned

    /**
     * current operation
     */
    TCmService                  iCurrentService;

    /**
     * Active object state
     */
    TMSEngineAOState           iState;
    };

#endif  // __MSENGINEAO_H__

// End of file
