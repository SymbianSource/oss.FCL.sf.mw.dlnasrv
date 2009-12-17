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
* Description:  Source file for UPnP Browse UI Implementation
*
*/


// INCLUDE FILES
// System
#include <aknPopupHeadingPane.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <StringLoader.h>

// upnp stack api
#include <upnpstring.h>

// upnpframework / avcontroller api
#include "upnpavdevice.h"
#include "upnpavcontroller.h"
#include "upnpavcontrollerfactory.h"
#include "upnpavdevicelist.h"
#include "upnpavdevice.h"
#include "upnpavcontroller.h"

// upnpframework / internal api's
#include "upnpcommonutils.h"

// common ui internal
#include <upnpcommonui.rsg>
#include <upnpcommonui.mbg> //for icons
#include "upnpselectiondialog.h"
#include "upnpcommonui.h"

// debug stuff
_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

// CONSTANTS
_LIT( KAknCommonUIMbmFileName, "\\resource\\apps\\upnpcommonui.mbm" );
const TInt KLength = 100;

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUPnPSelectionDialog* CUPnPSelectionDialog::NewL(
    MUPnPAVController& aAVControl )
    {
    __LOG( "CUPnPSelectionDialog::NewL" );
    
    CUPnPSelectionDialog* self = 
                           new ( ELeave ) CUPnPSelectionDialog( aAVControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::ConstructL()
    {
    __LOG( "CUPnPSelectionDialog::ConstructL" );
    
    iDeviceArray = CUpnpAVDeviceList::NewL();
    // Register as an observer to device operations

    iTypeOfDevicesToSearch = EUPnPSearchAllDevices;
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::CUPnPSelectionDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
CUPnPSelectionDialog::CUPnPSelectionDialog( MUPnPAVController& aAVControl ):
                                           iAVControl( aAVControl )
    {
    __LOG( "CUPnPSelectionDialog::CUPnPSelectionDialog" );
    
    iDeviceObserver = iAVControl.DeviceObserver();
    iAVControl.RemoveDeviceObserver();
    iAVControl.SetDeviceObserver( *this );
    iPopup = NULL;
    iExitReason = KErrNone;
    }

// Destructor
CUPnPSelectionDialog::~CUPnPSelectionDialog()
    {
    __LOG( "CUPnPSelectionDialog::~CUPnPSelectionDialog" );

    delete iListBox;

    // Unregister as an observer to device operations
    iAVControl.RemoveDeviceObserver();
    if( iDeviceObserver )
        {
        iAVControl.SetDeviceObserver( *iDeviceObserver );    
        }

    delete iDeviceArray;
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::PreLayoutDynInitL();
// called by framework before dialog is shown
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::PreLayoutDynInitL( const TDesC& aTitle )
    {
    __LOG( "CUPnPSelectionDialog::PreLayoutDynInitL" );

    // Browse dialog title text
    iPopup->SetTitleL( aTitle ); 

    CAknIconArray* icons = new ( ELeave ) CAknIconArray(2);
    CleanupStack::PushL( icons );

    // Mif icons
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TFileName iconsPath( AknIconUtils::AvkonIconFileName( ) );

    TFileName mbmFileName( KAknCommonUIMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    // Media server icon
    AppendIconToArrayL( icons, skin, 
                        mbmFileName, 
                        KAknsIIDDefault, 
                        EMbmUpnpcommonuiQgn_server_icon, 
                        EMbmUpnpcommonuiQgn_server_icon_mask );
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( icons );
    CleanupStack::Pop( icons );
    
    // Enable horizontal scrolling
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    LoadListItemsL();
    }


// --------------------------------------------------------------------------
// CUPnPSelectionDialog::LoadListItemsL
// Sends the browse request to UPnP AV Controller. When result set arrives,
// UPnP AV Controller will call the "BrowseResultsL" call back method,
// which is implemented below.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::LoadListItemsL()
    {
    __LOG( "CUPnPSelectionDialog::LoadListItemsL" );

    CTextListBoxModel* model = iListBox->Model();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    
    //contents inside the array will be owned by iDeviceArray
    CUpnpAVDeviceList* tempDeviceArray = NULL;
    // Get the devices from the UPnP Stack according to the search criteria
    if( iTypeOfDevicesToSearch == EUPnPSearchAllDevices )
        {
            CUpnpAVDeviceList* tempDeviceArray2 = NULL;
            tempDeviceArray2 = iAVControl.GetMediaServersL();
            if( tempDeviceArray2->Count() > 0 )
                {
                CleanupStack::PushL( tempDeviceArray2 );
                CleanupStack::PushL( tempDeviceArray );
                for( TInt i = 0; i < tempDeviceArray2->Count(); i++ )
                    {
                    tempDeviceArray->AppendDeviceL( 
                                                *( *tempDeviceArray2 )[i] );
                    }
                
                CleanupStack::Pop( tempDeviceArray );
                CleanupStack::Pop( tempDeviceArray2 );
                tempDeviceArray2->Reset();
                
                }
            
            tempDeviceArray2 = iAVControl.GetMediaRenderersL();
            
            if(tempDeviceArray2->Count() > 0 )
                {
                CleanupStack::PushL( tempDeviceArray2 );
                CleanupStack::PushL( tempDeviceArray );
                for( TInt i = 0; i < tempDeviceArray2->Count(); i++ )
                    {
                    tempDeviceArray->AppendDeviceL( 
                                                *( *tempDeviceArray2 )[i] );
                    }
                
                CleanupStack::Pop( tempDeviceArray );    
                CleanupStack::Pop( tempDeviceArray2);
                tempDeviceArray2->Reset();
                }
            
            delete tempDeviceArray2;
            
        }
    else if( iTypeOfDevicesToSearch == EUPnPSearchAllServerDevices ||
      iTypeOfDevicesToSearch == EUPnPSearchServerDevicesWithCopyCapability ||
      iTypeOfDevicesToSearch == EUPnPSearchServerDevicesWithSearchCapability )
        {
        tempDeviceArray = iAVControl.GetMediaServersL();
        }
    else
        {
        tempDeviceArray = iAVControl.GetMediaRenderersL();
        }
    
    CleanupStack::PushL( tempDeviceArray );
    if ( tempDeviceArray->Count() )
        {
        for ( TInt i=0; i < tempDeviceArray->Count(); i++ )
            {
            // Check if the device match with the search criteria
            if( MatchWithSearchCriteria( 
                     const_cast<CUpnpAVDevice*>( ( *tempDeviceArray)[i] ) ) )
                {
                iDeviceArray->AppendDeviceL( *( *tempDeviceArray )[i] );
                }
            else
                {
                CUpnpAVDevice* tempDevice = ( *tempDeviceArray )[i];
                delete tempDevice;
                tempDevice = NULL;
                }
            }        
        
        
        for ( TInt i=0; i < iDeviceArray->Count(); i++ )
            {
            // Get device friendly name and replace illegal characters.
            HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL(               
                ( *iDeviceArray )[i]->FriendlyName().Left( KLength ) );
            CleanupStack::PushL( tmpfriendlyname);
            
            HBufC* device = UpnpString::ToUnicodeL( *tmpfriendlyname );
            CleanupStack::PushL( device);

            TBuf<KMaxFileName> item;
            item.Format( _L( "%d\t%S\t\t" ),0, device ); 

            CleanupStack::PopAndDestroy( device );
            CleanupStack::PopAndDestroy( tmpfriendlyname );
            listBoxItems->AppendL( item );            
            }

        // Updates CBA.
        UpdateCommandSetL();

        iListBox->HandleItemAdditionL(); // Update listbox
        iListBox->SetCurrentItemIndexAndDraw( 0 ); // select new item
 
        }     
    else
        {
        HBufC* waitText = StringLoader::LoadLC( 
                            R_UPNPCOMMONUI_EXTERNAL_WAITING_NOTE );
        iListBox->View()->SetListEmptyTextL( *waitText );
        CleanupStack::PopAndDestroy( waitText ); 
        }
        
    CleanupStack::Pop( tempDeviceArray );
    tempDeviceArray->Reset();
    delete tempDeviceArray;    

    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::UPnPDeviceDiscovered
// Returns discovered device from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::UPnPDeviceDiscovered( 
                                               const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPSelectionDialog::UPnPDeviceDiscovered" );

    TRAPD( error, UPnPDeviceDiscoveredL( aDevice ) );
    if( error )
        {
        __LOG1( "UPnPDeviceDiscoveredL, leave %d .", error );
        }
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::UPnPDeviceDiscoveredL
// Returns discovered device from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::UPnPDeviceDiscoveredL( 
                                               const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPSelectionDialog::UPnPDeviceDiscovered" );

    
    if ( MatchWithSearchCriteria( const_cast<CUpnpAVDevice*>( &aDevice ) ) )
        {
        CUpnpAVDevice* tempDevice = CUpnpAVDevice::NewL( aDevice );
        CleanupStack::PushL( tempDevice );
        //ownership transferred                                      
        iDeviceArray->AppendDeviceL( *tempDevice );
        CleanupStack::Pop( tempDevice );

        HBufC8* tmpfriendlyname = 
            UPnPCommonUtils::ReplaceIllegalFilenameCharactersL( 
            ( ( CUpnpAVDevice* )tempDevice )->FriendlyName() );

        CleanupStack::PushL( tmpfriendlyname );
        TPtrC8 friendlyname = *tmpfriendlyname;

        CTextListBoxModel* model = iListBox->Model();
        MDesCArray* textArray = model->ItemTextArray();
        CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
        TBuf<KMaxFileName> item;
        HBufC* tmpStr = 
                    UpnpString::ToUnicodeL( friendlyname.Left( KLength ) );
        CleanupStack::PushL( tmpStr );
        item.Format( _L( "%d\t%S\t\t" ),0, tmpStr ); 
        CleanupStack::PopAndDestroy( tmpStr );
        CleanupStack::PopAndDestroy( tmpfriendlyname );
        listBoxItems->AppendL( item  );
        iListBox->HandleItemAdditionL(); // Update listbox

        // Updates CBA.
        UpdateCommandSetL();

        iListBox->ActivateL();
        iListBox->DrawDeferred();
        }
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::UPnPDeviceDisappeared
// Returns disappeared device from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::UPnPDeviceDisappeared(
                                            const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPSelectionDialog::UPnPDeviceDisappeared" );

    TRAPD( error, UPnPDeviceDisappearedL( aDevice ) );
    if ( error )
        {
        __LOG1( "UPnPDeviceDisappearedL, leave %d", error );
        }
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::UPnPDeviceDisappearedL
// Returns disappeared device from UPnP AV control point.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::UPnPDeviceDisappearedL(
    const CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPSelectionDialog::UPnPDeviceDisappearedL" );

    CTextListBoxModel* model = iListBox->Model();
    TInt currentItem = iListBox->CurrentItemIndex();
    MDesCArray* textArray = model->ItemTextArray();
    CDesCArray* listBoxItems = static_cast<CDesCArray*>( textArray );
    TInt i( 0 );
    TPtrC8 ptr( aDevice.Uuid() );
    
    while( i < iDeviceArray->Count() && 
           ptr.Compare( ( *iDeviceArray )[i]->Uuid() ) )
        {
        i++;
        }
    if ( i < iDeviceArray->Count() )
        {
        iDeviceArray->RemoveAndDestroy( i );
        listBoxItems->Delete( i, 1 );
        AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
            iListBox, currentItem, ETrue );
        iListBox->DrawDeferred();
        }

    if ( !iDeviceArray->Count() )
        {
        // show "Waiting devices" text
        HBufC* waitText = StringLoader::LoadLC( 
                                R_UPNPCOMMONUI_EXTERNAL_WAITING_NOTE );
        iListBox->View()->SetListEmptyTextL( *waitText );
        CleanupStack::PopAndDestroy( waitText ); 

        // Updates CBA.
        UpdateCommandSetL();
        }
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::WLANConnectionLost
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::WLANConnectionLost() 
    {
    __LOG( "CUPnPSelectionDialog::WLANConnectionLost" );
    DismissItself( KErrDisconnected );
    };
    
// --------------------------------------------------------------------------
// CUPnPSelectionDialog::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an
// icon array.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::AppendIconToArrayL( CAknIconArray* aArray,
                                               MAknsSkinInstance* aSkin,
                                               const TDesC& aMbmFile,
                                               const TAknsItemID& aID,
                                               TInt aBitmapId,
                                               TInt aMaskId )
    {
    __LOG( "CUPnPSelectionDialog::AppendIconToArrayL" );
    __ASSERTD( aArray != NULL, __FILE__, __LINE__ );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC( aSkin, aID,
        bitmap, mask, aMbmFile, aBitmapId, aMaskId );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    // icon now owns the bitmaps, no need to keep on cleanup stack.
    CleanupStack::Pop( 2 ); // mask, bitmap
    bitmap = NULL;
    mask = NULL;

    CleanupStack::PushL( icon );
    aArray->AppendL( icon );

    // aArray now owns the icon, no need to delete.
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::CreatePopupL
// Creates a selection popup.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::CreatePopupL( 
                                const TDesC& aTitle,
                                TUPnPDeviceTypesToSearch aDeviceType )
    {
    __LOG( "CUPnPSelectionDialog::CreatePopupL" );

    // Store the type of devices that are searched
    iTypeOfDevicesToSearch = aDeviceType;

    // Create and configure the list box 
    iListBox = new (ELeave) CAknSingleGraphicPopupMenuStyleListBox;
    iPopup = CAknPopupList::NewL( iListBox, 
                                  R_UPNPCOMMONUI_SOFTKEYS_EMPTY_CANCEL,
                                  AknPopupLayouts::EDynMenuWindow );
    iListBox->ConstructL( iPopup, EAknListBoxSelectionList );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, 
        CEikScrollBarFrame::EAuto );
    PreLayoutDynInitL( aTitle );
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::StartPopupL
// Parses the friendly names from the list of UPnPAVDevices and
// executes the selection dialog. Returns ID of the selected item.
// --------------------------------------------------------------------------
TInt CUPnPSelectionDialog::StartPopupL( CUpnpAVDevice& aDevice )
    {
    __LOG( "CUPnPSelectionDialog::StartPopupL" );

    TInt ret = KErrNone;
    
    // Start animation
    RDebug::Print( _L( "CUPnPSelectionDialog::\
StartPopupL header animation" ) );
    iPopup->Heading()-> SetHeaderAnimationL( 
                        R_UPNPCOMMONUI_ANIMATION_FOR_SELECTION_DIALOG );

    TInt popupOk = iPopup->ExecuteLD();
    iPopup = NULL;
    if ( popupOk )
        {
        if ( iListBox->CurrentItemIndex() >= 0 )
            {
            CUpnpAVDevice* device =
                        ( *iDeviceArray )[iListBox->CurrentItemIndex()];
            aDevice.CopyFromL( *device );
            ret = KErrNone;
            }
        }
    else
        {
        if( KErrNone == iExitReason )
            {
            ret = KErrCancel;    
            }
        else
            {
            ret = iExitReason;
            }
        
        }
    return ret;    
    }


// --------------------------------------------------------------------------
// CUPnPSelectionDialog::DismissItself
// Dismiss the selection dialog via an error code.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::DismissItself( TInt aError )
    {
    iExitReason = aError;
    if( iPopup )
        {
        iPopup->CancelPopup();
        }
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::MatchWithSearchCriteria
// Checks if a given device matches with the search criteria.
// --------------------------------------------------------------------------
TBool CUPnPSelectionDialog::MatchWithSearchCriteria( CUpnpAVDevice *aDevice )
    {
    TBool returnValue = EFalse;
    if( aDevice )
        {
        // If all device types are accepted
        if( iTypeOfDevicesToSearch == EUPnPSearchAllDevices )
            {
            returnValue = ETrue;
            }
        // If all media server devices are accepted
        else if( iTypeOfDevicesToSearch == EUPnPSearchAllServerDevices &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaServer )
            {
            returnValue = ETrue;
            }
        // If all rendering devices are accepted
        else if( iTypeOfDevicesToSearch == EUPnPSearchAllRenderingDevices &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaRenderer )
            {
            returnValue = ETrue;
            }
        // If media server devices with copy capability are accepted
        else if( iTypeOfDevicesToSearch ==
                 EUPnPSearchServerDevicesWithCopyCapability &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaServer &&
                 aDevice->CopyCapability() )
            {
            returnValue = ETrue;
            }
        // If media server devices with search capability are accepted
        else if( iTypeOfDevicesToSearch ==
                 EUPnPSearchServerDevicesWithSearchCapability &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaServer &&
                 aDevice->SearchCapability() )
            {
            returnValue = ETrue;
            }
        // If rendering devices with image capability are accepted
        else if( iTypeOfDevicesToSearch ==
                 EUPnPSearchRenderingDevicesWithImageCapability &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaRenderer &&
                 aDevice->ImageCapability() )
            {
            returnValue = ETrue;
            }
        // If rendering devices with video capability are accepted
        else if( iTypeOfDevicesToSearch ==
                 EUPnPSearchRenderingDevicesWithVideoCapability &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaRenderer &&
                 aDevice->VideoCapability() )
            {
            returnValue = ETrue;
            }
        // If rendering devices with audio capability are accepted
        else if( iTypeOfDevicesToSearch ==
                 EUPnPSearchRenderingDevicesWithAudioCapability &&
                 aDevice->DeviceType() == CUpnpAVDevice::EMediaRenderer &&
                 aDevice->AudioCapability() )
            {
            returnValue = ETrue;
            }
        else
            {
            returnValue = EFalse;
            }
        }
    return returnValue;
    }

// --------------------------------------------------------------------------
// CUPnPSelectionDialog::UpdateCommandSetL
// Updates command set of the dialog by the search type of the devices.
// --------------------------------------------------------------------------
void CUPnPSelectionDialog::UpdateCommandSetL()
    {
    // Default values, used when no items in the list.
    TInt resId = R_UPNPCOMMONUI_SOFTKEYS_EMPTY_CANCEL;
    if ( iDeviceArray->Count() )
        {
        // Updates the commands set by iTypeOfDevicesToSearch variable.
        switch( iTypeOfDevicesToSearch )
            {
            case EUPnPSearchServerDevicesWithCopyCapability:
                // When copying or moving we should use "Ok" lsk 
                // instead of "Select".
                resId = R_AVKON_SOFTKEYS_OK_CANCEL;
                break;
            default:
                resId = R_AVKON_SOFTKEYS_SELECT_CANCEL;
                break;
            }
        }
    
    CEikButtonGroupContainer* bgc = iPopup->ButtonGroupContainer();    
    bgc->SetCommandSetL( resId );
    bgc->DrawDeferred();
    }

// End of file
