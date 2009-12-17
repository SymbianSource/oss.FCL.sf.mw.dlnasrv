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
* Description:      callback interface for browsing session
*
*/






#ifndef M_UPNPAVBROWSINGSESSIONOBSERVER_H
#define M_UPNPAVBROWSINGSESSIONOBSERVER_H

// INCLUDES
#include "upnpavsessionobserverbase.h"

// FORWARD DECLARATIONS
class CUpnpDevice;
class CUpnpObject;

/**
* Defines the response interface for the browsing session operations of the
* UPnP AV Controller.
*
* @since Series 60 3.1
*/
class MUPnPAVBrowsingSessionObserver : public MUPnPAVSessionObserverBase
    {

    public:

        /**
         * Returns a browse result received from a Media Server.
         *         
         * @since Series 60 3.1
         * @param aBrowseResponse browse response xml document
         * @param aError system wide error code
         * @param aMatches number of returned items
         * @param aTotalCount total number of objects on container
         * @param aUpdateId update id number
         * @return None
         */
        virtual void BrowseResponse(
                        const TDesC8& aBrowseResponse,
                        TInt aError,
                        TInt aMatches,
                        TInt aTotalCount,
                        const TDesC8& aUpdateId
                        ) = 0;

        /**
         * Returns a search result received from a Media Server.
         *
         * @param aSearchResponse search response xml document
         * @param aError system wide error code
         * @param aMatches number of returned items
         * @param aTotalCount total number of resulted items
         * @param aUpdateId update id number
         * @return None
         */
        virtual void SearchResponse( 
                        const TDesC8& aSearchResponse,
                         TInt aError,
                         TInt aMatches,
                         TInt aTotalCount,
                         const TDesC8& aUpdateId
                         ) = 0;

        /**
         * Returns search capabilities of the requested Media Server.
         *
         * @since Series 60 3.1
         * @param status aStatus  information
         * @param aSearchCapabilities the search capabilities
         * @return None
         */
        virtual void SearchCapabilitiesResponse( 
                         TInt aError,
                         const TDesC8& aSearchCapabilities 
                         ) = 0;

        /**
         * Notifies that the create container operation is complete.
         *
         * @since Series 60 3.1
         * @param aError status information
         * @param aObjectId  object ID of the new container
         */
        virtual void CreateContainerResponse( TInt aError, 
            const TDesC8& aObjectId ) = 0;

        /**
         * Notifies that the requested UPnP Object deletion is complete.
         *
         * @since Series 60 3.1
         * @param aStatus status information
         * @return None
         */
        virtual void DeleteObjectResponse( TInt aError ) = 0;
        
        /**
         * Notifies that the Media Server we have a session with has
         * disappeared. Session is now unusable and must be closed. 
         *
         * @since Series 60 3.1
         * @param aReason reason code
         * @return None
         */        
        virtual void MediaServerDisappeared(
            TUPnPDeviceDisconnectedReason aReason ) = 0;
            
        virtual void ReserveLocalMSServicesCompleted( TInt aError ) = 0;            
       
    };

#endif // M_UPNPAVBROWSINGSESSIONOBSERVER_H

// End of File
