/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      MdH Connection Monitor class declaration
*
*/





#ifndef C_MDHCONNECTIONMMONITOR_H
#define C_MDHCONNECTIONMMONITOR_H


// INCLUDES
#include <e32base.h>
#include <rconnmon.h>

#include "mdhconnectionmonitorobserver.h"

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
* CMdHConnectionMonitor provides connection monitor for 
* Metadata harvester.
*
* @lib mdhserver.exe
* @since S60 3.1
*/
class CMdHConnectionMonitor :  public CBase,
                               public MConnectionMonitorObserver
    {
    
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CMdHConnectionMonitor* NewL(
        MMdHConnectionMonitorObserver& aObserver, TInt aAccessPoint );
    
    /**
     * Destructor.
     */
    virtual ~CMdHConnectionMonitor();


protected: // From MConnectionMonitorObserver

    /**
     * Catches the Connection monitor events 
     * @since S60 3.1
     * @param aConnMonEvent event
     * @return none
     */
    void EventL( const CConnMonEventBase& aConnMonEvent ) ;

        
private:

    /**
     * C++ default constructor.
     */
    CMdHConnectionMonitor( MMdHConnectionMonitorObserver& aObserver,
        TInt aAccessPoint );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();


private:    // Data

    // Connection monitor server
    RConnectionMonitor              iConnectionMonitor;

    // Connection id
    TInt                            iConnectionId;

    // Callback pointer
    // not owned
    MMdHConnectionMonitorObserver& iObserver;

    // Accesspoint to be observed
    TInt                            iAccessPoint;

    };

#endif // C_MDHCONNECTIONMMONITOR_H
            
// End of File
