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
* Description:  CUpnpUploadCommand
*
*/


#include "upnpuploadcommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"
#include "upnphttpmessagefactory.h"

#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;


_LIT8(KBrowseFilter, "res,res@protocolInfo,res@importUri");

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpUploadCommand* CUpnpUploadCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpUploadCommand* self = new( ELeave ) CUpnpUploadCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpUploadCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//    
void CUpnpUploadCommand::ConstructL() 
	{
	CUpnpCommand::BaseConstructL();
	} 

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::CUpnpUploadCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//     
CUpnpUploadCommand::CUpnpUploadCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::~CUpnpUploadCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpUploadCommand::~CUpnpUploadCommand()
	{
    delete iObjectId;
    delete iImportURI;   
    delete iSrcPath;
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpUploadCommand::SessionId() 
	{
    return CUpnpCommand::SessionId();   
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}
	
// -----------------------------------------------------------------------------
// CUpnpUploadCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::ExecuteL()
	{
	//ASSERT(iType == EPutFile);
    
    HBufC8* uuid = NULL;
    HBufC8* dstPath = NULL;
    HBufC* srcPath = NULL;

   	DEBUGSTRING(("Execute Upload command"));
    	            
    uuid = ReadDes8FromMessageLC(0); 
    DEBUGSTRING8(("  uuid: %S",uuid));       
	srcPath = ReadDes16FromMessageLC(1);                     	
	DEBUGSTRING16(("  srcPath: %S",srcPath));   			
	dstPath = ReadDes8FromMessageLC(2); 
	DEBUGSTRING8(("  dstPath: %S",dstPath));   		

    // convert to 8-bit representation 
    iSrcPath = HBufC8::NewL(srcPath->Length()); 
    iSrcPath->Des().Copy(*srcPath);
                      
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));
    	 
    iPathResolver->ResolveIdL(*dstPath, *this);
    
    CleanupStack::PopAndDestroy(dstPath);          
    CleanupStack::PopAndDestroy(srcPath);          
	CleanupStack::PopAndDestroy(uuid);          
	}

// -----------------------------------------------------------------------------
// CCUploadCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::SetResultL(const RMessage2& /*aMessage*/) 
	{
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::InterpretL(TInt aErrorCode, CUpnpAction* /*aAction*/) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));	
   
    if (aErrorCode == EHttp200Ok)
    	{    	
    	if( iImportURI) 
    		{    	    	        
        	CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpPostL( *iImportURI );       
	    	CleanupStack::PushL( msg );
			msg->SetOutFilenameL( *iSrcPath );		    	    			
        	iAVCPManager.SendL(msg);  
        	iSessionId = msg->SessionId();
        	iAVCPManager.RegisterForHttp(*this);         
        
        	CleanupStack::PopAndDestroy( msg );              
    		}
    	else
    		{
    		iMessage.Complete(KErrPathNotFound); 
        	delete this;	
    		}
    	}
    else 
    	{
        iMessage.Complete(KErrAbort); 
        delete this;
    	}   
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::InterpretL(TInt aErrorCode, CUpnpHttpMessage* /*aMessage*/) 
	{
	DEBUGSTRING(("HTTP Interpret %d", aErrorCode));	
    if (aErrorCode == KHttpPostStarted) 
    	{
      	// transfer pending   
      	return;
    	}
    iAVCPManager.UnregisterForHttp(*this); 
    if (aErrorCode == EHttp200Ok) 
    	{
    	iMessage.Complete(KErrNone);
    	delete this; 
    	}
    else  
    	{
       	iMessage.Complete(KErrNotFound);         
       	delete this;
    	}    
	}

// -----------------------------------------------------------------------------
// CUpnpUploadCommand::ResolvedId
// -----------------------------------------------------------------------------
// 
void CUpnpUploadCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{
    
    if (aErrCode == KErrNone)  
    	{
        iObjectId = aIdElement->Id().AllocL();
        iImportURI = (aIdElement->ImportURI()!= KNullDesC8()) ? aIdElement->ImportURI().AllocL(): NULL;
            
        iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(), *iObjectId, KBrowseMetadata, KBrowseFilter, 0, 0, KNullDesC8);    
      
        iAVCPManager.RegisterForAction(*this);        
    	}
    else 
    	{
        iMessage.Complete(KErrNotFound);
        delete this;
    	}
	}
