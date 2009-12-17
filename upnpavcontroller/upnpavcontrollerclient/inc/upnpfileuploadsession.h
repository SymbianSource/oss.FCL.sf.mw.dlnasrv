/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      File tranfer session
*
*/






#ifndef M_UPNPFILEUPLOADSESSION_H
#define M_UPNPFILEUPLOADSESSION_H

// EXTERNAL INCLUDES
#include <e32std.h>

// INTERNAL INCLUDES
#include "upnpfiletransfersession.h"

// CLASS DECLARATION
/**
 *  AV Controller file upload session interface.
 *
 *  @since S60 v3.2
 */
class MUPnPFileUploadSession : public MUPnPFileTransferSession
    {

public:

    /**
     * Start upload. Uploads the given file to the remote server
     *
     * @param aFilePath filepath pointing to a local file
     * @param aKey identifies the upload operation
     */        
    virtual void StartUploadL( const TDesC& aFilePath, TInt aKey ) = 0;

protected:

    /**
     * Protected destructor
     */        
    virtual ~MUPnPFileUploadSession() {};
    
    };

#endif // M_UPNPFILEUPLOADSESSION_H

// End of file
