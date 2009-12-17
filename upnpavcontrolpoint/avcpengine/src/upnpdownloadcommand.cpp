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
* Description:  CUpnpDownloadCommand
*
*/


#include "upnpdownloadcommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"
#include "upnphttpmessagefactory.h"

#include "upnpstring.h"

#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;

_LIT8(KFilterFlag, "res,res@protocolInfo");

// Operation option flags
const TUint KRemoteAccessOptionGetToStartOfFile = 0x01;

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpDownloadCommand* CUpnpDownloadCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpDownloadCommand* self = new( ELeave ) CUpnpDownloadCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//   
void CUpnpDownloadCommand::ConstructL() 
	{
	CUpnpCommand::BaseConstructL();
	} 

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::CUpnpDownloadCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//     
CUpnpDownloadCommand::CUpnpDownloadCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::~CUpnpDownloadCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpDownloadCommand::~CUpnpDownloadCommand()
	{
    delete iObjectId;   
    delete iDestPath;
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpDownloadCommand::SessionId() 
	{
    return CUpnpCommand::SessionId();   
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}
	
// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::ExecuteL()
	{
	//ASSERT(iType == EGetFile);
    
    HBufC8* uuid = NULL;
    HBufC8* srcPath = NULL;
    HBufC* destPath = NULL;    
            
    DEBUGSTRING(("Execute Download command "));
    	    
    uuid = ReadDes8FromMessageLC(0); 
    DEBUGSTRING8(("  uuid: %S",uuid));    
	srcPath = ReadDes8FromMessageLC(1);     
	DEBUGSTRING8(("  srcPath: %S",srcPath));                    	
	destPath = ReadDes16FromMessageLC(2); 		
	DEBUGSTRING16(("  destPath: %S",destPath));    

    TPckg<TFilePosition> posPkg(iPosition);
	iMessage.ReadL( 3,posPkg );    
	
    // convert to 8-bit representation 
    iDestPath = HBufC8::NewL(destPath->Length()); 
    iDestPath->Des().Copy(*destPath);
            
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));
    	 
    iPathResolver->ResolveIdL(*srcPath, *this);
    
    
    CleanupStack::PopAndDestroy(destPath);          
    CleanupStack::PopAndDestroy(srcPath);          
	CleanupStack::PopAndDestroy(uuid);          
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::SetResultL(const RMessage2& /*aMessage*/) 
	{
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::InterpretL(TInt aErrorCode, CUpnpAction* aAction) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));	
    HBufC8* resURI = NULL;
    if (aErrorCode == EHttp200Ok && 
        iPathResolver->GetResUriL( aAction->ArgumentValue( KResult ), *iObjectId, resURI)) 
    	{    	    	    	    	
    
    	DEBUGSTRING8(("Send HTTP GET request for %S", resURI));
    	// encode URI
    	CleanupStack::PushL(resURI);
    	HBufC8* resURIencoded = UpnpString::StringReplaceL(*resURI, _L8(" "), _L8("%20"));    	
    	CleanupStack::PopAndDestroy(resURI);
    	
        CleanupStack::PushL(resURIencoded);
        CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpGetL( *resURIencoded );
        CleanupStack::PopAndDestroy(resURIencoded);
        
	    CleanupStack::PushL( msg );
		msg->SetInFilenameL( *iDestPath, ETrue );		
    	msg->SetHttpPriority( EPriorityLess );
    	
    	if (iPosition.iStart != 0 || iPosition.iLength != 0 ) 
    		{
    		DEBUGSTRING8(("  Use ramge header. From %d to %d (flag:%d) ", 
    			iPosition.iStart, iPosition.iLength, iPosition.iFlags));
    		iUseRange = ETrue;
    		if (iPosition.iFlags == KRemoteAccessOptionGetToStartOfFile)
    			msg->SetRangeL(iPosition.iStart, iPosition.iLength, ETrue);
    		else 
    			msg->SetRangeL(iPosition.iStart, iPosition.iLength, EFalse);    	    
    		}	
    	
        iAVCPManager.SendL(msg);  
        iSessionId = msg->SessionId();
        iAVCPManager.RegisterForHttp(*this);         
        
        CleanupStack::PopAndDestroy( msg );              
    	}
    else 
    	{
    	DEBUGSTRING8(("   No resouse URI found"));
        iMessage.Complete(KErrAbort); 
        delete this;
    	}   
	}

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::InterpretL(TInt aErrorCode, CUpnpHttpMessage* /*aMessage*/) 
	{
	DEBUGSTRING(("HTTP Interpret %d", aErrorCode));	
    if (aErrorCode == KHttpGetStarted) 
    	{
      	// transfer pending   
      	return;
    	}
    iAVCPManager.UnregisterForHttp(*this); 
    if (aErrorCode == EHttp200Ok || aErrorCode == EHttpPartialContent) 
    	{
    	if (iUseRange && aErrorCode == EHttp200Ok) 
    		{
    		iPosition.iLength = 0;
    	    TPckg<TFilePosition> posPkg(iPosition);
			iMessage.WriteL( 3,posPkg );
    		}
       	iMessage.Complete(KErrNone); 
       	delete this;
    	}
    else  
    	{
       	iMessage.Complete(KErrAbort);         
       	delete this;
    	}    
	}	

// -----------------------------------------------------------------------------
// CUpnpDownloadCommand::ResolvedId
// -----------------------------------------------------------------------------
// 
void CUpnpDownloadCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{
    DEBUGSTRING(("ResolvedId %d", aErrCode));    
    if (aErrCode == KErrNone)  
    	{
        iObjectId = aIdElement->Id().AllocL();
                    
        iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(), *iObjectId, KBrowseMetadata,KFilterFlag , 0, 0, KNullDesC8);    
      
        iAVCPManager.RegisterForAction(*this);        
    	}
    else 
    	{
        iMessage.Complete(KErrNotFound);
        delete this;
    	}
};

//End of File
