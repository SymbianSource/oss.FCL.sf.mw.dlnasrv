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






#ifndef __CMMEDIASERVER_H
#define __CMMEDIASERVER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
 *  CCmMediaServer class
 *    Capsulating media servers
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmMediaServer : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmMediaServer class.
     * @param aUDN
     * @return  pointer to CCmMediaServer class
     */
    IMPORT_C static CCmMediaServer* NewL();

    /**
     * Creates new CCmMediaServer class and
     * leaves the instance in the cleanup stack.
     * @param aUDN
     * @return  pointer to CCmMediaServer class
     */
    IMPORT_C static CCmMediaServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmMediaServer();

public:

    /**
     * MediaServer
     * @since Series 60 3.1
     * @param None
     * @return media servers udn
     */
    IMPORT_C const TDesC8& MediaServer() const;

    /**
     * SetUDNL
     * @since Series 60 3.1
     * @param aUDN, UDN of the media server
     * @return None
     */
    IMPORT_C void SetUDNL( const TDesC8& aUDN );

    /**
     * Media server's db id
     * @since Series 60 3.1
     * @param None
     * @return Server's db id
     */
    IMPORT_C TInt64 DbId() const;

    /**
     * SetDbId
     * @since Series 60 3.1
     * @param aDbId
     * @return None
     */
    IMPORT_C void SetDbId( const TInt64 aDbId );

    /**
     * SystemUpdateID
     * @since Series 60 3.1
     * @param None
     * @return systemupdateID
     */
    IMPORT_C TInt SystemUpdateID() const;

    /**
     * SetSystemUpdateID
     * @since Series 60 3.1
     * @param aSystemUpdateID
     * @return None
     */
    IMPORT_C void SetSystemUpdateID( const TInt aSystemUpdateID );
    
    /**
    * Externalizes container information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalizes container information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    void InternalizeL( RReadStream& aStream );

protected:

    /**
     * Constructor.
     */
    CCmMediaServer();

    /**
     * Second-phase constructor.
     */
    void ConstructL();


protected: // data

    /** UDN */
    HBufC8* iUDN; // owned

    /** Database id */
    TInt64 iDbId;

    /** SystemUpdateID */
    TInt iSystemUpdateID;

    };

#endif //  __CMMEDIASERVER_H