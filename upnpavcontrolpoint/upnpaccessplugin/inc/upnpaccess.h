/** @file
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  UPnP plugin interface for RSFW
*
*/


#ifndef C_CUPNPACCESS_H
#define C_CUPNPACCESS_H

// INCLUDES
#include <rsfwremoteaccess.h>
#include "upnpavcpenginerfsclient.h"

// CLASS DECLARATION

/**
 *  UPnP plugin for Rsfw.
 *  It enables mounting MS as new filesystem.
 *
 *  @lib upnpaccess.lib
 *  @since Series 60 3.1
 */
class CUpnpAccess: public CRsfwRemoteAccess
	{
	public: // Constructors and destructor
		/**
		 * Two-phased constructor.
		 */
		static CUpnpAccess* NewL();
		
		/**
		 * Destructor.
		 */
    virtual ~CUpnpAccess();
    /**
     * Set up parameters for operation.
     * @param aRsfwRemoteAccessObserver MRsfwRemoteAccessObserver for receiving
     *  asynchronous events from the accessor plugin,
     *  e.g. changes in connectivity.
     *  This parameter may be NULL
     *  NOTICE: this function is not supported by UPnP access plugin
     */
    void SetupL(MRsfwRemoteAccessObserver* /* aRsfwRemoteAccessObserver */);

    /**
     * Opens a connection to the server given by aServerName parameter.
     * For UPnPAccess only serve has to be defined and 
     * value should be equal to UDN of MS
     * So format URI for mounting has to be:
     * upnp://UDN
     *
     * @param aUri URI of the remote repository.
     *   The URI must not contain authority part (user name/password)
     * @param aFriendlyName server friendly name for access control dialog 
     *                                  (can be empty)
     * @param aUserName user name for access control (can be empty)
     * @param aPassword password for access control (can be empty)
     * @param aAuxData auxiliary parameters for connection setup (eg IAP info)
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */
    TUint OpenL(const TUriC& aUri,
    	    const TDesC& aFriendlyName,
			const TDesC& aUserName,
			const TDesC& aPassword,
			const TDesC& aAuxData,
			MRsfwRemoteAccessResponseHandler* aResponseHandler) ;

		/**
		* Gets contents of the directory given by aPathName parameter.
		*
		* @param aPathName path name of the directory
		* @param aDirentsp an array of directory entries to be filled.
		* @param aResponseHandler response handler
		* @return identifier of the created transaction
		*/    
		TUint GetDirectoryL(const TDesC& aPathName,
                        RPointerArray<CRsfwDirEnt>& aDirEnts,
                        MRsfwRemoteAccessResponseHandler* aResponseHandler);

		/**
		* Gets attributes of the directory given by aPathName parameter.
		* This function may also be called if the type of the object is
		* not yet known (e.g., the object could be a file).
		*
		* @param aPathName path name of the directory
		* @param aAttr attribute structure to be filled
		* @param aResponseHandler response handler
		* @return identifier of the created transaction
		*/    
    TUint GetDirectoryAttributesL(
			const TDesC& aPathName,
			CRsfwDirEntAttr*& aAttr,
			MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Gets attributes of the file given by aPathName parameter.
     *
     * @param aPathName path name of the file
     * @param aAttr attribute structure to be filled
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */         
		TUint GetFileAttributesL(const TDesC& aPathName,	CRsfwDirEntAttr*& aAttr,
			MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /** 
     * Sets attributes of the file or directory given by aPathName parameter.
     * This function is typically only used for files and even then
     * the implementation may do nothing since standard file attributes
     * are implied by the contents of the file or set in conjunction with
     * other operations on the file system object.
     * NOTICE: this function is not supported by UPnP access plugin
     *
     * @param aPathName path name of the file or directory
     * @param aAttr attribute structure
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint SetAttributesL(const TDesC& aPathName,
                         CRsfwDirEntAttr& aAttr,
                         MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Gets a remote file and copies it to a local file.
     * Note that byte ranges are not be implemented by all
     * file access protocols.
     *
     * @param aRemotePathName path name of the remote file
     * @param aLocalPathName path name of the local file
     * @param aOffset offset of the first byte to be accessed
     * @param aLength length of data to be accessed/was accessed (NULL/0=all)
     * @param aFlags operation qualifier.
     *   The following flags have been defined:
     *   KRemoteAccessOptionGetToStartOfFile: even if an offset is specified
     *      the fetched data is still put at the beginning of the local file.     
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint GetFileL(const TDesC& aRemotePathName,
                   const TDesC& aLocalPathName,
                   TInt aOffset,
                   TInt* aLength,
                   TUint aFlags,
                   MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Puts a file to the server.
     * NOTICE: this function is not supported by UPnP access plugin     
     *
     * @param aLocalPathName path name of the local file
     * @param aRemotePathName path name of the remote file
     * @param aMimeType MIME-type of the file
     *   (will be put to Content-Type, e.g. text/plain or
     *   application/octet-stream)     
     * @param aOffset offset of the first byte to be accessed
     * @param aLength length of data to be accessed (NULL/0=all)
     * @param aTotalLength total length of the file     
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */   
    TUint PutFileL(const TDesC& aLocalPathName,
                   const TDesC& aRemotePathName,
                   const TDesC8& aMimeType,                   
                   TInt aOffset,
                   TInt aLength,
                   TInt aTotalLength,                   
                   MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Puts a file to the server.
     *
     * @param aLocalPathName path name of the local file
     * @param aRemotePathName path name of the remote file
     * @param aMimeType MIME-type of the file (will be put to Content-Type,
     *   e.g. text/plain or application/octet-stream)    
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint PutFileL(const TDesC& aLocalPathName,
                   const TDesC& aRemotePathName,
                   const TDesC8& aMimeType,                   
                   MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Creates an empty file on the remote server
     *
     * @param aPathName path name of the new file
     * @param aOverWriting whether we are overwriting an existing file     
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint CreateFileL(const TDesC& aPathName, TBool aOverWriting,
                      MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Makes a directory.
     *
     * @param aPathName path name of the new directory
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */
    TUint MakeDirectoryL(const TDesC& aPathName,
                         MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Deletes a directory.
     *
     * @param aPathName path name of the directory to be deleted
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint DeleteDirectoryL(const TDesC& aPathName,
                           MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Deletes a file.
     *
     * @param aPathName path name of the file to be deleted
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */
    TUint DeleteFileL(const TDesC& aPathName,
                      MRsfwRemoteAccessResponseHandler* aResponseHandler);
		/**
     * Renames a file or a directory.
     * (may involve movement to another directory).
     * NOTICE: this function is not supported by UPnP access plugin        
     *
     * @param aSrcPathName path name of the object to be renamed
     * @param aDstPathName new path name of the object
     * @param aOverwrite allow overwriting an existing object
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */
    TUint RenameL(const TDesC& aSrcPathName,
                  const TDesC& aDstPathName,
                  TBool aOverwrite,
                  MRsfwRemoteAccessResponseHandler* aResponseHandler);

    /**
     * Obtains a lock for the given file system object
     * NOTICE: this function is not supported by UPnP access plugin       
     *
     * @param aPathName path name of the object to be locked
     * @param aLockFlags indicates whether a write or read lock is requested
     * @param aTimeout the timeout that is requested and granted
     * @param aLockToken lock token
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */
    TUint ObtainLockL(const TDesC& aPathName,
                      TUint aLockFlags,
                      TUint& aTimeout,
                      TDesC8*& aLockToken,
                      MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Releases the lock of the given file system object
     * NOTICE: this function is not supported by UPnP access plugin       
     *
     * @param aPathName path name of the object to be locked
     * @param aResponseHandler response handler
     */    
    TUint ReleaseLockL(const TDesC& aPathName,
                       MRsfwRemoteAccessResponseHandler* aResponseHandler);
    /**
     * Refreshes the lock of the given file system object
     * NOTICE: this function is not supported by UPnP access plugin       
     *
     * @param aPathName path name of the object to be locked
     * @param aTimeout the timeout that is requested and granted
     * @param aResponseHandler response handler
     * @return identifier of the created transaction
     */    
    TUint RefreshLockL(const TDesC& aPathName,
                       TUint& aTimeout,
                       MRsfwRemoteAccessResponseHandler* aResponseHandler);

    /**
     * Sets lock token for the a given resource
     * This lock token value replaces any previously cached token value
     *
     * @param aPathName path name
     * @param aLockToken lock token
     * @return error code
     */
    TInt SetLockToken(const TDesC& aPathName, const TDesC8& aLockToken);
                           
    /**
     * Cancels a transaction
     * Eventually the transaction will/should call HandleRemoteAccessResponseL
     * with status KErrCancel
     * NOTICE: this function is not supported by UPnP access plugin       
     *
     * @param aId the identifier of the transaction to be canceled
     */
    void Cancel(TUint aId);
    
    /**
     * Cancels a transaction
     * Eventually the transaction will/should call HandleRemoteAccessResponseL
     * with status KErrCancel
     * NOTICE: this function is not supported by UPnP access plugin       
     *
     * @param aTargetPath the identifier of the transaction to be canceled
     */    
    void Cancel(TDesC& aTargetPath);

	private:
		/**
		* Symbian 2nd phase constructor.
		*/	
    void ConstructL();

	private:
    // UDN of MS
    HBufC8* iUUID;
    // client to target server
    RUpnpAVCPEngineRFSClient iAVCPEngineClient;
    };

#endif // C_CUPNPACCESS_H

// End of File
