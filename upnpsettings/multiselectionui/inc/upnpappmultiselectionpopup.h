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
* Description:      CUPnPAppMultiselectionPopup class definition
*
*/






#ifndef UPNPAPPMULTISELECTIONPOPUP_H
#define UPNPAPPMULTISELECTIONPOPUP_H

// INCLUDES
#include <AknDialog.h>
#include <aknselectionlist.h>
#include <aknsfld.h>
#include <aknnavide.h>
#include <akntitle.h> //for status pane
#include <AknIconArray.h>
#include <AknWaitNoteWrapper.h>
#include <aknlistquerydialog.h> //caknlistquerydialog
#include <aknsfld.h> //caknsearchfield

#include "upnpfilesharingengine.h" // included to know THomeConnectMediaType
#include "upnpmultiselectionutility.h"


// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;

// CLASS DECLARATION

/**
 * CUPnPAppMultiselectionPopup dialog class
 * @since S60 3.1
 */
NONSHARABLE_CLASS( CUPnPAppMultiselectionPopup ) : public CAknListQueryDialog,
                                    MAknBackgroundProcess
    {
public: // Constructors and destructor
        /**
         * Two-phased constructor.
         * @since S60 3.1
         * @param aMediaType. Media type for UI. Either Images Or videos or \
music.
         * @param aFileSharingEngine. Pointer to filesharingengine.
         * @param aSelectionIndexArray. Array for initial selection.
         */
    static CUPnPAppMultiselectionPopup* NewL(
        THomeConnectMediaType aMediaType,
        CUPnPFileSharingEngine* aFileSharingEngine,
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        const HBufC* aHeading);

    /**
     * stores marked items to iUiMarked
     * 
     * @param aMarkedItems
     */
    void SetMarkedItemsL(CArrayFix<TInt>& aMarkedItems);
    
    /**
     * Destructor.
     */
    virtual ~CUPnPAppMultiselectionPopup();


protected:

    // New functions
    void ConstructL(THomeConnectMediaType aMediaType,
                    CUPnPFileSharingEngine* aFileSharingEngine,
                    CListBoxView::CSelectionIndexArray* aSelectionIndexArray);



protected:  // Functions from base classes
    /**
     * From MAknBackgroundProcess, handles backround processing
     * @since S60 3.1
     */
    virtual void  StepL ();

    /**
     * From MAknBackgroundProcess, handles backround processing
     * @since S60 3.1
     * @return TBool, ETrue when the process is done
     */
    virtual TBool  IsProcessDone () const;

    /**
     * From CAknSelectionListDialog, handles layout initialization
     * @since S60 3.1
     */
    void PreLayoutDynInitL();

    /**
     * From CAknSelectionListDialog, handles initialization
     * that doesn't affect layout
     * @since S60 3.1
     */
    void  PostLayoutDynInitL ();

    /**
     * From CAknSelectionListDialog, handles key events.
     * @since S60 3.1
     * @param TKeyEvent, Event to handled.
     * @param TEventCode, Type of the key event.
     * @return TKeyResponse, response code
     * (EKeyWasConsumed, EKeyWasNotConsumed).
     */
    TKeyResponse  OfferKeyEventL (const TKeyEvent &aKeyEvent,
                                  TEventCode aType);
    /**
     * From CAknSelectionListDialog, handles focus changes of the dialog
     * @since S60 3.1
     * @param TInt, pressed button id
     * @return TBool, ETrue if ready to close dialog, EFalse otherwise
     */
    TBool OkToExitL( TInt aButtonId );


private:
    /**
     * C++ default constructor.
     */
    CUPnPAppMultiselectionPopup(
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        const HBufC* aHeading );

private: //data
    // dialog control item

    CEikListBox*              iListBox; //not owned

    CAknFilteredTextListBoxModel* iModel; //not owned
    
    

    // Mediatype selected from listbox
    THomeConnectMediaType           iMediaType;
    // File sharing engine member
    CUPnPFileSharingEngine*         iFileSharingEngine; //not owned
    // Share all selection
    TInt*                            iShareAllSelection;
    // selected album indexes
    CArrayFixFlat<TInt>*            iSelectedAlbums;
    // title pane
    CAknTitlePane*                  iTitlePane;
    // selected item - needed for creation of CAknSelectionListDialog
    TInt                            iSelectedItem;
    // array for playlists
    CDesCArrayFlat*                 iFileArray;
    // wait note
    CAknWaitNoteWrapper*            iWaitNoteWrapper;
    // array of marked (shared) items.
    CArrayFix<TInt>*           iMarkedItems;
    // If this is not NULL it is used instead of "normal" calculation of
    // selections
    CArrayFix<TInt>* iPreviousSelection;
    
    // get marked items from appwizard
    CArrayFix<TInt>* iUiMarked;
    
    // The pointer to multiselectiondialog
    CUpnpMultiselectionUtility* iUtility;
    
    CDesCArray* iSettingsTextArray; //not owned

    // Variable to transfer information about wait note end
    TBool iDone;

    /**
     * Msk's label. (Owned)
     */
    HBufC* iMskSelectText;

    /**
     * Msk's label. (Owned)
     */
    HBufC* iMskUnselectText;

    /**
     * Internal state
     */
    enum TState
        {
        ELoadNone = 0,
        ELoadContent,
        ELoadIndexes,
        ELoadDone
        };
    TState iState;

    // Heading text
    const HBufC*                             iHeading; //not owned
    };
#endif

// End of File
