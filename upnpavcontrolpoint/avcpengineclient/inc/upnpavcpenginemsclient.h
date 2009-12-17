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
* Description:  RUpnpAVCPEngineMSClient
*
*/


#ifndef C_RUPNPAVCPENGINEMSCLIENT_H
#define C_RUPNPAVCPENGINEMSCLIENT_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "upnpavcpengineclient.h"

// FORWARD DECLARATIONS
class TAVDevice;
class CUpnpMetadata;


// CLASS DECLARATION
/**
* Class used for accessing MS's metadata. 
* Also it provides list of currently active MSs
*/
class RUpnpAVCPEngineMSClient : public RUpnpAVCPEngineClient
	{
	public:
		/**
		* C++ default constructor.
		*/	    
		IMPORT_C RUpnpAVCPEngineMSClient();
		/**
		* Get list of available MS devices
		* 
		* @param aDeviceList array to update
		*/
		IMPORT_C TInt GetDeviceListL(RArray<TAVDevice>& aDeviceList);		    
	    /**
	    * Requests for preparing metadata for given path
	    *
	    * @param aUUID UDN of MS
	    * @param aPathName pathname
	    * @param aIdPckg id of command with result (metadata)
	    * @param aSizePckg size of metadata
	    */ 	    
		void PrepareMetadataL(const TDesC8& aUUID, const TDesC8& aPathName, 
			TPckg<TInt>& aIdPckg, TPckg<TInt>& aSizePckg); 	

	    /**
	    * Requests for preparing metadata for given path
	    *
	    * @param aUUID UDN of MS
	    * @param aPathName pathname
	    * @param aIdPckg id of command with result (metadata)
	    * @param aSizePckg size of metadata
	    */ 	    
		void PrepareMetadata(TRequestStatus& aStatus, const TDesC8& aUUID, const TDesC8& aPathName, 
			TPckg<TInt>& aIdPckg, TPckg<TInt>& aSizePckg); 	

					
	    /**
	    * Requests for downloading metadata from result of previous run command
	    *
	    * @param aId id of command with result (metadata)
	    * @param aMetaData buffer for metadata
	    */
		void DownloadMetadataL(TInt aId, TDes8& aMetaData);
					
	    /**
	    * Requests for downloading metadata from result of previous run command
	    *
	    * @param aId id of command with result (metadata)
	    * @param aMetaData buffer for metadata
	    */
		void DownloadMetadata(TRequestStatus& aStatus, TInt aId, TDes8& aMetaData);
			      
	    /**
	    * Requests for set metadata for given path
	    *
	    * @param aUUID UDN of MS
	    * @param aPathName pathname
	    * @param aOldMetaData old metadata
			* @param aNewMetaData new metadata
	    */    
	    void SetMetadataL(const TDesC8& aUUID, const TDesC8& aPathName, const TDesC8& aOldMetaData, const TDesC8& aNewMetaData);    
	    
	    /**
	    * Requests for set metadata for given path
	    *
	    * @param aUUID UDN of MS
	    * @param aPathName pathname
	    * @param aOldMetaData old metadata
			* @param aNewMetaData new metadata
	    */    
	    void SetMetadata(TRequestStatus& aStatus, const TDesC8& aUUID, const TDesC8& aPathName, const TDesC8& aOldMetaData, const TDesC8& aNewMetaData);   	    
	};

#endif // C_RUPNPAVCPENGINEMSCLIENT_H

// End of File
