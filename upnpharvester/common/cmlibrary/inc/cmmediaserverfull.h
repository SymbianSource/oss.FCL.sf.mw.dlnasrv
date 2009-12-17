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
* Description:      Capsulating Media server
*
*/






#ifndef __CMMEDIASERVERFULL_H
#define __CMMEDIASERVERFULL_H

#include <e32base.h>
#include "cmmediaserver.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmMediaServerFull class
 *    Capsulating media servers
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmMediaServerFull : public CCmMediaServer
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmMediaServerFull class.
     * @param None
     * @return  pointer to CCmMediaServerFull class
     */
    IMPORT_C static CCmMediaServerFull* NewL();

    /**
     * Creates new CCmMediaServerFull class and
     * leaves the instance in the cleanup stack.
     * @param None
     * @return  pointer to CCmMediaServerFull class
     */
    IMPORT_C static CCmMediaServerFull* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmMediaServerFull();

public:

    /**
     * SetUDNL
     * @since Series 60 3.1
     * @return name
     */
    IMPORT_C TPtrC8 MediaServerName() const;

    /**
     * SetMediaServerNameL
     * @since Series 60 3.1
     * @param aName
     * @return None
     */
    IMPORT_C void SetMediaServerNameL( const TDesC8& aName );

    /**
     * VisibleDate
     * @since Series 60 3.1
     * @param None
     * @return visible date
     */
    IMPORT_C TTime VisibleDate() const;

    /**
     * SetVisibleDate
     * @since Series 60 3.1
     * @param Time
     * @return None
     */
    IMPORT_C void SetVisibleDate( TTime aTime );

    /**
     * IsActive
     * @since Series 60 3.1
     * @param None
     * @return Is active flag
     */
    IMPORT_C TUint8 IsActive() const;

    /**
     * SetIsActive
     * @since Series 60 3.1
     * @param aIsActive
     * @return None
     */
     IMPORT_C void SetIsActive( TUint8 aIsActive );

    /**
     * Gets copy capability information
     * @since Series 60 3.1
     * @return TBool, ETrue if copy capable, EFalse otherwise
     */
     IMPORT_C TBool CopyCapability() const;

    /**
     * Sets copy capability information
     * @since Series 60 3.1
     * @param aCopyCapable
     * @return None
     */
     IMPORT_C void SetCopyCapability( TBool aCopyCapability );

    /**
     * Sets server store usage status
     * @since Series60 3.2
     * @param aUsed, status value
     */
    IMPORT_C void SetStoreUsage(
                    TBool aUsed );

    /**
     * Gets server store usage status
     * @since Series60 3.2
     * @return TBool, usage status value
     */
    IMPORT_C TBool StoreUsage();

    /**
     * Sets server fill usage status
     * @since Series60 3.2
     * @param aUsed, status value
     */
    IMPORT_C void SetFillUsage(
                    TBool aUsed );

    /**
     * Gets server fill usage status
     * @since Series60 3.2
     * @return TBool, usage status value
     */
    IMPORT_C TBool FillUsage();

    /**
    * Externalizes media server information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalizes media server information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );


private:


private: // data

    /** Name */
    HBufC8* iName; // owned

    /** Visible date */
    TTime iVisibleDate;

    /** Is active flag */
    TUint8 iIsActive;

    /** Copy capability flag **/
    TBool iCopyCapability;
    
    /** Fill usage **/
    TBool iFillServer;
    
    /** Store usage **/
    TBool iStoreServer;

    };

#endif //  __CMMEDIASERVERFULL_H