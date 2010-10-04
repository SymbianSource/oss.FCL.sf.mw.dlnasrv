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
* Description:  RUpnpAVCPEngineRFSClient
*
*/


#ifndef C_RUPNPAVCPENGINERFSCLIENT_H
#define C_RUPNPAVCPENGINERFSCLIENT_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

#include "upnpavcpengineclient.h"

// FORWARD DECLARATIONS
class CRsfwDirEnt;
class CRsfwDirEntAttr;


// CLASS DECLARATION
/**
* Class provides function used by UPnPAccess plugin 
* It makes possible to pass requests to AVCPEngine.
*/
class RUpnpAVCPEngineRFSClient : public RUpnpAVCPEngineClient
	{
	public:
		/**
		* C++ default constructor.
		*/		    
		IMPORT_C RUpnpAVCPEngineRFSClient();
		/**
		* Requests for get directory listing
		*
		* @param aUUID UDN of MS
		* @param aPathName path of directory
		* @param aDirList array of directory to be updated
		* @return KErrNone if successful, otherwise one of the other system-wide error codes
		*/
		IMPORT_C TInt GetDirectoryL(const TDesC8& aUUID, const TDesC& aPathName, RPointerArray<CRsfwDirEnt>& aDirList); 
		/**
		* Requests for get directory attribute
		*
		* @param aUUID UDN of MS
		* @param aPathName path of directory
		* @param aDirAtt structure to be updated
		* @return KErrNone if successful, otherwise one of the other system-wide error codes
		*/	
    	IMPORT_C TInt GetDirectoryAttributeL(const TDesC8& aUUID, const TDesC& aPathName, CRsfwDirEntAttr*& aDirAtt);
		/**
		* Requests for get file attribute
		*
		* @param aUUID UDN of MS
		* @param aPathName path of file
		* @param aDirAtt structure to be updated
		* @return KErrNone if successful, otherwise one of the other system-wide error codes
		*/	    
    	IMPORT_C TInt GetFileAttributeL(const TDesC8& aUUID, const TDesC& aPathName, CRsfwDirEntAttr*& aFileAtt);
		/**
		* Requests for get (download) file from MS and store it in local cached file
		*
		* @param aUUID UDN of MS
		* @param aSrcPathName source file (MS)
		* @param aDstPathName destination file (local)		
		* @param aStart start position to read
		* @param aLength length to read
		* @param aRead number of byte that were read
		* @return KErrNone if successful, otherwise one of the other system-wide error codes		
		*/	
    	IMPORT_C TInt GetFileL(const TDesC8& aUUID, const TDesC& aSrcPathName, const TDesC& aDstPathName, 
    			TInt aStart, TInt aLength, TUint aFlags, TInt& aRead);
		/**
		* Requests for delete file
		*
		* @param aUUID UDN of MS
		* @param aPathName path of file
		* @return KErrNone if successful, otherwise one of the other system-wide error codes		
		*/    
    	IMPORT_C TInt DeleteFileL(const TDesC8& aUUID, const TDesC& aPathName);
		/**
		* Requests for delete directory
		*
		* @param aUUID UDN of MS
		* @param aPathName path of directory
		* @return KErrNone if successful, otherwise one of the other system-wide error codes		
		*/       
    	IMPORT_C TInt DeleteDirectoryL(const TDesC8& aUUID, const TDesC& aPathName);
		/**
		* Requests for create file
		*
		* @param aUUID UDN of MS
		* @param aPathName path of file
		* @return KErrNone if successful, otherwise one of the other system-wide error codes		
		*/   
    	IMPORT_C TInt CreateFileL(const TDesC8& aUUID, const TDesC& aPathName);
		/**
		* Requests for create directory
		*
		* @param aUUID UDN of MS
		* @param aPathName path of directory
		* @return KErrNone if successful, otherwise one of the other system-wide error codes		
		*/      
    	IMPORT_C TInt MakeDirectoryL(const TDesC8& aUUID, const TDesC& aPathName);
		/**
		* Requests for put (upload) file from local to MS 
		*
		* @param aUUID UDN of MS
		* @param aSrcPathName source file (local)
		* @param aDstPathName destination file (MS)	
		* @return KErrNone if successful, otherwise one of the other system-wide error codes			
		*/   
    	IMPORT_C TInt PutFileL(const TDesC8& aUUID, const TDesC& aSrcPathName, const TDesC& aDstPathName);     
	private:
		/**
		* Convert information about attributes store in TFileInfo into CDirEntAttr
		*
		* @param aFileInfo file information
		* @return converted object of CDirEntAttr
		*/
		CRsfwDirEntAttr* RUpnpAVCPEngineRFSClient::GetFileAttributesL(TFileInfo& aFileInfo);
		/**
		* Convert information about file store in TFileInfo into CDirEnt
		*
		* @param aFileInfo file information
		* @return converted object of CDirEnt
		*/
		CRsfwDirEnt* RUpnpAVCPEngineRFSClient::GetFileInformationL(TFileInfo& aFileInfo);
		
	public:
		/*
		 * Custom cleanup mechanism
		 */
		static void CleanupArray(TAny* aDirs);
	};

#endif // C_RUPNPAVCPENGINERFSCLIENT_H

// End of File
