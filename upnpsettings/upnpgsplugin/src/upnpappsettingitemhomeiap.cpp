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
* Description:      Implements home network IAP setting item class to home
*                network setting item list.
*
*/







// INCLUDE FILES
// System
#include <StringLoader.h>           // stringloader
#include <aknmessagequerydialog.h>  // caknmessagequerydialog
#include <aknradiobuttonsettingpage.h> // caknradiobuttonsettingpage
#include <WlanCdbCols.h>            // wlan_service
#include <wlanmgmtcommon.h>         // twlanssid
#include <agentdialog.h>            // rgenconagentdialogserver
#include <aknnotewrappers.h>        // cakninformationnote
#include <cmmanager.h>
#include <cmconnectionmethoddef.h>
#include <commdb.h>                 //CCommsDatabase          
#include "upnpavcontrollerfactory.h"
#include "upnpsettingsengine.h"
#include "upnpsettingsengine.hrh"

// internal
#include "upnpappsettingitemhomeiap.h"
#include "upnpappsettingslist.h"    // ewlanunsecured
#include "upnpfilesharingengine.h"  // cupnpfilesharingengine

#include <upnpgspluginrsc.rsg>

_LIT( KComponentLogfile, "upnpgsplugin.txt");
#include "upnplog.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::NewL()
// --------------------------------------------------------------------------
//
UPnPAppSettingItemHomeIAP* UPnPAppSettingItemHomeIAP::NewL(
    TInt aIdentifier,
    TInt& aIAPSetting,
    TInt& aIAPId,
    TInt& aWAPId,
    TBool& aSharingState )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    UPnPAppSettingItemHomeIAP* self = new (ELeave) UPnPAppSettingItemHomeIAP(
        aIdentifier,
        aIAPSetting,
        aIAPId,
        aWAPId,
        aSharingState );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::UPnPAppSettingItemHomeIAP()
// --------------------------------------------------------------------------
//
UPnPAppSettingItemHomeIAP::UPnPAppSettingItemHomeIAP(
    TInt aIdentifier,
    TInt& aIAPSetting,
    TInt& aIAPId,
    TInt& aWAPId,
    TBool& aSharingState ) :
    CAknEnumeratedTextPopupSettingItem(aIdentifier, aIAPSetting),
    iIAPSetting(aIAPSetting),
    iIAPId(aIAPId),
    iWAPId(aWAPId),
    iSharingState( aSharingState )
    {
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::ConstructL()
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::ConstructL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iCoeEnv = CCoeEnv::Static();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::~UPnPAppSettingItemHomeIAP()
// --------------------------------------------------------------------------
//
UPnPAppSettingItemHomeIAP::~UPnPAppSettingItemHomeIAP()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iItemText;
    delete iIAPName;

    iIAPIdArray.Close();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingPageHomeIAP::CreateAndExecuteSettingPageL ()
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::CreateAndExecuteSettingPageL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // if sharing is on, only information note is displayed
    UPnPAVControllerFactory::TAVControllerServerStatus status;
    UPnPAVControllerFactory::Status( status );
    if ( iSharingState || status == UPnPAVControllerFactory::EStatusActive )
        {
        CAknInformationNote* note = new (ELeave) CAknInformationNote;
        CleanupStack::PushL(note);
        HBufC* noteText = iCoeEnv->AllocReadResourceLC(
            R_QTN_IUPNP_IAP_TURN_SHARING_OFF);
        note->ExecuteLD(*noteText);
        CleanupStack::PopAndDestroy(noteText);
        CleanupStack::Pop(note);
        return;
        }

    // Resets the member array for iap ids.
    // Creates new array for iap names.
    iIAPIdArray.Reset();
    CDesCArray* array = new (ELeave) CDesCArrayFlat(5);
    CleanupStack::PushL(array);

    // adds static setting page items from resource
    // and stores the number of static items.
    AddStaticItemsL(array);
    TInt staticItemCount = array->Count();
    
    // adds wlan access points, array GETS the ownership
    CUPnPSettingsEngine::GetWLANAccessPointsL( array, iIAPIdArray );
    
    MAknQueryValue* qValue = QueryValue();
    User::LeaveIfNull(qValue);
    LoadL();
    StoreL();

    TInt idx = GetIAPIndex( staticItemCount );
    if ( idx >= array->MdcaCount() )
        {
        idx = array->MdcaCount() - 1;
        }
    // setting page is launched
    CAknSettingPage* dlg = new (ELeave) CAknRadioButtonSettingPage(
        SettingPageResourceId(),
        idx,
        array);

    if (dlg->ExecuteLD())
        {
        switch (idx)
            {
            case EUPnPSettingsEngineIAPItemNone:
                {
                DisplayInfoL(R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                             R_IBU_GALLERY_UPDATE_NOTAVAIL_NOTE_TEXT,
                             R_HOMECONNECT_INFO_QUERY);
                qValue->SetCurrentValueIndex(EUPnPSettingsEngineIAPItemNone);
                iIAPId = EUPnPSettingsEngineIAPIdNone;
                iIAPSetting = EUPnPSettingsEngineIAPItemNone;
                break;
                }
            case EUPnPSettingsEngineIAPItemAlwaysAsk:
                {
                qValue->SetCurrentValueIndex(EUPnPSettingsEngineIAPItemAlwaysAsk);
                iIAPId = EUPnPSettingsEngineIAPIdAlwaysAsk;
                iIAPSetting = EUPnPSettingsEngineIAPItemAlwaysAsk;
                DisplayInfoL(R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                             R_IBU_GALLERY_UPDATE_NOTE_TEXT,
                             R_HOMECONNECT_INFO_QUERY);
                break;
                }
            case EUPnPSettingsEngineIAPItemUserDefined: // Create new
                {
                // new iap wlan iap creation is started here
                
                TInt old_iap = iIAPId;
                
                CUPnPSettingsEngine::CreateAccessPointL();
                
                //We have to re-get all the wlan iaps again since
                //user may modify the iap list   
                RArray<TInt64> newArray;
                CleanupClosePushL( newArray );
                
                //We only are interested in the iap ids
                CUPnPSettingsEngine::GetWLANAccessPointsL(
                                                    NULL, 
                                                    newArray );
                
                if( newArray.Count() <= 0 ) //if no iap exisits
                    {
                    //if previous iap is not "None" or "Always ask"
                    if( old_iap > EUPnPSettingsEngineIAPIdAlwaysAsk )
                        {
                        DisplayInfoL(
                            R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                            R_IBU_GALLERY_UPDATE_NOTAVAIL_NOTE_TEXT,
                            R_HOMECONNECT_INFO_QUERY);
                        iIAPId = EUPnPSettingsEngineIAPIdNone;
                        
                        qValue->SetCurrentValueIndex(
                                            EUPnPSettingsEngineIAPItemNone );
                        iIAPSetting = EUPnPSettingsEngineIAPItemNone;
                        }
                    iIAPIdArray.Reset();
                    CleanupStack::PopAndDestroy( &newArray );
                    }
                else
                    {
                    TBool newiap = ETrue;
                    TInt lastItemIndex = newArray.Count() - 1;
                    //if the last item on the new iap list 
                    //is found in the old iap array
                    //then we think no new iap is created
                    for( TInt i = 0; i < iIAPIdArray.Count(); i++ )
                        {                        
                        if( newArray[lastItemIndex] == iIAPIdArray[i] )
                            {
                            newiap = EFalse; 
                            i = iIAPIdArray.Count();
                            }
                        }
                    
                    //get the new iap list
                    iIAPIdArray.Reset();
                    for( TInt i = 0; i < newArray.Count(); i++ )
                        {
                        iIAPIdArray.AppendL( newArray[i] );
                        }                        
                    CleanupStack::PopAndDestroy( &newArray );
                    
                    if( newiap )
                        {
                        iIAPId = iIAPIdArray[lastItemIndex];//get new iap
                        iIAPSetting = 
                                EUPnPSettingsEngineIAPItemUserDefined;
                        // show confirmation note if connection is 
                        // unsecured, the database store the value of
                        // the securitymode,such as EOpen,EWep,E802_1x,
                        // EWpa,if select other securitymode except 
                        // EOpen, the return value of the 
                        // CheckAPSecurityL fuction is not EWLanUnsecured
                        if ( CheckAPSecurityL(iIAPId) == EWLanUnsecured )
                            {
                            CAknQueryDialog* dlg = 
                                                CAknQueryDialog::NewL();
                            if (!(dlg->ExecuteLD(
                            R_UNSECURED_CONNECTIONS_CONFIRMATION_QUERY)))
                                {
                                //user cancels the dialog
                                UsePreviousIapL( old_iap ); 
                                }
                            }
                        else
                            {
                            // show 'Sharing Changed' note ( None -> sharing)
                            if(old_iap == EUPnPSettingsEngineIAPIdNone &&
                               iIAPId != old_iap )
                                {
                                DisplayInfoL(
                                     R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                                     R_IBU_GALLERY_UPDATE_NOTE_TEXT,
                                     R_HOMECONNECT_INFO_QUERY);
                                }  
                            }    
                        }
                    else 
                        {
                        UsePreviousIapL(old_iap);
                        } //if( newiap )
                    } //if( newArray.Count() <= 0 )
                break;
                }
            default: // Predefined iap is selected
                {
                TInt index;
                TInt old_iap = iIAPId;
                TBool iapchange = EFalse;
                // iap id is set
                if ( idx - staticItemCount >= 0 )
                    {
                    index = idx - staticItemCount;
                    if (CheckAPSecurityL(iIAPIdArray[index]) ==
                        EWLanUnsecured)
                        {
                        // show confirmation note if connection is unsecured
                        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                        if (dlg->ExecuteLD(
                            R_UNSECURED_CONNECTIONS_CONFIRMATION_QUERY))
                            {
                            //if "Continue"
                            iapchange = ETrue;
                            }
                        }
                    else
                        {
                        iapchange = ETrue;
                        }
                   
                    if( iapchange )
                        {
                        iIAPSetting = EUPnPSettingsEngineIAPItemUserDefined;
                        iIAPId = iIAPIdArray[index];
                        qValue->SetCurrentValueIndex(
                            EUPnPSettingsEngineIAPItemUserDefined);

                        // if previous iap was 'None' and current iap
                        // is not 'None'
                        if ( old_iap == EUPnPSettingsEngineIAPIdNone &&
                             iIAPId != old_iap )
                            {
                            DisplayInfoL(
                                R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                                R_IBU_GALLERY_UPDATE_NOTE_TEXT,
                                R_HOMECONNECT_INFO_QUERY);
                            }
                        }
                    }
                break;
                }
            }
        }
    LoadL();
    StoreL();

    CleanupStack::PopAndDestroy(array);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::ChangeIAPNameL()
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::ChangeIAPNameL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iIAPName;
    iIAPName = NULL;
    
    iIAPName = CUPnPSettingsEngine::GetCurrentIapNameL( iIAPId );
    
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::CheckAPSecurity
// Checks if selected access point is unsecured and shows warning note
// --------------------------------------------------------------------------
//
TInt UPnPAppSettingItemHomeIAP::CheckAPSecurityL(TInt aAccessPoint)
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TUint32 serviceId = 0;
    TUint32 securityMode = 0;

    CCommsDatabase* db = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( db );

    CCommsDbTableView* view = db->OpenViewMatchingUintLC(TPtrC(IAP),
                              TPtrC(COMMDB_ID), aAccessPoint);

    TInt error = view->GotoFirstRecord();

    if( error == KErrNone )
        {
        view->ReadUintL(TPtrC(IAP_SERVICE), serviceId);
        }

    CCommsDbTableView* wLanServiceTable = NULL;

    TRAPD(err,
    {// this leaves if the table is empty....
    wLanServiceTable = db->OpenViewMatchingUintLC(
        TPtrC( WLAN_SERVICE ),
        TPtrC( WLAN_SERVICE_ID ),
        serviceId );
    CleanupStack::Pop( wLanServiceTable );
    });

    if ( err == KErrNone )
        {
        CleanupStack::PushL( wLanServiceTable );

        TInt errorCode = wLanServiceTable->GotoFirstRecord();

        if ( errorCode == KErrNone )
            {//read securityMode value
            wLanServiceTable->ReadUintL(TPtrC( WLAN_SECURITY_MODE ),
                                        securityMode );
            }

        CleanupStack::PopAndDestroy(wLanServiceTable);
        }

    else if (err != KErrNotFound)
        {
        User::LeaveIfError(err);
        }

    CleanupStack::PopAndDestroy(2); // view, db
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );

    return securityMode;
    }

// --------------------------------------------------------------------------
// CUPnPAppSettingsList::DisplayInfoL();
// Shows message query to user
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::DisplayInfoL(TInt aHeaderResourceId,
                                             TInt aMessageResourceId,
                                             TInt aDialogResourceId)
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    HBufC* noteHeader = StringLoader::LoadL(
        aHeaderResourceId);
    CleanupStack::PushL(noteHeader);
    HBufC* noteMsg = StringLoader::LoadL(
        aMessageResourceId);
    CleanupStack::PushL(noteMsg);

    CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL(*noteMsg);

    dlg->PrepareLC(aDialogResourceId);
    dlg->QueryHeading()->SetTextL(*noteHeader);
    dlg->RunLD();

    CleanupStack::PopAndDestroy(noteMsg);
    CleanupStack::PopAndDestroy(noteHeader);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::SettingTextL()
// --------------------------------------------------------------------------
//
const TDesC& UPnPAppSettingItemHomeIAP::SettingTextL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iItemText;
    iItemText = NULL;
    LoadL();
    StoreL();

    switch ( iIAPSetting )
        {
        case EUPnPSettingsEngineIAPItemAlwaysAsk:
            {
            iItemText = iCoeEnv->AllocReadResourceL(
                R_QTN_IUPNP_ITEM_HOME_IAP_ALWAYS_ASK );
            break;
            }
        case EUPnPSettingsEngineIAPItemUserDefined:
            {
            // changes iap name according to iap id
            ChangeIAPNameL();

            if ( !iIAPName )
                {
                iItemText = iCoeEnv->AllocReadResourceL(
                    R_QTN_IUPNP_ITEM_HOME_IAP_USER_DEFINED );
                }
            else
                {
                iItemText = iIAPName->AllocL();
                }

            break;
            }
        case EUPnPSettingsEngineIAPItemNone:
            {
            iItemText = iCoeEnv->AllocReadResourceL(
                R_QTN_IUPNP_ITEM_HOME_IAP_NONE );
            break;
            }
        default:
            {
            break;
            }
        }
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return *iItemText;
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::AddStaticItemsL()
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::AddStaticItemsL(CDesCArray* aArray)
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // None
    HBufC* buf = iCoeEnv->AllocReadResourceLC(
        R_QTN_IUPNP_HOME_IAP_NONE);
    aArray->AppendL(*buf);
    CleanupStack::PopAndDestroy(buf); // buf
    buf = NULL;

    // Always ask
    buf = iCoeEnv->AllocReadResourceLC(
        R_QTN_IUPNP_HOME_IAP_ALWAYS_ASK);
    aArray->AppendL(*buf);
    CleanupStack::PopAndDestroy(buf);
    buf = NULL;

    // Create new
    buf = iCoeEnv->AllocReadResourceLC(
        R_QTN_IUPNP_HOME_IAP_CREATE_NEW);
    aArray->AppendL(*buf);
    CleanupStack::PopAndDestroy(buf);
    buf = NULL;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::UsePreviousIapL()
// --------------------------------------------------------------------------
//
void UPnPAppSettingItemHomeIAP::UsePreviousIapL( TInt aOldIap )
    {
    // dialog cancelled, using old iap value
    iIAPId = aOldIap;
    
    if( iIAPId == EUPnPSettingsEngineIAPIdNone )
        {
        iIAPSetting = EUPnPSettingsEngineIAPItemNone;
        }
    else if( iIAPId == EUPnPSettingsEngineIAPIdAlwaysAsk )
        {
        iIAPSetting = EUPnPSettingsEngineIAPItemAlwaysAsk;
        }
    else //search for the old iap
        {
        TBool oldiapthere = EFalse;
        for( TInt i = 0; i < iIAPIdArray.Count(); i++ )
            {
            if( iIAPId == iIAPIdArray[i] )
                {
                oldiapthere = ETrue;
                i = iIAPIdArray.Count();
                }
            }
        if( !oldiapthere )
            {
            iIAPId = EUPnPSettingsEngineIAPIdNone;
            iIAPSetting = EUPnPSettingsEngineIAPItemNone;
            DisplayInfoL(R_IBU_GALLERY_UPDATE_NOTE_TITLE_TEXT,
                             R_IBU_GALLERY_UPDATE_NOTAVAIL_NOTE_TEXT,
                             R_HOMECONNECT_INFO_QUERY);
            }
        else
            {
            iIAPSetting = EUPnPSettingsEngineIAPItemUserDefined;
            }    
        }   
    }
    
// --------------------------------------------------------------------------
// UPnPAppSettingItemHomeIAP::GetIAPIndex()
// --------------------------------------------------------------------------
//
TInt UPnPAppSettingItemHomeIAP::GetIAPIndex( TInt aStaticCount )
    {
    __LOG("GetIAPIndex");  
    TInt iapIndex = KErrNotFound;
    if( iIAPId == EUPnPSettingsEngineIAPIdNone ) //NONE
        {
        iapIndex = EUPnPSettingsEngineIAPItemNone;
        }
    else if( iIAPId == EUPnPSettingsEngineIAPIdAlwaysAsk ) //ALWAYS ASK
        {
        iapIndex = EUPnPSettingsEngineIAPItemAlwaysAsk;
        }
    else //pre-defined iap, index should never be Create New
        {
        iapIndex+=aStaticCount;
        for( TInt i = 0; i < iIAPIdArray.Count(); i++ )
            {
            iapIndex++;
            __LOG1("iap %d", iIAPIdArray[i] );  
            if( iIAPId == iIAPIdArray[i] )
                {
                i = iIAPIdArray.Count();
                }
            }
        }
        
    return iapIndex;    
    }    
// End of File
