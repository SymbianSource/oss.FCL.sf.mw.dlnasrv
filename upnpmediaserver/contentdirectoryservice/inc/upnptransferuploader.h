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
* Description:  class for receiving HTTP response using Symbian HTTP client 
*
*/


#ifndef C_CUPNPTRANSFERUPLOADER_H
#define C_CUPNPTRANSFERUPLOADER_H

// INCLUDES
#include "upnptransferbase.h"

// FORWARD DECLARATIONS
class CHttpUploader;

class CUpnpTransferUploader: public CUpnpTransferBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpTransferUploader* NewLC( MUpnpTransferObserver* aCd );
    static CUpnpTransferUploader* NewL( MUpnpTransferObserver* aCd );
    
    /**
    * Destructor.
    */
    virtual ~CUpnpTransferUploader();
    
public: // New functions
 
    /**
     * Method for uploading the file. Creates and enqueues the file
     *
     * @since S60 v3.2
     */
    void UploadFileL(TInt aKey);
    
    /**
     * Empty implementation
     *
     */
    void DownloadFileL(const TDesC8& aUri,
                       const TDesC& aTargetPath, 
                       TInt aKey);
    
    /**
     * Method for uploading the file. Creates and enqueues the file
     *
     */                   
    void UploadFileL( const TDesC8& aUri,const TDesC& aTargetPath,
                        const TDesC8& aContentType, TInt aKey);                       
                                                

 private: 
    /**
    * C++ default constructor.
    */
    CUpnpTransferUploader( MUpnpTransferObserver* aCd );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

 private:    // Data
};

#endif // C_CUPNPTRANSFERUPLOADER_H

// End of File
