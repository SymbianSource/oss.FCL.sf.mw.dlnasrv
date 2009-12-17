/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides services for filling MPX playlist
*
*/


#ifndef UPNPLITECONTAINERFILLER_H
#define UPNPLITECONTAINERFILLER_H

//  INCLUDES
#include <e32base.h>
#include "upnplitefiller.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CUpnpResultCacheUi;

/**
 * Provides services for filling MPX playlist
 *
 * @lib upnpmusicadapter.lib
 * @since S60 3.2
 */
class CUPnPLiteContainerFiller : public CUPnPLiteFiller
    {

public: // static factory construction

    /**
     * 1st phase constructor
     *
     * @param aMediaServer External Media Server device
     * @param aResultCache
     * @since Series 60 3.2
     * @return a new CUPnPLiteContainerFiller instance
     */
    IMPORT_C static CUPnPLiteContainerFiller* NewL(
        const CUpnpAVDevice& aMediaServer,
        const CUpnpResultCacheUi& aResultCache
        );

    /**
     * See MUPnPPlaylistFiller
     */
    void FillL( CUPnPMusicAdapter& aHost, CMPXMediaArray& aPlaylist );

    /**
     * See MUPnPPlaylistFiller
     */
    void CancelFill();

    /**
     * See MUPnPPlaylistFiller
     */
    TInt PlaylistSize();

public: // construction/destruction

    /**
     * Destructor
     *
     * @since Series 60 3.2
     */
    IMPORT_C virtual ~CUPnPLiteContainerFiller();

private:

    /**
     * Default constructor
     *
     * @since Series 60 3.2
     */
    CUPnPLiteContainerFiller( const CUpnpResultCacheUi& aResultCache );

    /**
     * 2nd phase constructor
     *
     * @param aMediaServer External Media Server device
     * @since Series 60 3.2
     */
    void ConstructL( const CUpnpAVDevice& aMediaServer );

private: // data

    /**
     * Item cache.
     * (not owned)
     */
    const CUpnpResultCacheUi&           iResultCache;
    
    /**
     * Playlist size as it items are being dynamically fetched
     */
    TInt                                iPlaylistActualSize;

    };

#endif  // UPNPLITECONTAINERFILLER_H

// End of File


