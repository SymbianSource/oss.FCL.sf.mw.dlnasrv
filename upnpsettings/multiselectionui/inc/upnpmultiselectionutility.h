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
* Description:      CUpnpMultiselectionUtility class definition
 *
*/






#ifndef UPNPMULTISELECTIONUTILITY_H
#define UPNPMULTISELECTIONUTILITY_H

// INCLUDES
#include <AknDialog.h>
#include <aknselectionlist.h>
#include <aknsfld.h>
#include <aknnavide.h>
#include <akntitle.h> //for status pane
#include <AknIconArray.h>
#include <aknlists.h> //cakncolumnlistbox
#include <AknWaitNoteWrapper.h>
#include <eiklbx.h>

// FORWARD DECLARATIONS
// NONE

// CLASS DECLARATION
enum TShareSelectionState 
    {
    EUndefined = 0, 
    EItemsSelected,
    EShareNoneSelected, 
    EShareAllSelected, 
    EShareNoneAndShareAllSelected,
    ENoShareNoneOrShareAllSelected
    };
/**
 * CUpnpMultiselectionUtility utility class
 * @since S60 3.1
 */
NONSHARABLE_CLASS( CUpnpMultiselectionUtility ): public CBase
    {
public: // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CUpnpMultiselectionUtility();

    /**
     * C++ default constructor.
     */
    CUpnpMultiselectionUtility();

    /**
     * Collect selected items to a listbox
     * @since S60 3.1
     */
    void CollectSelectedItemsL( CEikListBox* aListBox, 
                                TBool aFirstRun = EFalse );

    /**
     * Do internal decisions based on user's selections on the UI
     */
    void DoSelectionLogicL( CEikListBox* aListBox,
        TBool aFirstRun,
        const CArrayFix<TInt>* aIndexes,
        CAknListBoxFilterItems* aFilter,
        TShareSelectionState aSelectionState );

    /**
     * Loads a possibly skinned icon and adds it to icon array
     * @since S60 3.1
     * @param CAknIconArray, array of icons
     * @param MAknsSkinInstance, skin instance
     * @param TDesC, reference to icon file
     * @param TAknsItemID, skinned icon id
     * @param TInt, bitmap id
     * @param TInt, bitmap mask id
     */
    void AppendIconToArrayL( CAknIconArray* aArray,
                             MAknsSkinInstance* aSkin,
                             const TDesC& aMbmFile,
                             const TAknsItemID& aID,
                             TInt aBitmapId,
                             TInt aMaskId ) const;

    /**
     * Appends first item ("share all files") to the listbox
     * @since S60 3.1
     * @param CDesCArray, listbox array
     */
    void AppendShareAllSelectionL( CDesCArray* aListBoxArray ) const;

    /** 
     * Convert selections from UI to engine as "old style" selections
     * Effect only in case Share All is selected
     * @since S60 3.1
     * @param CArrayFix<TInt>& The selection indexes to modify
     */
    void ConvertSelectionsForEngineL( CArrayFix<TInt>& aSelections ) const;

    /** 
     * Sets iShareNoneSelected and iShareAllSelected member variables.
     * @since S60 3.1
     * @param TShareSelectionState share all and do not share selection state
     */
    void SetShareSelectionState( TShareSelectionState aSelectionState );

    /** 
     * Returns ShareSelectionState from the array.
     * @since S60 3.1
     * @param const CArrayFix<TInt>& array of selection indexes.
     */
    TShareSelectionState ShareSelectionStateFromArray( 
        const CArrayFix<TInt>& aIndexes ) const;
     
private:
    TBool iShareNoneSelected;
    TBool iShareAllSelected;
    };

#endif //UPNPMULTISELECTIONUTILITY_H

// End of File
