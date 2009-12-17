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
* Description:      Multiselection UI class definition
*
*/







#ifndef CUPNPMULTISELECTIONUI
#define CUPNPMULTISELECTIONUI

// INCLUDES
#include <e32base.h>
#include "upnpfilesharingengine.h" // included to know THomeConnectMediaType
#include <eiklbv.h> // for CListBoxView::CSelectionIndexArray
// CONSTANTS

// FORWARD DECLARATIONS
class CAknViewAppUi;
class CEikonEnv;
class CUPnPAppMultiselectionDialog;
class CUPnPAppMultiselectionPopup;


// CLASS DECLARATION

/**
 * CUPnPMultiselectionUi class
 *
 * UI for showing the filesharing window either in popup or dialog style
 *
 * @lib upnpmultiselectionui.lib
 */
class CUPnPMultiselectionUi : public CBase
    {

public:     // Constructors and destructor

    enum TUiStyle
        {
        EDialog,
        EPopup
        };

    /**
     * Two-phased constructor.
     * @since S60 3.1
     * @param aMediaType. Media type for UI. Either Images&videos or music.
     * @param aFileSharingEngine. Pointer to filesharingengine.
     * @param aAlbumContainer. Container for the items user has selected.
     * @param aVisualSharingState. State of the sharing for the home.
     * network UI
     * @param aSelectionIndexArray. Array for initial selection.
     * @param aUiStyle. Describes if popup or dialog style ui is used.
     */
    IMPORT_C static CUPnPMultiselectionUi* NewL(
        THomeConnectMediaType aMediaType,
        CUPnPFileSharingEngine* aFileSharingEngine,
        TInt& aVisualSharingState,
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray = NULL,
        TUiStyle aUiStyle = EDialog,
        const HBufC* aHeading = NULL
        );
 
    /**
     * Sets marked items to popup dialog
     * 
     * @param aMarkedItems
     */
    IMPORT_C void SetMarkedItemsL( CArrayFix<TInt>& aMarkedItems );
    
    /**
     * Destructor.
     */
    virtual ~CUPnPMultiselectionUi();

public:

    /**
     * Executes the file sharing UI as a dialog style
     *
     * @since S60 3.1
     * @return TInt, Error code
     */
    IMPORT_C TInt ExecuteDlgLD();

    /**
     * Executes the file sharing UI as a popup style
     *
     * @since S60 3.1
     * @return TInt, Error code
     */
    IMPORT_C TInt ExecutePopupLD();

private:

    /**
     * C++ default constructor
     */
    CUPnPMultiselectionUi();


    /**
     * Symbian default constructor.
     */
    void ConstructL(
        THomeConnectMediaType aMediaType,
        CUPnPFileSharingEngine* aFileSharingEngine,
        TInt& aVisualSharingState,
        CListBoxView::CSelectionIndexArray* aSelectionIndexArray,
        TUiStyle aStyle,
        const HBufC* aHeading
        );


private:    // Data
    /**
     * Resource file offse
     */
    TInt                        iResFileOffset;   // Resource file offset
    /**
     * Pointer to CEikonEnv
     */
    CEikonEnv*                  iCoeEnv;
    /**
     * Pointer to multiselection dialog
     */
    CUPnPAppMultiselectionDialog* iDlg;
    /**
     * Pointer to multiselection dialog
     */
    CUPnPAppMultiselectionPopup* iPopup;
    
    /**
     * Flag the FeatureManager is initialized or not
     */
    TBool                       iFeatureManagerInitialized;  
    };

#endif  // CUPNPMULTISELECTIONUI

// End of File
