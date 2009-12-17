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
* Description:  Definition for MediaServant fill list controller class
*
*/





#ifndef C_MSFILLRULECONTROLLER_H
#define C_MSFILLRULECONTROLLER_H

// INCLUDES
#include <e32base.h>
#include "cmcommontypes.h"


// FORWARD DECLARATIONS
class CMSEngine;
class CCmFillRuleContainer;
class CCmFillRule;
class CMSFillList;

/**
 * CMSFillRuleController class.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillRuleController ): public CBase
    {
    public:

        /**
         * two-phase constructor.
         */
        static CMSFillRuleController* NewL( CMSEngine& aMSEngine );

        /**
         * two-phase constructor.
         */
        static CMSFillRuleController* NewLC( CMSEngine& aMSEngine );
        
        /**
         * Destructor.
         */
        virtual ~CMSFillRuleController();

        /**
         * Returns rule pointer array
         * @since S60 5.1
         * @return RPointerArray<CMSFillList>, pointer array
         */
        RPointerArray<CMSFillList>* FillListArray();

        /**
         * Initialises container
         * @since S60 5.1
         */
        void InitializeRuleContainersL();

        /**
         * Creates default fill lists
         * @since S60 5.1
         */
        void CreateFillRulesL();

        /**
         * Deletes current rule
         * @since S60 5.1
         * @param TInt, index to be deleted
         */
        void DeleteRuleL( TInt aCurrentIndex );

        /**
         * Adds new rule
         * @since S60 5.1
         * @param aRule, new fill rule
         */
        void AddRuleL( CCmFillRule* aRule );

        /**
         * Changes priorites of two rules
         * @since S60 5.1
         * @param aTotalItem, items on the list
         * @param aCurrentItem, selected item
         * @param aDirection, move direction
         */
        void ChangePriority( TUint aTotalItem,
                             TUint aCurrentItem,
                             TInt aDirection );

        /**
         * Saves rules to server
         * @since S60 5.1
         */
        void SaveRulesL();

        /**
         * Reads rule container and recreates rule array
         * @since S60 5.1
         */
        void  UpdateArrayL();

        /**
         * Checks priority changes
         * @since S60 5.1
         * @return TBool, ETrue if changed, EFalse otherwise
         */
        TBool HasPriorityChangedL();

        /**
         * Checks activity changes
         * @since S60 5.1
         * @param aStatus, status to be checked
         * @return TBool, ETrue if changed, EFalse otherwise
         */
        TBool HasSelectionsChanged( TCmFillRuleStatus aStatus );

        /**
         * Restores original list selections
         * @since S60 5.1
         */
        void RestoreOriginalSelections();

    private:
        
        /**
         * Constructor.
         */
        CMSFillRuleController( CMSEngine& aMSEngine );

        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();
        
        /**
         * Sets priorities of rules (if deleted or added)
         * @since S60 5.1
         */
        void SetPriorities();

        /**
         * Sort function that arranges pointers by rule priority
         * @since S60 5.1
         * @param rule1, rule to be sorted
         * @param rule2, rule to be sorted
         */
        static TInt compare ( const CMSFillList& rule1,
                              const CMSFillList& rule2);


    private:

        /**
         * pointer to application engine
         */
        CMSEngine*                  iMSEngine;              // not owned
        /**
         * Fill rules
         */
        CCmFillRuleContainer*       iRuleContainer;         // owned
        /**
         * Pointer array to fillrules
         */
        RPointerArray<CMSFillList>  iFillListArray;         // owned

    };



/**************** FILL LIST CLASS ******************/


/**
 * CMSFillList class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillList ) : public CBase
    {
    public:
        
        /**
         * NewL
         * 2-phase constructor.
         */
        static CMSFillList* NewL( CMSEngine& aMSEngine,
                                  CCmFillRule& aFillList );

        /**
         * NewL
         * 2-phase constructor.
         */
        static CMSFillList* NewLC( CMSEngine& aMSEngine,
                                  CCmFillRule& aFillList );
                                  
        /**
         * Destructor.
         */
        virtual ~CMSFillList();

        /**
         * Fill list
         * @since S60 5.1
         * @return CCmFillRule, fill list
         */
        CCmFillRule* List();

        /**
         * List Size
         * @since S60 5.1
         * @return TUint32, List size in bytes
         */
        TUint32 ListSize();

        /**
         * List's item count
         * @since S60 5.1
         * @return TUint64, item count
         */
        TUint32 ItemCount();

        /**
         * Priority
         * @since S60 5.1
         * @return TInt, List priority
         */
        TInt Priority() const;

        /**
         * Original selection status
         * @since S60 5.1
         * @return TCmFillRuleStatus, selection status
         */
        TCmFillRuleStatus OriginalSelectionStatus();

    private:

        /**
         * Constructor.
         */
        CMSFillList( CMSEngine& aMSEngine, CCmFillRule& aFillList );

        /**
         * ConstructL
         * Second phase constructor.
         */
        void ConstructL();

    private:
        /**
         * Fill list size ( in bytes )
         */
        TUint32                             iListSize;
        /**
         * List item count
         */
         TUint32                            iItemCount;
        /**
         * MS Engine
         */
        CMSEngine*                          iMSEngine;  // not owned
        /**
         * Fill list
         */
        CCmFillRule*                        iFillList;  // not owned
        /**
         * Original selection state
         */
        TCmFillRuleStatus                   iSelected;
    };
#endif // C_MSFILLRULECONTROLLER_H


// End of File
