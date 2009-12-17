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
 * Description:  CUpnpHttpFileServeTransaction implementation.
 *
 */
#include "upnphttpfileservetransaction.h"
#include "upnpdlnafilter.h"
#include "upnpdlnafilterheaders.h"
#include "upnperrors.h"
#include "upnpcons.h"

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::NewL
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpFileServeTransaction* CUpnpHttpFileServeTransaction::NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri )
    {
    CUpnpHttpFileServeTransaction* self = 
        new (ELeave) CUpnpHttpFileServeTransaction( aClientContext );
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
void CUpnpHttpFileServeTransaction::ConstructL( 
    const TInetAddr& aSender, const TDesC8& aSenderUri )
    {
    iFilterHeaders = CUpnpDlnaFilterHeaders::NewL( );
    iSender = aSender;
    iSenderUri = aSenderUri.AllocL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::FilterHeaders
// 
// ---------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders& CUpnpHttpFileServeTransaction::FilterHeaders()
    {
    return *iFilterHeaders;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::CUpnpHttpFileServeTransaction
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpFileServeTransaction::CUpnpHttpFileServeTransaction( CUpnpDlnaFilter& aClientContext )
    : iClientContext( aClientContext )
    {        }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::~CUpnpHttpFileServeTransaction
// 
// ---------------------------------------------------------------------------
//    
CUpnpHttpFileServeTransaction::~CUpnpHttpFileServeTransaction()
    {
    delete iFilterHeaders;
    delete iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::OnCallbackL
// 
// ---------------------------------------------------------------------------
//    
void CUpnpHttpFileServeTransaction::OnCallbackL( TUpnpHttpServerEvent aEvent )
    {
    TRAPD( err, DoCallbackL( aEvent ) );
    if ( err )
        {
        SetHttpCode( err );
        }
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::SenderUri
// 
// ---------------------------------------------------------------------------
//    
const TDesC8& CUpnpHttpFileServeTransaction::SenderUri()
    {
    return *iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::PathWithNewMethodL
// 
// ---------------------------------------------------------------------------
//  
HBufC16*  CUpnpHttpFileServeTransaction::PathWithNewMethodL()
    {
    return iPathWithNewMethod.AllocL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpFileServeTransaction::DoCallbackL
// 
// ---------------------------------------------------------------------------
//  
void CUpnpHttpFileServeTransaction::DoCallbackL( TUpnpHttpServerEvent aEvent )
    {
    switch ( aEvent )
        {
        case EOnRequestStart:
            {
            // 1. Format request file path
            //  
            
            TRAPD( error, iClientContext.FormatPathL( this, iPathWithNewMethod ) );
            
           
            if ( error != KErrNone )
                {
                SetHttpCode( error );
                }
            else if ( iPathWithNewMethod.Length() == 0 )
                {
                SetHttpCode( -EHttpNotFound );
                // Bad name, so such file doesn't exist and cannot be returned.
                }
            else
                {
                error = iClientContext.AuthorizeRequestL( iPathWithNewMethod, iSender );
                if ( error == KErrNone )
                    {
                    RFile file;
                    if ( file.Open( iClientContext.FileSession(),
                                    iPathWithNewMethod,
                                    EFileShareReadersOnly | EFileRead ) == KErrNone ) 
                        {
                        SetDataSourceL( file );
                        }
                    else
                        {
                        SetHttpCode( -EHttpNotFound );
                        }
                    }
                else
                    {
                    SetHttpCode( error );
                    }
                }                
            break;
            }
        case EOnComplete:
            break;  
        case EOnResponseStart:
            InitializeFilterHeadersL();
            SetHttpCode( iClientContext.PrepareHeaderL( *this ) );
            break;
        default:
            break;
        }            
    }

void CUpnpHttpFileServeTransaction::InitializeFilterHeadersL()
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

