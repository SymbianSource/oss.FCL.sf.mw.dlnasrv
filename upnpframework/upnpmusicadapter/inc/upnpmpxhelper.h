/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class for some common MPX routines
*
*/


#ifndef C_UPNPMPXHELPER_H
#define C_UPNPMPXHELPER_H

// INCLUDES
#include <e32base.h>
#include "mpxmediatorobserver.h" // base class
#include "mpxcollectionobserver.h" // base class

// FORWARD DECLARATIONS
class CMPXMedia;
class MUPnPMpxHelperObserver;
class MMPXHarvesterUtility;
class MMPXCollectionUtility;
class CMPXCollectionMediator;


/**
 * A helper class for some common MPX routines
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPMpxHelper : public CBase
    , public MMPXMediatorObserver
    , public MMPXCollectionObserver
    {

public:

    /**
     * 1st phase constructor
     *
     * @param aModeId mode passed to MPX framework
     * @return a new CUPnPPlayListFiller instance
     * @since Series 60 3.1
     */
    static CUPnPMpxHelper* NewL( const TUid& aModeId );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    virtual ~CUPnPMpxHelper();

public: // the interface

    /**
     * Add a song or a playlist synchronously.
     * Note: does not take ownership of aMedia object. Caller must
     * delete aMedia after calling this method.
     * see mpxcollectionhelper in mpxmusicplayer
     * @param aMedia representation of the media to be added
     */
    void AddTrackL( CMPXMedia* aMedia );

    /**
     * add a song or a playlist asynchronously.
     * Note: does not take ownership of aMedia object. Caller must
     * delete aMedia after calling this method.
     * see mpxcollectionuihelper in mpxmusicplayer
     * @param aMedia representation of the media to be added
     * @param aObserver the observer interface
     */
    void AddPlaylistL( CMPXMedia* aMedia );

    /**
     * Open the collection in embedded mode with a Media Object.
     * see mpxcollectionuihelper in mpxmusicplayer
     * @param aHostId Host Process UID to identify this embedded instance
     * @param aMedia Media Object to open
     */
    void AddAndOpenL( const TUid& aHostId, CMPXMedia& aMedia );

protected:

    /**
     * Retrieve collection from URI
     */
    TInt FindCollectionIdL( const TDesC& aUri );

    /**
     * retrieve information for the required attributes
     */
    void FillInPlaylistDetailsL( CMPXMedia& aMedia );

    /**
     * see MMPXMediatorObserver
     */
    void HandleMediatorPathUpdatedL(
        CMPXMedia*& /*aMedia*/, TUid /*aOldPath*/ ) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleCollectionMediaL(
        const CMPXMedia& /*aMedia*/, TInt /*aError*/) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleCollectionMessageL(
        const TMPXCollectionMessage& /*aMessage*/ ) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleCollectionMessageL(
        const CMPXMessage& /*aMsg*/ ) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleOpenL(
        const CMPXMedia& /*aEntries*/,
        TInt /*aIndex*/, TBool /*aComplete*/, TInt /*aError*/ ) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleOpenL(
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/ ) {}

    /**
     * see MMPXCollectionObserver
     */
    void HandleCommandComplete(
        CMPXCommand* /*aCommandResult*/, TInt /*aError*/) {}

private: // private methods

    /**
     * constructor
     */
    CUPnPMpxHelper::CUPnPMpxHelper();

    /**
     * 2nd phase constructor
     */
    void CUPnPMpxHelper::ConstructL( const TUid& aModeId );

private: // members

    /**
     * MPX harvester utility
     */
    MMPXHarvesterUtility*       iHarvester;

    /**
     * MPX collection utility
     */
    MMPXCollectionUtility*      iCollectionUtil;

    /**
     * MPX collection mediator
     */
    CMPXCollectionMediator*     iMediator;

    /**
     * temporarily stored observer for async operations
     * NOT OWNED
     */
    MUPnPMpxHelperObserver*     iObserver;

    };

/**
 * Callback ingterface for async methods in CUPnPMpxHelper
 */
class MUPnPMpxHelperObserver
    {
public:

    /**
     * async response for AddL
     */
    void HandleAddComplete( TInt aError );

    /**
     * async response for OpenL
     */
    void HandleOpenComplete( TInt aError );

    };

#endif // C_UPNPMPXHELPER_H

