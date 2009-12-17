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
// CUpnpObjectBean::SetObjSearchable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpObjectBean::SetObjSearchable(TInt aObjSearchable)
{
        iObjSearchable = aObjSearchable;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpObjectBean::SetObjId(TInt aObjId)
{
        iObjId = aObjId;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjParentId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpObjectBean::SetObjParentId(TInt aObjParentId)
{
        iObjParentId = aObjParentId;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjRestricted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpObjectBean::SetObjRestricted(TBool aObjRestricted)
{
        iObjRestricted = aObjRestricted;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::SetObjRefId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpObjectBean::SetObjRefId(TInt aObjRefId)
{
        iObjRefId = aObjRefId;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpObjectBean::ObjId() const
{
        return iObjId;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjParentID
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpObjectBean::ObjParentId() const
{
        return iObjParentId;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjTitle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC8& CUpnpObjectBean::ObjTitle() const
{
        return *iObjTitle;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjClass
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC8& CUpnpObjectBean::ObjClass() const
{
        return *iObjClass;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjRestricted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CUpnpObjectBean::ObjRestricted() const
{
        return iObjRestricted;
}

// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjSearchable
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TBool CUpnpObjectBean::ObjSearchable() const
{
        return iObjSearchable;
}
// -----------------------------------------------------------------------------
// CUpnpObjectBean::ObjRefId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpObjectBean::ObjRefId() const
{
        return iObjRefId;
}
// End of File