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
* Description:  Header file for the selection storage class
*
*/



#ifndef CMSSELECTIONSTORAGE
#define CMSSELECTIONSTORAGE

// INCLUDES
#include <bautils.h>

// FORWARD DECLARATIONS
class CCmStoreRuleContainer;
class CCmFillRuleContainer;
class CMSEngine;
class CCmMediaServerFull;
class CCmStoreRule;
class CCmFillRule;

// CLASS DECLARATION

/**
 * CMSSelectionStorage class
 * Class implementing the storage for MediaServant wizard
 *
 * @since S60 5.1
 *
 */
NONSHARABLE_CLASS( CMSSelectionStorage ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aMSEngine, application engine
     * @param aServers, mediaserver list
     * @param aFs, file server session
     * @return CMSSelectionStorage pointer
     */
    static CMSSelectionStorage* NewL(
                        CMSEngine& aMSEngine,
                        RPointerArray<CCmMediaServerFull>& aServers,
                        RFs& aFs );

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aMSEngine, application engine
     * @param aServers, mediaserver list
     * @param aFs, file server session
     * @return CMSSelectionStorage pointer
     */
    static CMSSelectionStorage* NewLC(
                        CMSEngine& aMSEngine,
                        RPointerArray<CCmMediaServerFull>& aServers,
                        RFs& aFs );
    /**
     * Destructor.
     */
    virtual ~CMSSelectionStorage();

public:

    /**
     * Stores the media categories, selected devices and store operation 
     * type.
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void StoreMSSettingsL();

    /**
     * Sets fill list selections.
     *
     * @since S60 5.1
     * @param aSelections, selection array.
     * @return None     
     */
    void SetFillListSelections( CArrayFix<TInt>& aSelections );

    /**
     * Sets the store list selections.
     *
     * @since S60 5.1
     * @param aSelections, selection array.
     * @return None     
     */
    void SetStoreListSelections( CArrayFix<TInt>& aSelections );

    /**
     * Resets store list selections
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */
    void ClearStoreListSelections();

    /**
     * Sets the automatic/manual sync selection.
     *
     * @since S60 5.1
     * @param aSelection, service state manual/automatic
     * @return None     
     */
    void SetAutoManualSelection( TBool aSelection );

    /**
     * Returns the automatic/manual sync selection.
     *
     * @since S60 5.1
     * @param None     
     * @return automatic selection on/off.
     */
    TBool AutoManualSelection();

    /**
     * Stores drive information
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */
    void SetDriveInformationL();

    /**
     * Sets the selected servers for store rules.
     *
     * @since S60 5.1
     * @param aSelectedIndexes, selected server indexes
     * @param aServers, mediaserver list
     * @return None     
     */
    void SetServersForStoreRulesL(
                        CArrayFix<TInt>& aSelectedIndexes,
                        RPointerArray<CCmMediaServerFull>& aServers );

    /**
     * Sets the selected servers for fill rules.
     *
     * @since S60 5.1
     * @param aSelectedIndexes, selected server indexes
     * @return None     
     */
    void SetServersForFillRulesL( CArrayFix<TInt>& aSelectedIndexes );

private:

    /**
     * Removes the selected servers for store rule.
     *
     * @since S60 5.1     
     * @param aRule, store rule
     * @return None     
     */
    void RemoveServersFromStoreRuleL( CCmStoreRule* aRule );

    /**
     * Removes the selected servers for fill rule.
     *
     * @since S60 5.1
     * @param aRule, fill rule
     * @return None     
     */
    void RemoveServersFromFillRuleL( CCmFillRule* aRule );

    /**
     * Set wlan scanning on/off
     *
     * @since S60 5.1
     * @param None
     * @return None     
     */
    void SetWlanScanL();

    /**
     * Sets the selected servers for store rule.
     *
     * @since S60 5.1
     * @param aRule, store rule
     * @param aSelectedIndexes, selected server indexes,
     * @param aServers, mediaserver list
     * @return None     
     */
    void SetServersForStoreRuleL( 
                        CCmStoreRule* aRule,
                        CArrayFix<TInt>& aSelectedIndexes,
                        RPointerArray<CCmMediaServerFull>& aServers );

    /**
     * Sets the selected servers for fill rule.
     *
     * @since S60 5.1
     * @param aRule, fill rule
     * @param aSelectedIndexes, selected server indexes
     * @return None     
     */
    void SetServersForFillRuleL( CCmFillRule* aRule,
                                 CArrayFix<TInt>& aSelectedIndexes );        

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aMSEngine, application engine
     * @param aServers, mediaserver list
     * @param aFs, file server session
     * @return None         
     */
    CMSSelectionStorage( CMSEngine& aMSEngine,
                         RPointerArray<CCmMediaServerFull>& aServers,
                             RFs& aFs );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
       
private:

    /**
     * application engine
     */
    CMSEngine&                              iMSEngine;
    /**
     * Users selected Automatic or Manual synchronization type.
     * Efalse = Manual, ETrue = Automatic
     */
    TBool                                   iServiceState;
    /**
     * Selected store lists
     */
    CArrayFix<TInt>*                        iSelectedStoreLists; // owned
     
    /**
     * Selected fill lists
     */
    CArrayFix<TInt>*                        iSelectedFillLists; // owned       

    /**
     * Store rules
     */
    CCmStoreRuleContainer*                  iStoreRules;        // owned

    /**
     * Fill rules
     */
    CCmFillRuleContainer*                   iFillRules;         // owned

    /**
     * Mediaserver array
     */
    RPointerArray<CCmMediaServerFull>&      iServers;

    /**
     * file session
     */
    RFs&                                    iFs;        

    };

#endif      // CMSSELECTIONSTORAGE