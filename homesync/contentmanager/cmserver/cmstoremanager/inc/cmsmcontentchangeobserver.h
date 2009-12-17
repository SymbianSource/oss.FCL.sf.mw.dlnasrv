/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles devices internal content change events
*
*/



#ifndef __CMSMCONTENTCHANGEOBSERVER_H
#define __CMSMCONTENTCHANGEOBSERVER_H

#include <e32base.h>
#include <mdesession.h>

// FORWARD DECLARATIONS
class MCmSmFileProcessingObserver;

// CLASS DECLARATION
/**
 *  Handles devices internal content change events
 *  Store list processing will be started when notification is received
 *
 *  @lib cmstoremanager.lib
 *
 *  @since S60 v5.1
 */
class CCmSmContentChangeObserver : public CBase,
                                   public MMdEObjectObserver 
    {     
         
public:

    /**
     * Two-phased constructor.
     * Creates new CCmSmContentChangeObserver class
     *
     * @since S60 5.1
     * @param aSession, mds session
     * @param aObserver, observer class
     * @return  pointer to CCmSmContentChangeObserver class
     */
    static CCmSmContentChangeObserver* NewL( CMdESession& aSession,
        MCmSmFileProcessingObserver& aObserver );
    
    /**
     * Two-phased constructor.
     * Creates new CCmSmContentChangeObserver class
     *
     * @since S60 5.1     
     * @param aSession, mds session
     * @param aObserver, observer class
     * @return  pointer to CCmSmContentChangeObserver class
     */
    static CCmSmContentChangeObserver* NewLC( CMdESession& aSession,
        MCmSmFileProcessingObserver& aObserver );
    
    /**
     * Destructor.
     */
    virtual ~CCmSmContentChangeObserver();

    /**
     * Starts content change observers
     *
     * @since S60 5.1
     * @param None
     */
    void StartObserversL();

    /**
     * ETrue if observers are started
     *
     * @since S60 5.1
     * @param None
     * @return ETrue if observers started
     */    
    TBool IsStarted();
    
protected: 

// From base class MMdEObjectObserver

	/**
	 * From MMdEObjectObserver
	 * See base class definition
     */
    void HandleObjectNotification(CMdESession& aSession, 
						TObserverNotificationType aType,
						const RArray<TItemId>& aObjectIdArray);
		          
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * since S60 5.1
     * @param aSession, mde session
     * @param aObserver, process observer
     */
    CCmSmContentChangeObserver( CMdESession& aSession,
        MCmSmFileProcessingObserver& aObserver );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();

private: // data
    
    /**
     * MdS session
     */
    CMdESession& iMdSSession;                   // not owned
    
    /**
     * Observer to give notification about changed content
     */
    MCmSmFileProcessingObserver& iObserver;     // not owned
    
    /**
     * ETrue if observers started 
     */
    TBool iStarted;
    
    };

#endif //  __CMSMCONTENTCHANGEOBSERVER_H


