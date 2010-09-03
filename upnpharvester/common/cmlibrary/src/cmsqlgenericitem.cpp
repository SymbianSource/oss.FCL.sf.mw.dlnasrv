/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating fill and store file lists
*
*/






#include <e32std.h>
#include "cmsqlitemresource.h"
#include "cmsqlgenericitem.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlGenericItem* CCmSqlGenericItem::NewL()
    {   
    CCmSqlGenericItem* self = CCmSqlGenericItem::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
EXPORT_C CCmSqlGenericItem* CCmSqlGenericItem::NewLC()
    {    
    CCmSqlGenericItem* self = new ( ELeave ) CCmSqlGenericItem();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }    

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCmSqlGenericItem::~CCmSqlGenericItem()
    {
    delete iTitle;
    delete iUri;

    iResources.ResetAndDestroy();
    iResources.Close();            
    }
   
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetSize
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetSize( const TInt aSize )
    {
    iSize = aSize;
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetTitleL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetTitleL( const TDesC8& aTitle )
    {
    delete iTitle;
    iTitle = NULL;

    if( &aTitle )
        {
        iTitle = aTitle.AllocL();
        }
    else
        {
        iTitle = KNullDesC8().AllocL();
        }               
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetDate
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetDate( const TTime& aDate )
    {
    iDate = aDate;     
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetHarvestDate
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetHarvestDate( const TTime& aHarvestDate )
    {
    iHarvestDate = aHarvestDate;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetUpnpclassId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetUpnpclassId( const TInt64 aUpnpclassId )
    {
    iUpnpclassId = aUpnpclassId;     
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetUpnpProfileId
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetUpnpProfileId( 
    const TInt64 aUpnpProfileId )
    {
    iUpnpProfileId = aUpnpProfileId;     
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetUriL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetUriL( const TDesC8& aUri )
    {
    delete iUri;
    iUri = NULL;

    if( &aUri )
        {
        iUri = aUri.AllocL();
        }
    else
        {
        iUri = KNullDesC8().AllocL();
        }          
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::SetMediaType
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::SetMediaType( const TCmMediaType aMType )
    {
    iMType = aMType;     
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::NewResourceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::NewResourceL( const TDesC8& aUri, 
    const TInt aSize, const TInt aDuration, const TInt aBitrate,
    const TInt64 aResolutionId )
    {
    CCmSqlItemResource* item = CCmSqlItemResource::NewLC( 
        aUri, aSize, aDuration, aBitrate, aResolutionId );
    iResources.AppendL( item );
    CleanupStack::Pop( item );  
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::Size
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlGenericItem::Size() const
    {
    return iSize;
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::Title
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmSqlGenericItem::Title() const
    {
    return *iTitle;
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::Date
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CCmSqlGenericItem::Date() const
    {
    return iDate;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::HarvestDate
// ---------------------------------------------------------------------------
//
EXPORT_C TTime CCmSqlGenericItem::HarvestDate() const
    {
    return iHarvestDate;
    }
        
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::UpnpclassId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlGenericItem::UpnpclassId() const
    {
    return iUpnpclassId;
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::UpnpProfileId
// ---------------------------------------------------------------------------
//
EXPORT_C TInt64 CCmSqlGenericItem::UpnpProfileId() const
    {
    return iUpnpProfileId;
    }
    
// ---------------------------------------------------------------------------
// CCmSqlGenericItem::Uri
// ---------------------------------------------------------------------------
//
EXPORT_C TDesC8& CCmSqlGenericItem::Uri() const
    {
    return *iUri;
    } 

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::MediaType
// ---------------------------------------------------------------------------
//
EXPORT_C TCmMediaType CCmSqlGenericItem::MediaType() const
    {
    return iMType;
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::ResourceCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlGenericItem::ResourceCount() const
    {
    return iResources.Count();
    }

// ---------------------------------------------------------------------------
// CCmSqlGenericItem::GetResource
// ---------------------------------------------------------------------------
//
EXPORT_C void CCmSqlGenericItem::GetResource( TDesC8& aUri, TInt& aSize, 
    TInt& aDuration, TInt& aBitrate, TInt64& aResolutionId, TInt aIndex )
    {
    if( aIndex < iResources.Count() )
        {
        aUri = iResources[aIndex]->Uri();
        aSize = iResources[aIndex]->Size();
        aDuration = iResources[aIndex]->Duration();
        aBitrate = iResources[aIndex]->Bitrate();
        aResolutionId = iResources[aIndex]->ResolutionId();        
        }
    }
            
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlGenericItem::CCmSqlGenericItem()
    {
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlGenericItem::ConstructL()
    {
    iUri = KNullDesC8().AllocL();
    iTitle = KNullDesC8().AllocL();
    }    

// End of file

