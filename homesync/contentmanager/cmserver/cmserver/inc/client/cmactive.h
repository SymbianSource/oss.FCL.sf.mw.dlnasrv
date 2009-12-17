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
* Description:  Content Manager server's client ao header
*
*/


#ifndef CMACTIVE_H
#define CMACTIVE_H

#include <badesca.h>
#include "contentmanager.h"

NONSHARABLE_CLASS ( CCmActive ) : public CActive
    {
public:

    /**
     * Two-phased constructor.
     *
     * @param aCm, cmserver reference
     */
    static CCmActive* NewL( RContentManager& aCm );

    /**
     * Two-phased constructor.
     *
    * @param aCm, cmserver reference     
     */
    static CCmActive* NewLC( RContentManager& aCm );
    
    /**
     * Destructor.
     */
    virtual ~CCmActive();

    
// From base class CActive

    /**
     * From CActive
     * see base class definition
     */
    void RunL();

protected:

// From base class CActive

    /**
     * From CActive
     * see base class definition
     */
    void DoCancel();

    /**
     * From CActive
     * see base class definition
     */
    TInt RunError( TInt aError );

private:

    /**
     * Performs the first phase of two phase construction
     *
     * @since S60 5.1
     * @param aCm, client reference.
     */    
    CCmActive( RContentManager& aCm );
    
    /**
     * Performs the second phase construction.
     */    
    void ConstructL();
        
private:

    /**
     * server client interface
     */
    RContentManager& iCm;
    /**
     * idle indicator
     */
    TBool iIdle;
    };

#endif

// End of file
