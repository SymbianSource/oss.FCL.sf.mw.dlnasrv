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
* Description:      Capsulating Fill file list items
*
*/






#ifndef __CMFILLLISTITEM_H
#define __CMFILLLISTITEM_H

// INCLUDES
#include <e32base.h>
#include "cmbaselistitem.h"
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmFillListItem class
 *  Capsulating Fill file list items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmFillListItem : public CCmBaseListItem
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmFillListItem class.
     * @return  pointer to CCmFillListItem class
     */
    IMPORT_C static CCmFillListItem* NewL();
    
    /**
     * Creates new CCmFillListItem class and 
     * leaves the instance in the cleanup stack.
     * @return  pointer to CCmFillListItem class
     */
    IMPORT_C static CCmFillListItem* NewLC();
    
    /**
     * Creates new CCmFillListItem class and 
     * leaves the instance in the cleanup stack.
     * @param aPriority, item's priority
     * @param aListId, item's list id
     * @param aStatus, items's status
     * @param aSelected, is item on selected list
     * @return  pointer to CCmFillListItem class
     */
    IMPORT_C static CCmFillListItem* NewLC( const TUint aPriority, 
        const TUint aListId, const TUint aStatus,
        const TCmFillRuleStatus aSelected );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmFillListItem();

public:
    
    // Sets
    IMPORT_C void SetUpnpClassL( const TDesC& aUpnpClass );
    
    IMPORT_C void SetUriL( const TDesC8& aUri);
    
    IMPORT_C void SetItemIdL( const TDesC8& aItemId );
    
    IMPORT_C void SetListId( const TUint aId );
    
    IMPORT_C void SetDevId( const TUint8 aDevId );
    
    IMPORT_C void SetPriority( const TUint8 aPriority );    
    
    IMPORT_C void SetSelected( TCmFillRuleStatus aSelected  );
    
    IMPORT_C void SetDriveId( const TUint aDriveId );
    
    IMPORT_C void SetDriveNumber( const TInt aDriveNumber );
    
    // Gets
    IMPORT_C TDesC& UpnpClass() const;
    
    IMPORT_C TDesC8& Uri() const;
    
    IMPORT_C TDesC8& ItemId() const;
    
    IMPORT_C TUint ListId() const;
     
    IMPORT_C TUint8 DevId() const;
    
    IMPORT_C TUint8 Priority() const;   
    
    IMPORT_C TCmFillRuleStatus Selected() const;
    
    IMPORT_C TUint DriveId() const;
    
    IMPORT_C TInt DriveNumber() const;
    
    /**
    * Externalizes container information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Internalizes container information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );    

public:

    /**
     * Constructor.
     */
    CCmFillListItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Second-phase constructor.
     */    
    void ConstructL( const TUint aPriority, const TUint aListId, 
        const TUint aStatus, const TCmFillRuleStatus aSelected );    
    

private: // data
    
    HBufC* iUpnpClass; // owned
    HBufC8* iUri; // owned
    HBufC8* iItemId; // owned
    TUint iListId;
    TUint8 iDevId;
    TUint8 iPriority;
    TCmListItemStatus iStatus;
    TCmFillRuleStatus iSelected;
    TUint iDriveId;
    TInt iDriveNumber;
    
    };

#endif //  __CMFILLLISTITEM_H