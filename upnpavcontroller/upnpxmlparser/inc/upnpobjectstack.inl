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
// CUPnPObjectStack::CUPnPObjectStack
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStack::CUPnPObjectStack()
    {
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::~CUPnPObjectStack
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStack::~CUPnPObjectStack()
    {
    iStack->ResetAndDestroy();
    delete iStack;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::NewL
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUPnPObjectStack* CUPnPObjectStack::NewL()
    {
    CUPnPObjectStack* self = new (ELeave) CUPnPObjectStack();
    CleanupStack::PushL(self);
    self->iStack = new (ELeave) RPointerArray<CUpnpObject>();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::Pop
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUpnpObject* CUPnPObjectStack::Pop()
    {
    CUpnpObject* temp = NULL;

    if( Count() > 0 )
        {
        CUpnpObject* temp = iStack->operator[](iStack->Count() - 1);
        iStack->Remove(iStack->Count() - 1);
        }
    return temp;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::Top
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline CUpnpObject* CUPnPObjectStack::Top()
    {
    if( Count() > 0 )
        {
        return iStack->operator[](iStack->Count() - 1);
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::PushL
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStack::PushL( CUpnpObject* aObject )
    {
    iStack->AppendL( aObject );
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::Count
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline TInt CUPnPObjectStack::Count()
    {
    return iStack->Count();
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::Reset
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStack::Reset()
    {
    iStack->Reset();
    }

// --------------------------------------------------------------------------
// CUPnPObjectStack::ResetAndDestroy
// See upnpobjectstack.h
// --------------------------------------------------------------------------
inline void CUPnPObjectStack::ResetAndDestroy()
    {
    iStack->ResetAndDestroy();
    }

