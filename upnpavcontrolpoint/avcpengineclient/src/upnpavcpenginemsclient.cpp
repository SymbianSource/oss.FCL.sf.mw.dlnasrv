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
* Description:  Class used for accessing MS's metadata.
*
*/


// INCLUDE FILES
#include <e32math.h>
#include "upnpavcpenginemsclient.h"

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::RUpnpAVCPEngineMSClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RUpnpAVCPEngineMSClient::RUpnpAVCPEngineMSClient()
    :RUpnpAVCPEngineClient()
    {
    }

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::GetDeviceListL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RUpnpAVCPEngineMSClient::GetDeviceListL(RArray<TAVDevice>& aDeviceList) 
	{    
    TInt id;
    TInt size;
    TPckg<TInt> idPckg(id); 
    TPckg<TInt> sizePckg(size); 
    SendReceive(EPrepareDeviceList,TIpcArgs(&idPckg,&sizePckg));
    if ( size > 0 )   
    	{        
        TAVDevice* devices = new(ELeave)TAVDevice[size];
        TPtr8 result(reinterpret_cast<TUint8*>(devices), sizeof(TAVDevice)*size, sizeof(TAVDevice)*size);
        SendReceive(EDownloadDeviceList,TIpcArgs(id, &result));

        for (TInt i = 0; i < size; i++) 
        	{
           	aDeviceList.AppendL(devices[i]);
        	}
        delete[] devices;
    	}
    return size;
	}

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::PrepareMetadataL
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::PrepareMetadataL(const TDesC8& aUUID, const TDesC8& aPathName, TPckg<TInt>& aIdPckg, TPckg<TInt>& aSizePckg) 
	{    
    User::LeaveIfError( SendReceive(EPrepareMetadata,TIpcArgs(&aIdPckg,&aSizePckg, &aUUID, &aPathName )) );
	}

// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::PrepareMetadata
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::PrepareMetadata(TRequestStatus& aStatus, const TDesC8& aUUID, const TDesC8& aPathName, TPckg<TInt>& aIdPckg, TPckg<TInt>& aSizePckg) 
	{    
    SendReceive(EPrepareMetadata,TIpcArgs(&aIdPckg,&aSizePckg, &aUUID, &aPathName ), aStatus) ;
	}
	
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::DownloadMetadataL
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::DownloadMetadataL(TInt aId, TDes8& aMetaData) 
	{           
	User::LeaveIfError( SendReceive(EDownloadMetadata,TIpcArgs(aId, &aMetaData)));
    }
    
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::DownloadMetadataL
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::DownloadMetadata(TRequestStatus& aStatus, TInt aId, TDes8& aMetaData) 
	{           
	SendReceive(EDownloadMetadata,TIpcArgs(aId, &aMetaData), aStatus);
    }
	    	
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::SetMetadataL
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::SetMetadataL(const TDesC8& aUUID, const TDesC8& aPathName, const TDesC8& aOldMetaData, const TDesC8& aNewMetaData) 
	{
    User::LeaveIfError( SendReceive(ESetMetadata,TIpcArgs(&aUUID, &aPathName, &aOldMetaData, &aNewMetaData) ) );   
	}
	
// -----------------------------------------------------------------------------
// RUpnpAVCPEngineMSClient::SetMetadata
// -----------------------------------------------------------------------------
//
void RUpnpAVCPEngineMSClient::SetMetadata(TRequestStatus& aStatus, const TDesC8& aUUID, const TDesC8& aPathName, const TDesC8& aOldMetaData, const TDesC8& aNewMetaData) 
	{
    SendReceive(ESetMetadata,TIpcArgs(&aUUID, &aPathName, &aOldMetaData, &aNewMetaData), aStatus);   
	}	

//  End of File
