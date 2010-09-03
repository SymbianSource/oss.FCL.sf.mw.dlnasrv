/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Proxy class to handle multiple download sessions
*
*/


#include "upnpavcontroller.h"
#include "upnpavdevicelist.h"
#include "upnpitem.h"
#include "cmfmdownloadmngr.h"
#include "cmfmdownloadproxy.h"
#include "msdebug.h"

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::NewL
// ---------------------------------------------------------------------------
//
CCmFmDownloadProxy* CCmFmDownloadProxy::NewL( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::NewL() start"));    
    CCmFmDownloadProxy* self = CCmFmDownloadProxy::NewLC( );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::NewLC
// ---------------------------------------------------------------------------
//   
CCmFmDownloadProxy* CCmFmDownloadProxy::NewLC( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::NewLC() start"));    
    CCmFmDownloadProxy* self = 
        new ( ELeave ) CCmFmDownloadProxy( );
    CleanupStack::PushL( self );
    self->ConstructL( );
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::~CCmFmDownloadProxy
// ---------------------------------------------------------------------------
//
CCmFmDownloadProxy::~CCmFmDownloadProxy()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::~CCmFmDownloadProxy()"));
    CancelOperation();
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::CCmFmDownloadProxy
// ---------------------------------------------------------------------------
//        
CCmFmDownloadProxy::CCmFmDownloadProxy( )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::CCmFmDownloadProxy()"));
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::ConstructL
// ---------------------------------------------------------------------------
// 
void CCmFmDownloadProxy::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::ConstructL()"));
    }          

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::FetchFileL
// ---------------------------------------------------------------------------
//  
void CCmFmDownloadProxy::FetchFileL( MCmFmTransferObserver* aObserver, 
    MUPnPAVController* aAVController, CUpnpAVDeviceList* aDevices, 
    const CUpnpElement& aResElement, const CUpnpItem& aItem, RFile& aFile, 
    const TInt aInternalDeviceIndex, const TInt64 aInternalItemId )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::FetchFileL()"));
    
    // Starts download session if no session for defined device exists yet
    TInt index = StartNewDownloadSessionIfNeededL( aObserver, 
                                                   aAVController, 
                                                   aDevices, 
                                                   aInternalDeviceIndex );
        
    if( index > KErrNotFound && index < iDownloadSessions.Count() )
        {
        TRACE(Print(_L("[FILL MNGR]\t FetchFileL index = %d"), index ));
        iDownloadSessions[ index ]->
                FetchFileL( aResElement, aItem, aFile, aInternalItemId );   
        }
    else
        {
        User::Leave( KErrNotFound );
        } 
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::CloseDownloadSession
// ---------------------------------------------------------------------------
//
TBool CCmFmDownloadProxy::CloseDownloadSession( const TInt aSessionId )
    {
    TRACE(Print(_L("[FILL MNGR]\t CloseDownloadSession = %d"), aSessionId ));
    TBool sessionsLeft( EFalse );
    
    if( KErrNotFound < aSessionId && 
        aSessionId < iDownloadSessions.Count() && 
        iDownloadSessions[ aSessionId ] )
        {
        // Delete defined download session
        delete iDownloadSessions[ aSessionId ];
        iDownloadSessions[ aSessionId ] = NULL;
        iDownloadSessionCount--;
        TInt temp( KErrNotFound );
        for( TInt i = 0; i < iSessionIndexes.Count(); i++ )
            {
            if( iSessionIndexes[i].iSessionIndex == aSessionId )
                {
                temp = i; 
                }
            }
    
        if( temp != KErrNotFound )
            {
            iSessionIndexes.Remove( temp );
            }        
        }
    
    if( iDownloadSessionCount )
        {
        sessionsLeft = ETrue;
        }
    else
        {
        LOG(_L("[FILL MNGR]\t No sessions left!"));
        }
    
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::CloseDownloadSession() end"));        
    return sessionsLeft;    
    }
    
// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::CancelOperation
// ---------------------------------------------------------------------------
//     
void CCmFmDownloadProxy::CancelOperation()
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::CancelOperation()"));
    iDownloadSessions.ResetAndDestroy();
    iSessionIndexes.Reset();
    iSessionIndexes.Close();
    iDownloadSessionCount  = 0;                
    }
                    
// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::StartNewDownloadSessionIfNeededL
// ---------------------------------------------------------------------------
// 
TInt CCmFmDownloadProxy::StartNewDownloadSessionIfNeededL( 
    MCmFmTransferObserver* aObserver, MUPnPAVController* aAVController, 
    CUpnpAVDeviceList* aDevices, const TInt aInternalDeviceIndex )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::\
    StartNewDownloadSessionIfNeededL()"));
    
    TBool newServer( ETrue );
    TInt sessionIndex( KErrNotFound );
    for( TInt i = 0; i < iSessionIndexes.Count(); i++ )
        {
        if( iSessionIndexes[i].iDeviceIndex == aInternalDeviceIndex )
            {
            TRACE(Print(_L("[FILL MNGR]\t old server sessionIndex = %d"), 
                i ));
            newServer = EFalse;
            sessionIndex = i;
            // Index found => end loop
            i = iSessionIndexes.Count();
            }
        }
    
    if( newServer )
        {
        TRACE(Print(_L("[FILL MNGR]\t New server found")));       

        CreateNewSessionL( aObserver, aAVController, aDevices, 
            iDownloadSessionCount, sessionIndex );              
        }
    TRACE(Print(_L("[FILL MNGR]\t started sessionIndex = %d"), sessionIndex ));        
    
    return sessionIndex;              
    }

// ---------------------------------------------------------------------------
// CCmFmDownloadProxy::CreateNewSessionL
// ---------------------------------------------------------------------------
// 
void CCmFmDownloadProxy::CreateNewSessionL(  
    MCmFmTransferObserver* aObserver, MUPnPAVController* aAVController, 
    CUpnpAVDeviceList* aDevices, TInt aSessionCount, TInt& aSessionIndex )
    {
    LOG(_L("[FILL MNGR]\t CCmFmDownloadProxy::CreateNewSessionL()"));
    
    CCmFmDownloadMngr* mngr = CCmFmDownloadMngr::NewL( aAVController, aObserver,
       *(*aDevices)[aSessionCount], aSessionCount );
    CleanupStack::PushL( mngr );
    iDownloadSessions.AppendL( mngr );            
    CleanupStack::Pop( mngr );
    iDownloadSessionCount++;             
    TCmFmDownloadSessionIndexes index;
    index.iDeviceIndex = aSessionCount;   
    index.iSessionIndex = ( iDownloadSessions.Count() - 1 );
    iSessionIndexes.AppendL( index );
    aSessionIndex = index.iSessionIndex;     
    }
     
// End of file

