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
* Description:  Gallery notifier
*
*/



#include <f32file.h>
#include <ContentListingFactory.h>
#include <MCLFContentListingEngine.h>
#include "cmfmglxnotifier.h"
#include "msdebug.h"    


// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::NewL
// ---------------------------------------------------------------------------
// 
CCmFmGlxNotifier* CCmFmGlxNotifier::NewL( RFs& aFS )
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NewL() start"));    
    CCmFmGlxNotifier* self = CCmFmGlxNotifier::NewLC( aFS );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmGlxNotifier* CCmFmGlxNotifier::NewLC( RFs& aFS )
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NewLC() start"));    
    CCmFmGlxNotifier* self = new ( ELeave ) CCmFmGlxNotifier( );
    CleanupStack::PushL( self );
    self->ConstructL( aFS );
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::~CCmFmGlxNotifier
// ---------------------------------------------------------------------------
// 
CCmFmGlxNotifier::~CCmFmGlxNotifier()
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::~CCmFmGlxNotifier()"));
    }

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::CCmFmGlxNotifier
// ---------------------------------------------------------------------------
// 
CCmFmGlxNotifier::CCmFmGlxNotifier( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::CCmFmGlxNotifier()"));
    }

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmGlxNotifier::ConstructL( RFs& /*aFS*/ )
    {        
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::ConstructL()"));
    }    

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::NotifyGalleryL
// ---------------------------------------------------------------------------
//     
void CCmFmGlxNotifier::NotifyGalleryL( const TDesC& /*aFullPath*/ )
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NotifyGalleryL()"));
    
    MCLFContentListingEngine* clfEngine = 
            ContentListingFactory::NewContentListingEngineLC();
    
    TRAP_IGNORE( clfEngine->UpdateItemsL() );
    
    CleanupStack::PopAndDestroy(); // clfEngine
    }

// ---------------------------------------------------------------------------
// CCmFmGlxNotifier::NotifyGalleryL
// ---------------------------------------------------------------------------
// 
void CCmFmGlxNotifier::NotifyGalleryL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmGlxNotifier::NotifyGalleryL()"));
   
    MCLFContentListingEngine* clfEngine = 
            ContentListingFactory::NewContentListingEngineLC();
            
    TRAP_IGNORE( clfEngine->UpdateItemsL() );
    
    CleanupStack::PopAndDestroy(); // clfEngine
    }        
        
// End of file



