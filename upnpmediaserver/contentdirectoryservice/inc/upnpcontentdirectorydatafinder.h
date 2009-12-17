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
* Description:  This is a interface, which allows to comunicate with 
*				 ContentDirectory service to obtain informations related to 
*				 reqested Uri such as path on physical drive to requested Uri 
*				 or DLNA specyfic informations and flag for this Uri.
*
*/



#ifndef C_MUPNPCONTENTDIRECTORYDATAFINDER_H
#define C_MUPNPCONTENTDIRECTORYDATAFINDER_H

//  INCLUDES
#include <e32std.h>
#include "upnpprotocolinfo.h"
#include "upnpdlnaprotocolinfo.h"

// FORWARD DECLARATIONS
class CUpnpContentDirectory;

/**
* This is a interface, which allows to comunicate with ContentDirectory service
* to obtain informations related to reqested Uri such as path on physical drive
* to requested Uri or DLNA specyfic informations and flag for this Uri.
* @lib DLNAwebserver.lib
* @since Series 60 2.6
**/
class MUpnpContentDirectoryDataFinder 
    {
public: 

    /**
    * Checks if specified importUri is in database still active.
    * @since Series60 2.6
    * @param aImportUri uri to be searched in databse. 
    * @return objId if successful or KErrnone if object wasn't found.
    **/
    virtual TInt CheckImportUriL( const TDesC8& aImportUri ) = 0;
  
    /**
    * Gets title of the object with given id from ContentDirectory service. 
    * @since Series60 2.6
    * @param aObjectId id of the object.
    * @param aValue pointer to buffer in which the tilte of the object will be returned.
    *       Buffer for aValue has to allocated first.
    **/
    virtual void GetTitleForUriL( TInt aObjectId, TPtr& aValue ) = 0;
    
    /**
    * Finds protocolInfo by contentUri (Not by importUri).
    * @since Series60 2.6
    * @param aContentUri uri to be searched in database (Value of resource).
    * @param aProtocolInfo ProtocolInfo related to resorce which has been founded. 
    *       Ownership is transfered to the caller.
    * @return KErrNone if successful, otherwise another of the system error codes 
    **/
    virtual TInt GetProtocolInfoL( const TDesC8& aContentUri, 
                                   CUpnpDlnaProtocolInfo*& aProtocolInfo ) = 0;
   
    /**
    * Find shared folder for urlpath
    * @since Series60 2.6
    * @param aUrlPath Name of URL path connected to shared folder.
    * @param aFileName Name of file.Can be empty.
    * @param aFolderPath Name of shared folder.
    * @return KErrNone or another of the system error codes.
    **/ 
    virtual TInt FindSharedFolderL( const TDesC& aUrlPath, 
                                    const TDesC& aFileName, 
                                    HBufC*& aFolderPath ) = 0;
                                    
    /**
    * Find protocolInfo by importUri.
    * @param aImportUritUri uri to be searched in database (Value of resource).
    * @return ProtocolInfo related to resorce which has been founded. Ownership 
    *       is transfered to the caller.
    */
    virtual CUpnpDlnaProtocolInfo* GetProtocolInfoByImportUriL( 
                                  const TDesC8& aImportUri) = 0;


    };

#endif      // C_MUPNPCONTENTDIRECTORYDATAFINDER_H
     
// End of File