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
* Description:  Handles duplicate detection for music
*
*/



#ifndef __CMFMMPXDUPLICATEDETECTOR_H
#define __CMFMMPXDUPLICATEDETECTOR_H

#include <e32base.h>
#include <mpxcollectionobserver.h>


// Forward declarations
class MMPXCollectionUtility;

// Class declarations
    
/**
 * CCmFmMpxDuplicateDetector class
 * Part of Fill manager. CCmFmMpxDuplicateDetector manages
 * duplicate detection 
 *
 * @lib cmfillmanager.lib
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmMpxDuplicateDetector ) : public CBase,
                         public MMPXCollectionObserver,
                         public MMPXCollectionFindObserver
    {

public:    

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class
     * @return  pointer to CCmFmMpxDuplicateDetector class
     */
    static CCmFmMpxDuplicateDetector* NewL( 
        MCmFmItemHandlerObserver& aObserver );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, observer class  
     * @return  pointer to CCmFmMpxDuplicateDetector class
     */
    static CCmFmMpxDuplicateDetector* NewLC( 
        MCmFmItemHandlerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CCmFmMpxDuplicateDetector();
    
public:

    /**
     * Check if file is duplicate.
     *
     * @since S60 5.1
     * @param aItem, item to be checked
     * @return None
     */    
    void CheckMusicL( const CCmFillListItem& aItem );
   
   /**
     * Opens connection to collection plugin
     *
     * @since S60 5.1
     * @param None
     * @return None
     */
    void OpenCollectionPluginL();
    
   /**
     * Closes connection to collection plugin
     *
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void CloseCollectionPlugin();    
	
protected:

// From base class MMPXCollectionObserver

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
	 void HandleCollectionMediaL(const CMPXMedia& aMedia, TInt aError);

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
	void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
	
    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
	void HandleOpenL( const CMPXMedia& aEntries,
	    TInt aIndex,TBool aComplete,TInt aError );
	
    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,TInt aError);
	
// From base class MMPXCollectionFindObserver

	/**
     * From MMPXCollectionFindObserver
     * See base class definition
     */
	 void HandleFindAllL(const CMPXMedia& aResults, 
	    TBool aComplete, TInt aError);	
	
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmFmMpxDuplicateDetector( MCmFmItemHandlerObserver& aObserver );

    /**
     * Performs the second phase construction.
     */
    void ConstructL( );
    

private: // data

    /**
     * Observer class
     */
    MCmFmItemHandlerObserver& iObserver;
    
    /**
     * Collection utility of MPX framework
     */
    MMPXCollectionUtility* iCollectionUtility;          // owned
    
    /**
     * Duplicate or not
     */
    TBool iDuplicate;
            
    };

#endif //  __CMFMMPXDUPLICATEDETECTOR_H
