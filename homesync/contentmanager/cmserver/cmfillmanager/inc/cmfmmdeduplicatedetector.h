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
* Description:  Handles duplicate detection for images and videos
*
*/



#ifndef __CMFMMDEDUPLICATEDETECTOR_H
#define __CMFMMDEDUPLICATEDETECTOR_H

#include <e32base.h>
#include <mdequery.h>


// Forward declarations
class CCmFillListItem;
class MCmFmItemHandlerObserver;
class CMdESession;
class CMdENamespaceDef;
class CMdEObjectDef;
class CMdEObjectQuery;
class CMdEPropertyDef;

// Class declarations

/**
 *  CCmFmMdEDuplicateDetector class
 *  Part of Fill manager. CCmFmMdEDuplicateDetector manages
 *  duplicate detection for images and videos
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmMdEDuplicateDetector ) : public CBase,
                                  public MMdEQueryObserver
    {

public:    

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class
     * @param aSession, mde session
     * @return  pointer to CCmFmMdEDuplicateDetector class
     */
    static CCmFmMdEDuplicateDetector* NewL( 
        MCmFmItemHandlerObserver& aObserver, CMdESession& aSession );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class  
     * @param aSession, mde session   
     * @return  pointer to CCmFmMdEDuplicateDetector class
     */
    static CCmFmMdEDuplicateDetector* NewLC( 
        MCmFmItemHandlerObserver& aObserver, CMdESession& aSession );

    /**
     * Destructor.
     */
    virtual ~CCmFmMdEDuplicateDetector();
    
public:

    /**
     * Check if file is duplicate.
     *
     * @since S60 5.1     
     * @param aItem, item to be checked
     * @return None
     */
    void CheckImageOrVideoL( const CCmFillListItem& aItem );

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


protected: 

// From base class MMdEQueryObserver  

    /**
     * From MMdEQueryObserver
     * See base class definition
     */
    void HandleQueryNewResults( CMdEQuery& aQuery,
                                TInt aFirstNewItemIndex,
                                TInt aNewItemCount );
          
    /**
     * From MMdEQueryObserver
     * See base class definition
     */
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError); 
	
private:
    
    /**
     * Set logic condition for image and video item query
     *
     * @since S60 5.1        
     * @param aItem, item to be checked
     * @return None
     */    
    void SetLogicConditionL( const CCmFillListItem& aItem );    

    /**
     * Check if file is duplicate by compareing with array.
     * @param aItem, item to be checked
     * @return None
     */
    void CheckImageOrVideoLIfDuplicated( const CCmFillListItem& aItem );

private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, item handler observer
     * @param aSession, mde session
     */
    CCmFmMdEDuplicateDetector( MCmFmItemHandlerObserver& aObserver, 
        CMdESession& aSession );

    /**
     * Performs the second phase construction.
     */
    void ConstructL( );
    

private:

    /**
     * Observer class
     */
    MCmFmItemHandlerObserver& iObserver;
 
     /**
      * MdE session
      */
    CMdESession& iSession;
       
    /**
     * mde namespace
     */
    CMdENamespaceDef* iNamespace;               // owned
    
    /**
     * Object definition ( Image/Video )
     */
    CMdEObjectDef* iObjectDef;                  // owned

    /**
     * Object definition ( Image/Video )
     */
    CMdEObjectDef* iMediaTypeDef; // owned

    /**
     * Object query
     */
    CMdEObjectQuery* iQuery;                    // owned
        
    /**
     * Queried property
     */
    CMdEPropertyDef* iTitle;                    // owned
    
    /**
     * Queried property
     */
    CMdEPropertyDef* iSize;                     // owned
    
    /**
     * Queried property
     */
    CMdEPropertyDef* iDate;                     // owned

    /**
     * Item to be checked
     */
    CCmFillListItem* iCheckItem;

    /**
     * Array of fill list items
     */
    RPointerArray<CCmFillListItem> iExistFiles; // Owned

    /**
     * current download media type
     */
    TCmMediaType iCurrentMediaType;

    };

#endif //  __CMFMMDEDUPLICATEDETECTOR_H
