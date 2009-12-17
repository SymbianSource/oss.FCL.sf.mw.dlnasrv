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
* Description:  Manages Upnp actions needed by Fill Manager
*
*/



#ifndef __CMFMUPNPMNGR_H
#define __CMFMUPNPMNGR_H

#include <e32base.h>
#include "upnpavbrowsingsessionobserver.h"
#include "upnpavdeviceobserver.h"

// Forward declarations
class CUPnPXMLParser;
class CCmFillListItem;
class MCmFmUPnPActionObserver;

// CLASS DECLARATION
 /**
  * Class represents methods to communication with AV Controller API.
  *
  * @lib cmfillmanager.lib
  *
  * @since S60 v3.1
  */
NONSHARABLE_CLASS( CCmFmUpnpMngr ): public CBase,
                      public MUPnPAVBrowsingSessionObserver,
                      public MUPnPAVDeviceObserver
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, action observer
     * @return  pointer to CCmFmUpnpMngr class
     */
    static CCmFmUpnpMngr* NewL( MCmFmUPnPActionObserver* aObserver );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aObserver, action observer
     * @return  pointer to CCmFmUpnpMngr class
     */
    static CCmFmUpnpMngr* NewLC( MCmFmUPnPActionObserver* aObserver );

    /**
     * Destructor.
     */
    virtual ~CCmFmUpnpMngr();

public: 

    /**
    * Sets av controller
    *
    * @since S60 5.1
    * @param aAVController, pointer to av controller
    * @return None
    */
    void SetAvController( MUPnPAVController* aAVController );
    
    /**
    * Lists media servers 
    *
    * @since S60 5.1
    * @param aDevices, device list
    * @return None
    */
    void GetMediaServersL( CUpnpAVDeviceList*& aDevices );
    
    /**
    * Checks uri
    *
    * @since S60 5.1
    * @param aDevice, upnp device
    * @param aItem, fill list item to be checked
    * @return None
    */    
    void CheckURIL( CUpnpAVDevice* aDevice, CCmFillListItem& aItem );
    
    /**
    * Cancel UPnP operation
    *
    * @since S60 5.1
    * @param None
    * @return None
    */    
    void CancelOperation();

    /**
    * Returns browsed uri
    *
    * @since S60 5.1
    * @param None
    * @return uri
    */    
    TDesC8& Uri() const;

    /**
    * Returns browsed upnp item
    *
    * @since S60 5.1
    * @param None
    * @return upnp item
    */     
    CUpnpItem& UpnpItem() const;
    
protected:

// From base class MUPnPAVBrowsingSessionObserver

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void BrowseResponse( const TDesC8& aBrowseResponse,
                                 TInt aError,
                                 TInt aMatches,
                                 TInt aTotalCount,
                                 const TDesC8& aUpdateId );

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void SearchResponse( const TDesC8& aSearchResponse,
                                 TInt aError,
                                 TInt aMatches,
                                 TInt aTotalCount,
                                 const TDesC8& aUpdateId );                     

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void SearchCapabilitiesResponse( 
                                TInt aStatus,
                                const TDesC8& aSearchCapabilities );

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void CreateContainerResponse( TInt aError, 
                                          const TDesC8& aObjectId );

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void DeleteObjectResponse( TInt aStatus );

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void MediaServerDisappeared(
            TUPnPDeviceDisconnectedReason aReason );

    /**
     * From MUPnPAVBrowsingSessionObserver
     * See base class definition
     */
    virtual void ReserveLocalMSServicesCompleted( TInt aStatus );

//public: 
// From base class MCmAVDeviceObserver

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */
    void UPnPDeviceDiscovered(const CUpnpAVDevice& aDevice);

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */
    void UPnPDeviceDisappeared(const CUpnpAVDevice& aDevice);

    /**
     * From MCmAVDeviceObserver
     * See base class definition
     */  
    void WLANConnectionLost();                                   
   
private:

    /**
     * Performs the first phase of two phase construction.
     *
     * @since S60 5.1
     * @param aObserver, UPnP action observer
     */
    CCmFmUpnpMngr( MCmFmUPnPActionObserver* aObserver );

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    
    /**
     * Parser all resolution which object owns
     *
     * @since S60 5.1
     * @param aResElementsArray, resource array
     * @return Optimal resource element
     */ 
    const CUpnpElement& ParseImageResolutions( 
        RUPnPElementsArray& aResElementsArray );

    /**
     * Initializes screen size member variable
     *
     * @since S60 5.1
     * @param None
     * @return None
     */    
    void ScreenSizeL();
    
    /**
     * Returns processed browse results received from a Media Server.
     *
     * UPnPAVController releases the memory allocated for 
     * aResultArray after calling the callback method.
     *
     * @since S60 5.1
     * @param aStatus TInt  status information
     * @param aTotalCount TInt total number of objects
     * @param aResultArray browse result array
     * @return None
     */
    void BrowseResponseL( TInt aStatus,
                          TInt aTotalCount,
                          const TDesC8& aResultArray );

    /**
     * Returns processed search results received from a Media Server.
     *
     * UPnPAVController releases the memory allocated for 
     * aResultArray after calling the callback method.
     *
     * @since S60 5.1
     * @param aStatus TInt  status information
     * @param aTotalCount TInt total number of objects
     * @param aResultArray RPointerArray of CUPnPObject objects
     * @return None
     */
    void SearchResponse( TInt aStatus,
                         TInt aTotalCount,
                         const RPointerArray<CUpnpObject>& aResultArray );   

private:

    /**
     * Pointer to AV Controller
     */
    MUPnPAVController* iAVController;                   // not owned

    /**
     * Browse action Observer
     */
    MCmFmUPnPActionObserver* iObserver;                 // not owned
    
    /**
     * Browse response parser
     */
    CUPnPXMLParser* iParser;                            // owned
    
    /**
     * CUpnpAVDevice
     */
    CUpnpAVDevice* iDevice;                             // Not owned
    
    /**
     * URI
     */
    HBufC8* iURI;                                       // owned
    
    /**
     * upnp item
     */
    CUpnpItem* iItem;                                   // owned
    
    /**
     * Browse started
     */
    TTime iBrowseStarted;
    
    /**
     * Browse finished
     */
    TTime iBrowseFinished;  
    
    /**
     * Browse session
     */
    MUPnPAVBrowsingSession* iBrowseSession;             // Not owned
    
    /**
     * If true select optimal image size to be filled
     */
    TBool iSelectOptimalImageSize;
    
    /**
     * Screen size of the device
     */
    TSize iScreenSize;
    
    };

#endif //  __CMFMUPNPMNGR_H
