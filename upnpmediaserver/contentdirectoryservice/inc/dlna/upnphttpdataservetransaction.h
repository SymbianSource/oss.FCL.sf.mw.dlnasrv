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
 * Description:  CUpnpHttpDataServeTransaction declaration.
 *
 */
#ifndef UPNPHTTPFILESERVETRANSACTION_H_
#define UPNPHTTPFILESERVETRANSACTION_H_

#include "upnphttpservertransaction.h"

class CUpnpDlnaFilter;
class CUpnpDlnaFilterHeaders;
class TInetAddr;

class CUpnpHttpDataServeTransaction: public CUpnpHttpServerTransaction
    {
public:
    ~CUpnpHttpDataServeTransaction();  
    
    const TDesC8& SenderUri();
    
    HBufC16* PathWithNewMethodL();
    
    static CUpnpHttpDataServeTransaction* NewL( 
        CUpnpDlnaFilter& aClientContext, const TInetAddr& aSender, const TDesC8& aUri );
    
    CUpnpDlnaFilterHeaders& FilterHeaders();
    
public:
    virtual void OnCallbackL( TUpnpHttpServerEvent aEvent );
    
    TBool IsOnFlyTransCoded();
    
protected:
    CUpnpHttpDataServeTransaction( CUpnpDlnaFilter& aClientContext );
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
    
    TBool iIsOnFlyTransCoded;
    };

#endif /* UPNPHTTPFILESERVETRANSACTION_H_ */
