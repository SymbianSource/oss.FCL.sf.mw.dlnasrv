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
* Description:      MdH Connection monitor observer interface class declaration.
*
*/






#ifndef M_MDHCONNECTIONMONITOROBSERVER_H
#define M_MDHCONNECTIONMONITOROBSERVER_H

//  INCLUDES
#include <e32std.h>

/**
 * MMdHConnectionMonitorObserver is an interface for Connection Monitor.
 *
 * @lib mdhserver.exe
 * @since S60 3.1
 */
class MMdHConnectionMonitorObserver
    {
public:
    
    /**
     * This function will be called when WLan connection is lost for mdh 
     *
     * @since S60 3.1
     */    
    virtual void ConnectionLost() = 0;
    };

#endif // M_MDHCONNECTIONMONITOROBSERVER_H
     
// End of File
