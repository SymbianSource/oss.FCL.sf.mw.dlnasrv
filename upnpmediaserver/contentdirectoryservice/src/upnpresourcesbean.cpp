/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Element table data handler
*
*/


// INCLUDE FILES
#include "upnpresourcesbean.h"
#include "upnpcontentdirectoryglobals.h"
#include <upnpelement.h>
#include "upnpcdutils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpResourcesBean::CUpnpResourcesBean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean::CUpnpResourcesBean()
{

}

// -----------------------------------------------------------------------------
// CUpnpResourcesBean::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpResourcesBean::ConstructL()
{

}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean* CUpnpResourcesBean::NewLC()
{
    CUpnpResourcesBean* self = new( ELeave ) CUpnpResourcesBean;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean* CUpnpResourcesBean::NewLC(const RDbRowSet& aRowSet)
{
    CUpnpResourcesBean* self = NewLC();
    self->SetL(aRowSet);
    return self;
}    
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean* CUpnpResourcesBean::NewL(const RDbRowSet& aRowSet)
{
    CUpnpResourcesBean* self = NewLC(aRowSet);
    CleanupStack::Pop();
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::~CUpnpResourcesBean
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean::~CUpnpResourcesBean()
{
    delete iPath;
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpResourcesBean::SetL(const RDbRowSet& aRowSet)
{
    // get cols ids
    CDbColSet* colSet = aRowSet.ColSetL();
    CleanupStack::PushL(colSet);
    const TInt idColNo          =    colSet->ColNo( KRscIdColName       );
    const TInt pathColNo        =    colSet->ColNo( KRscPathColName     );
    const TInt isReadonlyColNo  =    colSet->ColNo( KRscReadonlyColName );
    const TInt isThumbnailColNo  =   colSet->ColNo( KRscThumbnailColName );
    const TInt idObjColNo       =    colSet->ColNo( KRscIdObjColName );
    
    CleanupStack::PopAndDestroy(colSet);
    
    // for each column call setter
    if( idColNo != KDbNullColNo )
    {
        SetId(aRowSet.ColInt64(idColNo));
    }
    if( pathColNo != KDbNullColNo )
    {
        SetPathL(aRowSet.ColDes(pathColNo));
    }
    if( isReadonlyColNo != KDbNullColNo )
    {
        SetIsReadonly(aRowSet.ColUint8(isReadonlyColNo));
    }
    if( isThumbnailColNo != KDbNullColNo )
    {
        SetIsThumbnail(aRowSet.ColUint8(isThumbnailColNo));
    }
    if( idObjColNo != KDbNullColNo )
    {
        SetObjectId(aRowSet.ColInt(idObjColNo));
    }
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetPathL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpResourcesBean::SetPathL(const TDesC& aPath)
{
    delete iPath;
    iPath = NULL;
    iPath = aPath.AllocL();
}
// -----------------------------------------------------------------------------
// CUpnpResourcesBean::SetPathL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpResourcesBean::SetPathL(const TDesC8& aPath)
{
	delete iPath;
	iPath = NULL;
	iPath = UpnpCdUtils::Des8ToDesL(aPath);
}


//  End of File
