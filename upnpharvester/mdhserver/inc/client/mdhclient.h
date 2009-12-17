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
* Description:      Metadata Harvester server's client header
*
*/







#ifndef CMMDHCLIENT_H
#define CMMDHCLIENT_H

//  INCLUDES
#include    <e32base.h>
#include    "mdhclientsession.h"
#include    "mmdhclient.h"

// FORWARD DECLARATION
class RCmMdhSession;
class MCmServiceObserver;

// CLASS DECLARATION
/**
*  Metadata Harvester client
*
*  @lib cmmdhclient.lib
*  @since S60 3.1
*/
class CCmMdhClient : public CActive,
                     public MCmMdhClient
    {
    public:
    
        enum TCmMdhClientState  
            {
            ECmMdhClientIdle = 0,
            ECmMdhClientSearching,
            ECmMdhClientHarvesting
            };

    public:
    
        /**
        * Two-phased constructor.
        */
        static CCmMdhClient* NewL( MCmServiceObserver& aServer );
       
        /**
        * Two-phased constructor.
        */       
        static CCmMdhClient* NewLC( MCmServiceObserver& aServer );
 
        /**
        * Destructor
        */            
        virtual ~CCmMdhClient();
                   
    public:
            
        /**
        * Starts harvesting
        * @since S60 3.1
        * @return Error code
        */        
        TInt Harvest();
    
        /**
        * Cancels request
        * @since S60 3.1
        * @return Error code
        */        
        void Stop();
	
    /**
     * Deletes the object
     * @since S60 3.1
     */
		void Close();
	
    private:
            
        /**
         * From CActive
         * Callback function.
         * Invoked to handle responses from the server.
         */
        void RunL();

        /**
         * From CActive
         * Cancels any outstanding operation.
         */
        void DoCancel();
        
    private:
 
        /**
        * Default constructor
        */        
        CCmMdhClient( MCmServiceObserver& aServer );
        
        /**
        * ConstructL
        */        
        void ConstructL( );
                
    private:
    
        /** Instance of mdh session */
        RCmMdhSession iMdhSession;
        
        /** Observer */
        MCmServiceObserver& iServer;
        
        /** State of the client */
        TCmMdhClientState iState;
    };
    
#endif      // CMMDHCLIENT_H
            
// End of File
