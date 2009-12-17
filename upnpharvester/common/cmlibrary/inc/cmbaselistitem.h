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
* Description:      Capsulating fill and store file list items
*
*/






#ifndef __CMBASELISTITEM_H
#define __CMBASELISTITEM_H

// INCLUDES
#include <e32base.h>
#include "cmcommon.h"
#include "cmcommontypes.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmBaseListItem class
 *    Capsulating fill and store file list items
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmBaseListItem : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmBaseListItem class.
     * @return pointer to CCmBaseListItem class
     */
    IMPORT_C static CCmBaseListItem* NewL();
    
    /**
     * Creates new CCmBaseListItem class and 
     * leaves the instance in the cleanup stack.
     * @return pointer to CCmBaseListItem class
     */
    IMPORT_C static CCmBaseListItem* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmBaseListItem();

public:
    
    // Sets
    IMPORT_C void SetPrimaryTextL( const TDesC& aText );
    
    IMPORT_C void SetSecondaryTextL( const TDesC& aText );
    
    IMPORT_C void SetPathL( const TDesC& aText );
    
    IMPORT_C void SetSize( const TUint32 aSize );
    
    IMPORT_C void SetDate( const TTime aDate );
    
    IMPORT_C void SetStatus( const TCmListItemStatus aStatus );
    
    IMPORT_C void SetDbId( const TUint64 aId );
    
    IMPORT_C void SetRefId( const TUint64 aId );     
    
    IMPORT_C void SetMediaType( const TCmMediaType aMediaType );   
    
    // Gets
    IMPORT_C TDesC& PrimaryText() const;
    
    IMPORT_C TDesC& SecondaryText() const;
    
    IMPORT_C TDesC& Path() const;
    
    IMPORT_C TUint32 Size() const;
    
    IMPORT_C TTime Date() const;
    
    IMPORT_C TCmListItemStatus Status() const;
    
    IMPORT_C TUint64 DbId() const;
    
    IMPORT_C TUint64 RefId() const;        
    
    IMPORT_C TCmMediaType MediaType() const;
        
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

protected:

    /**
     * Constructor.
     */
    CCmBaseListItem();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

protected: // data

    HBufC* iPrimaryText; // owned
    HBufC* iSecondaryText; // owned
    HBufC* iPath; // owned
    TUint32 iSize;
    TTime iDate;
    TCmListItemStatus iStatus;
    TUint64 iId;
    TUint64 iRefId;    
    TCmMediaType iMediaType;    
    
    };

#endif //  __CMBASELISTITEM_H