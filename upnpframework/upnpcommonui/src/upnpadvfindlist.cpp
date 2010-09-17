/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List in advanced find results dialog
*
*/


// INCLUDE FILES
// System
#include <aknnotewrappers.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <AknWaitDialog.h>
#include <akntextsettingpage.h> //for text editor flags
#include <upnpstring.h>

// upnp stack api's
#include <upnpitem.h>

// upnpframework / avcontroller api
#include "upnpavbrowsingsession.h"

// upnpramework / avcontroller helper api
#include "upnpconstantdefs.h" // upnp definitions

// upnpframework / xml parser api
#include "upnpxmlparser.h"

// common ui internal
#include "upnpcommonui.hrh"
#include <upnpcommonui.rsg>
#include "upnpadvfindresultwindow.h"
#include "upnpadvfindlist.h"
#include "upnpadvfinddialog.h"


_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

//CONSTANTS
_LIT( KDate, "%d0000:" ); //Initial value for date elements
_LIT8( KAttributeRefID, "@refID" );
_LIT8( KTimeFormat8, "%d-%02d-%02d" );
const TInt KMaxRequestCount = 50;
const TInt KTmpDateFormatLength = 12;
// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::CUPnPAdvancedFindList
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPAdvancedFindList::CUPnPAdvancedFindList(
                                    MUPnPAVController& aAVControl,
                                    MUPnPAVBrowsingSession& aBrowsingSession,
                                    CUPnPAdvancedFindDialog& aParent,
                                    CUPnPCommonUI& aCommonUI ):
                                    iAVControl(aAVControl),
                                    iCommonUI(aCommonUI),
                                    iAdvancedFindDialog(aParent),
                                    iBrowseSession(aBrowsingSession),
                                    iShowingErrorNote(EFalse)
    {
    
    // Set date to January first of current year
    iDateFrom.HomeTime();
    TInt year = iDateFrom.DateTime().Year();
    TBuf<10> tempBuf;
    tempBuf.Format( KDate(), year );
    iDateFrom.Set( tempBuf ); 
    
    iDateUntil.HomeTime();
    
    iDateInitial.Set( tempBuf );    
        
        
    
    // backup existing browsesession observer and set this as current observer
    iBrowseObserver = iBrowseSession.Observer();
    iBrowseSession.RemoveObserver();
    iBrowseSession.SetObserver( *this );
 
    iSearchResponseReceived = EFalse;
    
    iAction = CUPnPCommonUI::EUPnPNone;
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::ConstructL()
    {
    }
// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUPnPAdvancedFindList* CUPnPAdvancedFindList::NewL(
                                    MUPnPAVController& aAVControl,
                                    MUPnPAVBrowsingSession& aBrowsingSession, 
                                    CUPnPAdvancedFindDialog& aParent,
                                    CUPnPCommonUI& aCommonUI )
    {
    __LOG( "CUPnPAdvancedFindList newL" ) ;
    CUPnPAdvancedFindList* self = new (ELeave) CUPnPAdvancedFindList(
                                                            aAVControl, 
                                                            aBrowsingSession,
                                                            aParent,
                                                            aCommonUI );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::NewL
// Destructor
// --------------------------------------------------------------------------
//
CUPnPAdvancedFindList::~CUPnPAdvancedFindList()
    {
    __LOG( "CUPnPAdvancedFindList::~CUPnPAdvancedFindList" );
    if ( iWaitNoteDialog ) 
        {
         // Stop showing wait note
        TRAP_IGNORE( iWaitNoteDialog->ProcessFinishedL() );
        delete iWaitNoteDialog;
        iWaitNoteDialog = NULL;
        }
        
    iBrowseSession.RemoveObserver();
    if( iBrowseObserver )
        {
        iBrowseSession.SetObserver( *iBrowseObserver );
        }
    
    iResultArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::MediaServerDisappeared
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::MediaServerDisappeared( 
                                TUPnPDeviceDisconnectedReason aReason )
    {
    iClose = ETrue;
    iCloseReason = aReason;
    TRAP_IGNORE( MediaServerDisappearedL( aReason ) );
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::MediaServerDisappearedL
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::MediaServerDisappearedL( 
                                TUPnPDeviceDisconnectedReason aReason )
    {
    __LOG( "CUPnPAdvancedFindList::MediaServerDisappearedL" );
    
    if( aReason == EDisconnected )
        {
        iExitCode = KErrSessionClosed;
        }
    else if( aReason == EWLANLost)
        {
        iExitCode = KErrDisconnected;
        }
    else
        {
        __PANICD( __FILE__, __LINE__);
        }
    
    if( iWaitNoteDialog ) //if it is on searching
        {
        __LOG( "CUPnPAdvancedFindList::iWaitNoteDialog ETrue" );
        DialogDismissedL( EEikBidCancel );
        iWaitNoteDialog->ProcessFinishedL();
        //no need to call DialogDismissedL;
        delete iWaitNoteDialog;
        iWaitNoteDialog = NULL;
        }
    else
        {
        if( iFindResultWindow )
            {
            __LOG( "CUPnPAdvancedFindList::iFindResultWindow->DismissItselfL" );
            iFindResultWindow->DismissItselfL( iExitCode );
            iFindResultWindow = NULL;
            }
        else //notify AdvancedFindDialog to close itself
            {
            if ( !iShowingErrorNote )
                {
                iAdvancedFindDialog.DismissItselfL( iExitCode ); 
                }
            else
                {
                __LOG( "CUPnPAdvancedFindList::iShowingErrorNote ETrue" );
                }                      
            }      
        }
    __LOG( "CUPnPAdvancedFindList::MediaServerDisappearedL End" );
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::DialogDismissedL
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::DialogDismissedL( TInt aButtonId )
    {
    __LOG( "CUPnPBrowseDialog::DialogDismissedL" );

    // If button is cancel, inform observer parent class 
    // that cancel has been made
    if( aButtonId == EEikBidCancel )
        {
        __LOG( "CUPnPBrowseDialog::DialogDismissedL: \
                                             Cancel was pressed." );
        
        iAction = CUPnPCommonUI::EUPnPNone; 
        iBrowseSession.CancelSearch();
        }

    __LOG( "CUPnPBrowseDialog::DialogDismissedL end" );
    //Do nothing
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::SearchResponse
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::SearchResponse( const TDesC8& aSearchResponse,
    TInt aError, TInt /*aMatches*/, TInt /*aTotalCount*/, const TDesC8&
    /*aUpdateId*/ )
    {
    __LOG( "CUPnPBrowseDialog::SearchResponse" );
    RPointerArray<CUpnpObject> array;   
    
    if( aError == KErrNone )
        {
        CUPnPXMLParser* parser = NULL;
        TRAP( aError, parser = CUPnPXMLParser::NewL();
              parser->ParseResultDataL( array,
              aSearchResponse ) );
        
        delete parser;
        }
        
    TRAP_IGNORE( SearchResponseL( aError, array ) );
    
    array.ResetAndDestroy();
    
    __LOG( "CUPnPBrowseDialog::SearchResponse -end" );
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::SearchResponseL
// Response to FindMetadataL function.
// --------------------------------------------------------------------------
void CUPnPAdvancedFindList::SearchResponseL( 
                            TInt aError,
                            const RPointerArray<CUpnpObject>& aResultArray )
    {
    
    __LOG( "CUPnPBrowseDialog::SearchResponseL" );
    iExitCode = aError;    
    if( KErrNone == aError )
        {
        iSearchResponseReceived = ETrue;
        TBuf<KMaxNameLength> previousTitleName;
        previousTitleName.Zero();
        //Go through the search result and add data to database
        
        
        for( TInt count = 0; count < aResultArray.Count(); count++ )
            {    
            //If an Item is found
            CUpnpObject* tmpObject = aResultArray[count];

            //If it is right type and name is longer than zero
            if ( tmpObject->ObjectType() == EUPnPItem )
                {
                CUpnpItem* item = static_cast<CUpnpItem*>( tmpObject );

                if ( item->Title().Length() > 0 )
                    {
                    
                    //Copy the item
                    item = CUpnpItem::NewL();
                    CleanupStack::PushL( item );
                    item->CopyL( *aResultArray[count] );
                    //And if item is correct type
                    iResultArray.AppendL( item ); // item ownership transferred
                    CleanupStack::Pop( item );
                    }
                }
            }
        }
    else
        {
        iCommonUI.GetUpnpAction( iAction );
        iCommonUI.HandleCommonErrorL( aError, NULL );
        __LOG( "CUPnPAdvancedFindList: \
                SearchResponse operation failed!" );
        }
    if ( iWaitNoteDialog )
        {
        TRAP_IGNORE( iWaitNoteDialog->ProcessFinishedL() );
        }
   __LOG( "CUPnPBrowseDialog::SearchResponseL -end" );
   }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::CreateSettingItemL
// Creates list items
// --------------------------------------------------------------------------
//
CAknSettingItem* CUPnPAdvancedFindList::CreateSettingItemL( 
                                                           TInt aIdentifier )
    {    
    CAknSettingItem* settingItem = NULL;
    switch (aIdentifier)
        {
        case EAdvancedFindMediaType:
            {
            iMediatype = 0;
            settingItem = new (ELeave) CAknEnumeratedTextPopupSettingItem(
                                       aIdentifier, iMediatype ); 
            break;
            }
        case EAdvancedFindFileName:
            {            
            settingItem = new (ELeave) CAknTextSettingItem( aIdentifier, 
                                                            iFileName );
            settingItem->SetSettingPageFlags(
                                   CAknTextSettingPage::EZeroLengthAllowed );
            break;
            }
        case EAdvancedFindArtist:
            {                        
            settingItem = new (ELeave) CAknTextSettingItem( aIdentifier,
                                                            iArtistText );
            settingItem->SetSettingPageFlags(
                                   CAknTextSettingPage::EZeroLengthAllowed );
            settingItem->SetHidden( ETrue );                        
            break;
            }
        case EAdvancedFindAlbum:
            {            
            settingItem = new (ELeave) CAknTextSettingItem( aIdentifier,
                                                            iAlbumText );
            settingItem->SetSettingPageFlags(
                                   CAknTextSettingPage::EZeroLengthAllowed );
            settingItem->SetHidden( ETrue );
            break;
            }
        case EAdvancedFindGenre:
            {            
            settingItem = new (ELeave) CAknTextSettingItem( aIdentifier,
                                                            iGenreText );
            settingItem->SetSettingPageFlags(
                                   CAknTextSettingPage::EZeroLengthAllowed );
            settingItem->SetHidden( ETrue );
            break;
            }
        case EAdvancedFindDateFrom:
            {            
            settingItem = new (ELeave) CAknTimeOrDateSettingItem(
                                           aIdentifier, 
                                           CAknTimeOrDateSettingItem::EDate,
                                           iDateFrom );
            break;            
            }
        case EAdvancedFindDateUntil:
            {            
            settingItem = new (ELeave) CAknTimeOrDateSettingItem(
                                           aIdentifier, 
                                           CAknTimeOrDateSettingItem::EDate,
                                           iDateUntil );
            break;
            }                
        default:
            break;
        }            
    return settingItem;
    }
    
// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::EditItemL
// --------------------------------------------------------------------------
//
void  CUPnPAdvancedFindList::EditItemL ( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemArray* arrayForTrueIndex = 
                                CAknSettingItemList::SettingItemArray();
    TInt indexForEditing = arrayForTrueIndex->
                                        ItemIndexFromVisibleIndex( aIndex );
    arrayForTrueIndex = NULL;           
    switch ( indexForEditing )
        {
        case EAdvancedFindMediaType:
            {
            iSettingPageOpen = ETrue;        
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );
            CAknSettingItemArray* arrayForHideCommands = 
                                    CAknSettingItemList::SettingItemArray();
            StoreSettingsL();        
            if ( iMediatype == EAdvancedFindMusic )
                {
                CAknSettingItem* settingItem = 
                            arrayForHideCommands->At( EAdvancedFindArtist );
                settingItem->SetHidden( EFalse );
                settingItem = NULL;                
                CAknSettingItem* settingItem2 = 
                            arrayForHideCommands->At( EAdvancedFindAlbum );
                settingItem2->SetHidden( EFalse );
                settingItem2 = NULL;            
                CAknSettingItem* settingItem3 = 
                            arrayForHideCommands->At( EAdvancedFindGenre );
                settingItem3->SetHidden( EFalse );    
                settingItem3 = NULL;
                }
            else 
                {                
                CAknSettingItem* settingItem = 
                            arrayForHideCommands->At( EAdvancedFindArtist );
                settingItem->SetHidden( ETrue );
                settingItem = NULL;                
                CAknSettingItem* settingItem2 = 
                            arrayForHideCommands->At( EAdvancedFindAlbum );
                settingItem2->SetHidden(ETrue);
                settingItem2 = NULL;                
                CAknSettingItem* settingItem3 = 
                            arrayForHideCommands->At( EAdvancedFindGenre );
                settingItem3->SetHidden( ETrue );
                settingItem3 = NULL;
                }            
            arrayForHideCommands = NULL;            
            }
            break;
        case EAdvancedFindFileName:
            {
            iSettingPageOpen = ETrue;            
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );
            StoreSettingsL();            
            }
            break;            
        case EAdvancedFindArtist:
            {
            iSettingPageOpen = ETrue;
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );
            StoreSettingsL();             
            }
            break;
        case EAdvancedFindAlbum:
            {
            iSettingPageOpen = ETrue;
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );
            StoreSettingsL();            
            }
            break;
        case EAdvancedFindGenre:
            {
            iSettingPageOpen = ETrue;
            CAknSettingItemList::EditItemL( indexForEditing,
                                            aCalledFromMenu );
            StoreSettingsL();            
            }            
            break;
        case EAdvancedFindDateFrom:
            {           
            iSettingPageOpen = ETrue; 
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );            
            StoreSettingsL();
            }
            break;
        case EAdvancedFindDateUntil:
            {            
            iSettingPageOpen = ETrue;
            CAknSettingItemList::EditItemL( indexForEditing, 
                                            aCalledFromMenu );
            StoreSettingsL();
            
            }
            break;            
        default:
            break;
        }
    
    iSettingPageOpen = EFalse;    
    //load settings to screen
    CAknSettingItemList::LoadSettingsL();
    CAknSettingItemList::DrawDeferred();
    
    
    if( iClose )
        {
        MediaServerDisappearedL(iCloseReason);
        }
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::MakeQueryL
// --------------------------------------------------------------------------
//
TInt CUPnPAdvancedFindList::MakeQueryL()
    {
    //Search criteria string
    HBufC8* searchCriteria = BuildSearchCriteriaL();
    CleanupStack::PushL( searchCriteria );

    HBufC8* sortCriteria = KSortCriteria().AllocL();
    CleanupStack::PushL( sortCriteria );

    iAction = CUPnPCommonUI::EUPnPSearch;
    // Make the search request
    iBrowseSession.SearchL( 
                        KContainerIdRoot,     /* object id of the container */
                        *searchCriteria,      /* criteria */
                        KFilterCommon,        /* filter */
                        0,                    /* start index */
                        KMaxRequestCount + 1, /* request count + 1 */
                        *sortCriteria );      /* sort criteria */
    CleanupStack::PopAndDestroy( sortCriteria );
    CleanupStack::PopAndDestroy( searchCriteria );            

    TInt ret = KErrNone;
    iWaitNoteDialog = new(ELeave) CAknWaitDialog( (
                                  REINTERPRET_CAST( CEikDialog**,
                                                    &iWaitNoteDialog ) ),
                                                    ETrue );
    iWaitNoteDialog->SetCallback( this );
 
    ret = iWaitNoteDialog->ExecuteLD( 
                R_UPNPCOMMONUI_ADVANCEDFIND_WAIT_NOTE_DIALOG );

    //Make result window
    //ret is KErrNone only when the DialogDismissedL by the FW.
    // ( not canceling)
    if( ret != KErrNone && iExitCode == KErrNone )
        {
        //If there was more than request count received
        if( iResultArray.Count() > KMaxRequestCount )
            {
            //Delete Item from array
            delete iResultArray[KMaxRequestCount];
            //Remove from array
            iResultArray.Remove( KMaxRequestCount );

            iShowingErrorNote = ETrue;
            //Display error note
            iCommonUI.DisplayErrorTextL(
                        R_UPNPCOMMONUI_ADVANCED_FIND_TOO_MANY_RESULTS_ERROR,
                        KMaxRequestCount );
            iShowingErrorNote = EFalse;            
            }        
        
        // During running DisplayErrorTextL() 
        // We also need to care about whether iExitCode been changed or not.
        if ( iExitCode != KErrNone )
            {
            iResultArray.ResetAndDestroy();
            return iExitCode;
            }
        else
            {
            iFindResultWindow = CAdvancedFindResultWindow::NewL(
                    R_UPNPCOMMONUI_RESULT_WINDOW_MENUBAR,
                    iResultArray,
                    iAVControl,
                    iBrowseSession,
                    iCommonUI );

            iFindResultWindow->SetMopParent( this );
            
            iShowingErrorNote = ETrue;
            
            iExitCode = iFindResultWindow->ExecuteLD( 
                            R_UPNPCOMMONUI_ADVANCED_FIND_RESULT_DIALOG );
            iShowingErrorNote = EFalse;
            
            iFindResultWindow = NULL;
            }          
        }
    CAknSettingItemList::LoadSettingsL();
    CAknSettingItemList::DrawDeferred();

    //Delete result array
    iResultArray.ResetAndDestroy();
    iSearchResponseReceived = EFalse;
    
    if( iExitCode != KErrSessionClosed && 
        iExitCode != KErrDisconnected &&
        iExitCode != EAknCmdExit &&
        iExitCode != EEikCmdExit )
        {
        iExitCode = KErrNone;
        }
    __LOG( "CUPnPAdvancedFindList::MakeQueryL End" );
    return iExitCode;
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::HandleResourceChange
// --------------------------------------------------------------------------
//
void CUPnPAdvancedFindList::HandleResourceChange( TInt aType ) 
    {
    __LOG( "CUPnPAdvancedFindList::HandleResourceChange" );
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPaneRect );        
        
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(
            TRect( TPoint( 0, 0 ), mainPaneRect.Size() ),
            AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
                
        ListBox()->SetRect( layoutRect.Rect() );
        }
        
    // Base call
    CAknSettingItemList::HandleResourceChange( aType );
    }    
    
    
// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::BuildSearchCriteriaL
// --------------------------------------------------------------------------
//
HBufC8* CUPnPAdvancedFindList::BuildSearchCriteriaL() const
    {
    __LOG( "CUPnPAdvancedFindList::BuildSearchCriteriaL" );   
    //Search criteria string
    HBufC8* searchCriteria = NULL;
    HBufC8* tmpStr = NULL;

    //Date variables for handling date search
    TBuf8<KTmpDateFormatLength> tmpDateFormat;
    TDateTime tmpDate;

    searchCriteria = HBufC8::NewL(
        KElementClass().Length() +
        KCriteriaSpace().Length() +
        KCriteriaDerivedFrom().Length() +
        KCriteriaSpace().Length() +
        KCriteriaQuot().Length() +
        KClassVideo().Length() +
        KCriteriaQuot().Length() +
        KCriteriaSpace().Length() +
        KCriteriaAnd().Length() +
        KCriteriaSpace().Length() +
        KAttributeRefID().Length() +
        KCriteriaSpace().Length() +
        KCriteriaExists().Length() +
        KCriteriaSpace().Length() +
        KCriteriaFalse().Length() );
    TPtr8 strPtr = searchCriteria->Des();

    strPtr.Copy( KElementClass() );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KCriteriaDerivedFrom );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KCriteriaQuot );

    //Set search for the mediatype
    __LOG1( "iMediatype: %d", iMediatype );
    switch( iMediatype )
        {
        case EAdvancedFindAll:
            {
            strPtr.Append( KClassItem() );
            break;
            }
        case EAdvancedFindImages:
            {
            strPtr.Append( KClassImage() );
            break;
            }
        case EAdvancedFindVideo:
            {
            strPtr.Append( KClassVideo() );
            break;
            }
        case EAdvancedFindMusic:
            {
            strPtr.Append( KClassAudio() );
            break;
            }
        default:
            {
            __PANICD( __FILE__, __LINE__ );
            break;
            }
        }

    strPtr.Append( KCriteriaQuot );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KCriteriaAnd );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KAttributeRefID );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KCriteriaExists );
    strPtr.Append( KCriteriaSpace );
    strPtr.Append( KCriteriaFalse );
    CleanupStack::PushL( searchCriteria );

    //Check if filename has been entered to search criteria
    __LOG1( "iFileName.Length: %d", iFileName.Length() );
    if( iFileName.Length() > 0 )
        {  
        __LOG( "add name condition" );
        tmpStr = UpnpString::FromUnicodeL( iFileName );
        
        //If inserted, add to search string
        searchCriteria = searchCriteria->ReAllocL(
            searchCriteria->Length() +
            KCriteriaSpace().Length() +
            KCriteriaAnd().Length() +
            KCriteriaSpace().Length() +
            KElementTitle().Length() +
            KCriteriaSpace().Length() +
            KCriteriaContains().Length() +
            KCriteriaSpace().Length() +
            KCriteriaQuot().Length() +
            tmpStr->Length() +
            KCriteriaQuot().Length() 
            );
        
        //pop out the old searchCriteria
        CleanupStack::Pop();
        //push the new searchCriteria which is created by ReAllocL
        CleanupStack::PushL( searchCriteria );
        
        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KElementTitle() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaContains() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( *tmpStr );
        strPtr.Append( KCriteriaQuot() );
        delete tmpStr;
        tmpStr = NULL;
        
        }

    //Add possibly artists to search criteria
    if( iArtistText.Length() > 0 && iMediatype == EAdvancedFindMusic )
        {
        __LOG( "add artists condition" );
        tmpStr = UpnpString::FromUnicodeL( iArtistText );
        searchCriteria = searchCriteria->ReAllocL(
             searchCriteria->Length() +
             KCriteriaSpace().Length() +
             KCriteriaAnd().Length() +
             KCriteriaSpace().Length() +
             KCriteriaOB().Length() +
             KElementArtist().Length() +
             KCriteriaSpace().Length() +
             KCriteriaContains().Length() +
             KCriteriaSpace().Length() +
             KCriteriaQuot().Length() +
             tmpStr->Length() + 
             KCriteriaQuot().Length() +
             KCriteriaSpace().Length() +
             KCriteriaCB().Length()
             ); 

       //pop out the old searchCriteria
        CleanupStack::Pop();
        //push the new searchCriteria which is created by ReAllocL
        CleanupStack::PushL( searchCriteria ); 

        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaOB() );
        strPtr.Append( KElementArtist() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaContains() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( *tmpStr ); //Artist name
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaCB() );
        delete tmpStr;
        tmpStr = NULL;
        }

    //Add possibly albums to search criteria
    if( iAlbumText.Length() > 0 && iMediatype == EAdvancedFindMusic )
        {
        __LOG( "add albums condition" );
        tmpStr = UpnpString::FromUnicodeL( iAlbumText );
        searchCriteria = searchCriteria->ReAllocL(
            searchCriteria->Length() +
            KCriteriaSpace().Length() + 
            KCriteriaAnd().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaOB().Length() + 
            KElementAlbum().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaContains().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaQuot().Length() + 
            tmpStr->Length() + 
            KCriteriaQuot().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaCB().Length() 
            );

        //pop out the old searchCriteria
        CleanupStack::Pop();
        //push the new searchCriteria which is created by ReAllocL
        CleanupStack::PushL( searchCriteria );

        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaOB() );
        strPtr.Append( KElementAlbum() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaContains() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( *tmpStr ); //Artist name
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaCB() );
        delete tmpStr;
        tmpStr = NULL;
        }

    //Add possibly genres to search criteria
    if( iGenreText.Length() > 0 && iMediatype == EAdvancedFindMusic )
        {
        __LOG( "add genres condition" );
        tmpStr = UpnpString::FromUnicodeL( iGenreText );

        searchCriteria = searchCriteria->ReAllocL(
            searchCriteria->Length() +
            KCriteriaSpace().Length() + 
            KCriteriaAnd().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaOB().Length() + 
            KElementGenre().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaContains().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaQuot().Length() + 
            tmpStr->Length() + 
            KCriteriaQuot().Length() + 
            KCriteriaSpace().Length() + 
            KCriteriaCB().Length() 
            );

        //pop out the old searchCriteria
        CleanupStack::Pop();
        //push the new searchCriteria which is created by ReAllocL
        CleanupStack::PushL( searchCriteria );
                                                
        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaOB() );
        strPtr.Append( KElementGenre() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaContains() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( *tmpStr ); //Genre name
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaCB() );
        delete tmpStr;
        tmpStr = NULL;
        }

    // To check whether need to add time condition
    TBool addTimeCondition = ETrue;
    
    if( iFileName.Length() > 0 )
        {
        __LOG( "iFileName > 0" );
        
        // Make temp TDateTime item with current home time to compare with date
        // until.
        TTime tempTTimeforCurrent;
        tempTTimeforCurrent.HomeTime();
        TDateTime tempTDateTimeforCurrent = tempTTimeforCurrent.DateTime();
        TDateTime tempTDateTimeforUntil = iDateUntil.DateTime();
        TBuf8<KTmpDateFormatLength> tempDateFormatCurrent;
        TBuf8<KTmpDateFormatLength> tempDateFormatUntil;
        tempDateFormatCurrent.Format( KTimeFormat8(),
            tempTDateTimeforCurrent.Year(), 
            tempTDateTimeforCurrent.Month() + 1,
            tempTDateTimeforCurrent.Day() + 1 );
        tempDateFormatUntil.Format( KTimeFormat8(),
            tempTDateTimeforUntil.Year(),
            tempTDateTimeforUntil.Month() + 1,
            tempTDateTimeforUntil.Day() +1 );
                
        if( ( iDateFrom == iDateInitial ) && 
                ( tempDateFormatCurrent == tempDateFormatUntil ) )
            {
            // Time criterias are not included into the find query
            // if they are not altered when Title search
            __LOG( "time criterias are not altered" );
            addTimeCondition = EFalse;
            }
        }
    
    if ( addTimeCondition )    
        {
        __LOG( "add time conditions" );
        
        // Add time from
        tmpDate = iDateFrom.DateTime();
        //Format the date according to UPnP
        tmpDateFormat.Format( KTimeFormat8, 
            tmpDate.Year(), 
            tmpDate.Month() + 1,
            tmpDate.Day() + 1 );

        searchCriteria = searchCriteria->ReAllocL(
            searchCriteria->Length() +
            KCriteriaSpace().Length() +
            KCriteriaAnd().Length() +
            KCriteriaSpace().Length() +
            KElementDate().Length() +
            KCriteriaSpace().Length() +
            KCriteriaGTE().Length() +
            KCriteriaSpace().Length() +
            KCriteriaQuot().Length() +
            tmpDateFormat.Length() +
            KCriteriaQuot().Length()  
            );
        CleanupStack::Pop();
        CleanupStack::PushL( searchCriteria );
    
        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KElementDate() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaGTE() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( tmpDateFormat );
        strPtr.Append( KCriteriaQuot() );
    
        // Add time until
        tmpDate = iDateUntil.DateTime();
        tmpDateFormat.Format( KTimeFormat8,
            tmpDate.Year(), 
            tmpDate.Month() + 1, 
            tmpDate.Day() + 1 );
    
        searchCriteria = searchCriteria->ReAllocL(
            searchCriteria->Length() +
            KCriteriaSpace().Length() +
            KCriteriaAnd().Length() +
            KCriteriaSpace().Length() +
            KElementDate().Length() +
            KCriteriaSpace().Length() +
            KCriteriaLTE().Length() +
            KCriteriaSpace().Length() +
            KCriteriaQuot().Length() +
            tmpDateFormat.Length() +
            KCriteriaQuot().Length()  
            );
        CleanupStack::Pop();
        CleanupStack::PushL( searchCriteria );                            
    
        strPtr.Set( searchCriteria->Des() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaAnd() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KElementDate() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaLTE() );
        strPtr.Append( KCriteriaSpace() );
        strPtr.Append( KCriteriaQuot() );
        strPtr.Append( tmpDateFormat );
        strPtr.Append( KCriteriaQuot() );
        }

    //Encode the searchcriteria for xml compatibility
    HBufC8* xmlEncodedSearchCriteria = 
                            UpnpString::EncodeXmlStringL( searchCriteria );
    
    CleanupStack::PopAndDestroy( searchCriteria ); //searchCriteria
    
    __LOG( "CUPnPAdvancedFindList::BuildSearchCriteriaL - end" );
    return xmlEncodedSearchCriteria;
    }

// --------------------------------------------------------------------------
// CUPnPAdvancedFindList::IsSettingPageOpen
// --------------------------------------------------------------------------
//
TBool CUPnPAdvancedFindList::IsSettingPageOpen() const
    {
    return iSettingPageOpen;
    }

// End of file
