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
* Description:  Definition for CMSMemoryCardMonitor class
*
*/


#ifndef __MSMMCMONITOR_H__
#define __MSMMCMONITOR_H__

#include <e32base.h>
#include <f32file.h>

// Memory card events
enum TMemoryCardEvent
    {
    EMemoryCardInsert = 0, //Insert memorycard
    EMemoryCardRemove,     //Remove memorycard
    EMemoryCardOthers      //Other event such as write, read, etc. 
    };

/**
 * Observer class for memory slot events
 *
 *  @since S60 5.1
 */
class MMSMemoryCardObserver
    {
    public:
        /**
         * Implement this method to be notified when Memory Card status
         * changes.
         *
         * @since S60 5.1
         * @param aEvent, memory slot event
         * @return None;
         */
        virtual void MemoryCardChangedL( TMemoryCardEvent aEvent ) = 0;
    };

/**
 * Memory card monitor class
 *
 * @since S60 5.1
 */
NONSHARABLE_CLASS( CMSMemoryCardMonitor ) : public CActive
    {
    public:

        /**
         * two-phase constructor
         *
         * @since S60 5.1
         * @param aObserver, memorycard
         * @param aFs, file server session
         * @return pointer to CMSMemoryCardMonitor
         */
        static CMSMemoryCardMonitor* NewL( MMSMemoryCardObserver* aObserver, 
                                           RFs& aFs );

        /**
         * two-phase constructor
         *
         * @since S60 5.1
         * @param aObserver, memorycard
         * @param aFs, file server session
         * @return pointer to CMSMemoryCardMonitor
         */
        static CMSMemoryCardMonitor* NewLC( MMSMemoryCardObserver* aObserver, 
                                           RFs& aFs );                                           
        
        /**
         * Destructor
         */
        virtual ~CMSMemoryCardMonitor();
        
        /**
         * Start monitoring memory card slot
         *
         * @since S60 5.1
         * @param None
         * @return None
         */
        void StartMonitor();
        
    protected:

// From base class CActive

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
        TInt RunError( TInt aError );
    
    private:
    
        /**
         * Performs the first phase of two phase construction.
         *
         * @since S60 5.1
         * @param aObserver, memorycard
         * @param aFs, file server session
         * @return None;
         */
        CMSMemoryCardMonitor( MMSMemoryCardObserver* aObserver, RFs& aRfs );
            
        /**
         * Performs the second phase construction.
         *
         * @since S60 5.1
         * @param aObserver, memorycard
         * @param aFs, file server session
         * @return None         
         */         
        void ConstructL(); 
        
        /**
         * Checks if memory card is available
         *
         * @since S60 5.1
         * @return TBool, ETrue if found, EFalse otherwise         
         */ 
        TBool MemoryCardExist() const;

    private:
            
        /**
         * Memorycard observer class
         */
        MMSMemoryCardObserver* iObserver;                   // not owned

        /**
         * Reference to file server session
         */
        RFs&                   iFsSession;
 
        /**
         * Memorycard exist or not
         */
        TBool                  iMemoryCardExist;
    };

#endif // __MSMMCMONITOR_H__

// End of File
