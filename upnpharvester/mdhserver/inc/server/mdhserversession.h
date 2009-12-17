/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Metadata Harvester server session header
*
*/







#ifndef CMMDHSERVERSESSION_H
#define CMMDHSERVERSESSION_H

//  INCLUDES
#include <e32std.h>
//include "mdhmediaservercontainer.h"

// FORWARD DECLARATIONS

class CCmMdhMediaserverContainer;
class CCmMdhServer;

// CLASS DECLARATION

/**
*  CCmMdhSession
*  This is a class for metadata harvester server main session
*
*  @lib -
*  @since Series 60 3.0
*/
class CCmMdhSession : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCmMdhSession* NewL( 
            CCmMdhMediaserverContainer* aMediaserverContainer, 
            CCmMdhServer& aServer );
        
        /**
        * Destructor.
        */
        ~CCmMdhSession();
        
    public: // Functions from base classes

        /**
        * ServiceL from base class CSession2.
        * @since Series 60 3.0
        * @param aMessage Message for Metadata Harvester
        * @return void
        */
        void ServiceL( const RMessage2 &aMessage );
        
    public: // New functions
    
    private:

       /**
        * Constructor.
        */
        CCmMdhSession( CCmMdhMediaserverContainer* aMediaserverContainer, 
            CCmMdhServer& aServer );
         
       /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Server returns the server reference.
        * @since Series 60 3.1
        * @param void
        * @return Metadata harvester server reference
        */      
        CCmMdhServer& Server();

        /**
        * Completes message with busy error code if server is busy
        * @since Series 60 3.1
        * @param aMessage message
        * @return ETrue if completed
        */      
        TBool CompleteIfBusy( const RMessage2& aMessage );
        

    private:    // Data
    
        // media server container pointer (not owned)
        CCmMdhMediaserverContainer* iMediaserverContainer;
                
        // Server
        CCmMdhServer& iServer;        
   
    };

#endif      // CMMDHSERVERSESSION_H   
            
// End of File
