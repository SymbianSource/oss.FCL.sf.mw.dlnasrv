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
* Description:  store item info class
*
*/



#ifndef __CMSMITEMINFO_H
#define __CMSMITEMINFO_H

#include <e32base.h>

// Forward declarations

// LITERALS

// CLASS DECLARATION
 /**
 *  Stored item class 
 *  @lib cmstoremanager.lib
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CCmSmItemInfo ): public CBase
    {
public:
    /**
     * Two-phased constructor.     
     *
     * @since S60 5.1 
     * @param aFile, file name
     * @param aId, media server's id
     * @return  pointer to CCmSmItemInfo class
     */
    static CCmSmItemInfo* NewL( TDesC& aFile, TInt64 aId );
    
    /**
     * Two-phased constructor.
     *
     * since S60 5.1
     * @param aFile, file name
     * @param aId, media server's id
     * @return  pointer to CCmSmItemInfo class
     */
    static CCmSmItemInfo* NewLC( TDesC& aFile, TInt64 aId );
    
    /**
     * Destructor.
     */
    virtual ~CCmSmItemInfo();        

private: 
    
    /**
     * Performs the second phase construction.
     *
     * @since S60 5.1
     * @param aFile, file name
     * @param aId, media server's id
     * @return None
     */
    void ConstructL( TDesC& aFile, TInt64 aId );
    
    /**
     * Performs the first phase of two phase construction.
     */
    CCmSmItemInfo(  );
    
public: 
    
    /**
     * File name
     */
    HBufC* iFile;                            // owned    
    /**
     * Media server identifier
     */
    TInt64 iId;    
    };

#endif //  __CMSMITEMINFO_H

