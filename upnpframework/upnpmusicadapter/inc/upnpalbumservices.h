/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  provides album handling services for UPnP framework
*
*/


#ifndef C_UPNPALBUMSERVICE_H
#define C_UPNPALBUMSERVICE_H

// INCLUDES
#include <e32base.h>
#include <mpxcommonframeworkdefs.h> // for TMPXItemId
#include <mpxcollectionobserver.h>  // MMPXCollectionObserver

// FORWARD DECLARATIONS
class CMPXMedia;

// CONSTANTS
const TInt KBufferSize = 255; // from DLNA

/**
 * A class providing abstract album (image/video) handling services.
 *
 * @lib upnpmusicadapter.lib
 * @since S60 v3.1
 */
class CUPnPAlbumServices : public CBase,
                           public MMPXCollectionObserver
    {

public: // construction / destruction

    /**
     * Static constructor
     */
    IMPORT_C static CUPnPAlbumServices* NewL();

    /**
     * desctuctor
     */
    virtual ~CUPnPAlbumServices();

public: // component services

    /**
     * Lists all albums in the library.
     * Returns a list of albums ID's and a names.
     * ID can be used to open or modify the album.
     * @param aAlbumIds array to receive album ID's
     * @param aAlbumNames array to receive album names
     */
    IMPORT_C void ListAlbumsL(
        CDesCArray& aAlbumIds,
        CDesCArray& aAlbumNames );

    /**
     * Retrieves contents (image/video) of given album.
     * @param aAlbumId ID of the album
     * @param aContentMedia array to receive the content file paths
     */
    IMPORT_C void OpenAlbumL(
        const TDesC& aAlbumId,
        CDesCArray& aContentMedia );
        
    /**
     * Checks if the given album exists in the library.
     * Note: does not check the existence of the actual media
     * @param aAlbumName name of the album
     */
    IMPORT_C TBool IsValidAlbumL(
        const TDesC& aAlbumName );

private: // private methods
    
    /**
     * Converts an ID from TMPXItemId form to descriptor form.
     * conversion is guaranteed symmetric to Desc2Id()
     * @param aId the id in TMPXItemId form
     * @return the id string representation
     */
    const TDesC& Id2Desc( const TMPXItemId& aId );
    
    /**
     * Converts an ID from descriptor form to TMPXItemId form.
     * conversion is guaranteed symmetric to Id2Desc()
     * @param aDesc the id as a descriptor
     * @return the id extracted from the string
     */
    TMPXItemId Desc2Id( const TDesC& aDesc );


public: // From MMPXCollectionObserver

    /** see MMPXCollectionObserver */
	 void HandleCollectionMediaL(const CMPXMedia& aMedia, TInt aError);

    /** see MMPXCollectionObserver */
	void HandleCollectionMessage( CMPXMessage* aMsg, TInt aErr );
	
    /** see MMPXCollectionObserver */
	void HandleOpenL( const CMPXMedia& aEntries,
	    TInt aIndex,TBool aComplete,TInt aError );
	
    /** see MMPXCollectionObserver */                         
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist,TInt aError);

private:

    /**
     * default constructor
     */
    CUPnPAlbumServices();

    /**
     * 2nd phase constructor
     */
    void ConstructL();
    
    /**
     * Locally handles the corresponding callback
     */
    void DoHandleCollectionMessageL( CMPXMessage* aMsg );

private: // data

    /**
     * Collection utility of MPX framework (Owned)
     */
    MMPXCollectionUtility*      iCollectionUtility;

    /**
     * temp buffer used in Id2Desc
     */
    TBuf<KBufferSize>           iTempBuffer;
    
    /**
     * Scheduler wait loop (Owned)
     */
    CActiveSchedulerWait*       iWait;
    
    /**
     * for async operation
     */
    TInt                        iStatus;
    
    /**
     * Container media (Owned)
     */
    CMPXMedia*                  iMedia;
    
    /** 
     * Array for album names
     */
    CDesCArray*                 iAlbumNames;
    
    /** 
     * Array for album ids
     */
    CDesCArray*                 iAlbumIds;

    };

#endif // C_UPNPALBUMSERVICE_H



