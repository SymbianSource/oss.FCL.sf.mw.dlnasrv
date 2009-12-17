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
* Description:  Duplicate detector
*
*/


#include "cmfilllistitem.h"
#include "cmfmmdeduplicatedetector.h"
#include "cmfmmpxduplicatedetector.h"
#include "cmfmduplicatedetector.h"
#include "msdebug.h"    

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::NewL
// ---------------------------------------------------------------------------
// 
CCmFmDuplicateDetector* CCmFmDuplicateDetector::NewL( 
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::NewL() start"));    
    CCmFmDuplicateDetector* self = 
        CCmFmDuplicateDetector::NewLC( aObserver, aSession );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmDuplicateDetector* CCmFmDuplicateDetector::NewLC( 
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::NewLC() start"));    
    CCmFmDuplicateDetector* self = 
        new ( ELeave ) CCmFmDuplicateDetector( );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aSession );
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::~CCmFmDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmDuplicateDetector::~CCmFmDuplicateDetector()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::\
    ~CCmFmDuplicateDetector()"));
    delete iCmFmMdEDuplicateDetector;
    delete iCmFmMpxDuplicateDetector;         
    }

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::CCmFmDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmDuplicateDetector::CCmFmDuplicateDetector( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::\
    CCmFmDuplicateDetector()"));
    }

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmDuplicateDetector::ConstructL(  
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::ConstructL()"));

    iCmFmMdEDuplicateDetector = 
        CCmFmMdEDuplicateDetector::NewL( aObserver, aSession );
    
    iCmFmMpxDuplicateDetector =
        CCmFmMpxDuplicateDetector::NewL( aObserver );    
    }
    
// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::CheckItemL
// ---------------------------------------------------------------------------
//
void CCmFmDuplicateDetector::CheckItemL( const CCmFillListItem& aItem )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDuplicateDetector::CheckItemL()"));    
    
    if( aItem.MediaType() == ECmAudio )
        {
        iCmFmMpxDuplicateDetector->CheckMusicL( aItem );
        }
    else
        {
        iCmFmMdEDuplicateDetector->CheckImageOrVideoL( aItem );
        }        
    }

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::AppendToArrayL
// ---------------------------------------------------------------------------
//
void CCmFmDuplicateDetector::AppendToArrayL( const CCmFillListItem& aItem )
    {
    if( aItem.MediaType() != ECmAudio )
        {
        iCmFmMdEDuplicateDetector->AppendToArrayL( aItem );
        }
    }

// ---------------------------------------------------------------------------
// CCmFmDuplicateDetector::ResetArray
// ---------------------------------------------------------------------------
//
void CCmFmDuplicateDetector::ResetArray()
    {
    iCmFmMdEDuplicateDetector->ResetArray();
    }
    
// End of file

