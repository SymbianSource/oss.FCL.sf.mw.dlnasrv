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
* Description:  Handles duplicate detection
*
*/



#ifndef __CMFMDUPLICATEDETECTOR_H
#define __CMFMDUPLICATEDETECTOR_H

#include <e32base.h>

// Forward declarations
class CCmFillListItem;
class CMdESession;
class CCmFmMdEDuplicateDetector;
class CCmFmMpxDuplicateDetector;
class MCmFmItemHandlerObserver;

// Class declaration
    
/**
 *  CCmFmDuplicateDetector class
 *  Part of Fill manager. CCmFmDuplicateDetector manages
 *  duplicate detection
 * 
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmDuplicateDetector ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class
     * @param aSession, mde session
     * @return  pointer to CCmFmDuplicateDetector class
     */
    static CCmFmDuplicateDetector* NewL( MCmFmItemHandlerObserver& aObserver,
        CMdESession& aSession );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class  
     * @param aSession, mde session   
     * @return  pointer to CCmFmDuplicateDetector class
     */
    static CCmFmDuplicateDetector* NewLC( MCmFmItemHandlerObserver& aObserver,
        CMdESession& aSession );

    /**
     * Destructor.
     */
    virtual ~CCmFmDuplicateDetector();    
    
public:

    /**
     * Check if file is duplicate.
     * @param aItem, item to be checked
     * @return None
     */
    void CheckItemL( const CCmFillListItem& aItem ); 

    /**
     * Add to items array for duplicated check
     * @param aItem, item to be append
     * @return None
     */
    void AppendToArrayL( const CCmFillListItem& aItem );
    
    /**
     * reset the duplicator check array
     * @param None
     * @return None
     */
    void ResetArray();

private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmFmDuplicateDetector( );

    /**
     * Performs the second phase construction.
     *
     * @since S60 5.1
     * @param aObserver, item handler observer
     * @param aSession, mde session
     */
    void ConstructL( MCmFmItemHandlerObserver& aObserver,
        CMdESession& aSession );
    

private:

    /**
     * MdE (Video & Image) duplicate detector
     */
    CCmFmMdEDuplicateDetector* iCmFmMdEDuplicateDetector; // owned
    
    /**
     * Mpx (Music) duplicate detector
     */
    CCmFmMpxDuplicateDetector* iCmFmMpxDuplicateDetector; // owned
                    
    };

#endif //  __CMFMDUPLICATEDETECTOR_H
