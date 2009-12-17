/** @file
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attribute table data handler - inline functions
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpAttributeBean::SetAtrId(TInt aElId)
{
        iAtrId = aElId;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrIsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpAttributeBean::SetAtrIsRequired(TBool aAtrIsRequired)
{
        iAtrIsRequired = aAtrIsRequired;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::SetAtrObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpAttributeBean::SetAtrElmId(TInt aAtrElmId)
{
        iAtrElmId = aAtrElmId;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AtrId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpAttributeBean::AtrId() const
{
        return iAtrId;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AtrName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC8& CUpnpAttributeBean::AtrName() const
{
        return *iAtrName;
}

// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AtrIsRequired
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CUpnpAttributeBean::AtrIsRequired() const
{
        return iAtrIsRequired;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AtrValue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC8& CUpnpAttributeBean::AtrValue() const
{
        return *iAtrValue;
}
// -----------------------------------------------------------------------------
// CUpnpAttributeBean::AtrObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpAttributeBean::AtrElmId() const
{
        return iAtrElmId;
}

// End of File