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
* Description:      class for containing and obtaining music track-specific
*                data from media server
*
*/






#ifndef C_UPNPTRACK_H
#define C_UPNPTRACK_H

// INCLUDES
#include <e32base.h>
#include "upnpitemresolverobserver.h"
#include "upnpresourceselector.h"

// FORWARD DECLARATIONS
class TMPXAttribute;
class TMPXAttributeData;
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class MUPnPTrackObserver;
class MMPXPlaybackPluginObserver;
class CUpnpItem;
class MUPnPItemResolver;

// CONSTANTS
const TInt KMaxElementSize = 256;


/**
 *  A helper class for containing and obtaining music track-specific data
 *  from media server
 *
 *  @lib upnpmusicplugins.lib
 *  @since S60 v3.1
 */
class CUPnPTrack
    : public CBase
    , public MUPnPItemResolverObserver
    {

public: // datatypes 

    /**
     * Defines direction of playback
     *
     * EDirectionRemote  - Used in local to remote and remote to remote
     *                     playback cases 
     * EDirectionLocal   - Used only in remote to local case.
     * 
     */
    enum TPlaybackDirection
        {
        EDirectionRemote = 0,
        EDirectionLocal
        };
public:

    /**
     * static constructor
     *
     * @param aAvController AVController
     * @return a new CUPnPTrack instance
     * @since Series 60 3.1
     */
    static CUPnPTrack* NewL( MUPnPAVController& aAvController );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    virtual ~CUPnPTrack();

    /**
     * Parse and saves the song path from given descriptor
     *
     * @param aCodedTrackUri track URI that may be coded
     * @param atrackObserver observer to notify when resolve is complete
     * @param aPlaybackDirection direction of playback
     * @since Series 60 3.1
     */
    void ResolveURIL( const TDesC& aCodedTrackUri,
        MUPnPTrackObserver& aTrackObserver, 
        TPlaybackDirection aPlaybackDirection );

    /**
     * Returns the original URI given in initialisation
     *
     * @return TDesC URI
     * @since Series 60 3.1
     */
    const TDesC& URI() const;

    /**
     * Returns the true UPNP URI of a remote track
     *
     * @return TDesC8 URI
     * @since Series 60 3.1
     */
    const TDesC8& UpnpURI() const;

    /**
     * Returns the remote upnp item that represents the track
     *
     * @return CUpnpItem the item reference
     * @since Series 60 3.1
     */
    const CUpnpItem& UpnpItem() const;
    
    /**
     * Returns the file path of local item 
     *
     * @return TDesC file path
     * @since Series 60 3.1
     */
    const TDesC& FilePath() const;

    /**
     * Return location of track. ETrue = remote. EFalse = local
     *
     * @return TBool location of track
     * @since Series 60 3.1
     */
    TBool IsRemote() const;

    /**
     * Get metadata information for track from media server
     * 
     * @param aAttrs list of wanted media propeties
     * @param aObs the plugin observer that will receive the metadata
     * @since Series 60 3.1
     */
    void GetMetaDataL( const TArray<TMPXAttribute>& aAttrs,
        MMPXPlaybackPluginObserver& aObs );

    /**
     * Delivers media changed event to playback framework
     * @param aObs the plugin observer
     */
    void SendMediaChangedEventL(
        MMPXPlaybackPluginObserver& aObs );

    /**
     * current track duration
     * @return track duration in milliseconds
     */
    TInt TrackDuration();

    /**
     * overrides current track duration. Normally a track can provide
     * duration information for current remote track. However sometimes
     * the data is missing. In this case it can be set using this method.
     * @param aMilliseconds the duration in milliseconds
     */
    void SetTrackDuration( TInt aMilliseconds );

    /**
     * 
     */
    void Delete();
    
protected: // MUPnPItemResolverObserver

    /**
     * See MUPnPItemResolverObserver
     */
    void ResolveComplete( const MUPnPItemResolver& aResolver, TInt aError );

private:

    /**
     * Default constructor
     *
     * @param aAvController AVController
     * @since Series 60 3.1
     */
    CUPnPTrack( MUPnPAVController& aAvController );

    /**
     * 2nd phase constructor
     *
     * @since Series 60 3.1
     */
    void ConstructL();
    
    /**
     * Parse and save HttpURI, media server id and object id from 
     * given descriptor
     *
     * @since Series 60 3.1
     * @param aSong descriptor to be parsed
     */
    void ParsePiecesL( const TDesC& aSong );
    
    /**
     * See MUPnPAVBrowsingSessionObserver
     *
     * @since Series 60 3.1
     */
    void BrowseResponseL( const TDesC8& aBrowseResponse, TInt aError );

    /**
     * Starts a browsing session within the object
     */
    void StartBrowsingSessionL();

    /**
     * Stops a browsing session within the object
     */
    void StopBrowsingSession();

    /**
     * Delivers metadata to observer
     * @param aAttrs list of wanted media propeties
     * @param aObs the plugin observer that will receive the metadata
     * @param aError error code to be delivered to client
     */
    void DeliverMedataL( const TArray<TMPXAttribute>& aAttrs,
        MMPXPlaybackPluginObserver& aObs, TInt aError = KErrNone );

    /**
     * Fills item metadata into given MediaL object
     * @param aMedia the MPX media object to fill metadata into
     * @param aAttrs list of metadata elements to fill
     */
    void FillMediaFromItemL( CMPXMedia& aMedia,
        const TArray<TMPXAttribute>& aAttrs );

    /**
     * Tests if given attribute exists in the attribute array
     * returns true if it is found.
     * @param aAttrs the attribute array
     * @param aAttrData the attribute to look for
     * @return ETrue if it was found, EFalse otherwise
     */
    TBool Exists( const TArray<TMPXAttribute>& aAttrs,
        const TMPXAttributeData& aAttrData ) const;

    /**
     * A helper method that converts text from 8-bit to 16-bit.
     * Note that a member buffer is used that has static length.
     */
    const HBufC16* CUPnPTrack::To16LC( const TDesC8& aText );

private: // data

    /**
     * Defines location of track
     *
     * @since Series 60 3.1
     */   
    enum TTrackLocation
        {
        ETrackLocationLocal = 0,
        ETrackLocationRemote
        };
    
    /**
     * Defines track state
     *
     * @since Series 60 3.2
     */  
    enum TTrackState
        {
        EStateIdle,
        EStateResolving,
        EStateSelfDestruct,
        EStateReady
        };
        

    /**
     * Location of track
     */
    TTrackLocation                      iTrackLocation;

    /**
     * URI for local track
     */
    HBufC*                              iOriginalURI;

    /**
     * Media server name 
     */
    HBufC8*                             iMediaServer;

    /**
     * Object id
     */
    HBufC8*                             iObjectId;

    /**
     * The AV Controller resource
     */
    MUPnPAVController&                  iAvController;

    /**
     * The rendering session
     */
    MUPnPAVBrowsingSession*             iBrowsingSession;

    /**
     * Track observer during resolving phase
     */
    MUPnPTrackObserver*                 iTrackObserver;

    /**
     * Observer for metadata queries
     * Stored in case metadata is not ready yet when queried
     */
    MMPXPlaybackPluginObserver*         iMetadataObserver;

    /**
     * Attributes in metadata query
     */
    RArray<TMPXAttribute>               iQueriedAttributes;

    /**
     * temp buffer used in 8->16 -bit conversion
     */
    TBuf16<KMaxElementSize>             iTempBuf;

    /**
     * track duration in milliseconds (from some other source)
     */
    TInt                                iTrackDuration;

    /**
     * Resolver for remote or local item (Owned).  
     */
    MUPnPItemResolver*                  iItemResolver;
    
    /**
     * Selectors for remote item.
     */
    TUPnPSelectDefaultResource          iDefaultSelector;
    
    /**
     * Selectors for local item.
     */
    TUPnPSelectFirstResource            iFirstSelector;
    
    /**
     * Tells is resolve item completed.
     */
    TBool                               iIsItemSolved;
    
    /**
     * Current track state
     */
    TTrackState                         iState;
    };


#endif // C_UPNPTRACK_H
