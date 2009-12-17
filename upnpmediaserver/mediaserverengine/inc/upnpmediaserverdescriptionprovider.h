/** @file
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpMediaServerDescriptionProvider declaration
*
*/

#ifndef CUPNPMEDIASERVERDESCRIPTIONPROVIDER_H
#define CUPNPMEDIASERVERDESCRIPTIONPROVIDER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32hashtab.h> 
#include "upnpdevicedescriptionprovider.h"

// FORWARD DECLARATION

class CUpnpSecurityManager;
class TInetAddr;

const TInt KMaxUriLength = 256;

// CLASS DECLARATION

/**
 * CUpnpMediaServerDescriptionProvider
 * Provides description of device and its services, and icons. 
 *
 * @lib avmediaserver.exe
 * @since S60 MCL 
 */
class CUpnpMediaServerDescriptionProvider : public CBase,
        public MUpnpDeviceDescriptionProvider
    {
public:

    /**
     * Destructor.
     */
    ~CUpnpMediaServerDescriptionProvider();

    /**
     * Two-phased constructor.
     */
    static CUpnpMediaServerDescriptionProvider* NewL();

    /**
     * Two-phased constructor.
     */
    static CUpnpMediaServerDescriptionProvider* NewLC();
    
    /**
     * Adds new map uri to file.
     * 
     * @since S60 MCL
     * @param aUri uri
     * @param aFile file for aUri
     */    
    void AddUriToFileMapL( const TDesC8& aUri, const TDesC16& aFile );
    
    /**
     * Removes map uri to file.
     * 
     * @since S60 MCL
     * @param aUri uri
     */    
    void RemoveUriToFileMapL( const TDesC8& aUri );
    
private:    // from MUpnpDeviceDescriptionProvider
    
    virtual TInt OnDeviceDescription( CUpnpDeviceDescriptionRequest& aRequest );
    
    virtual TInt OnServiceDescription( CUpnpDeviceDescriptionRequest& aRequest );    
    
    virtual TInt OnIcon( CUpnpDeviceDescriptionRequest& aRequest );            
    
    virtual TInt GetFile( const TDesC8& aUri, RFile& aFile ); 

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpMediaServerDescriptionProvider();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Main function to service requests.
     * 
     * @since S60 MCL
     * @param aRequest request to service
     * @return error code
     */
    TInt OnRequest( CUpnpDeviceDescriptionRequest& aRequest );

    /**
     * Check if resource is accessible.
     * Leaves with -EHttpNotFound when filename 
     * is not authorized for specific IP.
     * May leave with other system error code. 
     * 
     * @since S60 MCL
     * @param aFileName data source file name
     * @param aSender sender IP address
     */
    void AuthorizeRequestL( const TDesC& aFileName, const TInetAddr& aSender );
   
    /**
     * Gets a folder with device description.
     * Leave with KErrNotFound when aUri is not found.
     * 
     * @since S60 MCL
     * @param aUri uri to get file for
     * @param aPath on return contains path of file related to aUri
     */
    void GetFileFromUriL( const TDesC8& aUri, TDes& aPath );

    /**
     * Gets a path from uri. Removed IP and port.
     *  
     * @since S60 MCL
     * @param aUri uri to extract
     * @return allocated buffer with extracted uri lower case
     */
    HBufC8* ExtractPathAndFileFromUriL( const TDesC8& aUri );

private:
    
    // Security manager
    // Own
    CUpnpSecurityManager* iSecurityManager;
    
    // File server session used to open file
    RFs iFs;

    // Hash map uri to file
    RHashMap< TBufC8<KMaxUriLength>, TFileName > iUriMap;
    
    // Used for file name to aviod use this type on stack
    TFileName iFileName;
    };

#endif // CUPNPMEDIASERVERDESCRIPTIONPROVIDER_H
