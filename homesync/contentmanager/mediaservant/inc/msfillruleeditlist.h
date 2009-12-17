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
* Description:  Definition for MediaServant fill rule editor class
*
*/


#ifndef C_MSFILLRULEEDITLIST_H
#define C_MSFILLRULEEDITLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

#include "cmcommontypes.h"
#include "msengineobserver.h"
#include "msparameteragent.h"
#include "mediaservant.hrh"

// FORWARD DECLARATIONS
class CMSAppUi;
class CMSFillRuleEditView;
class CCmFillRuleContainer;
class CCmFillRule;
class CCmMediaServerFull;
class CSelectionItemList;
class CMSMetadataCollector;
class CCmSqlPropertyContainer;

// CLASS DECLARATION

/**
 * CMSFillRuleEditList general class. This class is used to
 * create setting items to several setting item lists.
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSFillRuleEditList ): public CAknSettingItemList,
                           public MMSEngineObserver
    {
    // process states
    enum TMSFillStates
        {
        EMSIdle = 0,
        EMSFillPreprocess,
        EMSFetchMetadata
        };

    public: // Constructor and destructor

        /**         
         * two-phase constructor.         
         */
        static CMSFillRuleEditList* NewL( CMSAppUi& aAppUi,
                                          CMSFillRuleEditView& aView );

        /**
         * two-phase constructor. 
         */
        static CMSFillRuleEditList* NewLC( CMSAppUi& aAppUi,
                                          CMSFillRuleEditView& aView );
                                          
        /**
         * Destructor.
         */
        virtual ~CMSFillRuleEditList();

        /**
         * Opens the selected listbox item
         * @since S60 5.1
         */
        void OpenSelectedListboxItemL();

        /**
         * Removes rule
         * @since S60 5.1
         */
        void RemoveCurrentRuleL();

        /**
         * Tests if rule is additional
         * @since S60 5.1
         * @return TBool, ETrue if additional
         */
        TBool IsCurrentRuleAdditional();

        /**
         * List media type
         * @since S60 5.1
         * @return TCmMediaType, media type of the list
         */
        TCmMediaType ListMediaType();

        /**
         * List media type
         * @since S60 5.1
         * @param aName name of fill rule
         * @return TBool, whether names are same
         */
        TBool CheckSameNameL( const TDesC& aName ) const;

// From base class CAknSettingItemList

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void ActivateL();

    protected:
    
// From base class CAknSettingItemList

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        CAknSettingItem* CreateSettingItemL( TInt aSettingId );

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void  HandleListBoxEventL ( CEikListBox *aListBox,
                                    TListBoxEvent aEventType);

        /**
         * From CAknSettingItemList
         * See base class definition
         */
        void EditItemL (TInt aIndex, TBool aCalledFromMenu);
        
// From base class CCoeControl

        /**
         * From CCoeControl
         * See base class definition
         */
        void SizeChanged();

        /**
         * From CCoeControl
         * See base class definition
         */
        void HandleResourceChange( TInt aType );

        /**
         * From CCoeControl
         * See base class definition
         */
        void GetHelpContext(TCoeHelpContext& aContext) const;        

// From base class MMSEngineObserver

        /**
         * From MMSEngineObserver
         * See base class definition
         */
        void ReadyL( TCmService aService, TInt aError );

    private:

        /**
         * Reads rules associated to the template
         * @since S60 5.1
         * @param aTemplate, Template ID
         */
        void ReadTemplateRulesL( TInt aTemplate );

        /**
         * Adds fill rule to the list
         * @since S60 5.1
         */
        void AddFillRuleL();

        /**
         * Sets default value defined by template
         * @since S60 5.1
         * @param aId, setting id
         * @param aValue, default value
         */
        void SetDefaultValue( TInt aId, TInt aValue );

        /**
         * Sets list type
         * @since S60 5.1
         * @param aType, command id
         */
        void SetListType( TInt aType );

        /**
         * Get values for the setting list
         * @since S60 5.1
         */
        void GetRuleValuesL();

        /**
         * Deletes rule
         * @since S60 5.1
         * @param aField, metadatafield to be deleted
         */
        void DeleteRule( TCmMetadataField aField );

        /**
         * Get values for the setting list
         * @since S60 5.1
         * @param aField, metadatafield to be deleted
         * @param aType, operator
         */
        void DeleteRule( TCmMetadataField aField, TCmOperatorType aType );

        /**
         * Adds additinal rules to fill list
         * @since S60 5.1
         * @param aArray, item array
         * @param aField, metadatafield to be added
         * @param aOperator, operator type
         */
        void AddAdditionalRuleL( CDesCArray* aArray,
                                 TCmMetadataField aField,
                                 TCmOperatorType aOperator );

        /**
         * Adds additinal rules to fill list
         * @since S60 5.1
         * @param aArray, item array
         * @param aField, metadatafield to be added
         * @param aOperator, operator type
         */
        void AddAdditionalRuleL( RArray<TInt>& aArray,
                                 TCmMetadataField aField,
                                 TCmOperatorType aOperator );

        /**
         * Adds additinal rules to fill list
         * @since S60 5.1
         * @param aArray, item array
         * @param aField, metadatafield to be added
         * @param aOperator, operator type
         */
        void AddAdditionalRuleL( TCmMetadataField aMetadataField,
                                 TCmOperatorType aOperator,
                                 TInt aValue );

        /**
         * Appends item to the list
         * @since S60 5.1
         * @param aArray, item array
         * @param aParam, item
         */
        void AppendItemToListL( RArray<TInt>* aArray, TInt aParam );

        /**
         * Sets audio file size selection
         * @since S60 5.1
         */
        void SetAudioFileSizeSelection();

        /**
         * Sets audio file size selection
         * @since S60 5.1
         */
        void SaveTrackFileSizeL();

        /**
         * Sets audio file duration selection
         * @since S60 5.1
         */
        void SetAudioFileDurationSelection();

        /**
         * Sets audio file size selection
         * @since S60 5.1
         */
        void SaveTrackDurationL();

        /**
         * Sets modified additional rules visible
         * @since S60 5.1
         */
        void DisplayModifiedAdditionalRules();

        /**
         * Searches item from the list be item ID
         * @since S60 5.1
         * @param aId, item id
         * @return CAknSettingItem, item pointer
         */
        CAknSettingItem* SearchItemById( TMSFillEditItems aId );

        /**
         * Sets shrinking status to image list
         * @since S60 5.1
         */
        void SetShrinkingStatus();

        /**
         * Initializes parameters
         * @since S60 5.1
         */
        void InitializeValues();

        /**
         * Sets navi pane text
         * @since S60 5.1
         */
        void CreateAndSetNaviPaneTextL();

        /**
         * Saves values to rule container
         * @since S60 5.1
         */
        void SaveValueL( CAknSettingItem* aItem );

        /**
         * Checks current item setting page acceptance
         * @since S60 5.1
         * @param aItem, edited item
         * @return TBool, ETrue if accepted EFalse if cancelled
         */
        TBool CheckSettingPageAcceptance( CAknSettingItem* aItem );

        /**
         * Constructs setting list and starts getting data from server
         * @since S60 5.1
         */
        void ConstructListAndStartDataReadingL();

        /**
         * Starts list preprocessing
         * @since S60 5.1
         */
        void PreprocessListL();

        /**
         * Saves list values from visible setting items
         * @since S60 5.1
         */
        void SaveListValuesL();

        /**
         * Creates selection list for server setting
         * @since S60 5.1
         */
        void CreateServerSettingListL();

        /**
         * Adds selected servers to rule
         * @since S60 5.1
         */
        void HandleServerSelectionL();

        /**
         * Deletes servers from rule
         * @since S60 5.1
         */
        void DeleteAllServersFromRuleL();

        /**
         * Saves and preprocesses list
         * @since S60 5.1
         * @param aItem, item to be processed
         */
        void SaveAndPreprocesslistL( CAknSettingItem* aItem );

        /**
         * Gets mediaservers and removes not active servers
         * @since S60 5.1
         */
        void GetServersAndCheckActivityL();

        /**
         * Updates multiselection setting items
         * @since S60 5.1
         */
        void UpdateSettingItemsL();

        /**
         * Handles list name change
         * @since S60 5.1
         * @param aItem, name setting item
         */
        void HandleListNameL( CAknSettingItem* aItem );

    private:

        /**
         * ConstructL
         * Second phase constructor.
         * @param aAppUi : application ui reference
         * @param aView : view reference
         */
        void ConstructL( CMSAppUi& aAppUi,
                         CMSFillRuleEditView& aView );

        /**
         * Constructor.
         */
        CMSFillRuleEditList();

    private:

        /**
         * pointer to AppUi object
         */
        CMSAppUi*                               iAppUi;         // not owned
        /**
         * amount of files
         */
        TInt                                    iAmount;
        /**
         * servers
         */
        TBuf<KMaxFileName>                      iSettingText;
        /**
         * pointer to view
         */
        CMSFillRuleEditView*                    iView;          // not owned
        /**
         * Fill rule
         */
        CCmFillRule*                            iRule;          // not owned
        /**
         * MediaServers
         */
        RPointerArray<CCmMediaServerFull>*      iServers;       //owned
        /**
         * Image shrinking
         */
        TMSShrinkImages                         iShrinkImages;
        /**
         * Starting with setting
         */
        TInt                                    iStartWith;
        /**
         * date from
         */
        TTime                                   iDateFrom;
        /**
         * date until
         */
        TTime                                   iDateUntil;
        /**
         * free text
         */
        TBuf<KMaxFileName>                      iFreeText;
        /**
         * list name
         */
        TBuf<KMaxFileName>                      iListName;
        /**
         * rule template
         */
        TMSTemplate                             iRuleTemplate;
        /**
         * array of visible rules
         */
        RArray<TMSFillEditItems>                iVisibleRules;
        /**
         * limit type (pieces or MBs )
         */
        TCmLimitType                            iLimitType;
        /**
         * Dummy text buffer for "Add more rules"
         */
        TBuf<10>                                iDummyText;
        /**
         * Track lenght
         */
        TInt                                    iTrackLengthEnum;
        /**
         * Track lenght
         */
        TInt                                    iMinTrackLength;
        /**
         * Track lenght
         */
        TInt                                    iMaxTrackLength;
        /**
         * File size
         */
        TInt                                    iFileSize;
        /**
         * File size enumeration
         */
        TInt                                    iFileSizeEnum;
        /**
         * Min file size
         */
        TInt                                    iMinFileSize;
        /**
         * Min file size set
         */
        TInt                                    iMinFileSizeSet;
        /**
         * Max file size
         */
        TInt                                    iMaxFileSize;
        /**
         * Max file size set
         */
        TInt                                    iMaxFileSizeSet;
        /**
         * Previously selected rule identifier
         */
        TInt                                    iPreviousRuleIdentifier;
        /**
         * array of metadata video titles
         */
        CCmSqlPropertyContainer*                iMetadataTitles;    //owned
        /**
         * Selected video title array
         */
        RArray<TInt>                            iVideoTitleArray;   //owned
        /**
         * Selected artist array
         */
        RArray<TInt>                            iArtistArray;       //owned
        /**
         * Selected genre array
         */
        RArray<TInt>                            iGenreArray;        //owned
        /**
         * Selected album array
         */
        RArray<TInt>                            iAlbumArray;        //owned
        /**
         * Selected track array
         */
        RArray<TInt>                            iTrackArray;        //owned
        /**
         * List media type
         */
        TCmMediaType                            iMediaType;
        /**
         * New list indicator
         */
        TBool                                   iNewList;
        /**
         * Metadata request state
         */
        TMSFillStates                           iState;
        /**
         * Selected list index from CMSFillView
         */
        TInt                                    iSelectedList;
        /**
         * setting page item array (servers)
         */
        CSelectionItemList*                     iServerArray;       // owned
        /**
         * metadata collector
         */
        CMSMetadataCollector*                   iMetadataCollector; // owned
    };

#endif // C_MSFILLRULEEDITLIST_H

// End of File
