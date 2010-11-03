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
* Description:      Push transport server
*
*/

#ifndef UPNPPUSHSERVER_H
#define UPNPPUSHSERVER_H

//  INCLUDES
#include <e32base.h>
#include <e32hashtab.h>

#include <upnphttpservertransactioncreator.h>
#include <upnphttpserverobserver.h>
#include "upnpcontentdirectorydatafinder.h"


// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpHttpServerSession;
class CUpnpElement;
class CUpnpDlnaFilter;
class CUpnpDlnaProtocolInfo;

// CONSTANTS
const TInt KPushServerPort = 58186;

// CLASS DECLARATION

/**
* Push controller server. A singleton class that instantiates itself whenever
* something is shared using the static "ShareL" methods. And as the last item
* is unshared, the singleton is destroyed.
*
* @lib pushtransportserver
*/
NONSHARABLE_CLASS( CUpnpPushServer)
    : public CBase
    , public MUpnpHttpServerTransactionCreator
    , public MUpnpHttpServerObserver
    , public MUpnpContentDirectoryDataFinder
    {

public: // the api

    /**
     * Share an item to the push transport server.
     * All the resources within the item become shared.
     * The given hash should be unique within the process.
     * It can be used to unshare.
     * 
     * @param aHash, TUint (usualy instance of the class using this functionality) 
     * @param aItem, upnpitem
     */
    IMPORT_C static void ShareL(
        TUint aHash,
        CUpnpItem& aItem );

    /**
     * Unshare an item identified by the hash.
     * 
     * @param aHash, TUint (usualy instance of the class using this functionality)
     */
    IMPORT_C static void UnshareL(
        TUint aHash );

    /**
     * Unshare all items
     */
    IMPORT_C static void UnshareAllL();

    /**
     * Returns true if server is running
     */
    IMPORT_C static TBool IsRunning();

private: // internal construction/destruction

	/**
	 * default constructor
	 */
    CUpnpPushServer();

    /**
	 * 2nd phase constructor
	 */
    void ConstructL();

	/**
	 * destructor
	 */
    ~CUpnpPushServer();

private: // MUpnpHttpServerTransactionCreator

	/**
	 * see MUpnpHttpServerTransactionCreator
	 */
    void NewTransactionL( const TDesC8& aMethod,
        const TDesC8& aUri, const TInetAddr& aSender,
        CUpnpHttpServerTransaction*& aTrans );

private: // MUpnpHttpServerObserver

    /**
     * see MUpnpHttpServerObserver
     */
    void HttpEventLD( CUpnpHttpMessage* aMessage );

private: // MUpnpContentDirectoryDataFinder

    /**
     * see MUpnpContentDirectoryDataFinder
     */
    TInt CheckImportUriL( const TDesC8& aImportUri );
    
    /**
     * see MUpnpContentDirectoryDataFinder
     */
    void GetTitleForUriL( TInt aObjectId, TPtr& aValue );
    
    /**
     * see MUpnpContentDirectoryDataFinder
     */
    TInt GetProtocolInfoL( const TDesC8& aContentUri,
        CUpnpDlnaProtocolInfo*& aProtocolInfo );
    
    /**
     * see MUpnpContentDirectoryDataFinder
     */
    TInt FindSharedFolderL( const TDesC& aUrlPath, 
        const TDesC& aFileName, HBufC*& aFolderPath );
    
    /**
     * see MUpnpContentDirectoryDataFinder
     */
    CUpnpDlnaProtocolInfo* GetProtocolInfoByImportUriL( 
        const TDesC8& aImportUri);

private: // internal methods

	/**
	 * shares a resource by hash.
	 * 
	 * @param aHash, TUint  
     * @param aResource, upnpelement
     */    
    void ShareResourceL(
        TUint aHash,
        CUpnpElement& aResource );

	/**
	 * unshared an entry by hash.
	 * 
	 * @param aHash, TUint  
     */
    void UnshareResource(
        TUint aHash );

	/**
	 * count of shared entries.
	 * 
     */
    TInt EntryCount();

	/**
	 * constructs an URI by hashcode.
	 * 
	 * @param aHash, TUint  
     * @param aFileExt, file extension of the element
     * @param aUriBuf, uri output buffer.
     */
    void HashToUri( TUint aHash, const TDesC& aFileExt, TDes8& aUriBuf );

	/**
	 * extracts hashcode from given uri.
	 * 
	 * @param aUri, uri  
     */
    TUint HashFromUri( const TDesC8& aUri );

	/**
	 * extracts hashcode from given uri
	 * 
	 * @param aUri, uri  
     */
    TUint HashFromUri( const TDesC16& aUri );
    
private:
	/**
	* An array entry that represents one resource that is being shared
	* within this push server.
	*/
	class TUpnpPushEntry
	    {
	    public:
	        TUpnpPushEntry( TUint aHash, const TDesC& aPath, 
	        		const TDesC8& aProtocolInfo ): iHash(aHash), 
	        		iPath(aPath), iProtocolInfo(aProtocolInfo) {}            
	        TUint Hash() const {return iHash;}
	        const TDesC& Path() const {return iPath;}
	        const TDesC8& ProtocolInfo() const {return iProtocolInfo;}
	    private:
	        TUint iHash;
	        const TDesC& iPath;
	        const TDesC8& iProtocolInfo;
	    };

private: // members

    // the http server
    CUpnpHttpServerSession* iHttpServer;

    // shared content
    RHashMap<TUint,TUpnpPushEntry> iEntries;
    
    CUpnpDlnaFilter* iFilter; //owned

    };


#endif  // UPNPPUSHSERVER_H

// End of File
