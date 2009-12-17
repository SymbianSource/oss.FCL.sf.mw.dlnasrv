/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      File sharing list class definition for Home Network
 *
*/






#ifndef UPNPAPPFILESHARINGLIST_H
#define UPNPAPPFILESHARINGLIST_H

//  INCLUDES
#include <aknsettingitemlist.h> // base class
#include <AknWaitNoteWrapper.h> // base class MAknBackgroundProcess

// FORWARD CLASS DECLARATION
class CUPnPFileSharingEngine;
class CUPnPAppWizard;

// CONSTANTS
const TInt KMaxTextSettingItemLenght = 128;

/**
 *  Settings list class declaration
 *  @since S60 3.1
 */
NONSHARABLE_CLASS( CUPnPAppFileSharingList )
    : public CAknSettingItemList
    , public MAknBackgroundProcess
    {
private:
    /**
     * The internal phases of wait note when changing sharing state
     */
    enum TSharingStarting
        {
        EPhaseNotActive,
        EPhaseProcessing,
        EPhaseCompleted
        };
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUPnPAppFileSharingList* NewL(
        CUPnPFileSharingEngine* aFileSharingEngine );

    /**
     * Destructor.
     */
    virtual ~CUPnPAppFileSharingList();

public: // New functions
    /**
     * From CAknSettingItemList, this launches the setting page.
     * @since S60 3.1
     * @param aIndex, index of selected list item
     * @param aCalledFromMenu, indicates if editing is called
     *        trough menu or from keyboard
     */
    void  EditItemL (TInt aIndex, TBool aCalledFromMenu);

    /**
     * Reads data from storage to member variables
     * @since S60 3.1
     */
    void LoadDataFromEngineL();

public: //From MUPnPFileSharingEngineObserver

    /**
     * Function informs when file sharing was enabled or disabled
     * @since S60 3.1
     * @param aEngine File sharing engine
     * @param aError, error code
     * @param aPhase ETrue if current phase is sharing activation,
     * otherwise EFalse
     */
    void HandleSharingStatus( 
        CUPnPFileSharingEngine& aEngine, 
        TInt aError, 
        TBool aPhase );

    /**
     * Callback to inform if connection has been lost
     * @since S60 3.1
     * @param aEngine File sharing engine
     */ 
    void HandleSharingConnectionLost( 
        CUPnPFileSharingEngine& aEngine );

public:

    /**
     * Processes progress information and sets secondary texts according to
     * received progress
     * @since S60 3.1
     * @param aProgressInfo, Type containg the progress information
     */
    void SetFilesharingTextsL(
        const RArray<TUpnpProgressInfo>& aProgressInfo );

    /**
     * Sets the default secondary texts if no sharing is ongoing
     * @since S60 3.1
     * @param aMediaType, Type of the media
     * @param aState, State of the selection
     */
    void SetDefaultSecondaryTextsL(
        const TInt& aMediaType,
        const TInt& aState );

    /**
     * Sets the secondary texts id there is sharing ongoing
     * @since S60 3.1
     * @param aMediaType, Type of the media
     * @param aProgress, The value of progress in percents
     */
    void SetProgressSecondaryTextsL(
        const TInt& aMediaType,
        const TInt& aProgress );

    /**
     * Update the visibity of item ,when the list is switched foregroud.
     */
     void UpdateVisibitySettingItemL();
     
protected:

    /**
     * From CAknSettingItemList
     */
    void HandleResourceChange( TInt aType );

    /**
     * From CoeControl
     */
    void FocusChanged(
        TDrawNow  aDrawNow );

protected:
    /**
     * From MAknBackgroundProcess
     */
    void  StepL();

    /**
     * From MAknBackgroundProcess
     */
    TBool  IsProcessDone() const;


private:

    /**
     * C++ default constructor.
     */
    CUPnPAppFileSharingList();

    /**
     * By default Symbian 2nd phase constructor is private.
     * @param aFileSharingEngine, engine pointer
     */
    void ConstructL( CUPnPFileSharingEngine* aFileSharingEngine );

    /**
     * From CAknSettingItemList, Framework method to create a setting item.
     * @since S60 3.1
     * @param aIdentifier,  list item id to be created
     * @return CAknSettingItem, pointer to list item
     */
    CAknSettingItem* CreateSettingItemL( TInt aIdentifier );

    /**
     * Create and show the wait note when starting sharing
     * @since S60 3.1
     */
    void ShowWaitNoteL();


private: // Data

    /**
     * list item text for visual albums
     */
    TBuf<KMaxTextSettingItemLenght>  iVisualAlbums;
    /**
     * list item text for playlists
     */
    TBuf<KMaxTextSettingItemLenght>  iPlayLists;
    /**
     * visibility setting item value
     */
    TInt iVisibility;
    /**
     *access point id setting item value
     */
    TInt iIapId;
    /**
     * engine pointer
     */
    CUPnPFileSharingEngine* iFileSharingEngine;
    /**
     * Indicator if wizard is run to update UI
     */
    TBool iWizardRun;
    /**
     * The state of sharing activation for progress note
     */
    TInt iStartingSharing;
    /**
     * Pointer to wizard
     */
    CUPnPAppWizard* iWizard;

    };

#endif      // UPNPAPPFILESHARINGLIST_H
// End of File
