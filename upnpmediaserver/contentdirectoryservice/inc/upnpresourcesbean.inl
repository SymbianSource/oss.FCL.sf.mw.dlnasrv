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
* Description:  Element table data handler - inline functions
*
*/


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpResourcesBean::SetId(TInt64 aId)
{
        iId = aId;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpResourcesBean::SetObjectId(TInt aObjectId)
{
        iObjectId = aObjectId;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetIsReadonlyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpResourcesBean::SetIsReadonly(TBool aIsReadonly)
{
        iIsReadonly = aIsReadonly;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetIsThumbnail
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline void CUpnpResourcesBean::SetIsThumbnail(TBool aIsThumbnail)
{
        iIsThumbnail = aIsThumbnail;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::Id
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt64 CUpnpResourcesBean::Id() const
{
        return iId;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::ObjectId
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpResourcesBean::ObjectId() const
{
        return iObjectId;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::Path
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TDesC& CUpnpResourcesBean::Path() const
{
        return *iPath;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::IsReadonly
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpResourcesBean::IsReadonly() const
{
        return iIsReadonly;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::IsThumbnail
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline TInt CUpnpResourcesBean::IsThumbnail() const
{
        return iIsThumbnail;
}
// End of File