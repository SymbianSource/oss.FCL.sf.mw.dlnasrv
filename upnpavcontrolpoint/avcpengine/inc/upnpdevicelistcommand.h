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
* Description:  Class responsbile for MS device list
*
*/


#ifndef C_CUPNPDEVICELISTCOMMAND_H
#define C_CUPNPDEVICELISTCOMMAND_H

//  INCLUDES
#include "upnpcommand.h"

// CLASS DECLARATION
/**
* Class responsible for getting list of active MSs.
*
*  @since Series 60 3.1
*/
class CUpnpDeviceListCommand : public CUpnpCommand
	{
	public:	
		/**
		* Two-phased constructor.
		*
		* @param aAVCPManager manager
		* @param aSession server-side session
		* @param aMessage message
		*/		
		static CUpnpDeviceListCommand* NewL(CUpnpAVCPManager& aAVCPManager, 
	  	CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Destructor.
		*/		       
		virtual ~CUpnpDeviceListCommand();	       
		
	public:	// Method from CUpnpCommand	
		/**
		* From CUpnpCommand perfroms operation connected with command
		* @since Series 60 3.1
		*/							
		void ExecuteL();
		/**
		* From CUpnpCommand set result from previous performed command.
		* It sets result from directory listing.
		* @since Series 60 3.1
		*/	
		void SetResultL(const RMessage2& aMessage);	

	private:
		/**
		* C++ default constructor.
		*/
		CUpnpDeviceListCommand(CUpnpAVCPManager& aAVCPManager, 
			CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage);
		/**
		* Symbian 2nd phase constructor.
		*/			
		void ConstructL();
	
	private:
		// size of array with devices
		TInt iDeviceCount;	
		// array of devices
		TAVDevice* iDevices;
		
};

#endif //C_CUPNPDEVICELISTCOMMAND_H
