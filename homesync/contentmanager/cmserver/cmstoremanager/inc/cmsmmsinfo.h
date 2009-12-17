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
* Description:  media server info class
*
*/



#ifndef __CMSMMSINFO_H
#define __CMSMMSINFO_H

#include <e32base.h>

// CLASS DECLARATION
 /**
  *  Media server info class
  *
  *  @lib cmstoremanager.lib
  *
  *  @since S60 5.1
  */    
NONSHARABLE_CLASS( CCmSmMsInfo ): public CBase
    {
public:

    /**
     * Two-phased constructor.
     * Creates new CCmSmMsInfo class
     *
     * @since S60 5.1
     * @param aUuid, server's uuid
     * @param aId, media server's id
     * @return  pointer to CCmSmMsInfo class
     */
    static CCmSmMsInfo* NewL( TDesC8& aUuid, TInt aId );
    
    /**
     * Two-phased constructor.
     * Creates new CCmSmMsInfo class
     *
     * @since S60 5.1
     * @param aUuid, server's uuid
     * @param aId, media server's id
     * @return  pointer to CCmSmMsInfo class
     */
    static CCmSmMsInfo* NewLC( TDesC8& aUuid, TInt aId );
    
    /**
     * Destructor.
     */
    virtual ~CCmSmMsInfo();        

private: 
    
    /**
     * Performs the second phase construction.
     */
    void ConstructL( TDesC8& aUuid, TInt aId );
    
    /**
     * Performs the first phase of two phase construction.
     */
    CCmSmMsInfo( );
    
public: 
    
    /**
     * Server's uuid
     */
    HBufC8* iUuid;                              // owned
    
    /**
     * Media server identifier
     */
    TInt iId;    
    };      
  
#endif //  __CMSMMSINFO_H

