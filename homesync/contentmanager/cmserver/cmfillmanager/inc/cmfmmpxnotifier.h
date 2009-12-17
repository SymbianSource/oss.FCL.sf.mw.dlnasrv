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
* Description:  Mpx nofifier class
*
*/



#ifndef __CMFMMPXNOTIFIER_H
#define __CMFMMPXNOTIFIER_H

#include <e32base.h>
#include "mpxmediatorobserver.h" // base class
#include "mpxcollectionobserver.h" // base class

// Forward declarations
class MMPXHarvesterUtility;
class MMPXCollectionUtility;
class CMPXCollectionMediator;

/**
 *  CCmFmMpxNotifier class
 *  Part of Fill manager.
 *  Mpx nofifier class
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmMpxNotifier ): public CBase,
                     public MMPXMediatorObserver,
                     public MMPXCollectionObserver
    {

public:    

    /**
     * Two-phased constructor.
     */
    static CCmFmMpxNotifier* NewL( );
    
    /**
     * Two-phased constructor.
     */
    static CCmFmMpxNotifier* NewLC( );

    /**
     * Destructor.
     */
    virtual ~CCmFmMpxNotifier();
    
public: 

    /**
     * Notifying Mpx about new music files
     *
     * @since S60 5.1
     * @param aTrackPath, path to track
     * @param aTrackMetadata, metadata to be added in mpx db
     * @return None
     */     
    void NotifyMpxL( const TDesC& aTrackPath,
                    const CUpnpItem& aTrackMetadata );
                    
protected:

// From base class MMPXMediatorObserver
    
    /**
     * From MMPXMediatorObserver
     * See base class definition
     */
    void HandleMediatorPathUpdatedL(
        CMPXMedia*& /*aMedia*/, TUid /*aOldPath*/ ) {}

// From base class MMPXCollectionObserver

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleCollectionMediaL(
        const CMPXMedia& /*aMedia*/, TInt /*aError*/) {}

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleCollectionMessageL(
        const TMPXCollectionMessage& /*aMessage*/ ) {}

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleCollectionMessageL(
        const CMPXMessage& /*aMsg*/ ) {}

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleOpenL(
        const CMPXMedia& /*aEntries*/,
        TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ ) {}

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleOpenL(
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/ ) {}

    /**
     * From MMPXCollectionObserver
     * See base class definition
     */
    void HandleCommandComplete(
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/) {}                    
    
private:
    
    /**
     * Finds an element within CUpnpItem
     * returns the element value converted to unicode
     *
     * Since S60 5.1
     * @param aSource the source CUpnpItem
     * @param aSourceField the element name within source
     * @return the vield value, or 0 if element not found.
     */
    HBufC16* GetElementL( const CUpnpItem& aSource,
        const TDesC8& aSourceField ) const;    

private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmFmMpxNotifier( );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private: // data
        
    /**
     * MPX harvester utility
     */
    MMPXHarvesterUtility* iHarvester;               // Owned

    /**
     * MPX collection utility
     */
    MMPXCollectionUtility* iCollectionUtil;         // Owned

    /**
     * MPX collection mediator
     */
    CMPXCollectionMediator* iMediator;              // Owned 
       
    };

#endif //  __CMFMMPXNOTIFIER_H
