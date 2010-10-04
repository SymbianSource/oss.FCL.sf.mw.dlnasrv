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
* Description:  Class provides function used by UPnPAccess plugin 
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <RsfwDirEnt.h>
#include <f32file.h>
#include <rsfwdirentattr.h>
#include <rsfwdirent.h>


#include "upnpavcpenginecommon.h"
#include "upnpavcpenginerfsclient.h"

typedef TBuf8<KMaxFileName> TFileName8;

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::RUpnpAVCPEngineRFSClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RUpnpAVCPEngineRFSClient::RUpnpAVCPEngineRFSClient()
    :RUpnpAVCPEngineClient()
    {
    }
	
// -----------------------------------------------------------------------------
// Cleanup mechanism
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineRFSClient::CleanupArray(TAny* aDirs)
    {
    TFileInfo* dirs = (TFileInfo*) aDirs ;
    delete[] dirs;
    }

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::GetDirectoryL(const TDesC8& aUUID, const TDesC& aPathName, RPointerArray<CRsfwDirEnt>& aDirList) 
	{
    TFileName8 pathname;
    pathname.Copy(aPathName);
    
    TInt id;
    TInt size;
    TPckg<TInt> idPckg(id); 
    TPckg<TInt> sizePckg(size);
    TInt err = SendReceive(EPrepareDirList,TIpcArgs(&idPckg,&sizePckg, &aUUID, &pathname )) ;
    if (err != KErrNone) 
    	{
        return err;    
    	}
    
    if ( size > 0 )   
    	{        
        TFileInfo* dirs = new TFileInfo[size];
        // put array on cleanup stack
        CleanupStack::PushL(TCleanupItem(CleanupArray, dirs));
        TPtr8 result(reinterpret_cast<TUint8*>(dirs), sizeof(TFileInfo)*size, sizeof(TFileInfo)*size);
        err = SendReceive(EDownloadDirList,TIpcArgs(id, &result));

        for (TInt i = 0; i < size; i++) 
        	{
        	CRsfwDirEnt* entry = GetFileInformationL(dirs[i]);
            aDirList.AppendL(entry);
        	}
        CleanupStack::PopAndDestroy(dirs);
    	}
    return err;
	}

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetDirectoryAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::GetDirectoryAttributeL(const TDesC8& aUUID, const TDesC& aPathName, CRsfwDirEntAttr*& aDirAtt) 
	{
	if (aPathName.Compare(KNullDesC) == 0) 
		{
		TFileInfo fi;
		fi.iAtt |= KEntryAttDir; 
		aDirAtt = GetFileAttributesL(fi);
		return KErrNone;
		}
	
    TFileName8 pathname;
    pathname.Copy(aPathName);	
	TFileInfo fileInfo;
    TPckg<TFileInfo> attrPckg(fileInfo);        
    TInt err = SendReceive(EDirAttributes, TIpcArgs(&aUUID, &pathname,  &attrPckg) );
    //convert TFileInfo to CDirEntAttr
    if (err == KErrNone)
    	aDirAtt = GetFileAttributesL(fileInfo);
    
    return err;
	} 

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetFileAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::GetFileAttributeL(const TDesC8& aUUID, const TDesC& aPathName, CRsfwDirEntAttr*& aFileAtt) 
	{
    TFileName8 pathname;
    pathname.Copy(aPathName);
    	
	TFileInfo fileInfo;
    TPckg<TFileInfo> attrPckg(fileInfo);       
    TInt err =  SendReceive(EFileAttributes, TIpcArgs(&aUUID, &pathname,  &attrPckg) ) ;
    //convert TFileInfo to CDirEntAttr
    if (err == KErrNone)
    	aFileAtt = GetFileAttributesL(fileInfo);
    
    return err;
	} 

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::GetFileL(const TDesC8& aUUID, const TDesC& aSrcPathName, const TDesC& aDstPathName,
						TInt aStart, TInt aLength, TUint aFlags, TInt& aRead) 
	{
    TFileName8 srcpathname;
    srcpathname.Copy(aSrcPathName);
    	
    TFilePosition position;
    position.iStart = aStart;
    position.iLength = aLength;
    position.iFlags = aFlags;
    
    TPckg<TFilePosition> posPckg(position);       

    TInt err = SendReceive(EGetFile, TIpcArgs(&aUUID, &srcpathname,  &aDstPathName, &posPckg) ) ;
    aRead = position.iLength;
    return err;
	} 

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::DeleteFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::DeleteFileL(const TDesC8& aUUID, const TDesC& aPathName) 
	{   
    TFileName8 pathname;
    pathname.Copy(aPathName);    	 
    TInt err = SendReceive(EDeleteFile, TIpcArgs(&aUUID, &pathname) );
    return err;
	} 
 
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::DeleteDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::DeleteDirectoryL(const TDesC8& aUUID, const TDesC& aPathName) 
	{  
    TFileName8 pathname;
    pathname.Copy(aPathName);    	  
    TInt err = SendReceive(EDeleteDirectory, TIpcArgs(&aUUID, &pathname) );
    return err;    
	} 

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::CreateFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::CreateFileL(const TDesC8& aUUID, const TDesC& aPathName) 
	{ 
    TFileName8 pathname;
    pathname.Copy(aPathName);    	 	   
    TInt err =  SendReceive(ECreateFile, TIpcArgs(&aUUID, &pathname) );
    return err;     
	}  

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::MakeDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::MakeDirectoryL(const TDesC8& aUUID, const TDesC& aPathName) 
	{ 
    TFileName8 pathname;
    pathname.Copy(aPathName);	   
    TInt err =  SendReceive(ECreateDirectory, TIpcArgs(&aUUID, &pathname) );
    return err;     
	} 

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::PutFileL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineRFSClient::PutFileL(const TDesC8& aUUID, const TDesC& aSrcPathName, const TDesC& aDstPathName) 
	{
    TFileName8 dstpathname;
    dstpathname.Copy(aDstPathName);	
	TInt err =  SendReceive(EPutFile, TIpcArgs(&aUUID, &aSrcPathName,  &dstpathname) );
    return err;     
	} 
	
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetAttributes
// -----------------------------------------------------------------------------
//	
CRsfwDirEntAttr* RUpnpAVCPEngineRFSClient::GetFileAttributesL(TFileInfo& aFileInfo) 
	{
	CRsfwDirEntAttr* result = CRsfwDirEntAttr::NewLC();
	
	result->SetMimeTypeL(aFileInfo.iMimeType);
	result->SetModified(aFileInfo.iModified);
	result->SetAtt(aFileInfo.iAtt);
	result->SetSize(aFileInfo.iSize);
	CleanupStack::Pop(result);
	return result;
	}

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineRFSClient::GetFileInformation
// -----------------------------------------------------------------------------
//	
 CRsfwDirEnt* RUpnpAVCPEngineRFSClient::GetFileInformationL(TFileInfo& aFileInfo) 
	{
	return  CRsfwDirEnt::NewL(aFileInfo.iName, GetFileAttributesL(aFileInfo));
	}	
//  End of File
