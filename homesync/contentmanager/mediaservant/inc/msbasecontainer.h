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
* Description:  Definition for MediaServant document class
*
*/



#ifndef __MSBASECONTAINER_H__
#define __MSBASECONTAINER_H__

// INCLUDES
#include <coecntrl.h>
#include <aknsfld.h>


// FORWARD DECLARATIONS
class CEikListBox;
class CEikTextListBox;
class CEikColumnListBox;
class CEikFormattedCellListBox;
class CTextListBoxModel;
class CAknIconArray;

// CLASS DECLARATION

/**
 * CMSBaseContainer  container control class.
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSBaseContainer ): public CCoeControl
    {

    public:

        /**
         * CMSBaseContainer default constructor.
         */
        CMSBaseContainer();
        
        /**
         * ~CMSBaseContainer
         * Destructor.
        */
        virtual ~CMSBaseContainer();
        
        /**
         * Shows note
         * @since S60 3.1
         * @param aResource, text to be shown
         * @param aError, error code
         * @return None
         */
        void ShowNoteL ( TInt aResource, TInt aError );

        /**
         * Sets default context pane icon
         * @since S60 3.1
         * @param None
         * @return None
         */
        void SetDefaultContextPaneIconL();

    protected:

        /**
         * SetListBoxFromResourceL
         * Sets listbox from resource using ConstructFromResourceL() of
         * CEikColumnListBox class.
         * @since S60 3.1
         * @param aListBox Pointer of listbox.
         * @param aResourceId Resource ID of listbox.
         * @return None
         */
        void SetListBoxFromResourceL(
            CEikColumnListBox* aListBox,
            const TInt aResourceId );

        /**
         * SetListBoxFromResourceL
         * Sets listbox from resource using ConstructFromResourceL() of
         * CEikFormattedCellListBox class.
         * @since S60 3.1
         * @param aListBox Pointer of listbox.
         * @param aResourceId Resource ID of listbox.
         * @return None
         */
        void SetListBoxFromResourceL(
            CEikFormattedCellListBox* aListBox,
            const TInt aResourceId );

        /**
         * CreateScrollbarL
         * Creates scrollbar.
         * @since S60 3.1
         * @param aListBox Pointer of created listbox.
         * @return None
         */
        void CreateScrollbarL( CEikListBox* aListBox );

        /**
         * CreateFindBoxL
         * Creates FindBox and returns it's pointer.
         * @since S60 3.1
         * @param aListBox Pointer of listbox.
         * @param aModel Pointer of model.
         * @param aStyle Style of FindBox.
         * @return Pointer of CAknSearchField.
         */
        CAknSearchField* CreateFindBoxL(
            CEikListBox* aListBox,
            CTextListBoxModel* aModel,
            CAknSearchField::TSearchFieldStyle aStyle );

        /**
         * Appends item to listbox array
         * @since S60 3.1
         * @param aListBox, pointer of listbox
         * @param aModel, listbox model
         * @param aIcon, icon index with "\t"
         * @param aID, skinned icon id
         * @param aBitmapId, bitmap id
         * @param aMaskId, bitmap mask id
         * @return None
         */
        void AppendItemToListBoxArrayL( CEikListBox* aListBox,
                                        CTextListBoxModel* aModel,
                                        TInt aIconIndex,
                                        TDesC& aPrimaryText,
                                        TDesC& aSecondaryText );

        /**
         * Appends item to listbox array
         * @since S60 3.1
         * @param aListBox, pointer of listbox
         * @param aModel, listbox model
         * @param aIconIndex, icon index
         * @param aItemText, text to be displayed
         * @return None
         */
        void AppendItemToListBoxArrayL( CEikListBox* aListBox,
                                        CTextListBoxModel* aModel,
                                        TInt aIconIndex,
                                        TDesC& aItemText );

        /**
         * Loads a possibly skinned icon and adds it to icon array
         * @since S60 3.1
         * @param aArray, array of icons
         * @param aSkin, skin instance
         * @param aMbmFile, reference to icon file
         * @param aID, skinned icon id
         * @param aBitmapId, bitmap id
         * @param aMaskId, bitmap mask id
         * @return None
         */
        void AppendIconToArrayL(CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId);

        /**
         * Sets context pane icon         
         * @since S60 3.1
         * @param aIconBitmap, bitmap
         * @param aIconmask, bitmap mask
         * @return None
         */
        void SetContextPaneIconL( const CFbsBitmap* aIconBitmap,
                                  const CFbsBitmap* aIconMask );    
        
    private:
        
        /**
         * control environment
         */
        CCoeEnv*               iCoeEnv;
    };

#endif // __MSBASECONTAINER_H__

// End of File
