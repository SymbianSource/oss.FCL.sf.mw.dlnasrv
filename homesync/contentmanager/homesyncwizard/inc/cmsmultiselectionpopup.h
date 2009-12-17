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
* Description:  CMSMultiselectionPopup class definition
*
*/


#ifndef CMSMULTISELECTIONPOPUP_H
#define CMSMULTISELECTIONPOPUP_H

// INCLUDES
#include <aknnavide.h>
#include <AknIconArray.h>
#include <aknlistquerydialog.h>

// CLASS DECLARATION

/**
 * CMSMultiselectionPopup dialog class
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMultiselectionPopup ) : public CAknListQueryDialog
    {
public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aItemArray, list item array
     * @param aSelectionIndexArray, currently selected items
     * @param aHeading, popup heading
     * @return CMSMultiselectionPopup* the new CMSMultiselectionPopup object
     */
    static CMSMultiselectionPopup* NewL(
        CDesCArrayFlat* aItemArray,
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        const TDesC& aHeading );

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aItemArray, list item array
     * @param aSelectionIndexArray, currently selected items
     * @param aHeading, popup heading
     * @return CMSMultiselectionPopup* the new CMSMultiselectionPopup object
     */
    static CMSMultiselectionPopup* NewLC(
        CDesCArrayFlat* aItemArray,
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        const TDesC& aHeading );                

    /**
     * Destructor.
     */
    virtual ~CMSMultiselectionPopup();

public:

    /**
     * Updates the list of items
     *
     * @since S60 5.1
     * @param aItemArray, list item array
     * @return None
     */
    void UpdateAndDrawPopupL( CDesCArrayFlat* aItemArray );

protected:

// From base class CAknSelectionListDialog

    /**
     * From CAknSelectionListDialog
     * See base class defition
     */
    void PreLayoutDynInitL();

    /**
     * From CAknSelectionListDialog
     * See base class defition
     */
    TBool OkToExitL( TInt aButtonId );


private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aSelectionIndexArray, selected indexes
     * @param aHeading, popup heading
     * @return None
     */
    CMSMultiselectionPopup(
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        const TDesC& aHeading );

    /**
     * Performs the second phase construction.
     *
     * @since S60 5.1
     * @param aItemArray, list items
     * @param aSelectionIndexArray, selected indexes
     * @return None
     */
    void ConstructL( CDesCArrayFlat* aItemArray,
                     CListBoxView::CSelectionIndexArray*
                                            aSelectionIndexArray);

   /**
    * Loads a possibly skinned icon and adds it to icon array
    *
    * @since S60 5.1
    * @param CAknIconArray, array of icons
    * @param MAknsSkinInstance, skin instance
    * @param TDesC, reference to icon file
    * @param TAknsItemID, skinned icon id
    * @param TInt, bitmap id
    * @param TInt, bitmap mask id
    * @return None
    */
    void AppendIconToArrayL( CAknIconArray* aArray,
                             MAknsSkinInstance* aSkin,
                             const TDesC& aMbmFile,
                             const TAknsItemID& aID,
                             TInt aBitmapId,
                             TInt aMaskId ) const;

   /**
    * Creates listbox items and sets item array
    *
    * @since S60 5.1
    */
    void CreateAndSetListboxItemsL();

private:

    /**
     * Dialog listbox
     */
    CEikListBox*                      iListBox;         // owned
    /**
     * Item array
     */    
    CDesCArrayFlat*                   iItemArray;       // not owned
    /**
     * Selected item indexes
     */
    CArrayFix<TInt>*                  iSelectionIndexes;// not owned
    /**
     * Dialog title
     */
    TPtrC                             iHeading;    
    };
#endif

// End of File
