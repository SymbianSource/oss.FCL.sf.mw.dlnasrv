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
* Description:  CUpnpCommand
*
*/


// INCLUDE FILES
#include "upnpcommand.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"
#include "upnpavcpenginesession.h"
// commands
#include "upnpdevicelistcommand.h"
#include "upnpbrowsecommand.h"
#include "upnpdownloadcommand.h"
#include "upnpuploadcommand.h"
#include "upnpcreatecommand.h"
#include "upnpdeletecommand.h"
#include "upnpmetadatacommand.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCommand::CUpnpCommand
// C++ default constructor can NOT contain any code, that
// might leae.
// -----------------------------------------------------------------------------
CUpnpCommand::CUpnpCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   iAVCPManager(aAVCPManager),
                   iSession(aSession),
                   iMessage(aMessage)                       
    {
    iType = (TAVCPEngineRqst)iMessage.Function();
    iId = aAVCPManager.NewCommandId();
    }

// -----------------------------------------------------------------------------
// CUpnpCommand::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCommand::BaseConstructL()
    {    
    }

// -----------------------------------------------------------------------------
// CUpnpCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCommand* CUpnpCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
    {
    	CUpnpCommand* command = NULL;
		switch (aMessage.Function()) 
			{  
	        case EPrepareDeviceList:	        
		        command = CUpnpDeviceListCommand::NewL(aAVCPManager, aSession, aMessage);
	        break;
	        case EPrepareDirList:
            case EDirAttributes:          
            case EFileAttributes:	        
		        command = CUpnpBrowseCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;
	        case EGetFile:
		        command = CUpnpDownloadCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;
	        case EPutFile:
		        command = CUpnpUploadCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;	        
	        case EDeleteFile:
            case EDeleteDirectory:	        
		        command = CUpnpDeleteCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;	       
	        case ECreateFile:
            case ECreateDirectory:	        
		        command = CUpnpCreateCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;	 	        
	        case EPrepareMetadata:
            case ESetMetadata:                      
		        command = CUpnpMetadataCommand::NewL(aAVCPManager, aSession, aMessage);	        
	        break;	        
			}    

    return command;
    }	

// -----------------------------------------------------------------------------
// CUpnpCommand::~CUpnpCommand
// desctructor
// -----------------------------------------------------------------------------
//    
CUpnpCommand::~CUpnpCommand()
    {
        iSession.RemoveCommand(this);
    }
    	
// -----------------------------------------------------------------------------
// CUpnpCommand::Id
// -----------------------------------------------------------------------------
//	
TInt CUpnpCommand::Id() 
	{
	return iId;
	}

// -----------------------------------------------------------------------------
// CUpnpCommand::SessionId
// -----------------------------------------------------------------------------
//	
TInt CUpnpCommand::SessionId() 
	{
	return iSessionId;
	}

// -----------------------------------------------------------------------------
// CUpnpCommand::ReadDes8FromMessageLC
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpCommand::ReadDes8FromMessageLC(TInt aIndex) 
	{
	HBufC8* result = NULL;
    TInt length = iMessage.GetDesLength( aIndex );	    
    result = HBufC8::NewLC(length);                             
   	TPtr8 resultPtr = result->Des();
    iMessage.ReadL( aIndex, resultPtr );     
	return result;	
	}
 
// -----------------------------------------------------------------------------
// CUpnpCommand::ReadDes16FromMessageLC
// -----------------------------------------------------------------------------
//        
HBufC* CUpnpCommand::ReadDes16FromMessageLC(TInt aIndex) 
	{
	HBufC* result = NULL;
    TInt length = iMessage.GetDesLength( aIndex );	    
    result = HBufC::NewLC(length);                             
   	TPtr resultPtr = result->Des();
    iMessage.ReadL( aIndex, resultPtr );     
	return result;
	}
       
// -----------------------------------------------------------------------------
// CUpnpCommand::RunError
// -----------------------------------------------------------------------------
//        
void CUpnpCommand::RunError(TInt /*aErrorCode*/)
	{
    if (iMessage.Handle() != 0)
    	iMessage.Complete(KErrAbort);
	delete this;
	}        

//  End of File
