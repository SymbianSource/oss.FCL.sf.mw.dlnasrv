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
* Description:      CUPnPAppMultiselectionDialog class definition
*
*/






#ifndef UPNPAPPMULTISELECTIONDIALOG_H
#define UPNPAPPMULTISELECTIONDIALOG_H

// INCLUDES
#include <AknDialog.h>
#include <aknselectionlist.h>
#include <aknsfld.h>
#include <aknnavide.h>
#include <akntitle.h> //for status pane
#include <AknIconArray.h>
#include <AknWaitNoteWrapper.h>

#include "upnpfilesharingengine.h" // included to know THomeConnectMediaType
#include "upnpmultiselectionutility.h"



// FORWARD DECLARATIONS
class CAknColumnListBox;

// CLASS DECLARATION

/**
* CUPnPAppMultiselectionDialog dialog class
* @since S60 3.1
*/
NONSHARABLE_CLASS( CUPnPAppMultiselectionDialog ) : 
    public CAknSelectionListDialog,
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
        static CUPnPAppMultiselectionDialog* NewL(
                            THomeConnectMediaType aMediaType,
                            CUPnPFileSharingEngine* aFileSharingEngine,
                            CArrayFix<TInt>* aSelectionIndexArray );

        /**
        * Destructor.
        */
       virtual ~CUPnPAppMultiselectionDialog();


    protected:  // New functions
        void ConstructL(THomeConnectMediaType aMediaType,
                        CUPnPFileSharingEngine* aFileSharingEngine,
                        CArrayFix<TInt>* aSelectionIndexArray );

    protected:  // Functions from base classes
        /**
        * From MAknBackgroundProcess, handles backround processing
        * @since S60 3.1
        */
        virtual void StepL();

        /**
        * From MAknBackgroundProcess, handles backround processing
        * @since S60 3.1
        * @return TBool, ETrue when the process is done
        */
        virtual TBool IsProcessDone() const;

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
        void PostLayoutDynInitL();

        /**
        * From CAknSelectionListDialog, handles key events.
        * @since S60 3.1
        * @param TKeyEvent, Event to handled.
        * @param TEventCode, Type of the key event.
        * @return TKeyResponse, response code 
        * (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        TKeyResponse OfferKeyEventL (const TKeyEvent &aKeyEvent,
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
        CUPnPAppMultiselectionDialog();

    private: //data
        // dialog control item
        CAknColumnListBox*              iListBox; // not owned
        // Mediatype selected from listbox
        THomeConnectMediaType           iMediaType;
        // File sharing engine member
        CUPnPFileSharingEngine*         iFileSharingEngine; //not owned
        // title pane
        CAknTitlePane*                  iTitlePane;
        // selected item - needed for creation of CAknSelectionListDialog
        TInt                            iSelectedItem;

        CAknWaitNoteWrapper*            iWaitNoteWrapper;
        // array of marked (shared) items.
        CArrayFix<TInt>*           iMarkedItems; //not owned

        CUpnpMultiselectionUtility*      iUtility; //owned
        
        CAknFilteredTextListBoxModel* iModel; //not owned

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
    };

#endif // UPNPAPPMULTISELECTIONDIALOG_H

// End of File
