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
* Description:  CMSMultiselectionDialog class implementation
*
*/


#ifndef MSMULTISELECTIONDIALOG_H
#define MSMULTISELECTIONDIALOG_H

// INCLUDES
#include <AknDialog.h>
#include <aknselectionlist.h>
#include <aknsfld.h>
#include <aknnavide.h>
#include <AknIconArray.h>

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CAknSearchField;
class CSelectionItemList;
class CAknTitlePane;

// CLASS DECLARATION

/**
 * CMSMultiselectionDialog dialog class
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMultiselectionDialog ) :
    public CAknSelectionListDialog
    {
    public:
        
        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aItemArray, list of items
         * @param aTitle, dialog title
         */
        static CMSMultiselectionDialog* NewL(
                            CSelectionItemList* aItemArray,
                            const TDesC& aTitle );

        /**
         * Two-phased constructor.
         *
         * @since S60 5.1
         * @param aItemArray, list of items
         * @param aTitle, dialog title
         */
        static CMSMultiselectionDialog* NewLC(
                            CSelectionItemList* aItemArray,
                            const TDesC& aTitle );                            

        /**
         * Destructor.
         */
       virtual ~CMSMultiselectionDialog();


    protected:  // New functions

        void ConstructL( const TDesC& aTitle );

        /**
         * Loads a possibly skinned icon and adds it to icon array
         *
         * @since S60 5.1
         * @param aArray, array of icons
         * @param aSkin, skin instance
         * @param aMbmFile, reference to icon file
         * @param aID, skinned icon id
         * @param aBitmapId, bitmap id
         * @param aMaskId, bitmap mask id
         */
        void AppendIconToArrayL( CAknIconArray* aArray,
                                 MAknsSkinInstance* aSkin,
                                 const TDesC& aMbmFile,
                                 const TAknsItemID& aID,
                                 TInt aBitmapId,
                                 TInt aMaskId) const;


        /**
         * Appends first item ("share all files") to the listbox
         *
         * @since S60 5.1
         * @param CDesCArray, listbox array
         * @return None
         */
        void AppendShareAllSelectionL(CDesCArray* aListBoxArray) const;


    protected:

// From base class CAknSelectionListDialog
        
        /**
         * From CAknSelectionListDialog
         * See base class definition
         */
        void PreLayoutDynInitL();

        /**
         * From CAknSelectionListDialog
         * See base class definition
         */
        TBool OkToExitL( TInt aButtonId );


    private:
        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aItemArray, list item array
         * @return None
         */
        CMSMultiselectionDialog( CSelectionItemList* aItemArray );

        /**
         * Generates text array for listbox
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void GenerateInternalArrayL();

        /**
         * Sets checkbox statas
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void SetAllSelectionsL();

        /**
         * Deselects other items if first one is selected
         *
         * @since S60 5.1
         * @param None
         * @return None         
         */
        void HandleSelectionsL();

        /**
         * Updates item array selections
         *         
         * @since S60 5.1
         * @param None
         * @return None
         */
        void UpdateAllSelectionsL();
        
        /**
         * handle for the pointer device input
         *
         * @since S60 5.1
         * @param aPointerEvent, pointerevent
         * @return None                           
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );


        /**
         * Handeles selection key press
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void HandleSelectionKeyPressL();

    private:
    
        /**
         * dialog control
         */
        CAknColumnListBox*              iListBox;           // owned
        
        /**
         * title pane
         */
        CAknTitlePane*                  iTitlePane;         // not owned
       
        /**
         * previous view title
         */
        HBufC*                          iPreviousTitle;     // owned
       
        /**
         * selected item - needed for creation of CAknSelectionListDialog
         */
        TInt                            iSelectedItem;
       
        /**
         * array for listbox items
         */
        CSelectionItemList*             iItemArray;         // not owned

        /**
         * internal array for listbox item texts
         */
        CDesCArrayFlat*                 iInternalItemArray; // owned
        
        /**
         * First item selection status
         */
        TBool                           iFirstItemSelected;
        /**
         * iNaviPane
         */
        CAknNavigationControlContainer* iNaviPane;          // not owned
        
        /**
         * navi decorator
         */
        CAknNavigationDecorator*    iNaviDecorator;         // owned
    };

#endif  // MSMULTISELECTIONDIALOG_H

// End of File
