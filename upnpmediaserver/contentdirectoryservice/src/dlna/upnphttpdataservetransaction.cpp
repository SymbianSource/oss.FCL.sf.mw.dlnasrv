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
 * Description:  CUpnpHttpDataServeTransaction implementation.
 *
 */
#include "upnphttpdataservetransaction.h"
#include "upnpdlnafilter.h"
#include "upnpdlnafilterheaders.h"
#include "upnperrors.h"
#include "upnpcons.h"

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::NewL
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpDataServeTransaction* CUpnpHttpDataServeTransaction::NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri )
    {
    CUpnpHttpDataServeTransaction* self = 
        new (ELeave) CUpnpHttpDataServeTransaction( aClientContext );
    CleanupStack::PushL( self );
    self->ConstructL( aSender, aUri );
    CleanupStack::Pop( self );
    return self;
    }    
    
// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CUpnpHttpDataServeTransaction::ConstructL( 
    const TInetAddr& aSender, const TDesC8& aSenderUri )
    {
    iFilterHeaders = CUpnpDlnaFilterHeaders::NewL( );
    iSender = aSender;
    iSenderUri = aSenderUri.AllocL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::FilterHeaders
// 
// ---------------------------------------------------------------------------
//
CUpnpDlnaFilterHeaders& CUpnpHttpDataServeTransaction::FilterHeaders()
    {
    return *iFilterHeaders;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::CUpnpHttpDataServeTransaction
// 
// ---------------------------------------------------------------------------
//
CUpnpHttpDataServeTransaction::CUpnpHttpDataServeTransaction( CUpnpDlnaFilter& aClientContext )
    : iClientContext( aClientContext )
    {        }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::~CUpnpHttpDataServeTransaction
// 
// ---------------------------------------------------------------------------
//    
CUpnpHttpDataServeTransaction::~CUpnpHttpDataServeTransaction()
    {
    delete iFilterHeaders;
    delete iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::OnCallbackL
// 
// ---------------------------------------------------------------------------
//    
void CUpnpHttpDataServeTransaction::OnCallbackL( TUpnpHttpServerEvent aEvent )
    {
    TRAPD( err, DoCallbackL( aEvent ) );
    if ( err )
        {
        SetHttpCode( err );
        }
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::IsOnFlyTransCoded
// ---------------------------------------------------------------------------
//  
TBool CUpnpHttpDataServeTransaction::IsOnFlyTransCoded()
    {
    return iIsOnFlyTransCoded;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::SenderUri
// 
// ---------------------------------------------------------------------------
//    
const TDesC8& CUpnpHttpDataServeTransaction::SenderUri()
    {
    return *iSenderUri;
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::PathWithNewMethodL
// 
// ---------------------------------------------------------------------------
//  
HBufC16*  CUpnpHttpDataServeTransaction::PathWithNewMethodL()
    {
    return iPathWithNewMethod.AllocL();
    }

// ---------------------------------------------------------------------------
// CUpnpHttpDataServeTransaction::DoCallbackL
// 
// ---------------------------------------------------------------------------
//  
void CUpnpHttpDataServeTransaction::DoCallbackL( TUpnpHttpServerEvent aEvent )
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
                _LIT( KTranscodedTag, "?transcoded=true");
                if( iPathWithNewMethod.Find(KTranscodedTag()) >= 0 )
                    {
                    iIsOnFlyTransCoded = ETrue;
                    TFileName fileName(iPathWithNewMethod);
                    iPathWithNewMethod.Zero();
                    iPathWithNewMethod.Append(
                         fileName.Mid(0,fileName.Find(KTranscodedTag())) );
                    }
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

void CUpnpHttpDataServeTransaction::InitializeFilterHeadersL()
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

