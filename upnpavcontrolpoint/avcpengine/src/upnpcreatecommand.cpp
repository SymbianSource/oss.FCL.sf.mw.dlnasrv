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
* Description:  CUpnpCreateCommand
*
*/


#include "upnpcreatecommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"

#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;
#include "upnpstring.h"


_LIT8(KCreateFileTemplate, 
    "<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \
        xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" \
        xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\"> \
	        <item  parentID=\"%S\" restricted=\"false\"> \
	            <dc:title>%S</dc:title> \
	            <upnp:class>object.item</upnp:class> \
	            <res protocolInfo=\"*:*:*:*\"></res> \
            </item> \
        </DIDL-Lite>");
        
_LIT8(KCreateDirTemplate,     
"<DIDL-Lite xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \
        xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\" \
        xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\"> \
	        <container parentID=\"%S\" restricted=\"false\"> \
	            <dc:title>%S</dc:title> \
	            <upnp:class>object.container</upnp:class> \
            </container> \
        </DIDL-Lite>");


// -----------------------------------------------------------------------------
// CUpnpCreateCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCreateCommand* CUpnpCreateCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpCreateCommand* self = new( ELeave ) CUpnpCreateCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpCreateCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//   
void CUpnpCreateCommand::ConstructL() 
	{
 	CUpnpCommand::BaseConstructL();
	} 

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::CUpnpCreateCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//       
CUpnpCreateCommand::CUpnpCreateCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::~CUpnpCreateCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpCreateCommand::~CUpnpCreateCommand()
	{
    delete iObjectId;
    delete iTargetName;
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpCreateCommand::SessionId() 
	{
    return CUpnpCommand::SessionId();   
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpCreateCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}
	
// -----------------------------------------------------------------------------
// CUpnpCreateCommand::ExecuteL
// -----------------------------------------------------------------------------
//
void CUpnpCreateCommand::ExecuteL()
	{
	//ASSERT(iType == ECreateDirectory || iType == ECreateFile );
    
    HBufC8* uuid = NULL;
    HBufC8* path = NULL;       
    DEBUGSTRING(("Execute Create command "));        	
    uuid = ReadDes8FromMessageLC(0);    	            	
    DEBUGSTRING8(("  uuid: %S",uuid));	    
    path = ReadDes8FromMessageLC(1);       	            	  
    DEBUGSTRING8(("  path: %S",path));
    
	TPtr8 pathPtr = path->Des();	  
	// removes also last element of path  
    iTargetName = GetLastPathElementL(pathPtr);
    
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));    
    iPathResolver->ResolveIdL(*path, *this);    

	CleanupStack::PopAndDestroy(path);        
	CleanupStack::PopAndDestroy(uuid);          
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::SetResultL
// -----------------------------------------------------------------------------
//
void CUpnpCreateCommand::SetResultL(const RMessage2& /*aMessage*/) 
	{
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::Interpret
// -----------------------------------------------------------------------------
//
void CUpnpCreateCommand::InterpretL(TInt aErrorCode, CUpnpAction* aAction) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));	
    if (aErrorCode == EHttp200Ok )    	 
    	{    	 
        if (iType == ECreateDirectory)
        	{
        	iMessage.Complete(KErrNone);
        	}
        else
        	{
        	if(iPathResolver->BrowseCreateObjectL( aAction->ArgumentValue( KResult )))
        		{
        		iMessage.Complete(KErrNone);            
        		}
        	else
        		{
        		iMessage.Complete(KErrAbort);
        		}       
        	}
        	
    	}
    else 
    	{
        iMessage.Complete(KErrAbort); 
    	}  
    delete this;
	}

// -----------------------------------------------------------------------------
// CUpnpCreateCommand::ResolvedId
// -----------------------------------------------------------------------------
//
void CUpnpCreateCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{       
    if (aErrCode == KErrNone)  
    	{
    	iElement = aIdElement;
        iObjectId = aIdElement->Id().AllocL();
        HBufC8* createMsg;
        if (iType == ECreateDirectory )  
        	{            
            createMsg = HBufC8::NewLC(KCreateDirTemplate().Length() + iObjectId->Length() +  iTargetName->Length());
            createMsg->Des().AppendFormat(KCreateDirTemplate, iObjectId, iTargetName);
        	}
        else 
        	{
            createMsg = HBufC8::NewLC(KCreateFileTemplate().Length() + iObjectId->Length() +  iTargetName->Length());                        
            createMsg->Des().AppendFormat(KCreateFileTemplate, iObjectId, iTargetName);
        	}
        HBufC8* encoded = UpnpString::EncodeXmlStringL( createMsg );
        CleanupStack::PopAndDestroy(createMsg);
	    CleanupStack::PushL(encoded);	      
        
        iSessionId = iAVCPManager.CdsCreateObjectActionL(iPathResolver->UUID(), *iObjectId, *encoded);          
        iAVCPManager.RegisterForAction(*this);
        
        CleanupStack::PopAndDestroy(encoded);	         
        
    	}
    else 
    	{
        iMessage.Complete(KErrNotFound);
        delete this;
    	}
    
	}
