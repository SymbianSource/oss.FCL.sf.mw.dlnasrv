/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Media server device
 *
*/


#ifndef C_CUPNPMEDIASERVER_H
#define C_CUPNPMEDIASERVER_H

// INCLUDES
#include <in_sock.h>
#include <upnpnetworkeventobserver.h>
#include "upnpmediaserver.pan"
#include "upnpmediaserversettings.h"
#include "upnpmediaservereventhandler.h"
#include "upnpnotifytimer.h"
#include "upnpsymbianserverbase.h"
#include "upnpmediaserverclient.h"


// CONSTANTS
_LIT(KMSPublicAccessedDir,"public");
_LIT(KMSMediaDir,"Media");
_LIT(KMSDeviceXmlDescDir,"MediaServer1");
_LIT(KMSDeviceXmlDescFile,"MediaServer1.xml");
_LIT(KCDXmlDescFile,"ContentDirectory1.xml");
_LIT(KCMXmlDescFile,"ConnectionManager1.xml");
_LIT(KIconDirectory, "icon");
_LIT(KBackupSuffix,".bak");
_LIT(KDescTemplateDrive,"Z:");
//
const TUint KMSDescriptionMaxSize = 204800;

//note that this value should be less than upnp server process shutdown timeout value
const TInt KStopAVMediaServerDelay = 1500000;

// FORWARD DECLARATIONS
class CUpnpContentDirectoryService;
class CUpnpContainerList;
class CUpnpItem;
class CUpnpContainer;
class CUpnpCM;
class CUpnpSecurityManager;
class CUpnpDeviceImplementation;
class CUpnpIconManager;
class CUpnpDiskRemoveDetector;
class CUpnpDeviceDescriptionStore;

class CUpnpConnectionManagerNetworkEventProvider;
class CUpnpMediaServerDescriptionProvider;



// CLASS DECLARATION

/**
 *  UPnP Media Server container.
 *  Provides interface for Media Server maintanace purposes.
 *
 *  @since Series 60 3.1
 */
class CUpnpMediaServer : public CUpnpSymbianServerBase, MUpnpSettingObserver,
        MUpnpContentDirectoryEventObserver, MUpnpNetworkEventObserver
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CUpnpSymbianServerBase* NewLC();

    /**
     * Destructor.
     */
    virtual ~CUpnpMediaServer();

public:
    // New functions
    void GetSharedItemListL( TInt aContId,
        CUpnpBrowseCriteria* aBrowseCriteria, TInt* TotalMatches,
        CUpnpItemList* aItemList );
    void GetSharedContainerListL( TInt aContId,
        CUpnpBrowseCriteria* aBrowseCriteria, TInt* TotalMatches,
        CUpnpContainerList* aContainerList );
    void UnshareItemListL( RArray<TInt>& aItemList, TInt* aExecutionStatus );
    CUpnpItem* GetSharedItemL( TInt aItem );
    CUpnpContainer* GetSingleContainerL( TInt aContainerId );
    void UnshareRootL();
    void UnshareItemL( TInt aId );
    void UnshareContainerL( TInt aId );

    /**
     * Shares new item
     */
    void ShareObjectL( CUpnpObject* aItem );

    /**
     * Shares new items
     */
    void ShareItemListL( CUpnpItemList* aItemList, TInt* aStatus );

    /**
     * Creates a new reference
     */
    void ShareReferenceL( CUpnpItem* aItem );

    /**
     * Shares new 'reference item' list
     */
    void ShareReferenceListL( CUpnpItemList* aItemList, TInt* aStatus );

    /**
     * Starts AV Media Server - Device, Service, ContentDirecory, ConnectionManager
     */
    void StartAVMediaServerL();

    /**
     * Starts AV Media Server in offline mode
     */
    void StartAVMediaServerOfflineL();

    /**
     * Stops AV Media Server - Device, ContentDirecory, ConnectionManager
     * Starts timeout timer
     */
    void StopAVMediaServer( TBool aSilent );

    /**
     * Restarts AV Media Server - Device, ContentDirecory, ConnectionManager
     */
    void RestartAVMediaServerL();

    /**
     * Check status of AV Media Server
     */
    RUpnpMediaServerClient::TServerStatus Status() const;

    /**
     * Sets description properties for device, changes are available after restarting
     * the server.
     * Values are read from Central Repository
     * @param aDevice device to change
     */
    void SetDescriptionSettingsL( CUpnpDeviceImplementation* aDevice );

    /**
     * Adds icon to public\MediaServer1\icon and adds icon element
     * to device description xml
     * @param aProperty device to change
     */
    void AddIconL( const TDesC8& aProperty );

    /**
     * Updates icon in public\MediaServer1\icon and updates icon element
     * in device description xml
     * @param aProperty
     * @param aUrl icon url
     */
    void UpdateIconL( const TDesC8& aProperty, const TDesC8& aUrl );

    /**
     * Removes icon from public\MediaServer1\icon and removes icon element
     * from device description xml
     * @param aUrl icon url
     */
    void RemoveIconL( const TDesC8& aUrl );

    /**
     * Removes all icons from public\MediaServer1\icon and removes icon elements
     * from device description xml
     */
    void RemoveIconsL();

    /**
     * Sets description property for device, changes are available after restarting
     * the server.
     * Value is read from Central Repository
     * @param aDevice device to change
     * @param aKey Central Repository property key
     * @param aName property name (XML)
     */
    void SetDescriptionPropertyL( CUpnpDeviceImplementation* aDevice,
        TInt aKey, const TDesC8& aName );

    /**
     * Sets description required property for device, changes are available after restarting
     * the server.
     * Value is read from Central Repository
     * @param aDevice device to change
     * @param aKey Central Repository required property key
     * @param aName property name (XML)
     */
    void SetDescriptionRequiredPropertyL( CUpnpDeviceImplementation* aDevice,
        TInt aKey, const TDesC8& aName );

    /**
     * Removes white characters from input descriptor
     * @param aString to change
     */
    void RemoveWhiteSpaces( TDes8& aString );

    /**
     * Initialize Media Server files in private directory.
     * @param aOverwrite indicate whether files should be overwrite if exist
     */
    void InitializeL( TBool aOverwrite );

    /**
     * Reset Content Directory DB
     * It means that it rename original file adding suffix KBackupSuffix,
     * s during next running Content Directory will create new DB file.
     */
    void ResetDbL();

    /**
     * Delete Content Directory DB
     * It means that it deletes database file
     */
    void DeleteDbL();

    /**
     * Subscribe events, adds observer to the list
     * @param aObserver event observer
     */
    void SubscribeEvents( MUpnpContentDirectoryEventObserver* aObserver );

    /**
     * Unsubscribe events, removes observer from list
     * @param aObserver event observer
     */
    void UnsubscribeEvents( MUpnpContentDirectoryEventObserver* aObserver );

    TInetAddr GetAddress();

    /**
     * Set the download and thumbnail path in CD according to the value of the upload directory.
     * @param aContentDirectory
     * @return full name of description file
     */
    void SetUploadDirToCDL( const TDesC& aPath );

    /**
     * Cancels scheduled shutdown of MS
     */
    void CancelAVMediaServerStop();

    /**
    * Set the upload and thumbnail path in CD and in central repository.
    * @param aPath new path
    */
    void SetUploadDirL( const TDesC& aPath );

public:
    // From MUpnpContentDirectoryEventObserver
    /**
     * Called when transfer event happens
     * @param aTransferEvent transfer event
     */
    virtual void FileTransferEvent( CUpnpFileTransferEvent *aTransferEvent );

    /**
    * Checks if interface is up
    * @result return ETrue if started in online mode
    */
    TBool IsInterfaceUp();

    // From MUpnpSettingObserver
    /**
     * Function notify when setting will change
     */
    virtual void SettingChangedL();

private:
    // New methods

    /**
     * Constructor
     */
    CUpnpMediaServer();

    /**
     * Perform the second phase construction of a CUpnpMessageHandler object
     */
    void ConstructL();

    /**
     * Init Media Server directory structure in private directory.
     * @param aFs handle to RFs
     * @return a updateId of container
     */
    void InitPrivateDirL( RFs &aFs );

    /**
     * Checks is description file exists. If not creates it from teplate stored on Z:
     * If parameter aOverwrite
     * @param aFs handle to RFs
     * @param aTempleteDescFile xml description file name
     * @param aOverwrite indicate whether file should be overwrite if exists
     * @return full name of description file
     */
    HBufC* InitDescFileL( RFs &aFs, const TDesC& aTempleteDescFile,
        TBool aOverwrite );

    /**
     * Create full name using drive MS directory structure and file name.
     * @param aFs handle to RFs
     * @param aDrive drive
     * @param aName file name
     * @return full name of description file
     */
    HBufC* CreateFullNameL( RFs &aFs, const TDesC& aDrive,
                const TDesC& aName );

private:
    /**
     * Stops MS immediatly
     */
    void StopAVMediaServerImmediately( TBool aSilent, TBool aWlanLost );

private://from CUpnpSymbianServerBase

    /**
     * Create a time server session, and return a pointer to the created object
     * @param aMessage client message
     * @result pointer to new session
     */
    CSession2* NewSessionL( const RMessage2& aMessage ) const;

    /**
     * @return ETrue when media server is in state that it can be stopped
     */
    TBool CanBeStopped() const;

    /**
     * Returns version that is supported by this server.
     */
    TVersion SupportedVersion() const;

    /**
     * Returns server name
     */
    const TDesC& ServerName() const;
    
    /**
     * Change media server status
     */
    void SetStatus( RUpnpMediaServerClient::TServerStatus aStatus );
    
    /**
     * Method should be invoked to notify all subscribed clients about
     * media server change. It is invoked if after RunL method status
     * is changed, and in every case that server change its status
     * without user request (eq. wlan lost) 
     */
    void NotifyStatusChanged();

private:    // from MUpnpNetworkEventObserver

    /**
     * It is called from bottom layer when network event occurs [callback method]     
     * @result None
     */
    void NetworkEvent( CUpnpNetworkEventBase* aEvent );

private:    // from CUpnpSymbianServerBase
    virtual void RunL();
    
private:
    /**
     * Callback if timer expires
     */
    void TimerEventL( CUpnpNotifyTimer* aTimer );

private:

    /*! @var iDevice Media Server Device */
    CUpnpDeviceImplementation* iDevice;
    /*! @var iContentDirecory Content Directory */
    CUpnpContentDirectoryService* iContentDirecory;
    /*! @var iCm Connection Manager  */
    CUpnpCM* iCm;
    /*! @var iIconManager Icon Manager  */
    CUpnpIconManager* iIconManager;

    /*! @var iMsName Media Server Device description file full name */
    HBufC* iMsName;
    /*! @var iCdName Content Directory Service description file full name */
    HBufC* iCdName;
    /*! @var iCmName Connection Manager Service description file full name */
    HBufC* iCmName;

    /*! @var iMsStatus Media Server operational status - started/started offline/stopped */
    RUpnpMediaServerClient::TServerStatus iMsStatus;

    /*! @var interface for reading and setting Media Server settings */
    CUpnpMediaServerSettings* iSettings;

    /*! @var security manager pointer */
    CUpnpSecurityManager* iManager;

    /*! @var list of event observers (subscribed sessions) */
    RPointerArray<MUpnpContentDirectoryEventObserver> iObserverList;

    // memory card remove watcher
    CUpnpDiskRemoveDetector* iDiskDetector;

    // flag that stores shut down type
    TBool iIsSilentShutdown;

    //please note that it only stops media server as UPnP entity (sends bye bye etc.)
    //not symbian server object
    CUpnpNotifyTimer* iStopAVMediaServerTimer;
    
    // Network events provider used for WLAN lost and address change
    CUpnpConnectionManagerNetworkEventProvider* iConnectionManagerNetworkEventProvider;
    
    // Provides files for description part
    CUpnpMediaServerDescriptionProvider* iDescriptionProvider;
    
    // Services device description
    CUpnpDeviceDescriptionStore* iDescriptionStore;
    };

#endif // C_CUPNPMEDIASERVER_H
