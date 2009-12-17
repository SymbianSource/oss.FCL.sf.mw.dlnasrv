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






#ifndef MCMMDHCLIENT_H
#define MCMMDHCLIENT_H

//  INCLUDES

// FORWARD DECLARATION

// CLASS DECLARATION
/**
*  Metadata Harvester client
*
*  @lib mcmmdhclient.lib
*  @since S60 3.1
*/
class MCmMdhClient
    {
    public:
            
        /**
        * Starts harvesting
        * @since S60 3.1
        * @return Error code
        */        
        virtual TInt Harvest() = 0;
		
		/**
        * Cancels request
        * @since S60 3.1
        */        
        virtual void Stop() = 0;

    /**
     * Deletes the object
     * @since S60 3.1
     */
		virtual void Close() = 0;
		
    };
    
#endif      // MCMMDHCLIENT_H
            
// End of File
