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
* Description:      Metadata Harvester's media server container header
*
*/







#ifndef CMMDHMEDIASERVERCONTAINER_H
#define CMMDHMEDIASERVERCONTAINER_H

//  INCLUDES
#include <e32std.h>
#include <upnpavcontrolpoint.h>

#include "mdhconnectionmonitorobserver.h"
#include "mdhavcpadapter.h"
#include "cmcommon.h"

//  CONSTANTS

_LIT8( KRootContainerId, "0" );
    
_LIT8( KSearchCriteria, 
    "upnp:class derivedfrom &quot;object.item&quot; and @refID exists false");
    
_LIT8( KSearchFilter,    "*" );
_LIT8( KMdhSearchFilter, "dc:title,dc:date,upnp:class,"
    "upnp:album,upnp:artist,upnp:genre,res,upnp:albumArtURI,"
    "res@size,res@duration,@restricted,res@resolution,"
    "res@bitrate" );
    
_LIT8( KSortCriteria,    "" );


// FORWARD DECLARATIONS
class CMdHConnectionMonitor;
class CCmMdhMetadataCollector; 
class CCmMdhServer;
class CCmDmMain;
class CCmSettingsEngine;
class CCmMediaServerFull;
class CCmSearchResponseHash;
class MCmSettings;

// DATA TYPES

enum TCmMdhMediaserverState
    {
    ECmMdhMediaserverSynchronized = 1,
    ECmMdhMediaserverNotSynchronized,
    ECmMdhMediaserverNoSearchCaps
    };

// CLASS DECLARATION
/**
*  CCmMdhMediaserverInfo
*  Encapsulates server information
*
*  @since S60 3.1
*/

class CCmMdhMediaserverInfo : public CBase
    {
    public:
    
        /**
         * Creates new CCmMdhMediaserverInfo class
         * @return  pointer to CCmSmItemInfo class
         */    
        static CCmMdhMediaserverInfo* NewL();
        
        /**
         * Creates new CCmMdhMediaserverInfo class
         * @return  pointer to CCmSmItemInfo class
         */           
        static CCmMdhMediaserverInfo* NewLC();
        
        /**
         * Destructor
         */           
        ~CCmMdhMediaserverInfo();
        
    private:
    
        /**
         * Default constructor
         */              
        CCmMdhMediaserverInfo();
        
        /**
         * ConstructL
         */         
        void ConstructL();
    
    public:
        
        /** Media server identifier */
        HBufC8* iUuid;
        
        /** State of media server */
        TCmMdhMediaserverState iMediaserverState;
        
        /** media server's Systemupdate id */
        TInt iSystemUpdateId;    
    }; 
   
   
// CLASS DECLARATION

/**
*  CCmMdhMediaserverContainer
*  This is a class for metadata harvester server's
*  media server container component
*
*  @since S60 3.1
*/
class CCmMdhMediaserverContainer : public CCmMdhAvControlPointAdapter,
                                   public MMdHConnectionMonitorObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCmMdhMediaserverContainer* NewL( CCmMdhServer& aMdhServer );
        
        /**
        * Destructor.
        */
        virtual ~CCmMdhMediaserverContainer();
        
    public: // Functions from base classes

        /**
        * Handles UPnP device discoveries.
        * @param aDevice Device that is discovered.
        */
        void DeviceDiscoveredL( CUpnpDevice* aDevice );

        /**
        * Handles UPnP device disappears.
        * @param aDevice Device that disappeared.
        */
        void DeviceDisappearedL( CUpnpDevice* aDevice );   
        
        /**
        * Observer callback for Content Directory Search function.
        * @since Series 60 2.0
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aContainerId
        * @param aSearchCriteria
        * @param aFilter
        * @param aIndex
        * @param arequest
        * @param aSortCriteria
        * @param aResult
        * @param aReturned
        * @param aMatches
        * @param aUpdateID
        */
        void CdsSearchResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aContainerId,
            const TDesC8& aSearchCriteria,
            const TDesC8& aFilter,
            TInt aIndex,
            TInt aRequest,
            const TDesC8& aSortCriteria,
            const TDesC8& aResult,
            TInt aReturned,
            TInt aMatches,
            const TDesC8& aUpdateID );     
        
        /**
        * Observer callback for Content Directory 
        * GetSearchCapabilities function.
        * @since Series 60 2.0
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSearchCaps
        */
        void CdsSearchCapabilitiesResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aSearchCaps );
            
        /**
        * Observer callback for Content Directory 
        * GetSystemUpdateID function.
        * @since Series 60 2.0
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aSystemUpdateId
        */
        void CdsSystemUpdateIdResponse(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            TInt aSystemUpdateId );
        
    
    public: // From MUPnPConnectionMonitorObserver
    
         /**
         * This function will be called when WLan connection is lost
         * for UPnP 
         *
         * @since S60 3.1
         */ 
         virtual void ConnectionLost();
    
             
    public: // New functions
    
        /**
        * Searchs for media servers
        * @since Series 60 2.0
        */
        void SearchMediaserversL();
        
        /**
        * Harvests media servers
        * @since Series 60 2.0
        */
        void HarvestMediaserversL();
        
        /**
        * Static timer callback function
        * @since Series 60 2.0
        * @param aNy media server container instance
        */
        static TInt SearchTimerCompletedL( TAny* aNy );
        
        /**
        * Timer callback function
        * @since Series 60 2.0
        */
        TInt DoSearchTimerCompletedL();
        
        /**
        * Harvest completion callback function
        * @param aErr error code
        * @since Series 60 2.0
        */
        void HarvestCompleteL( TInt aErr );
        
        /**
        * Cancels harvest
        * @since Series 60 2.0
        */
        void CancelSearchL();
        
        /**
        * Cancels search
        * @since Series 60 2.0
        */
        void CancelHarvestL();
        
        /**
        * Gets media server id from database
        * @since Series 60 2.0
        * @param aSearchCaps
        * @return media server id
        */
        TInt IdForMediaServerL( TDesC8& aUuid );
        
        /**
        * Sends progress info
        * @since Series 60 2.0
        * @param aProgress progressed item count
        */
        void SendProgressInfo( TInt aProgress );

        /**
        * Add hash for search response
        * @since S60 3.2
        * @param aMediaServerId, media server id
        * @param aSearchIndex, search index
        * @param aItemCount, item count
        * @param aHash, hash code
        * @return error code
        */        
        TInt AddHashValueForResponseL( TInt aMediaServerId, 
                                       TInt aSearchIndex, 
                                       TInt aItemCount, 
                                       const TDesC8& aHash ); 

        /**
        * Retrieves hash codes for defined media server
        * @since S60 3.2
        * @param aMediaServerId, media server id
        * @param aHashValues, hash codes
        * @return None
        */                                     
        void HashValuesForMediaServerL( TInt aMediaServerId,
            RPointerArray<CCmSearchResponseHash>& aHashValues );

        /**
        * Deletes hash values
        * @since S60 3.2
        * @param aMediaServerId, media server id
        * @param aSearchIndex, search index
        * @return None
        */
        void DeleteOldHashValuesL( const TInt aMediaserverId,
            const TInt aSearchIndex );                        
  
    private:

       /**
        * Constructor.
        */
        CCmMdhMediaserverContainer(CCmMdhServer& aMdhServer);
         
       /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Harvest a media server
        * @since Series 60 2.0
        * @param aMediaServerIndex media server array index
        */
        void DoHarvestMediaserverL( TInt aMediaserverIndex );
        
        
        /**
        * Adds a media server to database if it doesn't exist there
        * @since Series 60 2.0
        * @param aMediaServe media server device object
        * @return whether server is active or not (new servers are not active)
        */
        TBool AddToDbIfNewL( CUpnpDevice& aMediaserver );
        
        /**
        * Decreases pending responses counter and finishes search if ready
        * @since Series 60 2.0
        */
        void DecResponsesAndFinishIfReadyL();
        
        /**
        * Leaving version of method CdsSystemUpdateIdResponse.
        * @since Series 60 3.1
        * @param aErr UPnP error code.
        * @param aSystemUpdateId
        */
        void CdsSystemUpdateIdResponseL(
            const TDesC8& aUuid,
            TInt aErr,
            TInt aSystemUpdateId );

        /**
        * Leaving version of GetSearchCapabilities function.
        * @since Series 60 3.1
        * @param aUuid Source device UUID. 
        * @param aErr UPnP error code.
        * @param aSearchCaps
        */
        void CdsSearchCapabilitiesResponseL(
            const TDesC8& aUuid,
            TInt aErr,
            const TDesC8& aSearchCaps );
            
        /**
        * Leaving version of CdsSearchResponseL function.
        * @since Series 60 3.1
        * @param aUuid Source device UUID. 
        * @param aSessionId 
        * @param aErr UPnP error code.
        * @param aContainerId
        * @param aSearchCriteria
        * @param aFilter
        * @param aIndex
        * @param arequest
        * @param aSortCriteria
        * @param aResult
        * @param aReturned
        * @param aMatches
        * @param aUpdateID
        */
        void CdsSearchResponseL(
            const TDesC8& aUuid,
            TInt aSessionId,
            TInt aErr,
            const TDesC8& aResult,
            TInt aReturned,
            TInt aMatches );  
            
    private:    // Data
    
        // metadata collector container
        CCmMdhMetadataCollector* iMetadataCollector;    // owned
        
        // av control point
        CUpnpAVControlPoint* iAvControlPoint;           // owned
        
        // databse manager
        CCmDmMain* iDbManager;                          // owned
        
        // reference to host server
        CCmMdhServer& iMdhServer;
        
        // Settings engine
        MCmSettings*        iSettings;                  // owned
        
        // Connection monitor
        CMdHConnectionMonitor* iMdHConMon;              // owned
        
        // timer 
        CPeriodic* iTimer;                              // owned
        
        // array for media server information
        RPointerArray<CCmMdhMediaserverInfo> iMediaservers; // items owned
        
        // array of old media servers
        RPointerArray<CCmMediaServerFull> iOldMediaServers; // items owned
        
        TCmProgressInfo iProgressInfo;
        
        // index for storing which media server we are harvesting
        TInt iMediaserverIndex;
        
        TInt iResponsesPending;
        
        TInt iTotalItemCount;
 
        // The IAP that is used
        TInt iIap;
        
        TBool iHarvestErrCode;
        
        TBool iHarvestActive;
        
        TBool iAllFound;
        
        //index of havest item which has been sent to property
        TInt iPropertyItemIndex;
   
    };

#endif      // CMMDHMEDIASERVERCONTAINER_H   
            
// End of File
