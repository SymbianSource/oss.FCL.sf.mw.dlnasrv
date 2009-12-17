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
* Description:  Fill manager interface
*
*/


#ifndef M_CMFMMANAGER_H
#define M_CMFMMANAGER_H

// INCLUDE FILES
#include <e32std.h>
#include <badesca.h>
#include "cmcommon.h"
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class CCmSqlPropertyCollector;
class CCmSqlPropertyContainer;
class MUPnPAVController;
class CCmMediaServerFull;

/**
 *  Defines the fill manager interface
 *
 *  This class defines the fill manager interface. Provides methods to
 *  execute fill specific services
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MCmFmManager )
    {

public:

    /**
     * Starts fill service
     *
     * @since S60 5.1
     * @param None
     * @return  None
     */    
    virtual void StartFillL() = 0;

    /**
     * Cancels fill service
     *
     * @since S60 5.1     
     * @param None
     * @return  None
     */        
    virtual void CancelOperation() = 0;

    /**
     * Pre-process fill lists
     *
     * @since S60 5.1     
     * @param None
     * @return  None
     */        
    virtual void PreProcessListsL() = 0;

    /**
     * Processes defined fill list
     *
     * @since S60 5.1     
     * @param aFillListName, name of the fill list
     * @return  None
     */    
    virtual void PreProcessListL( const TDesC8& aFillListName ) = 0;

    /**
     * Provides metadata column item list
     *
     * @since S60 5.1     
     * @param aType, metadata field
     * @param aMedia, media type
     * @param aArray, array of field values
     * @return  None
     */
    virtual void GetColItemsL( TCmMetadataField aType, 
        TCmMediaType aMedia, CDesCArray& aArray ) = 0;
    
    /**
     * Provides metadata column item list
     *
     * @since S60 5.1     
     * @param aType, metadata field
     * @param aMedia, media type
     * @param aPropertys, container of field values
     * @return  None
     */
    virtual void GetColItemsL( TCmMetadataField aType, 
        TCmMediaType aMedia, CCmSqlPropertyContainer& aPropertys ) = 0;

    /**
     * Provides metadata items list
     *
     * @since S60 5.1     
     * @param aPropertys, property collector reference
     * @return  None
     */    
    virtual void GetMetadataItemsL(
                            CCmSqlPropertyCollector& aPropertys ) = 0;
    /**
     * Sets av controller
     *
     * @since S60 5.1     
     * @param aAVController, pointer to av controller
     * @return  None
     */        
    virtual void SetAvController( MUPnPAVController* aAVController ) = 0; 

    /**
     * Update fill list item priorities
     *
     * @since S60 5.1     
     * @param None
     * @return  None
     */     
    virtual void UpdatePriorities() = 0;

    /**
     * Update fill list item reference values
     *
     * @since S60 5.1     
     * @param None
     * @return  None
     */    
    virtual void UpdateRefIds() = 0;
    
    /**
     * Delete metadata from defined servers
     *
     * @since S60 5.1
     * @param None     
     * @return  None
     */ 
    virtual void DeleteMetadataL() = 0;       
        
    /**
     * Closes the utility and deletes the object
     *
     * @since S60 5.1
     * @param None     
     * @return  None     
     */
    virtual void Close() = 0;            
    
    };


#endif // M_CMFMMANAGER_H

//  End of File
