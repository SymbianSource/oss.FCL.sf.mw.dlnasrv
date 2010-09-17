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
* Description:      server impl. of session against media server
*
*/






// INTERNAL INCLUDES
#include "upnpfiletransfersessionbase.h"
#include "upnpavdeviceextended.h"
#include "upnpavcontrollerserver.h"
#include "upnpdevicerepository.h"
#include "upnpfiletransferitem.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::CUPnPFileTransferSessionBase
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
CUPnPFileTransferSessionBase::CUPnPFileTransferSessionBase(
    CUpnpAVControllerServer& aServer, TInt aSessionId ) :
    iServer( aServer ),
    iSessionId( aSessionId )
    {
    __LOG( "CUPnPFileTransferSessionBase::CUPnPFileTransferSessionBase" );
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::~CUPnPFileTransferSessionBase
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
CUPnPFileTransferSessionBase::~CUPnPFileTransferSessionBase()
    {
    __LOG( "CUPnPFileTransferSessionBase::~CUPnPFileTransferSessionBase" );
    
    delete iDevice;
    iTransferItems.Close();
    iEventQueu.Close();
    if( iEventMsg )
        {
        iEventMsg->Complete( KErrCancel );
        delete iEventMsg;
        }
    
    __LOG( "CUPnPFileTransferSessionBase::~CUPnPFileTransferSessionBase \
- end" );    
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::ConstructL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPFileTransferSessionBase::ConstructL( const TDesC8& aUuid )
    {
    __LOG( "CUPnPFileTransferSessionBase::ConstructL" );
    
    const RPointerArray<CUpnpAVDeviceExtended>& devList =
        iServer.DeviceRepository().DeviceList();
    TInt count = devList.Count();
    TInt i;
    for( i = 0; i < count; i++ )
        {
        if( devList[ i ]->Uuid() == aUuid )
            {
            iDevice = CUpnpAVDeviceExtended::NewL( *devList[ i ] );
            }             
        }
    if( !iDevice )
        {
        User::Leave( KErrNotFound );
        }        
    }
    
// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::SessionId
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
TInt CUPnPFileTransferSessionBase::SessionId() const
    {
    __LOG( "CUPnPFileTransferSessionBase::SessionId" );
    
    return iSessionId;
    }
    
// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::Uuid
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
const TDesC8& CUPnPFileTransferSessionBase::Uuid() const
    {
    __LOG( "CUPnPFileTransferSessionBase::Uuid" );
    
    return iDevice->Uuid();
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::CheckIfKeyExists
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
TBool CUPnPFileTransferSessionBase::CheckIfKeyExists( TInt aKey )
    {
    __LOG( "CUPnPFileTransferSessionBase::CheckIfKeyExists" );
    
    TBool retVal = EFalse;
    TInt count = iTransferItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTransferItems[ i ].iKey == aKey )
            {
            retVal = ETrue;
            i = count;
            }
        }
    return retVal;    
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::CheckIfKeyExists
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
TBool CUPnPFileTransferSessionBase::CheckIfKeyExists( TInt aKey,
    TInt& aIndex )
    {
    __LOG( "CUPnPFileTransferSessionBase::CheckIfKeyExists" );
    
    TBool retVal = EFalse;
    TInt count = iTransferItems.Count();
    for( aIndex = 0; aIndex < count; aIndex++ )
        {
        if( iTransferItems[ aIndex ].iKey == aKey )
            {
            retVal = ETrue;
            break;
            }
        }
    return retVal;    
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::FindItemByKey
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
TInt CUPnPFileTransferSessionBase::FindItemByKey( TUpnpFileTransferEvent&
    aItem, TInt aKey )
    {
    __LOG( "CUPnPFileTransferSessionBase::FindItemByKey" );
    
    TBool retVal = KErrNotFound;
    TInt count = iTransferItems.Count();
    for( TInt i = 0; i < count; i++ )
        {
        if( iTransferItems[ i ].iKey == aKey )
            {
            aItem = iTransferItems[ i ];
            retVal = KErrNone;
            i = count;
            }
        }
    return retVal;        
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::FindItemByKey
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
TInt CUPnPFileTransferSessionBase::FindItemByKey( TUpnpFileTransferEvent&
    aItem, TInt aKey, TInt& aIndex )
    {
    __LOG( "CUPnPFileTransferSessionBase::FindItemByKey" );
    
    TBool retVal = KErrNotFound;
    TInt count = iTransferItems.Count();
    for( aIndex = 0; aIndex < count; aIndex++ )
        {
        if( iTransferItems[ aIndex ].iKey == aKey )
            {
            aItem = iTransferItems[ aIndex ];
            retVal = KErrNone;
            break;
            }
        }
    return retVal;
    }

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::ReadTransferItemFromMessageL
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPFileTransferSessionBase::ReadTransferItemFromMessageL(
    const RMessage2& aMessage, TInt aSlot, CUpnpFileTransferItem* aItem ) 
    {
    __LOG( "CUPnPFileTransferSessionBase::ReadTransferItemFromMessageL" );
    
    // create buffer
    TInt len = aMessage.GetDesMaxLength( aSlot );
    HBufC8* buf = HBufC8::NewLC( len );
    TPtr8 ptr( buf->Des() );
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );
    
    // read stream
    RDesReadStream stream( *buf );
    CleanupClosePushL( stream );
    
    // internalize object
    stream >> *aItem;
    
    // clean up
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( buf );
    }    

// --------------------------------------------------------------------------
// CUPnPFileTransferSessionBase::Complete
// See upnpdownloadsession.h
// --------------------------------------------------------------------------
void CUPnPFileTransferSessionBase::Complete(
    const TUpnpFileTransferEvent& aEvent )
    {
    __LOG( "CUPnPFileTransferSessionBase::Complete" );          
    
    if( iEventMsg )
        {
        __LOG( "Complete" );
        __LOG1( "iEvent = %d", aEvent.iEvent );
        __LOG1( "iKey = %d", aEvent.iKey );
        __LOG1( "iStatus = %d", aEvent.iStatus );        
        TPckg<TUpnpFileTransferEvent> resp1( aEvent );
        iEventMsg->Write( 1, resp1  ); // Write response using slot 1
        iEventMsg->Complete( KErrNone );
        delete iEventMsg; iEventMsg = NULL;
        }
    else
        {
        __LOG( "Complete - Add to queu" );
        __LOG1( "iEvent = %d", aEvent.iEvent );
        __LOG1( "iKey = %d", aEvent.iKey );
        __LOG1( "iStatus = %d", aEvent.iStatus );        
        iEventQueu.AppendL( aEvent );
        }
    }
    
// End of file
