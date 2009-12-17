/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class for sending HTTP request using Symbian HTTP client 
*
*/


#ifndef C_CUPNPTRANSFERDOWNLOADER_H
#define C_CUPNPTRANSFERDOWNLOADER_H

// INCLUDES
#include "upnptransferbase.h"
#include "upnpsender.h"
#include "upnpfiletransfer.h"

// FORWARD DECLARATIONS
class CHttpDownloader;

class CUpnpTransferDownloader: public CUpnpTransferBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpTransferDownloader* NewLC( MUpnpTransferObserver* aCd );
    static CUpnpTransferDownloader* NewL( MUpnpTransferObserver* aCd );
    
    /**
    * Destructor.
    */
    virtual ~CUpnpTransferDownloader();

public: // New functions
 
    /**
     * Method for downloading the file. Creates and enqueues the file
     *
     */
    void DownloadFileL(const TDesC8& aUri,
                       const TDesC& aTargetPath, 
                       TInt aKey);
     /**
     * Method for downloading the file. Creates and enqueues the file
     *
     * @since S60 v3.2
     */
     void DownloadFileL( TInt aKey );

 private: 
    /**
    * C++ default constructor.
    */
    CUpnpTransferDownloader( MUpnpTransferObserver* aCd );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

 private:    // Data

};

#endif // C_CUPNPTRANSFERDOWNLOADER_H

// End of File
