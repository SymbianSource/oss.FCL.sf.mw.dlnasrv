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
* Description:  Store manager interface
*
*/


#ifndef M_CMSTOREMANAGER_H
#define M_CMSTOREMANAGER_H

// INCLUDE FILES
#include <e32std.h>
#include <badesca.h>
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class MUPnPAVController;
class CCmSqlPropertyContainer;

/**
 * Defines the store manager interface
 *
 * This class defines the store manager interface. Provides methods to
 * execute storing
 *
 * @lib cmstoremanager.lib
 *
 * @since S60 5.1
 */
class MCmSmManager
    {

public:

    /**
     * Starts store service
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    virtual void StartStoreL() = 0;

    /**
     * Cancels store service
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */        
    virtual void CancelOperation() = 0;

    /**
     * Starts store list processing
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */        
    virtual void PreProcessLists() = 0;

    /**
     * Provides album list
     *
     * @since S60 5.1
     * @param aMedia, media type
     * @param aArray, list of values
     * @return  None
     */
    virtual void GetColItemsL( TCmMediaType aMedia, 
        CDesCArray& aArray ) = 0;
        
    /**
     * Provides metadata column item list
     *
     * @since S60 5.1
     * @param aMedia, media type
     * @param aPropertys, list of values
     * @return  None
     */
    virtual void GetColItemsL( TCmMediaType aMedia, 
        CCmSqlPropertyContainer& aPropertys ) = 0;        
    
    /**
     * Sets av controller
     *
     * @since S60 5.1
     * @param aAVController, av controller pointer
     * @return  None
     */        
    virtual void SetAvController( MUPnPAVController* aAVController ) = 0;   
    
    /**
     * Closes the utility and deletes the object
     *
     * @since S60 5.1
     */
    virtual void Close() = 0;
    };


#endif // M_CMSTOREMANAGER_H
