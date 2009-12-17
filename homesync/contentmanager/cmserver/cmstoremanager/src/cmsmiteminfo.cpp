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
* Description:  Store item info class
*
*/


#include "cmsmiteminfo.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmSmItemInfo::NewL
// ---------------------------------------------------------------------------
// 
CCmSmItemInfo* CCmSmItemInfo::NewL( TDesC& aFile, TInt64 aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmItemInfo::NewL"));
    
    CCmSmItemInfo* self = CCmSmItemInfo::NewLC( aFile, aId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmItemInfo::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmItemInfo* CCmSmItemInfo::NewLC( TDesC& aFile, TInt64 aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmItemInfo::NewLC"));
    
    CCmSmItemInfo* self = new ( ELeave ) CCmSmItemInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aFile, aId );    
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmItemInfo::~CCmSmItemInfo
// ---------------------------------------------------------------------------
// 
CCmSmItemInfo::~CCmSmItemInfo()
    {
    LOG(_L("[STORE MNGR]\t CCmSmItemInfo::~CCmSmItemInfo"));
    
    delete iFile;      
    }
    
// ---------------------------------------------------------------------------
// CCmSmItemInfo::CCmSmItemInfo
// ---------------------------------------------------------------------------
//            
CCmSmItemInfo::CCmSmItemInfo()
    {
    LOG(_L("[STORE MNGR]\t CCmSmItemInfo::CCmSmItemInfo"));
    }

// ---------------------------------------------------------------------------
// CCmSmItemInfo::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmItemInfo::ConstructL( TDesC& aFile, TInt64 aId )
    {
    LOG(_L("[STORE MNGR]\t CCmSmItemInfo::ConstructL"));
    
    iFile = aFile.AllocL();
    iId = aId;                                     
    }  
                        
// End of file




