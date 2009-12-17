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
* Description:      Capsulating sql items
*
*/






#include <e32std.h>
#include "cmsqlitemresource.h"
#include "msdebug.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCmSqlItemResource* CCmSqlItemResource::NewL()
    {   
    CCmSqlItemResource* self = CCmSqlItemResource::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
CCmSqlItemResource* CCmSqlItemResource::NewLC()
    {    
    CCmSqlItemResource* self = new ( ELeave ) CCmSqlItemResource();
    CleanupStack::PushL( self );
    self->ConstructL(); 
    return self;  
    }

// ---------------------------------------------------------------------------
// NewLC( const TDesC8& aUri, 
//    const TInt aSize, const TInt aDuration, const TInt aBitrate,
//    const TInt64 aResolutionId )
// ---------------------------------------------------------------------------
//
CCmSqlItemResource* CCmSqlItemResource::NewLC( const TDesC8& aUri, 
    const TInt aSize, const TInt aDuration, const TInt aBitrate,
    const TInt64 aResolutionId )
    {
    CCmSqlItemResource* self = new ( ELeave ) CCmSqlItemResource();
    CleanupStack::PushL( self );
    self->ConstructL( aUri, aSize, aDuration, aBitrate, aResolutionId ); 
    return self;     
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCmSqlItemResource::~CCmSqlItemResource()
    {
    delete iUri;
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::SetUriL
// ---------------------------------------------------------------------------
//        
void CCmSqlItemResource::SetUriL( const TDesC8& aUri )
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
// CCmSqlItemResource::SetSize
// ---------------------------------------------------------------------------
//        
void CCmSqlItemResource::SetSize( const TInt aSize )
    {
    iSize = aSize;   
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::SetDuration
// ---------------------------------------------------------------------------
//        
void CCmSqlItemResource::SetDuration( const TInt aDuration )
    {
    iDuration = aDuration;   
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::SetBitrate
// ---------------------------------------------------------------------------
//        
void CCmSqlItemResource::SetBitrate( const TInt aBitrate )
    {
    iBitrate = aBitrate;   
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::SetResolutionId
// ---------------------------------------------------------------------------
//        
void CCmSqlItemResource::SetResolutionId( 
    const TInt64 aResolutionId )
    {
    iResolutionId = aResolutionId;   
    }
                
// ---------------------------------------------------------------------------
// CCmSqlItemResource::Uri
// ---------------------------------------------------------------------------
//     
TDesC8& CCmSqlItemResource::Uri() const
    {
    return *iUri;
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::Size
// ---------------------------------------------------------------------------
//        
TInt CCmSqlItemResource::Size() const
    {
    return iSize;
    }    

// ---------------------------------------------------------------------------
// CCmSqlItemResource::Duration
// ---------------------------------------------------------------------------
//        
TInt CCmSqlItemResource::Duration() const
    {
    return iDuration;
    } 

// ---------------------------------------------------------------------------
// CCmSqlItemResource::Bitrate
// ---------------------------------------------------------------------------
//        
TInt CCmSqlItemResource::Bitrate() const
    {
    return iBitrate;
    }

// ---------------------------------------------------------------------------
// CCmSqlItemResource::ResolutionId
// ---------------------------------------------------------------------------
//        
TInt CCmSqlItemResource::ResolutionId() const
    {
    return iResolutionId;
    }
                
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlItemResource::CCmSqlItemResource()
    {               
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlItemResource::ConstructL()
    {
    iUri = KNullDesC8().AllocL();
    }    

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CCmSqlItemResource::ConstructL( const TDesC8& aUri, 
    const TInt aSize, const TInt aDuration, const TInt aBitrate,
    const TInt64 aResolutionId )
    {
    if ( &aUri )
        {
        iUri = aUri.AllocL();
        }
    else
        {
        iUri = KNullDesC8().AllocL();
        }    
    iSize = aSize;
    iDuration = aDuration;
    iBitrate = aBitrate;
    iResolutionId = aResolutionId;
    } 
    
// End of file

