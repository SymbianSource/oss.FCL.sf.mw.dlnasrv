/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  CUpnpHttpFileReceiveTransaction implementation.
 *
 */     

#include <bautils.h> 
#include "upnphttpfilereceivetransaction.h"
#include "upnpdlnafilter.h"
#include "upnpdlnafilterheaders.h"
#include "upnperrors.h"
#include "upnpcons.h"

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::NewL
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpFileReceiveTransaction* CUpnpHttpFileReceiveTransaction::NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri )
    {
    CUpnpHttpFileReceiveTransaction* self = 
        new (ELeave) CUpnpHttpFileReceiveTransaction( aClientContext );
    CleanupStack::PushL( self );
    self->ConstructL( aSender, aUri );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileReceiveTransaction::ConstructL( 
    const TInetAddr& aSender, const TDesC8& aSenderUri )
    {
    iFilterHeaders = CUpnpDlnaFilterHeaders::NewL( );
    iSender = aSender;
    iSenderUri = aSenderUri.AllocL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::CUpnpHttpFileReceiveTransaction
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpFileReceiveTransaction::CUpnpHttpFileReceiveTransaction(
        CUpnpDlnaFilter& aClientContext ) :
    iClientContext(aClientContext)
    {
    }
        
// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::~CUpnpHttpFileReceiveTransaction
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpFileReceiveTransaction::~CUpnpHttpFileReceiveTransaction()
    {
    delete iFilterHeaders;
    delete iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::OnCallbackL
// 
// ---------------------------------------------------------------------------
//    
const TDesC8& CUpnpHttpFileReceiveTransaction::SenderUri()
    {
    return *iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::FilterHeaders
// 
// ---------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders& CUpnpHttpFileReceiveTransaction::FilterHeaders()
    {
    return *iFilterHeaders;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::OnCallbackL
// 
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileReceiveTransaction::OnCallbackL( TUpnpHttpServerEvent aEvent )
    {
    TRAPD( err, DoCallbackL( aEvent ) );
    if ( err )
        {
        SetHttpCode( err );
        }
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileReceiveTransaction::DoCallbackL
// 
// ---------------------------------------------------------------------------
//
void CUpnpHttpFileReceiveTransaction::DoCallbackL( TUpnpHttpServerEvent aEvent )
    {
    switch ( aEvent )
        {
        case EOnRequestStart:
            {
            iFileName.Zero();
            InitializeFilterHeadersL();
            User::LeaveIfError(  iClientContext.CheckDLNAPostCorrelationsL(
                                          *this ) );
            
            HBufC* fileNameBuf = iClientContext.DetermineDownloadPathL( *this );           
            if ( fileNameBuf )
                { 
                CleanupStack::PushL( fileNameBuf );
                
                RFile file;
                if ( file.Open( iClientContext.FileSession(),
                                *fileNameBuf,
                                EFileWrite ) == KErrNone ) 
                    {
                    SetDataSinkL( file );
                    SetHttpCode( iClientContext.AuthorizeRequestL( *fileNameBuf, iSender ) );
                    }
                else
                    {
                    BaflUtils::EnsurePathExistsL( iClientContext.FileSession(),
                                                   fileNameBuf->Des() );
                    if ( file.Create( iClientContext.FileSession(),
                                                    *fileNameBuf ,
                                                    EFileWrite ) != KErrNone ) 
                        {
                        SetHttpCode( -EHttpNotFound );
                        }
                    else
                        {
                        SetDataSinkL( file );
                        SetHttpCode( iClientContext.AuthorizeRequestL( *fileNameBuf, iSender ) );
                        iFileName.Copy( *fileNameBuf );
                        }
                    }
                CleanupStack::PopAndDestroy( fileNameBuf );
                } 
            else
                {
                SetHttpCode( -EHttpNotFound );
                }
            
            break;
            }
        case EOnComplete:
            break;    
        default:
            break;
        }            
    }

void CUpnpHttpFileReceiveTransaction::InitializeFilterHeadersL()
    {
    //Copy current KHdrContentFeatures and KHdrTransferMode to filterHeaders
    if ( QueryRequestHeader( UpnpDLNA::KHdrContentFeatures ).Length() > 0 )
        {
        iFilterHeaders->AddHeaderL( UpnpDLNA::KHdrContentFeatures,
                QueryRequestHeader( UpnpDLNA::KHdrContentFeatures ) );            
        }
    if ( QueryRequestHeader( UpnpDLNA::KHdrTransferMode ).Length() > 0 )
        {
        iFilterHeaders->AddHeaderL( UpnpDLNA::KHdrTransferMode,
                QueryRequestHeader( UpnpDLNA::KHdrTransferMode ) );            
        }
    }

