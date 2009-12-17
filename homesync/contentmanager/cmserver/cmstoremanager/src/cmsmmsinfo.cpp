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
* Description:  Store media server info class
*
*/


#include "cmsmmsinfo.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmSmMsInfo::NewL
// ---------------------------------------------------------------------------
// 
CCmSmMsInfo* CCmSmMsInfo::NewL( TDesC8& aUuid, TInt aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMsInfo::NewL"));
    
    CCmSmMsInfo* self = CCmSmMsInfo::NewLC( aUuid, aId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmMsInfo::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmMsInfo* CCmSmMsInfo::NewLC( TDesC8& aUuid, TInt aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMsInfo::NewLC"));
    
    CCmSmMsInfo* self = new ( ELeave ) CCmSmMsInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aUuid, aId );    
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmMsInfo::~CCmSmMsInfo
// ---------------------------------------------------------------------------
// 
CCmSmMsInfo::~CCmSmMsInfo()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMsInfo::~CCmSmMsInfo"));
    
    delete iUuid;      
    }
    
// ---------------------------------------------------------------------------
// CCmSmMsInfo::CCmSmMsInfo
// ---------------------------------------------------------------------------
//            
CCmSmMsInfo::CCmSmMsInfo()
    {
    LOG(_L("[STORE MNGR]\t CCmSmMsInfo::CCmSmMsInfo"));
    }

// ---------------------------------------------------------------------------
// CCmSmMsInfo::Construct
// ---------------------------------------------------------------------------
//  
void CCmSmMsInfo::ConstructL( TDesC8& aUuid, TInt aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmMsInfo::ConstructL"));
    
    iUuid = aUuid.AllocL();
    iId = aId;                                     
    } 
                        
// End of file




