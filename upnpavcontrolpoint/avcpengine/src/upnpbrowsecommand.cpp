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
* Description:  CUpnpBrowseCommand
*
*/


#include "upnpbrowsecommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"

#include "upnphttpmessagefactory.h"
#include "upnpstring.h"
#include "upnpcons.h"

#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpBrowseCommand* CUpnpBrowseCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpBrowseCommand* self = new( ELeave ) CUpnpBrowseCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	
  
// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//  
void CUpnpBrowseCommand::ConstructL() 
	{
	CUpnpCommand::BaseConstructL();
	} 

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::CUpnpBrowseCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
CUpnpBrowseCommand::CUpnpBrowseCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::~CUpnpBrowseCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpBrowseCommand::~CUpnpBrowseCommand()
	{
    delete iObjectId;
    delete iObjectName;
    iResult.Reset();
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpBrowseCommand::SessionId() 
	{
    return CUpnpCommand::SessionId();   
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::ExecuteL()
	{
	//ASSERT(iType == EPrepareDirList || iType == EDirAttributes || iType == EFileAttributes);
    
    HBufC8* uuid = NULL;
    HBufC8* path = NULL;
    if (iType == EPrepareDirList) 
    	{
    	DEBUGSTRING(("Execute Browse command (dir listing)"));
    	// set command id
        TPckg<TInt> numPckg(iId);               
        iMessage.WriteL(0, numPckg);            
        
        uuid = ReadDes8FromMessageLC(2);    	            
        DEBUGSTRING8(("  uuid: %S",uuid));	
    	path = ReadDes8FromMessageLC(3);
    	DEBUGSTRING8(("  path: %S",path));	    	    	    		    	
    	}
    else 
    	{    	
    	DEBUGSTRING(("Execute Browse command (attributes)"));
    	uuid = ReadDes8FromMessageLC(0);
    	DEBUGSTRING8(("  uuid: %S",uuid));	    	            	
    	path = ReadDes8FromMessageLC(1);
    	DEBUGSTRING8(("  path: %S",path));	      	    	
    	}
    
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));
    DEBUGSTRING(("Resolve path %S", path));	 
    iPathResolver->ResolveIdL(*path, *this);

	CleanupStack::PopAndDestroy(path);        
	CleanupStack::PopAndDestroy(uuid);          
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::SetResultL(const RMessage2& aMessage) 
	{
	//ASSERT(aMessage.Function() == EDownloadDirList);

     TInt size = iResult.Count();
     TFileInfo* dirs = new  TFileInfo[size];
     CleanupStack::PushL(TCleanupItem(CleanupArray, dirs));
      for (TInt i = 0; i < size; i++) 
      	{
         dirs[i] = iResult[i];
      	}
      TPtr8 result(reinterpret_cast<TUint8*>(dirs), sizeof(TFileInfo)*size, sizeof(TFileInfo)*size);          
      aMessage.WriteL(1, result);  
	  CleanupStack::PopAndDestroy(dirs);
	}
	
// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::CleanupArray
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::CleanupArray(TAny* aDirs)
    {
    TFileInfo* dirs = (TFileInfo*) aDirs ;
    delete[] dirs;
    }


// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::SetAttForObject
// -----------------------------------------------------------------------------
// 
TBool CUpnpBrowseCommand::SetAttForObject(RArray<TFileInfo>& aList, const TDesC8& aName)
	{
	TBool result(EFalse);
    for (TInt i = 0; i < aList.Count(); i++) 
    	{
        if (aList[i].iName.Compare(aName) == 0) 
        	{   
            iAttr = aList[i]; 
            result = ETrue;
            break;   
        	}
    	}
	return result;    	
	}
	
// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::SendHeadRequestL
// -----------------------------------------------------------------------------
//
void CUpnpBrowseCommand::SendHeadRequestL(const TDesC8& aUrl) 
	{
	DEBUGSTRING8(("Send HTTP HEAD request for %S", &aUrl));
	
	HBufC8* resURIencoded = UpnpString::StringReplaceL(aUrl, _L8(" "), _L8("%20"));    			

	// we want wait for HEAD to get more info
	CleanupStack::PushL(resURIencoded);
	CUpnpHttpMessage* msg = RUpnpHttpMessageFactory::HttpHeadL( *resURIencoded );
	CleanupStack::PopAndDestroy(resURIencoded);

	CleanupStack::PushL( msg );    	            

	iAVCPManager.SendL(msg);  
	iSessionId = msg->SessionId();
	iAVCPManager.RegisterForHttp(*this);         

	CleanupStack::PopAndDestroy( msg ); 
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::InterpretL(TInt aErrorCode, CUpnpAction* aAction) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));
    if (aErrorCode == EHttp200Ok ) 
    	{

        TInt startIndex = StrToIntL( aAction->ArgumentValue( KStartingIndex ) );         
        TInt requestCount = StrToIntL( aAction->ArgumentValue( KRequestedCount ) );         
        
        TInt numberReturned = StrToIntL( aAction->ArgumentValue( KNumberReturned ) ); 
        TInt totalMatches = StrToIntL( aAction->ArgumentValue( KTotalMatches ) );         
                                        
        HBufC8* resURI = NULL;

        if (iType == EFileAttributes) 
        	{
        	iPathResolver->ParseBrowseResponseL( aAction->ArgumentValue( KResult ), &iResult, *iObjectId, &resURI);        	
        	}
        else 
        	{
        	iPathResolver->LockReset();
         	iPathResolver->ParseBrowseResponseL( aAction->ArgumentValue( KResult ), &iResult);
        	}

        if (iType == EPrepareDirList)  
        	{            
        	
            if ( startIndex+numberReturned < totalMatches)  
            	{
                iStartIndex +=  KRequestedCountLimit;
               	iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(),*iObjectId, KBrowseDirectChildren, KDefaultBrowseFilter, iStartIndex, KRequestedCountLimit, KNullDesC8);                
                iAVCPManager.RegisterForAction(*this);
            	}
            else 
            	{
            	iPathResolver->UnlockReset();
                TInt num(iResult.Count());                                    
                TPckg<TInt> numPckg(num);       
                // size of directory list
                iMessage.WriteL(1, numPckg); 
                iMessage.Complete(KErrNone);
                // don't destroy and wait for next command
            	}
        	}
        else 
        	{            
            if (SetAttForObject(iResult, *iObjectName))
            	{
                if (iType == EFileAttributes && resURI) 
                	{
                	CleanupStack::PushL(resURI);
                	SendHeadRequestL(*resURI);
                	CleanupStack::PopAndDestroy(resURI);                	                 
                	resURI = NULL;
                	}
                else 
                	{                        
                    TPckg<TFileInfo> attrPckg(iAttr); 
                    iMessage.WriteL(2,attrPckg);
                    iMessage.Complete(KErrNone);                     
                    delete this;                  
                	}
            	}
            else 
            	{
                iMessage.Complete(KErrNotFound);
				delete resURI;
                delete this;   
            	}
        	}       
    	}
    else 
    	{
    	iPathResolver->UnlockReset();
        iMessage.Complete(KErrAbort); 
        delete this;
    	}  
     
	}

// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpBrowseCommand::InterpretL(TInt aErrorCode, CUpnpHttpMessage* aMessage)  
	{
	DEBUGSTRING(("HTTP Interpret %d", aErrorCode));
      // transfer pending   
    if (aErrorCode == KHttpGetStarted) 
    	{
      	return;
    	}    
    iAVCPManager.UnregisterForHttp(*this);     
    if (aErrorCode == EHttp200Ok || aErrorCode == EHttpPartialContent) 
    	{
		// set some attrs
		iAttr.iMimeType = aMessage->GetHeaderValue(UpnpHTTP::KHdrContentType);  
		TDesC8& size = aMessage->GetHeaderValue(UpnpHTTP::KHdrContentLength);  
		TLex8 intLex( size );                			    	
		intLex.Val( iAttr.iSize );
	    }
    
	TPckg<TFileInfo> attrPckg(iAttr); 
	iMessage.WriteL(2,attrPckg);       
	iMessage.Complete(KErrNone);
	delete this; 
	}
    
// -----------------------------------------------------------------------------
// CUpnpBrowseCommand::ResolvedId
// -----------------------------------------------------------------------------
//     
void CUpnpBrowseCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{
    DEBUGSTRING(("ResolvedId %d", aErrCode));	 
    if (aErrCode == KErrNone)  
    	{    	
        iObjectId = aIdElement->Id().AllocL();
        iObjectName = aIdElement->Name().AllocL();
        DEBUGSTRING(("Resolved object %S %S", iObjectId, iObjectName));
        if (iType == EPrepareDirList) 
        	{            
            iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(), *iObjectId, KBrowseDirectChildren, KDefaultBrowseFilter, iStartIndex, KRequestedCountLimit, KNullDesC8);
        	}
        else 
        	{
            iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(), *iObjectId, KBrowseMetadata, KDefaultBrowseFilter, 0, 0, KNullDesC8);    
        	}        
        iAVCPManager.RegisterForAction(*this);        
    	}
    else 
    	{
        iMessage.Complete(KErrNotFound);
        delete this;
    	}
	}
// End of File
