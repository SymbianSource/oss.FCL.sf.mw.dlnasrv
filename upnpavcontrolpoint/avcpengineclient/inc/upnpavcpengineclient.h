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
* Description:  RUpnpAVCPEngineClient
*
*/


#ifndef C_RUPNPAVCPENGINECLIENT_H
#define C_RUPNPAVCPENGINECLIENT_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>
#include "upnpavcpenginecommon.h"

// CLASS DECLARATION
/**
* Generic class for client-interface. 
* It enables to connect to server and starts server if it is not started.
*/
class RUpnpAVCPEngineClient : public RSessionBase
	{
	public:
		/**
		* C++ default constructor.
		*/	
		RUpnpAVCPEngineClient();
		/**
		* Connect to server, if server is not started, it starts.
		* 
		* @return status of operation, if OK, KErrNone
		*/
		IMPORT_C virtual TInt Connect();
		/**
		* Return version of client, used for connect
		*
		* @return version
		*/
		TVersion Version() const;		
	};

#endif // C_RUPNPAVCPENGINECLIENT_H

// End of File
