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
* Description:  CUpnpDeviceListCommand
*
*/


#include "upnpdevicelistcommand.h"

#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"


// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDeviceListCommand* CUpnpDeviceListCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpDeviceListCommand* self = new( ELeave ) CUpnpDeviceListCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//   
void CUpnpDeviceListCommand::ConstructL() 
	{
 	CUpnpCommand::BaseConstructL();
	} 
  
// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::CUpnpDeviceListCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//    
CUpnpDeviceListCommand::CUpnpDeviceListCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::~CUpnpDeviceListCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpDeviceListCommand::~CUpnpDeviceListCommand()
	{
    delete[] iDevices;
	}


// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpDeviceListCommand::ExecuteL()
	{
	//ASSERT(iType == EPrepareDeviceList);

	RPointerArray<CUpnpDevice> devices = iAVCPManager.DeviceList();
	iDeviceCount = devices.Count();
    DEBUGSTRING8(("Execute DeviceListCommand no. %d",iDeviceCount));

	// set command id	
    TPckg<TInt> idPckg(iId);       		
	iMessage.WriteL(0, idPckg);         
	   
	// set device count - list only MS devices
	TPckg<TInt> devCountPckg(iDeviceCount);       		
	iMessage.WriteL(1, devCountPckg);            
	
	iMessage.Complete(KErrNone);
	
    if ( iDeviceCount > 0) 
    	{                
        iDevices = new(ELeave)TAVDevice[iDeviceCount];
        for (TInt i = 0; i < iDeviceCount; i++) 
        	{
            CUpnpDevice* device = devices[i];
            iDevices[i].iUDN.Copy( device->Uuid() );
            iDevices[i].iFriendlyName.Copy( device->DescriptionProperty(KAVCPEngineFriendlyName) );  
        	}
    	}
    else 
    	{
        delete this;   
    	}   
	}

// -----------------------------------------------------------------------------
// CUpnpDeviceListCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpDeviceListCommand::SetResultL(const RMessage2& aMessage) 
	{
	DEBUGSTRING8(("Set Result from DeviceListCommand"));
	//ASSERT(aMessage.Function() == EDownloadDeviceList);
    if (iDevices) 
    	{
      	TPtr8 result(reinterpret_cast<TUint8*>(iDevices), sizeof(TAVDevice)*iDeviceCount, sizeof(TAVDevice)*iDeviceCount);
      	aMessage.WriteL(1, result);  
    	}	
	}
