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







#ifndef CMMDHCLIENTSESSION_H
#define CMMDHCLIENTSESSION_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES

// CLASS DECLARATION

/**
*  Metadata Harvester session
*
*  @lib cmmdhclient.lib
*  @since S60 3.1
*/
class RCmMdhSession : public RSessionBase
    {
    public:  // Constructors and destructor
        
        /**
        * Session constructor.
        */
        RCmMdhSession();
        
      
    public: // New functions
        
        /**
        * Make the client-server connection with Metadata Harvester.
        * @since S60 3.1
        * @param None
        * @return Status code depending on the success.
        */
        TInt Connect();
        
        /**
        * Cancel ongoing search or harvest operation
        * @since S60 3.1
        * @param None
        * @return void
        */
        void Cancel();
    
        /**
        * Search for media neighbourhood servers
        * @since S60 3.1
        * @param aStatus asynchronous request status
        * @return void
        */
        void SearchMediaservers( TRequestStatus& aStatus );
        
        
        /**
        * Harvest and synchronize metadata
        * @since S60 3.1
        * @param aStatus, asynchronous request status
        * @return void
        */
        void Harvest( TRequestStatus& aStatus );
        
    };
    

#endif      // CMMDHCLIENTSESSION_H
            
// End of File
