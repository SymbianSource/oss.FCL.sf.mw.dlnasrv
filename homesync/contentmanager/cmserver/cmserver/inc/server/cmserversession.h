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
* Description:  content manager server session header
*
*/



#ifndef CMSERVERSESSION_H
#define CMSERVERSESSION_H


// FORWARD DECLARATIONS
class CCmServer;

// CLASS DECLARATION

/**
*  CCmSession
*  This is a class for content manager server main session
*
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CCmSession ) : public CSession2
    {
public:

    /**
     * Two-phased constructor.
     */
    static CCmSession* NewL( CCmDmMain& aDbManager,
                                 MCmSettings& aSettings );

    /**
     * Destructor.
     */
    virtual ~CCmSession();

// From base class CSession2    

    /**
     * From CSession2.
     * see base class definition
     */
    void ServiceL( const RMessage2 &aMessage );

protected:

    /**
     * Panics the client
     *
     * @since S60 5.1
     * @param aMessage, received message
     * @param aPanic, panic code
     */
    void PanicClient( const RMessage2& aMessage, TInt aPanic ) const;

private:

    /**
     * Constructor.
     */
    CCmSession( CCmDmMain& aDbManager, MCmSettings& aSettings );

    /**
     * Performs the second phase construction.
     */   
    void ConstructL();

    /**
     * Performs the first phase of two phase construction
     * @since S60 3.1
     * @param void
     * @return content manager server reference
     */
    CCmServer& Server();

private:

    /**
     * Database manager
     */
    CCmDmMain&                  iDbManager;
    /**
     * Settings engine
     */
    MCmSettings&                iSettings;
    /**
     * Buffer for collected data
     */
    CBufFlat*                   iDataBuffer;        // owned
    /**
     * Metadata collector
     */
    CCmSqlPropertyCollector*    iCollector;         // owned
    };

#endif      // CMSERVERSESSION_H

// End of File
