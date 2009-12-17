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
* Description:  CUpnpDeleteCommand
*
*/


#include "upnpdeletecommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"


#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;


// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDeleteCommand* CUpnpDeleteCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpDeleteCommand* self = new( ELeave ) CUpnpDeleteCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//   
void CUpnpDeleteCommand::ConstructL() 
	{
 	CUpnpCommand::BaseConstructL();
 	} 

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::CUpnpDeleteCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//       
CUpnpDeleteCommand::CUpnpDeleteCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::~CUpnpDeleteCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpDeleteCommand::~CUpnpDeleteCommand()
	{    
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpDeleteCommand::SessionId() 
	{
    return CUpnpCommand::SessionId();   
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpDeleteCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}
	
// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpDeleteCommand::ExecuteL()
	{
	//ASSERT(iType == EDeleteDirectory || iType == EDeleteFile );
    
    HBufC8* uuid = NULL;
    HBufC8* path = NULL;       
    DEBUGSTRING(("Execute Delete command"));
            	
    uuid = ReadDes8FromMessageLC(0);    
    DEBUGSTRING8(("  uuid: %S",uuid)); 	            	
    path = ReadDes8FromMessageLC(1);       	         	    		  
    DEBUGSTRING8(("  path: %S",path)); 
    
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));
    	 
    iPathResolver->ResolveIdL(*path, *this);

	CleanupStack::PopAndDestroy(path);        
	CleanupStack::PopAndDestroy(uuid);          
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpDeleteCommand::SetResultL(const RMessage2& /*aMessage*/) 
	{
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpDeleteCommand::InterpretL(TInt aErrorCode, CUpnpAction* /*aAction*/) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));	
    if (aErrorCode == EHttp200Ok) 
    	{
		iMessage.Complete(KErrNone); 		            
    	}
    else 
    	{
        iMessage.Complete(KErrAbort); 
    	}   
	delete this;    	
	}

// -----------------------------------------------------------------------------
// CUpnpDeleteCommand::ResolvedId
// -----------------------------------------------------------------------------
// 
void CUpnpDeleteCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{    
    if (aErrCode == KErrNone)  
    	{        
    	//remove from remote MS
        iSessionId = iAVCPManager.CdsDestroyObjectActionL(iPathResolver->UUID(), aIdElement->Id());    
        iAVCPManager.RegisterForAction(*this);              
    	// remove from local cache
    	iPathResolver->RemoveElementD(aIdElement);     	
    	}
    else 
    	{
        iMessage.Complete(KErrNotFound);
        delete this;
    	}
	}
