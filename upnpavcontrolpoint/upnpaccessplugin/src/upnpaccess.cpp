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
* Description:  Implements remote access plugin API - loopback to File Server
 *
*/


// INCLUDE FILES
#include "upnpaccess.h"
#include "upnpmdebug.h"

#include <e32std.h>
#include <ecom/implementationproxy.h>

// ============================= LOCAL FUNCTIONS ===============================

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
    {
        {{0x101F9779}, (TProxyNewLPtr)CUpnpAccess::NewL}
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy*
ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CUpnpAccess::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUpnpAccess::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CUpnpAccess* CUpnpAccess::NewL()
    {
    CUpnpAccess* self = new (ELeave) CUpnpAccess;
    DEBUGSTRING(("in NewL 0x%x", self));
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CUpnpAccess::~CUpnpAccess()
    {
    DEBUGSTRING(("in destructor 0x%x", this));
    delete iUUID;
    iAVCPEngineClient.Close();
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::SetupL
// ----------------------------------------------------------------------------
//
    void CUpnpAccess::SetupL(MRsfwRemoteAccessObserver*)
    {    
    DEBUGSTRING(("in SetupL"));
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::OpenL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::OpenL(const TUriC& aUri,
	                    const TDesC& /* aFriendlyName */,
						const TDesC& /* aUserName */,
                        const TDesC& /* aPassword */,                     
                        const TDesC& /* aAuxData */,
                        MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
   
    TPtrC host(aUri.Extract(EUriHost));
    DEBUGSTRING16(("open plugin for '%S'", &host));
        
    iUUID = HBufC8::NewL(host.Length());
    iUUID->Des().Copy(host);
    
    User::LeaveIfError(iAVCPEngineClient.Connect());
    
    aResponseHandler->HandleRemoteAccessResponse(0, KErrNone);
    return 0;
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::GetDirectoryL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::GetDirectoryL(const TDesC& aPathName,
                                RPointerArray<CRsfwDirEnt>& aDirEnts,
                                MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("get directory '%S'", &aPathName));

    TInt err = iAVCPEngineClient.GetDirectoryL(*iUUID, aPathName, aDirEnts);
    
    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;
    
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::GetDirectoryAttributesL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::GetDirectoryAttributesL(const TDesC& aPathName,
                                          CRsfwDirEntAttr*& aAttr,
                                          MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    // This function reads attributes of a directory
    // (not attributes of the files in the directory)
    DEBUGSTRING16(("Get directory attributes of '%S'", &aPathName));
    
    TInt err = iAVCPEngineClient.GetDirectoryAttributeL(*iUUID, aPathName, aAttr);

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;
    
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::GetFileAttributesL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::GetFileAttributesL(const TDesC& aPathName,
                               CRsfwDirEntAttr*& aAttr,
                               MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("Get file attributes of '%S'", &aPathName));
    
    TInt err = iAVCPEngineClient.GetFileAttributeL(*iUUID, aPathName, aAttr);
        
    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;  
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::SetAttributesL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::SetAttributesL(const TDesC& aPathName,
                           CRsfwDirEntAttr& /*aAttr*/,
                           MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("Set attributes of '%S'",
                  &aPathName));

    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);
    return 0;  
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::GetFileL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::GetFileL(const TDesC& aRemotePathName,
                           	const TDesC& aLocalPathName,
                           	TInt aOffset,
                           	TInt* aLength,
                   			TUint aFlags,                           
                           	MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
       
    DEBUGSTRING16(("get file '%S'", &aRemotePathName));
    DEBUGSTRING16(("     into file '%S'", &aLocalPathName));
    
    TInt length = 0;
    if (aLength) 
    {
        length = *aLength;
    }
    DEBUGSTRING(("offset=%d, length=%d", aOffset, length));        
    	
    TInt read;
    TInt err = iAVCPEngineClient.GetFileL(*iUUID, 
    		aRemotePathName, aLocalPathName, aOffset, length, aFlags, read);
    		
    if (aLength) {
    	*aLength = read;    	
    }
             
    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0; 

    }

// ----------------------------------------------------------------------------
// CUpnpAccess::MakeDirectoryL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::MakeDirectoryL(const TDesC& aPathName,
                           MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("Make directory '%S'", &aPathName));
    
    TInt err = iAVCPEngineClient.MakeDirectoryL(*iUUID, aPathName);    

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;  
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::CreateFileL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::CreateFileL(const TDesC& aPathName, TBool aOverWriting,
                              MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("Create file '%S'", &aPathName));
    
    TInt err(0);
    if (aOverWriting) 
    	{
    	DEBUGSTRING16(("delete file '%S'", &aPathName));    
    	err = iAVCPEngineClient.DeleteFileL(*iUUID, aPathName);       	
    	}
    
    if (err == KErrNone) 
    	err = iAVCPEngineClient.CreateFileL(*iUUID, aPathName);    

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;  
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::PutFileL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::PutFileL(const TDesC& aLocalPathName,
                        	const TDesC& aRemotePathName,
                   			const TDesC8& /*aMimeType*/,                             
                           	MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("put file '%S'", &aLocalPathName));
    DEBUGSTRING16(("     to remote file '%S'", &aRemotePathName));
        
    TInt err = iAVCPEngineClient.PutFileL(*iUUID, aLocalPathName, aRemotePathName);    

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;   
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::PutFileL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::PutFileL(const TDesC& aLocalPathName,
                           	const TDesC& aRemotePathName,
							const TDesC8& /*aMimeType*/,                                                        	
                           	TInt /* aOffset */,
                           	TInt /* aLength */,
                           	TInt /* aTotalLength */,                   			
                           	MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("put file '%S'", &aLocalPathName));
    DEBUGSTRING16(("     to remote file '%S'", &aRemotePathName));

    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);
    return 0; 
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::DeleteDirectoryL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::DeleteDirectoryL(const TDesC& aPathName,
                             MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("delete directory '%S'", &aPathName));
    
    TInt err = iAVCPEngineClient.DeleteDirectoryL(*iUUID, aPathName);    

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;  
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::DeleteFileL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::DeleteFileL(const TDesC& aPathName,
                              MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("delete file '%S'", &aPathName));
    
    TInt err = iAVCPEngineClient.DeleteFileL(*iUUID, aPathName);    

    aResponseHandler->HandleRemoteAccessResponse(0, err);
    return 0;     
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::RenameL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::RenameL(const TDesC& aSrcPathName,
                          const TDesC& aDstPathName,
                          TBool /*aOverwrite*/,
                          MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    
    DEBUGSTRING16(("Rename '%S' to '%S'", &aSrcPathName, &aDstPathName));
    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);
    return 0;
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::ObtainLockL
// ----------------------------------------------------------------------------
//

TUint CUpnpAccess::ObtainLockL(const TDesC&  aPathName ,
                        TUint /* aLockFlags */,
                        TUint& /*aTimeOut*/,
                        TDesC8*& /* aLockToken */,
                        MRsfwRemoteAccessResponseHandler*  aResponseHandler )
    {
    DEBUGSTRING16(("obtain lock '%S'", &aPathName));    

    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);    
    return 0;
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::ReleaseLockL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::ReleaseLockL(const TDesC&  aPathName,
                         MRsfwRemoteAccessResponseHandler*  aResponseHandler)
    {
    DEBUGSTRING16(("release lock '%S'", &aPathName));

    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);
    return 0;
    }

// ----------------------------------------------------------------------------
// CUpnpAccess::RefreshLockL
// ----------------------------------------------------------------------------
//
TUint CUpnpAccess::RefreshLockL(const TDesC&  aPathName, TUint& /*aTimeOut*/,
                         MRsfwRemoteAccessResponseHandler* aResponseHandler)
    {
    DEBUGSTRING16(("refresh lock '%S'", &aPathName));
    
    aResponseHandler->HandleRemoteAccessResponse(0, KErrNotSupported);    
    return 0;
    }
    
// ----------------------------------------------------------------------------
// CUpnpAccess::SetLockToken
// ----------------------------------------------------------------------------
//
TInt CUpnpAccess::SetLockToken(const TDesC& /* aPathName */,
                              const TDesC8& /* aLockToken */)
    {
    return KErrNotSupported;
    }
    
// ----------------------------------------------------------------------------
// CUpnpAccess::CancelL
// ----------------------------------------------------------------------------
//
void CUpnpAccess::Cancel(TDesC& /* aTargetPath */)
    {
    // Nothing to cancel
    }
// ----------------------------------------------------------------------------
// CUpnpAccess::CancelL
// ----------------------------------------------------------------------------
//
void CUpnpAccess::Cancel(TUint /* aId */)
    {
    // Nothing to cancel
    }


//  End of File
