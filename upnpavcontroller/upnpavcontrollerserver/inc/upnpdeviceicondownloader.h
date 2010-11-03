/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef C_UPNPDEVICEICONDOWNLOADER_H
#define C_UPNPDEVICEICONDOWNLOADER_H

// Include files
#include <e32base.h>
#include <f32file.h>
#include <httptransferobserver.h>

// FORWARD DECLARATIONS
class CHttpDownloader;

/**
 * Class for observing device icon download
 */
class MUpnpDeviceIconDownloadObserver
    {
public:
    /**
     * Notifies device icon download completed
     * 
     * @param aDeviceUuid the device uuid indicates the device the icon belongs
     * @param aError system wide error code indicates the download status
     */
    virtual void DeviceIconDownloadedL( const TDesC8& aDeviceUuid, TInt aError ) = 0;
    };

/**
 * Class for device icon downloading
 */
class CUpnpDeviceIconDownloader: public CBase,
                                 public MHttpTransferObserver
    {

public:

    /**
     * Static 1st phase constructor
     * 
     * @param aObserver download observer
     * @param aIap used access point
     * @return new instance
     */
    static CUpnpDeviceIconDownloader* NewL(
            MUpnpDeviceIconDownloadObserver& aObserver,
            TUint aIap );

    /**
     * Destructor
     */
    virtual ~CUpnpDeviceIconDownloader();
    
private: // From MHttpTransferObserver
    void TransferProgress( TAny* aKey, TInt aBytes, TInt aTotalBytes );
    void ReadyForTransferL( TAny* aKey );
    void TransferCompleted( TAny* aKey, TInt aStatus );

public: // New functions
    /**
     * Starts download
     * 
     * @param aDeviceUuid uuid of device
     * @param aDeviceIconUrl url of device icon
     */
    void StartDownloadL( const TDesC8& aDeviceUuid, const TDesC8& aDeviceIconUrl );

    /**
     * Cancels download
     * 
     * @param aDeviceUuid uuid of device
     */
    void CancelDownload( const TDesC8& aDeviceUuid );

    /**
     * Transfers icon file to the client
     * 
     * @param aMessage message from client
     * @param aSlot message slot to be used
     * @param aDeviceUuid the device that's icon is transferred
     */
    void TransferFileToClientL( const RMessage2& aMessage, TInt aSlot,
            const TDesC8& aDeviceUuid );

private:
    /**
     * Private class for download queue handling
     */    
    class CEntry : public CBase
        {
    public:
        static CEntry* NewLC( const TDesC8& aDeviceUuid, const TDesC8& aDeviceIconUrl );
        virtual ~CEntry();
        inline TPtrC8 DeviceUuid() const { return iDeviceUuid->Des(); }
        inline TPtrC8 DeviceIconUrl() const { return iDeviceIconUrl->Des(); }
        static TInt Compare( const TDesC8* aDeviceUuid, const CEntry& aEntry );
    private:
        inline CEntry() {}
        void ConstructL( const TDesC8& aDeviceUuid, const TDesC8& aDeviceIconUrl );

    private:
        HBufC8* iDeviceUuid;
        HBufC8* iDeviceIconUrl;
        };

private:
    CUpnpDeviceIconDownloader( MUpnpDeviceIconDownloadObserver& aObserver,
            TUint aIap );
    void ConstructL();
    void DownloadNext();
    void DownloadNextL( CEntry& aNext );
    void CleanupFilesL();
    TBool FileExistsL( const TDesC8& aDeviceUuid );
    void CleanupFilesIfNeededL();
    void CheckDiskSpaceL();
    TBool InProgress( const TDesC8& aDeviceUuid );

private:
    MUpnpDeviceIconDownloadObserver& iObserver;
    TInt iIap;
    RFs iFs;
    RPointerArray< CEntry > iQueue;
    CEntry* iCurrent; // Own, can be NULL
    CHttpDownloader* iDownloader; // Own
    TBool iCleanupOnShutdown;
    };


#endif // C_UPNPDEVICEICONDOWNLOADER_H
