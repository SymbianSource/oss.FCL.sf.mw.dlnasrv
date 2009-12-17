/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Fill rule and filled data managing Active object
*
*/



#ifndef __CMFMAO_H
#define __CMFMAO_H

#include <e32base.h>



// Forward Declarations
class CCmFmFillRuleFilledDataMngr;
      
 /**
  *  Class asyncronous request completions from sqlite wrapper 
  *
  *  @lib cmfillmanager.lib
  *
  *  @since S60 5.1
  */
NONSHARABLE_CLASS( CCmFmAo ): public CActive
    {
                
public:    

    /**
     * Creates new CCmFmAo class
     *
     * @since S60 5.1
     * @param aMngr, main class
     * @return  pointer to CCmFmAo class
     */
    static CCmFmAo* NewL( CCmFmFillRuleFilledDataMngr& aMngr );
    
    /**
     * Creates new CCmFmAo class
     *
     * @since S60 5.1
     * @param aMngr, main class
     * @return  pointer to CCmFmAo class
     */
    static CCmFmAo* NewLC( CCmFmFillRuleFilledDataMngr& aMngr );

    /**
     * Destructor.
     */
    virtual ~CCmFmAo(); 

// Functions from base class CActive

    /**
     * From CActive
     * See base class definition
     */
    void RunL();

    /**
     * From CActive
     * See base class definition
     */
    void DoCancel();
    
    /**
     * From CActive
     * See base class definition
     */
    virtual TInt RunError( TInt aError );
        
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aMngr, fill rule filled data manager
     */
    CCmFmAo( CCmFmFillRuleFilledDataMngr& aMngr );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:
    
    /**
     * Main manager class
     */
    CCmFmFillRuleFilledDataMngr& iMngr;
    
    /**
     * State variable if ETrue ao is on idle state
     */
    TBool iIdle;
 
    };

#endif //  __CMFMAO_H

//  End of File
