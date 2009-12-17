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
* Description:      Header file for the Setup wizard class
*
*/






#ifndef CUPNPAPPWIZARD
#define CUPNPAPPWIZARD

// INCLUDES
#include <e32base.h>
#include <AknWaitNoteWrapper.h>  // base class MAknBackgroundProcess

#include "upnpfilesharingengine.h" //CUPnPFileSharingEngine & THomeConnectMediaType
#include "upnpfilesharingengineobserver.h" //MUPnPFileSharingEngineObserver



// CONSTANTS
const TInt KUPnPAlwaysAskInd = 0;
const TInt KUPnPCreateNewInd = 1;


// FORWARD DECLARATIONS
class CAknViewAppUi;
class CUPnPSettingsEngine;
class CEikonEnv;
class CUPnPMultiselectionUi;
// CLASS DECLARATION

/**
 * CUPnPAppWizard class
 *
 * Class implementing the functionality of the setup wizard
 * @since S60 v3.1
 * @lib upnpappwizard.lib
 */
class CUPnPAppWizard : public CBase,
                       private MUPnPFileSharingEngineObserver,
                       MAknBackgroundProcess
    {
public:     // Constructors and destructor

    enum TSharingStarting
        {
        ENotActive,
        EStartingSharing,
        ESharingStarted
        };

    enum TShareArrayState
        {
        ESetMarked,
        EResetArray,
        EAddToArray
        };
    
    /**
     * Two-phased constructor.
     * @since S60 v3.1
     * @param aFirstStepText, Text for the first wizard step, owned by \
client. If length of the aFirstStepText is 0 default text is used
     * @param aEng, Pointer to filesharing engine instance
     */
    IMPORT_C static CUPnPAppWizard* NewL( const TDesC& aFirstStepText,
                                          CUPnPFileSharingEngine* aEng );

    /**
     * Destructor.
     */
    virtual ~CUPnPAppWizard();

public:     // New functions

    /**
     * Starts wizard
     *
     * @since S60 v3.1
     * @return TInt, Error code
     */
    IMPORT_C TInt StartL();

    /**
	 * Modifies iShareImgAndVideoArray and iShareMusicArray arrays
	 *
	 * @param aArrayState
	 * @param aValue
	 * @param aUiDlg
	 */
    void ModifyMarkedArrayL( TShareArrayState aArrayState 
    		               , TInt aValue = 0
    		               , CUPnPMultiselectionUi* aUiDlg = NULL );

private:
    /**
     * CSelectionStorage class
     *
     * Private class for user selections and handling their 
     * storage and retrieval
     * @since S60 v3.1
     */
    NONSHARABLE_CLASS( CSelectionStorage ) : public CBase
        {
        public :

        enum TSharingStartResult
            {
            ESharingStarted,
            ENothingToShare,
            ESharingStartFailed
            };
        /**
        * C++ default constructor
        * @param aSetEng. Pointer to CUPnPSettingsEngine, not owned
        * @param aFileEng. Pointer to CUPnPFileSharingEngine, not owned
        */
        CSelectionStorage( CUPnPSettingsEngine* aSetEng,
                       CUPnPFileSharingEngine* aFileEng );
        /**
        * Destructor.
        */
        virtual ~CSelectionStorage();

        /**
        * Store locally users selected device name
        * @since S60 v3.1
        * @param aDeviceName. The new name
        */
        void SetDeviceNameL( TDes& aDeviceName );

        /**
        * Get device name from local store of from cenrep
        * @since S60 v3.1
        * @param aDeviceName. Descriptor to store the name
        * @return success of the operation
        */
        TInt GetDeviceNameL( TDes& aDeviceName );

        /**
        * Sets the IAP id of the user selected IAP
        * @since S60 v3.1
        * @param aIapId. Iap id of the selected IAP
        * @param aIndexForUi, Index of the iap in UI list
        */
        void SetIapId( const TInt aIapId,
                   const TInt aIndexForUi );
        /**
        * Function for getting the iapid from local storage or from cenrep
        * @since S60 v3.1
        * @param aIapId. Iap id of the selected IAP
        * @return success of the operation
        */
        TInt GetIapId( TInt& aIapId );

        /**
        * Initialize Iap name and iap id arrays with some fixed texts
        * @since S60 v3.1
        * @param aIapNameArr. Array pointer for the names
        * @param aIApIdArr. Array pointer for iap IDs
        */
        void InitializeIapArraysL( CDesCArray* aIapNameArr,
                               RArray<TInt64>& aIapIdArr ) const;

        /**
        * Stores the user selections from filesharing popus to local storage
        * @since S60 v3.1
        * @param aMedia. Type of media
        * @param aSelections. Array of the selections user has made.
        * Stored only locally
        */
        void SetSharedContent( THomeConnectMediaType aMedia,
                               CArrayFix<TInt>* aSelections );

        /**
        * Stores the devicename and IAP from local store to cenrep
        * @since S60 v3.1
        * @return success of the operation
        */
        TInt StoreSettingsL();
        
        /**
        * Stores the selected image and video albums from local store to 
        * cenrep
        * @since S60 v3.2
        * @return error code
        */
        TInt ImageStoreSharedFilesL();       

        /**
        * Gets the users sharing selections as indexes from local storage
        * @since S60 v3.1
        * @param aMedia. The kind of selections to get
        * @param aSelections. Array for selections
        */
        void GetSelectionsL( 
        THomeConnectMediaType aMedia,
        CArrayFix<TInt>* aSelections );

        /**
        * Determines if the is need to start sharing
        * @since S60 v3.1
        * @return ETrue if sharing needs to be activated.
        */
        TBool HasSomethingToShare() const;

        /** 
        * Gets the currently selected IAP index for the listbox UI
        * @since S60 v3.1
        * @param aCurrentSelection. The new current selection
        * @param aIapIdArr. Array of IAP ids of the IAPs shown in listbox

        */
        void GetCurrentSelection( TInt& aCurrentSelection,
                              RArray<TInt64>& aIapIdArr );

        /** 
        * Gets the WAP iap associated to given IAPid
        * @since S60 v3.1
        * @param aIAPId. Id to match
        * @return. The id of the corresponding WAP record
        */
        TInt ConvertIAPIdL( TInt aIAPId );
                       
    private :
        /**
        * Pointer to UPnP settings engine
        * Not owned
        */
        CUPnPSettingsEngine* iSetEng;

        /**
        * Pointer to UPnP File sharing engine
        * Not owned
        */
        CUPnPFileSharingEngine* iFileEng;

        /**
        * Pointer to UPnP device name
        * Not owned
        */
        HBufC* iDeviceName;
        
        /**
        * Users selected IAP id
        *
        */
        TInt iIapId;

        /**
        * Index for home network UI, to show the correct item as selected 
        *
        */
        TInt iIndexForUi;
        
        /**
        * The selection of Image& video sharing for Home Network UI
        */
        TBool iVisualImageSelection;
        
        /**
        * The selection of music sharing for Home Network UI
        */
        TBool iVisualMusicSelection;
        
        /**
        * The Array of the selected indexes user has made for sharing images 
        * and videos
        * owned
        */
        CArrayFix<TInt>* iImageSelectionArray;
        /**
        * The Array of the selected indexes user has made for sharing music 
        * owned
        */
        CArrayFix<TInt>* iMusicSelectionArray;       
        
        };


    /**
     * Internal states of wizard 
     */
    enum TWizardStep
        {
        EInfo1 = 0,
        EInfo2,
        EDeviceName,
        EInfo4,
        EAccessPoint,
        EInfo6,
        EShareImages,
        EShareMusic,
        EInfo9,
        EStoreSettings,
        EFinished
        };

    /**
     * C++ default constructor
     */
    CUPnPAppWizard();

    /**
     * Show the information note with OK BACK buttons
     * @since S60 v3.1
     * @param TInt, resource ID
     * @return keycode of the button user has pressed.
     */
    TInt ShowInfoStepL( TInt aMain ) const;

    /**
     * Show the first step. The text is given in constructor
     * @param TInt, resource ID
     * @return keycode of the button user has pressed.
     */
    TInt ShowFirstStepL( TInt aMain ) const;
    /**
     * Shows popup with text and header and edit box
     * @since S60 v3.1
     * @param TInt, resource ID
     * @return keycode of the button user has pressed.
     */
    TInt ShowTextStepL( TInt aMain );

    /**
     * Show the information note without buttons
     * @since S60 v3.1
     * @param TInt, resource ID

     */
    void ShowInfoNoteL( TInt aMain ) const;

    /**
     * Show the UI dialog with selection list
     * @since S60 v3.1
     * @param TInt, resource ID
     * @return keycode of the button user has pressed.
     */
    TInt ShowListStepL( TInt aMain );


    /**
     * Determines the next wizard step
     * @since S60 v3.1
     * @param TInt& The keycode of the users selection from previous dialog

     */
    void SetNextStep( const TInt& aSelection );

    /**
     * Shows the step for setting shared content
     * @since S60 v3.1
     * @param TInt, resource ID
     * @return keycode of the button user has pressed.
     */
    TInt ShowMultiselectionStepL( TInt aText );

    /**
     * Show error note with given resource id
     * @since S60 v3.1
     * @param TInt, resource ID
     */
    void ShowErrorNoteL( TInt aMain ) const;

    /**
     * Show wait note with given resource id
     * @since S60 v3.1
     * @param TInt, resource ID
     */
    void ShowWaitNoteL( TInt aMain );

private: //From MUPnPFileSharingEngineObserver
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

private:
    /** 
     * Shows progress note to indicate progress of sharing files
     * @since S60 v3.1
     * @param TInt, The number of objects
     */
    void  ShowProgressNoteL( TInt aObjectCount );

    /**
     * Called when waitnote wrapper is finished
     * From MProgressDialogCallback
     */
    void DialogDismissedL( TInt aButtonId );

    /**
     * EPOC default constructor.
     */
    void ConstructL( const TDesC& aFirstStepText, 
                     CUPnPFileSharingEngine* aEng );

    void StepL();

    TBool IsProcessDone() const;

    /**
     * Display info note after starting sharing content
     * @since S60 v3.1
     * @param TInt, Id of note header resource
     * @param TInt, Id of note message resource
     * @param TInt, Id of note resource
     */
    void DisplayInfoL(
        TInt aHeaderResourceId,
        TInt aMessageResourceId,
        TInt aDialogResourceId );


private:    // Data

    CAknViewAppUi* iAppUi;  // Not owned
    TInt iResFileOffset;   // Resource file offset
    CEikonEnv* iCoeEnv; // Not owned
    /**
     * The position of the wizard
     */
    TInt iStep;
    /**
     * The class for retrieving and setting shared items
     */
    CUPnPFileSharingEngine* iFileEng;
    /**
     * The class for setting  the IAP id and deviceName
     */
    CUPnPSettingsEngine* iSetEng;
    /**
     * The storage for the user selections
     */
    CSelectionStorage* iSelections;
    
    /**
     * Wait used in asynchronous operations
     * Started when there is operation ongoing in filesharingengine
     */
    CActiveSchedulerWait iWait;

    /** 
     * Array for IAP names
     */
    CDesCArray* iIapNameArr;
    
    /**
     * Status of the sharing when starting wizard
     * ETrue = Sharing on
     */
    TBool iSharingState;
   
    /**
     * Text for the first wizard step 
     *
     */
    HBufC* iFirstStepText;

    /** 
     * Indicator if starting sharing is ongoing
     */
    TInt iStartingSharing;

    TBool iDialogDismissed;
    
    /**
     * stores indexes if img and/or video is shared marked 
     */
    CArrayFix<TInt>* iShareImgAndVideoArray;
    
    /**
     * stores indexes if music is shared marked 
     */
    CArrayFix<TInt>* iShareMusicArray;
    
    /**
     * Flag the FeatureManager is initialized or not
     */
    TBool iFeatureManagerInitialized;    
    };

#endif  // CUPNPAPPWIZARD

// End of File
