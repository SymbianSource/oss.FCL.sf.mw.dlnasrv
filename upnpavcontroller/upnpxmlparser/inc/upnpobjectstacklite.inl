/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Stack for XML SAX Parser.
*
*/






// --------------------------------------------------------------------------
// CUPnPObjectStackLite::CUPnPObjectStackLite
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStackLite::CUPnPObjectStackLite()
    {
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::~CUPnPObjectStackLite
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStackLite::~CUPnPObjectStackLite()
    {
    iStack->ResetAndDestroy();
    delete iStack;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::NewL
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStackLite* CUPnPObjectStackLite::NewL()
    {
    CUPnPObjectStackLite* self = new (ELeave) CUPnPObjectStackLite();
    CleanupStack::PushL(self);
    self->iStack = new (ELeave) RPointerArray<CUpnpObjectLite>();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::Pop
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUpnpObjectLite* CUPnPObjectStackLite::Pop()
    {
    CUpnpObjectLite* temp = NULL;

    if( Count() > 0 )
        {
        CUpnpObjectLite* temp = iStack->operator[](iStack->Count() - 1);
        iStack->Remove(iStack->Count() - 1);
        }
    return temp;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::Top
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUpnpObjectLite* CUPnPObjectStackLite::Top()
    {
    if( Count() > 0 )
        {
        return iStack->operator[](iStack->Count() - 1);
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::PushL
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStackLite::PushL( CUpnpObjectLite* aObject )
    {
    iStack->AppendL( aObject );
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::Count
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline TInt CUPnPObjectStackLite::Count()
    {
    return iStack->Count();
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::Reset
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStackLite::Reset()
    {
    iStack->Reset();
    }

// --------------------------------------------------------------------------
// CUPnPObjectStackLite::ResetAndDestroy
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStackLite::ResetAndDestroy()
    {
    iStack->ResetAndDestroy();
    }

