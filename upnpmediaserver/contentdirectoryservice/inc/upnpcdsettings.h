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
* Description:  ContentDirectory Initializer
*
*/

#ifndef C_CUPNPCDSETTINGS_H
#define C_CUPNPCDSETTINGS_H

//  INCLUDES
#include <e32base.h>

//FORWARD DECLARATIONS
class CUpnpDevice;
class MUpnpThumbnailCreatorObserver;
class MUpnpContentDirectoryEventObserver;
class MUpnpTransferInterface;
// CLASS DECLARATION

/**
*  Stores ContentDirectory settings values.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
struct TUpnpCdSettings
{
    CUpnpDevice*                        iDevice; 
    const TDesC&                        iDownloadDir;
    const TDesC&                        iThumbnailDir;
    const TDesC&                        iDescriptionPath;
    MUpnpThumbnailCreatorObserver*      iThmbObs;
    TBool                               iCreateThumbnail;
    TInt                                iMaxRequestCount;
    MUpnpContentDirectoryEventObserver* iEventObserver;    
    const TDesC&                        iObjectsXmlPath;
    MUpnpTransferInterface*             iTransfer;
};

#endif      // C_CUPNPCDSETTINGS_H   
// End of File
