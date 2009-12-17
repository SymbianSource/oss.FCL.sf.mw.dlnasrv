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
* Description:  Fills MPX playlist
*
*/


#ifndef UPNPLITESELECTIONFILLER_H
#define UPNPLITESELECTIONFILLER_H

//  INCLUDES
#include <e32base.h>
#include "upnplitefiller.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CUpnpAVDevice;
class CUpnpObjectLite;

/**
 * Provides services for filling MPX playlist
 *
 * @lib upnpmusicadapter.lib
 * @since S60 3.2
 */
class CUPnPLiteSelectionFiller : public CUPnPLiteFiller
    {

public: // static factory construction

    /**
     * 1st phase constructor
     *
     * @param aObjectList Objects to be filled
     * @param aMediaServer External Media Server device
     * @since Series 60 3.2
     * @return a new CUPnPLiteSelectionFiller instance
     */
    IMPORT_C static CUPnPLiteSelectionFiller* NewL(
        const RPointerArray<CUpnpObjectLite>& aObjectList,
        const CUpnpAVDevice& aMediaServer
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
    IMPORT_C virtual ~CUPnPLiteSelectionFiller();

private:

    /**
     * Default constructor
     *
     * @since Series 60 3.2
     */
    CUPnPLiteSelectionFiller();

    /**
     * 2nd phase constructor
     *
     * @param aObjectList Object to be filled
     * @param aMediaServer External Media Server device
     * @since Series 60 3.2
     */
    void ConstructL( 
        const RPointerArray<CUpnpObjectLite>& aObjectList,
        const CUpnpAVDevice& aMediaServer );

private: // data

    /**
     * Object list to be resolved
     * Note: NO OWNERSHIP ON THE CONTENT
     */        
    RPointerArray<CUpnpObjectLite>          iObjectList;
    };

#endif  // UPNPLITESELECTIONFILLER_H

// End of File


