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
* Description:      Main application window
*
*/





#include <StringLoader.h> 
#include <homemedia.rsg>

// debug log support
_LIT( KComponentLogfile, "homemedia.txt");
#include "upnplog.h"

#include "homemediacontainer.h"
#include "upnppluginloader.h"
#include "upnpfilesharingengine.h"
#include "iupnp.hlp.hrh"

//CONSTANTS
#define KHomeMEdiaUID           0x2000F8D6 // home media app UID
static const TInt KNumberOfItemsBeforePlugins( 2 );
_LIT( KFormatWithOneStr , "%d\t%S\t\t" );
_LIT( KFormatWithTwoStr , "%d\t%S\t%S\t" );

// --------------------------------------------------------------------------
// CHomeMediaContainer::NewL
// --------------------------------------------------------------------------
//
CHomeMediaContainer* CHomeMediaContainer::NewL(const TRect& aRect,
    CUPnPFileSharingEngine& aFileSharing,
    const RPointerArray<CUPnPPluginInterface>& aPluginArray)
    {
    CHomeMediaContainer* self = 
        CHomeMediaContainer::NewLC(aRect, aFileSharing, aPluginArray );
    CleanupStack::Pop(self); 
    return self;
    }

// --------------------------------------------------------------------------
// CHomeMediaContainer::NewLC
// --------------------------------------------------------------------------
//
CHomeMediaContainer* CHomeMediaContainer::NewLC(const TRect& aRect,
    CUPnPFileSharingEngine& aFileSharing,
    const RPointerArray<CUPnPPluginInterface>& aPluginArray)
    {
    CHomeMediaContainer* self = 
        new (ELeave) CHomeMediaContainer(aFileSharing,aPluginArray);
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// --------------------------------------------------------------------------
// CHomeMediaContainer::~CHomeMediaContainer
// --------------------------------------------------------------------------
//
CHomeMediaContainer::~CHomeMediaContainer()
    {
    }

// --------------------------------------------------------------------------
// CHomeMediaContainer::UpdateL
// --------------------------------------------------------------------------
//
void CHomeMediaContainer::UpdateL()
    {
    if( iListBoxItems->Count() > 0 )
        {
        iListBoxItems->Reset();
        }
    //Set Browse item
    HBufC* columnText = StringLoader::LoadLC( R_HOMEMEDIA_BROWSEHOME_TEXT );
    AddColumnL(KFormatWithOneStr,0,columnText);
    CleanupStack::PopAndDestroy( columnText );

    //Set Sharing item
    HBufC* secondaryText = NULL;
    if ( iFileSharing.SharingStateL() )
        {
        secondaryText = 
            StringLoader::LoadLC( R_HOMEMEDIA_SHARE_ACTIVE_TEXT );
        }
    else
        {
        secondaryText = 
            StringLoader::LoadLC( R_HOMEMEDIA_SHARE_INACTIVE_TEXT );
        }

    columnText = StringLoader::LoadLC( R_HOMEMEDIA_SHARECONTENT_TEXT );
    AddColumnL( KFormatWithTwoStr,
                1,
                columnText,
                secondaryText );
    CleanupStack::PopAndDestroy( columnText );
    CleanupStack::PopAndDestroy( secondaryText );

    //Load plugins icons & texts
    UpdatePluginsL();
   }

// --------------------------------------------------------------------------
// CHomeMediaContainer::CHomeMediaContainer
// --------------------------------------------------------------------------
//
CHomeMediaContainer::CHomeMediaContainer( 
    CUPnPFileSharingEngine& aFileSharing,
    const RPointerArray<CUPnPPluginInterface>& aPluginArray ):
    iFileSharing( aFileSharing ),
    iPluginArray( aPluginArray )
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CHomeMediaContainer::ConstructL
// --------------------------------------------------------------------------
//
void CHomeMediaContainer::ConstructL(const TRect& aRect)
    {
    CAknDoubleLargeStyleListBox* listBoxModel
                                   = new (ELeave)CAknDoubleLargeStyleListBox;
    BaseConstructL( aRect , listBoxModel );
    UpdateL();
    }

// --------------------------------------------------------------------------
// CHomeMediaContainer::UpdatePluginsL
// --------------------------------------------------------------------------
//
void CHomeMediaContainer::UpdatePluginsL()
    {
    __LOG("CHomeMediaContainer::UpdatePluginsL begin");
    CEikFormattedCellListBox* listBox = 
        static_cast<CEikFormattedCellListBox*>( &ListBox() );

    CArrayPtr<CGulIcon>& icons = 
        *listBox->ItemDrawer()->FormattedCellData()->IconArray();
    // Remove old plugins icons
    TInt pluginIcons = icons.Count() - KNumberOfItemsBeforePlugins;
    while( pluginIcons )
        {
        pluginIcons--;
        CGulIcon* icon = icons[ KNumberOfItemsBeforePlugins ];
        icons.Delete( KNumberOfItemsBeforePlugins );
        delete icon;
        icon = NULL;
        }

    for( TInt i = 0; i < iPluginArray.Count(); i++ )
        {
        //append icon
        const CGulIcon& icon = iPluginArray[i]->GetIcon();
        CGulIcon* copyIcon = CGulIcon::NewL( icon.Bitmap(), icon.Mask() );
        CleanupStack::PushL( copyIcon );
        // Icon is handled by us, but bitmaps by plugin
        // when we delete icon, we delete icon only but not bitmaps
        copyIcon->SetBitmapsOwnedExternally( ETrue );
        icons.AppendL( copyIcon ); //Ownership goes to array
        CleanupStack::Pop( copyIcon );

        HBufC* primary = NULL;
        HBufC* secondary = NULL;

        // if title of the plugin is not null alloc space for it
        if ( &(iPluginArray[i]->GetTitle()) )
            {
            primary = ( iPluginArray[i]->GetTitle() ).AllocLC();
            }
        else
            {
            primary = KNullDesC().AllocLC();
            }

        // if secondary text of the plugin is not null alloc space for it
        if ( &(iPluginArray[i]->GetSecondaryText()) )
            {
            secondary = ( iPluginArray[i]->GetSecondaryText() ).AllocLC();
            }
        else
            {
            secondary = KNullDesC().AllocLC();
            }

        AddColumnL( KFormatWithTwoStr,
                    i + KNumberOfItemsBeforePlugins,
                    primary,
                    secondary );

        // deletion of the texts after they have been set to listbox item
        CleanupStack::PopAndDestroy( secondary );
        CleanupStack::PopAndDestroy( primary );
        }
    __LOG("CHomeMediaContainer::UpdatePluginsL end");
    }


// ---------------------------------------------------------------------------
// CHomeMediaContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CHomeMediaContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {

    aContext.iMajor = TUid::Uid(0x2000F8D6);
    aContext.iContext = KUPNP_HLP_MAIN_VIEW;
    }
