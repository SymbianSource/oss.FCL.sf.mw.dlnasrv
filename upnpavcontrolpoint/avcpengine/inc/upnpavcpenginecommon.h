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
* Description:  Common data
*
*/


#ifndef C_UPNPAVCPENGINECOMMON_H
#define C_UPNPAVCPENGINECOMMON_H

//INCLUDES
#include <e32base.h>


// CONSTANTS
// Server name
_LIT( KAVCPEngineName,     "AVCPEngine" );
// Filename
_LIT( KAVCPEngineFilename, "AVCPEngine" );

_LIT8( KAVCPEngineFriendlyName, "friendlyName" ); 


// The server version. A version must be specified when 
// creating a session with the server
const TUint KAVCPEngineMajorVersionNumber=0;
const TUint KAVCPEngineMinorVersionNumber=1;
const TUint KAVCPEngineBuildVersionNumber=1;

// ENUMERATIONS

// Operation codes used in message passing between client and server
enum TAVCPEngineRqst
	{
	EPrepareDeviceList,
	EDownloadDeviceList,
	EPrepareDirList,
	EDownloadDirList,
	EDirAttributes,
	EFileAttributes,
	EGetFile,
	EDeleteDirectory,
	EDeleteFile,
	ECreateDirectory,
	ECreateFile,
	EPutFile, 
	EPrepareMetadata,
	EDownloadMetadata,	
	ESetMetadata	
    };

// maximum length of UDN
const TInt KUUIDLength = 50;
// maximum length of friendly-name
const TInt KFriendlyNameLength = 50;
// maximum length of mime type
const TInt KMimeLength = 100;

// structure describing MS device
class TAVDevice
    {
    public:        
    	TBuf8<KFriendlyNameLength> iFriendlyName;    	    
    	TBuf8<KUUIDLength> iUDN;
    };
// structure describing position in file, used for partial read
class TFilePosition
    {
    public:        
    	TInt iStart;    	    
    	TInt iLength;
    	TBool iFlags;
    };

// structure describing file/directory
class TFileInfo
    {
    public: 
    	inline TFileInfo(); 
    	// file/directory name
    	TBuf8<KMaxFileName> iName;        
		// MIME type      
		TBuf8<KMimeLength> iMimeType;     	
		// attribute bits
		TUint iAtt;
		// file size in bytes        
		TInt iSize;      
		// last modified         
		TTime iModified; 
    };

#include "upnpavcpenginecommon.inl"

#endif // C_UPNPAVCPENGINECOMMON_H

// End Of File