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
* Description:      session towards a media server
*
*/






#ifndef C_UPNPAVBROWSINGSESSION_H
#define C_UPNPAVBROWSINGSESSION_H

// INCLUDE FILES
#include <e32std.h>
#include "upnpavsessionbase.h"

// FORWARD DECLARATIONS
class MUPnPAVDeviceObserver;
class MUPnPAVBrowsingSessionObserver;
class MUPnPAVFileObserver;
class CUpnpObject;
class CUpnpItem;
class CUpnpAVDevice;
class CUpnpContainer;

/**
 *  AV Controller browsing session interface.
 *
 *  This class defines the browsing interface of AV Controller. Includes
 *  operations to search, browse, copy, create container, delete object and
 *  to get search capabilities.
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since S60 v3.1
 */
class MUPnPAVBrowsingSession : public MUPnPAVSessionBase
    {

public:

    /**
     * Defines the type of browse (metadata or direct children)
     *
     * @since Series 60 3.1
     */
    enum TBrowseFlag
        {
        EDirectChildren = 0,
        EMetadata
        };

    /**
     * Defines the download location (default, phone memory, memory card or
     * RAM Drive)
     *
     * @since Series 60 3.1
     */
    enum TMemoryType
        {
        EDefault = 0,
        EPhone ,
        EMemoryCard,
        ERAMDrive
        };

    /**
     * Defines the sharing status of a downloaded item
     *
     * @since Series 60 3.1
     */
    enum TFileSharing
        {
        EFileIsShared = 0,
        EFileIsNotShared,
        EShareBySettings
        };

    /**
     * Defines the container type
     *
     * @since Series 60 3.1
     */
    enum TContainerType
        {
        EStorageContainer = 0,      // object.container
        EPlaylistContainer          // object.container.playlistContainer
        };

public:

    /**
     * Sets the browsing session observer
     *
     * @since Series 60 3.1
     * @param aObserver browsing session observer
     */
     virtual void SetObserver( MUPnPAVBrowsingSessionObserver&
        aObserver) = 0;
     
    /**
     * Removes the browsing session observer
     *
     * @since Series 60 3.1
     * @param None
     */
    virtual void RemoveObserver() = 0;

    /**
     * Returns the browsing session observer
     *
     * @since Series 60 3.1
     * @param None
     */
    virtual MUPnPAVBrowsingSessionObserver* Observer() const = 0;

public:

    /**
     * Browses the content directory of a given Media
     * Server.
     *
     * @since Series 60 3.1
     * @param aId Container or object id.
     * @param aFilter TDesC8& filter
     * @param aBrowseFlag Metadata or DirectChildren
     * @param aStartIndex TInt start index
     * @param aRequestedCount TInt requested count
     * @param aSortCriteria TDesC8& sort criteria
     */
    virtual void BrowseL( const TDesC8& aId,
                          const TDesC8& aFilter,
                          TBrowseFlag aBrowseFlag,
                          TInt aStartIndex,
                          TInt aRequestedCount,
                          const TDesC8& aSortCriteria ) = 0;

    /**
     * Cancels the browse operation (ignores the result set when it
     * is received).
     *
     * @since Series 60 3.1
     * @param none
     * @return none
     */
    virtual void CancelBrowse() = 0;

    /**
     * Searches the content directory of a given Media Server.
     *
     * @since Series 60 3.1
     * @param aId container id
     * @param aSearchCriteria TDesC8& search criteria
     * @param aFilter TDesC8& filter
     * @param aStartIndex TInt start index
     * @param aRequestedCount TInt requested count
     * @param aSortCriteria TDesC8& sort criteria
     */
    virtual void SearchL(   const TDesC8& aId,
                            const TDesC8& aSearchCriteria,
                            const TDesC8& aFilter,
                            TInt aStartIndex,
                            TInt aRequestedCount,
                            const TDesC8& aSortCriteria ) = 0;

    /**
     * Cancels the search operation (ignores the result set when it
     * is received).
     *
     * @since Series 60 3.1
     * @param none
     */
    virtual void CancelSearch() = 0;

    /**
     * Gets the search capabilities of the Media Server that
     * has been set as the "source device".
     *
     * @since Series 60 3.1
     * @param None
     */
    virtual void GetSearchCapabilitiesL() = 0;

    /**
     * Creates a new container to the target Media Server.
     *
     * @since Series 60 3.1
     * @param aTitle (const TDesC8&) title for the new container
     * @param aParentId (const TDesC8&) container ID of the container into
     *        which the new container is to be created
     * @param aContainerType (TContainerType) type of the new container
     */
    virtual void CreateContainerL( const TDesC8& aTitle,
                                   const TDesC8& aParentId,
                                   TContainerType aContainerType ) = 0;

    /**
     * Deletes an UPnP item on the target UPnP media server.
     *
     * @since Series 60 3.1
     * @param aId id of an object to be destroyed 
     */
    virtual void DeleteObjectL( const TDesC8& aId ) = 0;


protected:

    virtual ~MUPnPAVBrowsingSession() {};

    };


#endif // C_UPNPAVBROWSINGSESSION_H
