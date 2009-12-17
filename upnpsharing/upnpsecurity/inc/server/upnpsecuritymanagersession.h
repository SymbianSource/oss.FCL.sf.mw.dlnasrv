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
 * Description:  defines CUpnpSecurityManagerSession
 *
 */

#ifndef C_CUPNPSECURITYMANAGERSESSION_H
#define C_CUPNPSECURITYMANAGERSESSION_H

// INCLUDES
#include <e32base.h>
#include "upnpauthorizationnote.h"

// FORWARD DECLARATIONS
class CUpnpSecurityManagerEngine;

// CLASS DECLARATION

/**
 * An instance of class CUpnpSecurityManagerSession is created for each client
 */
class CUpnpSecurityManagerSession : public CSession2,
    public MUpnpAuthorizationNoteObserver
    {

public:
    /**
     * Create a CUpnpSecurityManagerSession object using two phase construction,
     * and return a pointer to the created object.
     * @param aEngine the message handler engine
     * @result pointer to new session
     */
    static CUpnpSecurityManagerSession* NewL( CUpnpSecurityManagerEngine& aEngine );

    /**
     * Create a CUpnpSecurityManagerSession object using two phase construction,
     * and return a pointer to the created object
     * (leaving a pointer to the object on the cleanup stack).
     * @param anEngine the message handler engine
     * @result pointer to new session
     */
    static CUpnpSecurityManagerSession* NewLC( CUpnpSecurityManagerEngine& aEngine );

    /**
     * Destroy the object and release all memory objects.
     */
    virtual ~CUpnpSecurityManagerSession();

public:
    // From CSession

    /**
     * Called after a service request from client; from class CSession.
     * @param aMessage message from client (containing requested operation and any data)
     */
    void ServiceL( const RMessage2& aMessage );

private:
    /**
     * Perform the first phase of two phase construction.
     * @param aServer the message handler server
     * @param aEngine the message handler engine
     */
    CUpnpSecurityManagerSession( CUpnpSecurityManagerEngine& aEngine );

    /**
     * Causes the client thread to panic.
     * @param panic code
     */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

    /**
     * Second phase constructor
     */
    void ConstructL();
    
public:
    // MUpnpAuthorizationNoteObserver methods

    /**
     * NoteResponseL
     * A callback function.
     * @param aAuthorized The result of authorization.
     */
    virtual void NoteResponseL( TBool aAuthorized );    
    
private:
    /**
     * 
     */
    void EventAddFileL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventRemoveFileL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventAddAddressL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventRemoveAddressL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventResetFileListL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventResetAddressListL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventCheckAuthorizationL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventQueryAuthorisationL( const RMessage2& aMessage );

    /**
     * 
     */
    void EventDebugSimulateNextUserAnswerL( const RMessage2& aMessage );

private:

    /* @var iEngine Reference to the engine */
    CUpnpSecurityManagerEngine& iEngine;
    
    //accept/disallow connection window
    CUpnpAuthorizationNote* iAuthorizationNote;
    
    RMessage2 iQueryAuthMessage;
    };

#endif // C_CUPNPSECURITYMANAHERSESSION_H
// End of File
