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
* Description:  Media Server Client
*
*/


// INCLUDE FILES
#include "upnpmediaserverclient.h"
#include "upnpmediaserverglobals.h"
#include "upnpmediaserversettings.h"
#include "upnpicon.h"

// Number of message slots to reserve for this client server session.
const TUint KMediaServerMessageSlots = 10;
const TInt KMediaServerRetryCount = 2;

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::RUpnpMediaServerClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RUpnpMediaServerClient::RUpnpMediaServerClient()
    :RUpnpSessionBase()
    {
    iAddressPkg = NULL;
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::RUpnpMediaServerClient
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::Close()
    {
    delete iAddressPkg;
    iAddressPkg = NULL;
    RSessionBase::Close();
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Connect
// Connect to Media Server session.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Connect()
    {
    return RUpnpSessionBase::Connect( KMediaServerName,
             TVersion( KMediaServerMajorVersionNumber,
                       KMediaServerMinorVersionNumber,
                       KMediaServerBuildVersionNumber ),
             KMediaServerMessageSlots,
             KMediaServerRetryCount,
             KMediaServerFilename,
             KServerUid3 );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Status
// Gets media server operational status
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Status(TInt& aStat)
    {
    TPckgBuf<TInt> statusBuf(aStat);
    TIpcArgs args(&statusBuf);
    TInt error = SendReceive(EMediaServerStatus, args);
    aStat = statusBuf();
    return error;
    }
    
// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Start
// Client requests server to terminate its process
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Start()
    {
    return SendReceive( EMediaServerStart );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Start
// Client requests server to terminate its process (asynchonous version)
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::Start(TRequestStatus& aStatus)
    {
    SendReceive( EMediaServerStart,aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Start
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::StartOffline()
    {
    return SendReceive( EMediaServerStartOffline);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Stop()
    {
    return SendReceive( EMediaServerStop );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Stop( TServerStopMode aMode )
    {
    TInt error = 0;
    if ( aMode == EStopSilent)
        {
        error = SendReceive( EMediaServerStopSilent );
        }
    else if ( aMode == EStopNormal)
        {
        error = SendReceive( EMediaServerStop );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::Restart()
    {
    return SendReceive( EMediaServerRestart );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpMediaServerClient::GetAddress(TInetAddr& aAddr)
    {
    TPckg<TInetAddr> pkg(aAddr);
    TIpcArgs args(&pkg);
    return SendReceive( EMediaServerGetIpAndPort, args );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Stop
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::GetAddressL(TInetAddr& aAddr,
                                                    TRequestStatus& aStatus)
    {
    delete iAddressPkg;
    iAddressPkg = NULL;
    iAddressPkg = new (ELeave) TPckg<TInetAddr>(aAddr);
    TIpcArgs args(iAddressPkg);
    SendReceive( EMediaServerGetIpAndPort, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::SetUploadDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::SetUploadDirectoryL( const TDesC& aPath,
    TRequestStatus& aStatus )
    {
    CUpnpMediaServerSettings* settings = CUpnpMediaServerSettings::NewL();
    CleanupStack::PushL( settings );
    User::LeaveIfError( settings->Set(UpnpMediaServerSettings::EUploadDirectory, aPath) );
    CleanupStack::PopAndDestroy( settings );

    TIpcArgs args;
    args.Set(0, &aPath);
    SendReceive( EMediaServerSetUploadDirectory, args, aStatus );
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItem
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedItem( TInt aItemId,
                                            TDes8& aRcvdBufSize,
                                            TRequestStatus& aStatus )
    {
    TIpcArgs args(aItemId, &aRcvdBufSize);
    SendReceive(EMediaServerGetSharedItem, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItemL
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedItemResult(TDes8& aResult,
                                                    TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult);
    SendReceive(EMediaServerGetSharedItemResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareContainer
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedContainer( TInt aContainerId,
                                            TDes8& aRcvdBufSize,
                                            TRequestStatus& aStatus )
    {
    TIpcArgs args(aContainerId, &aRcvdBufSize);
    SendReceive(EMediaServerGetSharedContainer, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareContainer
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedContainerResult(TDes8& aResult,
                                                    TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult);
    SendReceive(EMediaServerGetSharedContainerResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItem
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareItem( TDes8& aSend,
                                        TDes8& aRcvdBufSize,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aSend, &aRcvdBufSize);
    SendReceive(EMediaServerShareItem, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::AddIcon
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::AddIcon( CUpnpIcon* aIcon, TRequestStatus& aStatus)
    {
    TIpcArgs args(&aIcon->GetIconBufferPtr());
    SendReceive(EMediaServerAddIcon, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UpdateIcon
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::UpdateIcon( TDes8& aSend, CUpnpIcon* aIcon,
    TRequestStatus& aStatus )
    {
    TIpcArgs args(&aIcon->GetIconBufferPtr(), &aSend);
    SendReceive(EMediaServerUpdateIcon, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::RemoveIcon
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::RemoveIcon( TDes8& aSend, TRequestStatus& aStatus)
    {
    TIpcArgs args(&aSend);
    SendReceive(EMediaServerRemoveIcon, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::RemoveIcons
// -----------------------------------------------------------------------------
//
EXPORT_C void RUpnpMediaServerClient::RemoveIcons( TRequestStatus& aStatus)
    {
    TIpcArgs args(NULL);
    SendReceive(EMediaServerRemoveIcons, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItemResult
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareItemResult( TDes8& aResult,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult);
    SendReceive(EMediaServerShareItemResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareContainer
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareContainer( TDes8& aSend,
                                        TDes8& aRcvdBufSize,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aSend, &aRcvdBufSize);
    SendReceive(EMediaServerShareContainer, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareContainerResult
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareContainerResult( TDes8& aResult,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult);
    SendReceive(EMediaServerShareContainerResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItemL
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareItemList( TDes8& aSend,
                                        TDes8& aRcvdBufSize,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aSend, &aRcvdBufSize);
    SendReceive(EMediaServerShareItemList, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareItemListResult
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareItemListResult( TDes8& aResult,
                                        TDes8& aExecStatus,
                                        TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult, &aExecStatus);
    SendReceive(EMediaServerShareItemListResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UnShareRoot
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::UnshareRoot(TRequestStatus& aStatus)
    {
    SendReceive(EMediaServerUnshareRoot,  aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UnShareItemL
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::UnshareItem(TInt aItemId, TRequestStatus& aStatus)
    {
    TIpcArgs args(aItemId);
    SendReceive(EMediaServerUnshareItem, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UnShareContainerL
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::UnshareContainer(TInt aItemId, TRequestStatus& aStatus)
    {
    TIpcArgs args(aItemId);
    SendReceive(EMediaServerUnshareContainer, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UnshareItemListL
// -----------------------------------------------------------------------------
void RUpnpMediaServerClient::UnshareItemListL(TDes8& aSend, TDes8& aExecStatus,
                                                    TRequestStatus& aStatus)
    {
    TIpcArgs args(&aSend, &aExecStatus);
    SendReceive(EMediaServerUnshareItemList, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::GetSharedItemList
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedItemList(TInt aContainerId, TDes8& aSend,
                                TDes8& aRcvdBufSize, TRequestStatus& aStatus)
    {
    TIpcArgs args(aContainerId, &aSend, &aRcvdBufSize);
    SendReceive(EMediaServerGetSharedItemList, args, aStatus);
    }
// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::GetSharedItemList
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedContainerList(TInt aContainerId, TDes8& aSend,
                                TDes8& aRcvdBufSize, TRequestStatus& aStatus)
    {
    TIpcArgs args(aContainerId, &aSend, &aRcvdBufSize);
    SendReceive(EMediaServerGetSharedContainerList, args, aStatus);
    }
// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::GetSharedItemList
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedItemListResult(TDes8& aResult, TDes8& aTotalMatches,
                                                    TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult, &aTotalMatches);
    SendReceive(EMediaServerGetSharedItemListResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::GetSharedContainerList
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetSharedContainerListResult(TDes8& aResult, TDes8& aTotalMatches,
                                                    TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult, &aTotalMatches);
    SendReceive(EMediaServerGetSharedContainerListResult, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareReference
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareReference( TDes8& aSend,
                                            TDes8& aRcvdBufSize,
                                            TRequestStatus& aStatus )
    {
    TIpcArgs args(&aSend, &aRcvdBufSize);
    SendReceive(EMediaServerShareReference, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ShareReferenceList
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ShareReferenceList( TDes8& aSend,
                                                TDes8& aRcvdBufSize,
                                                TRequestStatus& aStatus )
    {
    TIpcArgs args(&aSend, &aRcvdBufSize);
    SendReceive(EMediaServerShareReferenceList, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::SubscribeEvents
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::SubscribeEvents(TRequestStatus& aStatus)
    {
    SendReceive(EMediaServerSubscribeEvents, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::UnsubscribeEvents
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::UnsubscribeEvents()
    {
    SendReceive(EMediaServerUnsubscribeEvents);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::ListenTransferEvent
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::ListenTransferEvent(TDes8& aEventSize, TRequestStatus& aStatus)
    {
    TIpcArgs args(&aEventSize);
    SendReceive(EMediaServerListenTransferEvent, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::GetTransferEventBody
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::GetTransferEventBody(TDes8& aResult, TRequestStatus& aStatus)
    {
    TIpcArgs args(&aResult);
    SendReceive(EMediaServerGetTransferEventBody, args, aStatus);
    }

// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::Status
// Notify about media server operational status
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::StatusChangeNotify( TRequestStatus& aStatus )
    {
    SendReceive( EMediaServerStatusChange, TIpcArgs(), aStatus );
    }
    
// -----------------------------------------------------------------------------
// RUpnpMediaServerClient::CancelStatusChanged
// Cancel notification to ms status
// -----------------------------------------------------------------------------
//
void RUpnpMediaServerClient::CancelStatusChangeNotify()
    {
    SendReceive( EMediaServerStatusChangeCancel );
    }

