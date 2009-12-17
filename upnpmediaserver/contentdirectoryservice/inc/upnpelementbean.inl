/** @file
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Element table data handler - inline functions
*
*/


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpElementBean::SetElmId(TInt aElId)
{
        iElmId = aElId;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmHasAttribute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpElementBean::SetElmHasAttribute(TBool aElmHasAttribute)
{
        iElmHasAttribute = aElmHasAttribute;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmIsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpElementBean::SetElmIsRequired(TBool aElmIsRequired)
{
        iElmIsRequired = aElmIsRequired;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::SetElmObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpElementBean::SetElmObjId(TInt aElmObjId)
{
        iElmObjId = aElmObjId;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpElementBean::ElmId() const
{
        return iElmId;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmHasAttribute
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CUpnpElementBean::ElmHasAttribute() const
{
        return iElmHasAttribute;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmIsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CUpnpElementBean::ElmIsRequired() const
{
        return iElmIsRequired;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmValue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC8& CUpnpElementBean::ElmValue() const
{
        return *iElmValue;
}
// -----------------------------------------------------------------------------
// CUpnpElementBean::ElmObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpElementBean::ElmObjId() const
{
        return iElmObjId;
}

// End of File