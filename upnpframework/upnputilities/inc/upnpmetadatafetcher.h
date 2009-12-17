/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Meta data fetching utilities
*
*/


#ifndef UPNPMETADATAFETCHER_H
#define UPNPMETADATAFETCHER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpObject;

// CLASS DECLARATION
    
/**
* Static meta data fetching utilities.
*
* @lib upnputilities.lib
* @since S60 3.1
*/
class UPnPMetadataFetcher
    {

public:

    /**
     * Resolves the metadata and fills in the CUpnpObject's metadata 
     * fields accordingly.
     * 
     * @since Series 60 3.1
     * @param aObject (CUpnpObject&) the target CUpnpObject
     * @param aFilePath (const TDesC&) the filepath of the file
     * @param aMimeType (const TDesC8&) the mime type of the file
     */
    IMPORT_C static void FetchMetadataL( CUpnpObject& aObject, 
                                         const TDesC& aFilePath, 
                                         const TDesC8& aMimeType );

    /**
     * Resolves the metadata and fills in the CUpnpObject's metadata 
     * fields accordingly.
     * 
     * @since Series 60 3.1
     * @param aObject (CUpnpObject&) the target CUpnpObject
     * @param aFilePath (const TDesC&) the filepath of the file
     */
    IMPORT_C static void FetchMetadataL( CUpnpObject& aObject, 
                                         const TDesC& aFilePath );

    /**
     * Creates a new CUpnpItem from a local file. Resolves the metadata and 
     * fills in the new CUpnpItem's metadata fields accordingly.
     * 
     * @since Series 60 3.1
     * @param aFilePath (const TDesC&) the filepath of the file
     * @return (CUpnpItem*) pointer to the newly created CUpnpItem,
     *         or NULL if the creation of the item or metadata resolving
     *         fails. Pushes the object into the CleanupStack.
     */
    IMPORT_C static CUpnpItem* CreateItemFromFileLC( 
                                            const TDesC& aFilePath );

    /**
     * Creates a new CUpnpItem from a local file. Resolves the metadata and 
     * fills in the new CUpnpItem's metadata fields accordingly.
     * 
     * @since Series 60 3.1
     * @param aFilePath (const TDesC&) the filepath of the file
     * @return (CUpnpItem*) pointer to the newly created CUpnpItem,
     *         or NULL if the creation of the item or metadata resolving
     *         fails.
     */
    IMPORT_C static CUpnpItem* CreateItemFromFileL(
                                            const TDesC& aFilePath );

    };

#endif // UPNPMETADATAFETCHER_H

// End of File
