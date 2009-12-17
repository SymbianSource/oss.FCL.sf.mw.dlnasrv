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
 * Description:  CUpnpHttpFileReceiveTransaction declaration.
 *
 */
#ifndef UPNPHTTPFILERECEIVETRANSACTION_H_
#define UPNPHTTPFILERECEIVETRANSACTION_H_

// INCLUDES
#include "upnphttpservertransaction.h"
#include <in_sock.h>

// FORWARD DECLARATION
class CUpnpDlnaFilter;
class CUpnpDlnaFilterHeaders;
class CUpnpDlnaFilterHeaders;

/**
 *  Transaction serving incoming dlna transfer ( http post, upload usecase ). 
 *  
 */
class CUpnpHttpFileReceiveTransaction: public CUpnpHttpServerTransaction
    {
public:
    /**
    * Constructors and destructors.
    */
    static CUpnpHttpFileReceiveTransaction* NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri );
        
    ~CUpnpHttpFileReceiveTransaction();
    
    /**
     * Getter.
     */
    CUpnpDlnaFilterHeaders& FilterHeaders();

    /**
     * Base class callback implementation
     */ 
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent );
    
    const TDesC8& SenderUri();

protected:
    CUpnpHttpFileReceiveTransaction( CUpnpDlnaFilter& aClientContext );
    void ConstructL( const TInetAddr& aSender, const TDesC8& aSenderUri );  
    
private:
    void DoCallbackL( TUpnpHttpServerEvent aEvent );    

    void InitializeFilterHeadersL();
    
private:
    // Sender Uri.
    HBufC8* iSenderUri;
    // Sender address.
    TInetAddr iSender;
    
    CUpnpDlnaFilter& iClientContext;
    CUpnpDlnaFilterHeaders* iFilterHeaders;
    TFileName iFileName;
    };

#endif /* UPNPHTTPFILERECEIVETRANSACTION_H_ */
