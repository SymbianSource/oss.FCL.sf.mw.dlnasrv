/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Metadata Harvester server executable header file
*
*/







#ifndef MDHSERVER_H
#define MDHSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "mdhmediaservercontainer.h"

// ---------------------------------------------------------------
// Server's policy
// ---------------------------------------------------------------

//Total number of ranges
const TUint KCmMdhServerRangeCount = 2;

//Definition of the ranges of IPC numbers
const TInt KCmMdhServerRanges[KCmMdhServerRangeCount] = 
        {
        0,
        3
        };

//Policy to implement for each of the above ranges        
const TUint8 KCmMdhServerElementsIndex[KCmMdhServerRangeCount] = 
        {
        1, //applies to 1st range
        CPolicyServer::ENotSupported
        };

//Specific capability checks
const CPolicyServer::TPolicyElement KCmMdhServerElements[] = 
        {
        {_INIT_SECURITY_POLICY_C3(ECapabilityNetworkServices,
            ECapabilityReadUserData, ECapabilityWriteUserData ),
            CPolicyServer::EFailClient },
        {_INIT_SECURITY_POLICY_C1(ECapabilityNetworkServices),
            CPolicyServer::EFailClient}
        };

//Package all the above together into a policy
const CPolicyServer::TPolicy KCmMdhServerPolicy =
        {
        //specifies all connect attempts should pass
        CPolicyServer::EAlwaysPass, 
        KCmMdhServerRangeCount,
        KCmMdhServerRanges,
        KCmMdhServerElementsIndex,
        KCmMdhServerElements
        };


// DATA TYPES

enum TCmMdhServerState
        {
        ECmMdhServerStateIdle = 0,
        ECmMdhServerStateSearching,
        ECmMdhServerStateHarvesting
        };

// FORWARD DECLARATIONS

// FUNCTION PROTOTYPES

// Method to panic server in case of serious error
void PanicServer( TInt aPanic );
// Method to panic client if e.g. message is malformed
void PanicClient( const RMessage2& aMessage, TInt aPanic );


// CLASS DECLARATION

/**
*  Dummy Server Core class
*
*  @lib CmMdhServer
*  @since Series 60 3.1
*/
class CCmMdhServer : public CPolicyServer
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCmMdhServer* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CCmMdhServer();

    public: // New functions
    
        /**
        * Sets server's state
        * @since S60 3.1 
        * @param aServerState, server state
        * @param aErrCode error code.
        */
        void SetServerStateL( TCmMdhServerState aServerState, 
                              TInt aErrCode = KErrNone );

        /**
        * Decrement server sessions
        * @since S60 3.1 
        */        
        void DecrementSessions();

        /**
        * Returns server's current state
        * @since S60 3.1
        * @return server state
        */         
        TCmMdhServerState ServerState();
 
         /**
        * Set current asyncronous message
        * @since S60 3.1
        * @param aMessage, message
        */        
        void SetCurrentAsyncMsg( const RMessage2& aMessage );
    
         /**
        * Creates media server container instance
        * @since S60 3.1
        */         
        void CreateMediaserverContainerL();
        
         /**
        * Destroys media server container instance
        * @since S60 3.1
        */        
        void DestroyMediaserverContainer();   
                
         /**
        * Returns media server container reference
        * @since S60 3.1
        * @return media server container
        */        
        CCmMdhMediaserverContainer& MediaserverContainer();
    
        /**
        * Increments sessions
        * @since S60 3.1
        */ 
        void IncrementSessions();             
    
    private:

        /**
        * C++ default constructor.
        */
        CCmMdhServer();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Functions from base classes
        
        CSession2* NewSessionL( const TVersion& aVersion,
                                const RMessage2& aMessage ) const;
    

    private:    // Data
        
        /** Current asyncronous message */
        RMessage2                   iCurrentAsyncMsg;
        
        /** Media server container ( owned ) */
        CCmMdhMediaserverContainer* iMediaserverContainer; 
        
        /** Server state */
        TCmMdhServerState           iServerState;
        
        /** Count of sessions */
        TInt                       iSessionCount;
        
    };


#endif // MDHSERVER_H
           
// End of File
