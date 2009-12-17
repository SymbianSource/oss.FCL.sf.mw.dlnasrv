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



#ifndef C_CUPNPTRANSERINTERFACE_H
#define C_CUPNPTRANSERINTERFACE_H

//  INCLUDES
#include <e32std.h>

// Transfer interface for handling transfer
class MUpnpTransferInterface
    {
 public:
    virtual void UploadFileL( TInt aKey,
                              const TDesC8& aUri,
                              const TDesC& aTargetPath,
                              const TDesC8& aContentType
                              ) = 0;
                        
    virtual void DownloadFileL( TInt aKey,
                                const TDesC8& aUri,
                                const TDesC& aTargetPath 
                                ) = 0;
                            
    virtual void TrackUploadProgress( TInt aKey, 
                                TBool aSwitchOn )=0;

    virtual void TrackDownloadProgress( TInt aKey, 
                                TBool aSwitchOn )=0;
                                  
    virtual void CancelUpload( TInt aKey ) = 0;                                  
    
    virtual void CancelDownload( TInt aKey ) = 0;
    
    };

    
#endif      // C_CUPNPTRANSERINTERFACE_H 
        
// End of File