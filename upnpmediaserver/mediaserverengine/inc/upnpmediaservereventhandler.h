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
* Description:  Defines CUpnpMediaServerEventHandler class
*
*/


#ifndef C_CUPNPMEDIASERVEREVENTHANDLER_H
#define C_CUPNPMEDIASERVEREVENTHANDLER_H

// INCLUDES
#include "upnpmediaserverobserver.h"
#include "upnpcontentdirectoryeventobserver.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpObject;
class CUpnpItemList;
class CUpnpBrowseCriteria;
class CUpnpMediaServerEventHandler;
class CUpnpMediaServer;
class CUpnpFileTransferEventList;
class MUpnpContentDirectoryEventObserver;

// CLASS DECLARATION
class CUpnpMediaServerEventHandler: public CBase, MUpnpContentDirectoryEventObserver
{

public:    
    /*
    * Create a CUpnpMediaServerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpMediaServerEventHandler* NewL(CUpnpMediaServer* aServer);

    /**
    * Create a CUpnpMediaServerSession object using two phase construction,
    * and return a pointer to the created object
    * @result pointer to new session
    */
    static CUpnpMediaServerEventHandler* NewLC(CUpnpMediaServer* aServer);

    /**
    * Perform the second phase construction of a CUpnpMediaServerSession object
    */
    void ConstructL() ;
    
    /**
    * Destroy the object and release all memory objects
    */
    virtual ~CUpnpMediaServerEventHandler();
       
    /**
    * Destroy the object and release all memory objects
    */
    void EventMediaServerSubscribeEvents( const RMessage2& aMessage );
    /**
    * Destroy the object and release all memory objects
    */
    
    void EventMediaServerUnsubscribeEvents( const RMessage2& aMessage );
    /**
    * Destroy the object and release all memory objects
    */
    void EventMediaServerServerListenTransferEventL( const RMessage2& aMessage );

    /**
    * Destroy the object and release all memory objects
    */
    void EventMediaServerGetTransferEventBodyL( const RMessage2& aMessage );
    
    /**
    * Destroy the object and release all memory objects
    */
    virtual void FileTransferEvent(CUpnpFileTransferEvent *aEvent);

private:
    /**
    * Destroy the object and release all memory objects
    */
    void ServeEventL(CUpnpFileTransferEvent *aEvent); 

    /**
    * Serialize events queue and sends buffer length to client
    */
    void ListenTransferEventCompleteL();

    /**
    * Destroy the object and release all memory objects
    */
    CUpnpMediaServerEventHandler(CUpnpMediaServer* aServer);

private:
    //Media server
    CUpnpMediaServer*   iMediaServer;
    //reponse length 
    TInt                iRespLength;
    //package for reponse length 
    TPckg<TInt>         iRespLengthPkg; 
    //buffer for response 
    HBufC8*             iResponseBuffer;
    //last asynchronous message handle
    RMessage2 iCurrentMessage;
    //event queue 
    CUpnpFileTransferEventList* iFileTransferEventList;
    //indicates that there is listen request was issued  
    TBool iListenRequestPending;        
};

#endif // C_CUPNPMEDIASERVEREVENTHANDLER_H