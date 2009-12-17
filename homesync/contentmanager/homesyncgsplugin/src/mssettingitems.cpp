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
* Description:  Setting item class implementations
*
*/


#include "mssettingitems.h"
#include "msdebug.h"

// --------------------------------------------------------------------------
// CMSTextSettingItem::CMSTextSettingItem
// --------------------------------------------------------------------------
//
CMSTextSettingItem::CMSTextSettingItem(  TInt aIdentifier, TDes& aText ) :
    CAknTextSettingItem( aIdentifier, aText )
    {
    LOG(_L("[MediaServant]\t CMSTextSettingItem::CMSTextSettingItem"));
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::SettingAccepted
// --------------------------------------------------------------------------
//
TBool CMSTextSettingItem::SettingAccepted()
    {
    LOG(_L("[MediaServant]\t CMSTextSettingItem::SettingAccepted"));

    return iOKPressed;
    }

// --------------------------------------------------------------------------
// CMSTextSettingItem::SetAcceptState
// --------------------------------------------------------------------------
//
void CMSTextSettingItem::SetAcceptState( TBool aState )
    {
    LOG(_L("[MediaServant]\t CMSTextSettingItem::SetAcceptState"));

    iOKPressed = aState;
    }

// End of File
