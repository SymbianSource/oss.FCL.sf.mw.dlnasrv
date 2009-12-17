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
 * Description:  CUpnpHttpFileServeTransaction declaration.
 *
 */
#ifndef UPNPHTTPFILESERVETRANSACTION_H_
#define UPNPHTTPFILESERVETRANSACTION_H_

#include "upnphttpservertransaction.h"

class CUpnpDlnaFilter;
class CUpnpDlnaFilterHeaders;
class TInetAddr;

class CUpnpHttpFileServeTransaction: public CUpnpHttpServerTransaction
    {
public:
    ~CUpnpHttpFileServeTransaction();  
    
    const TDesC8& SenderUri();
    
    HBufC16* PathWithNewMethodL();
    
    static CUpnpHttpFileServeTransaction* NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri );
    
    CUpnpDlnaFilterHeaders& FilterHeaders();
    
public:
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent );
    
protected:
    CUpnpHttpFileServeTransaction( CUpnpDlnaFilter& aClientContext );
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
    
    TFileName iPathWithNewMethod;
    };

#endif /* UPNPHTTPFILESERVETRANSACTION_H_ */
