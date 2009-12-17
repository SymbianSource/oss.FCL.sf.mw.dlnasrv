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
* Description:      a data class containing filetransfer-related data
*
*/






#ifndef T_TUPNPFILETRANSFEREVENT_H
#define T_TUPNPFILETRANSFEREVENT_H

#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION
/**
 *  UPnP AV Controller Client/Server communication helper class
 *  
 *
 *  @lib - 
 *  @since Series 60 3.2
 */
class TUpnpFileTransferEvent
    { 

public:
    
    /**
     * Defines filetransfer events
     */    
    enum TTransferEvent
        {
        EUndefined = 0,
        ETransferStarted,
        ETransferCompleted,
        ETransferProgress,
        EDeviceDisconnected
        };
        
public:        
    
    /**
     * Constructor
     */    
    inline TUpnpFileTransferEvent() :
        iEvent( EUndefined ),
        iKey( 0 ),
        iStatus( 0 ),
        iParam1( 0 ),
        iParam2( 0 ),
        iParam3( 0 )
        {}
        
    /**
     * Assigment operator
     */    
    inline TUpnpFileTransferEvent& operator=(
        const TUpnpFileTransferEvent& aEvent )
        {
        iEvent = aEvent.iEvent;
        iKey = aEvent.iKey;
        iStatus = aEvent.iStatus;
        iParam1 = aEvent.iParam1;
        iParam2 = aEvent.iParam2;
        iParam3 = aEvent.iParam3;
        return *this;
        }
        
public:
       
    /**
     * Filetransfer event
     */    
    TTransferEvent  iEvent;

    /**
     * Transfer key. Identifies the file transfer
     */    
    TInt            iKey;
    
    /**
     * Status (error) code
     */        
    TInt            iStatus;
    
    /**
     * Undefined param 1
     */        
    TInt            iParam1;
    
    /**
     * Undefined param 2
     */            
    TInt            iParam2;
    
    /**
     * Undefined param 3
     */            
    TAny*           iParam3;
    
    };

#endif // T_TUPNPFILETRANSFEREVENT_H

// End of file

