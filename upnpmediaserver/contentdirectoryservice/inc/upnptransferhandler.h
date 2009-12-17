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
* Description: 
*
*/



#ifndef C_CUPNPTRANSFERHANDLER_H
#define C_CUPNPTRANSFERHANDLER_H

//  INCLUDES
#include <e32std.h>
#include "upnpnotifytimer.h"
#include "upnperrors.h"
#include "upnpfiletransfertimerobserver.h"
#include "upnptransferinterface.h"
#include "upnptransferobserver.h"

// FORWARD DECLARATIONS
class CUpnpContentDirectory;
class CUpnpTransferDownloader;
class CUpnpTransferUploader;


/**
*  This is a facade class used for communication between CD and uploader/downloader
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class CUpnpTransferHandler: public CBase,
                            public MUpnpTransferInterface,
                            public MUpnpTransferObserver
    {
public: // Constructors and destructors

    /**
    * Two-phased constructor
    * @since Series60 2.6
    * @param aService Parent service
    */
    static CUpnpTransferHandler* NewL( );


    /**
    * Destructor.
    */
    virtual ~CUpnpTransferHandler();

    void SetObserver( MUpnpTransferObserver* aObserver );

public: // form MUpnpTransferInterface

    virtual void UploadFileL( TInt aKey,
                              const TDesC8& aUri,
                              const TDesC& aTargetPath,
                              const TDesC8& aContentType
                             );

    virtual void DownloadFileL( TInt aKey,
                                const TDesC8& aUri,
                                const TDesC& aTargetPath
                              );

    virtual void TrackUploadProgress( TInt aKey, TBool aSwitchOn );

    virtual void TrackDownloadProgress( TInt aKey, TBool aSwitchOn );

    virtual void CancelUpload( TInt aKey );

    virtual void CancelDownload( TInt aKey );

public: // form MUpnpTransferObserver

    virtual void TransferCompletedL( TInt aKey, TInt aResultCode );

    virtual void TransferProgress( TInt akey,
                                   TInt aBytes,
                                   TInt aTotalBytes);

protected:
    /**
    * C++ default constructor.
    */
    CUpnpTransferHandler();

    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL();

private:
    MUpnpTransferObserver* iObserver;

    CUpnpTransferDownloader* iDownloader;

    CUpnpTransferUploader* iUploader;
    };

#endif      // C_CUPNPTRANSFERHANDLER_H

// End of File