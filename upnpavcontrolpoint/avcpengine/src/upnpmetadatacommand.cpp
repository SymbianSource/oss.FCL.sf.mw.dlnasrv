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
* Description:  CUpnpMetadataCommand
*
*/


#include "upnpmetadatacommand.h"

#include "upnppathresolver.h"
#include "upnppathelement.h"
#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"

#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;

#include "upnpstring.h"


_LIT8(KFilterFlag, "*");

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMetadataCommand* CUpnpMetadataCommand::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpAVCPEngineSession& aSession, const RMessage2& aMessage)
	{
    CUpnpMetadataCommand* self = new( ELeave ) CUpnpMetadataCommand(aAVCPManager, aSession, aMessage);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;   
	}	

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//     
void CUpnpMetadataCommand::ConstructL() 
	{
 	CUpnpCommand::BaseConstructL();
	} 

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::CUpnpMetadataCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------   
//       
CUpnpMetadataCommand::CUpnpMetadataCommand(CUpnpAVCPManager& aAVCPManager, 
                   CUpnpAVCPEngineSession& aSession, 
                   const RMessage2& aMessage):
                   CUpnpCommand( aAVCPManager,
                   		     aSession, 
                   		     aMessage)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::~CUpnpMetadataCommand
// desctructor
// -----------------------------------------------------------------------------
// 
CUpnpMetadataCommand::~CUpnpMetadataCommand()
	{
    delete iObjectId;
    delete iResult;
    delete iOldMetadata;    
    delete iNewMetadata;    
	}

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::SessionId
// -----------------------------------------------------------------------------
// 
TInt CUpnpMetadataCommand::SessionId() 
{
    return CUpnpCommand::SessionId();   
}

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::RunError
// -----------------------------------------------------------------------------
// 
void CUpnpMetadataCommand::RunError(TInt aErrorCode) 
	{
    return CUpnpCommand::RunError(aErrorCode);   
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::ExecuteL
// -----------------------------------------------------------------------------
// 
void CUpnpMetadataCommand::ExecuteL()
	{
	//ASSERT(iType == EPrepareMetadata || iType == ESetMetadata);
    
    HBufC8* uuid = NULL;
    HBufC8* path = NULL;
    if (iType == EPrepareMetadata) 
    	{
    	DEBUGSTRING(("Execute Metadata command (EPrepareMetadata)"));    	
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
    	DEBUGSTRING(("Execute Metadata command (ESetMetadata)"));
		uuid = ReadDes8FromMessageLC(0);          
		DEBUGSTRING8(("  uuid: %S",uuid));		           	
    	path = ReadDes8FromMessageLC(1);
    	DEBUGSTRING8(("  path: %S",path));
    	
    	HBufC8* old =  ReadDes8FromMessageLC(2);
		iOldMetadata = UpnpString::EncodeXmlStringL(old);  		
		CleanupStack::PopAndDestroy( old );
		
		HBufC8* newXML =  ReadDes8FromMessageLC(3);
		iNewMetadata = UpnpString::EncodeXmlStringL( newXML );  		
		CleanupStack::PopAndDestroy( newXML);		  	  	    	    
    	}
    
    iPathResolver = &(iAVCPManager.PathResolverL(*uuid, &iSession));
    	 
    iPathResolver->ResolveIdL(*path, *this);

	CleanupStack::PopAndDestroy(path);        
	CleanupStack::PopAndDestroy(uuid);          
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::SetResultL
// -----------------------------------------------------------------------------
// 
void CUpnpMetadataCommand::SetResultL(const RMessage2& aMessage) 
	{
	//ASSERT(aMessage.Function() == EDownloadMetadata);
	if (iResult) 
		{        	        
        TPtr8 result = iResult->Des();
        aMessage.WriteL(1, result);          
		}
	}

// -----------------------------------------------------------------------------
// CUpnpMetadataCommand::Interpret
// -----------------------------------------------------------------------------
// 
void CUpnpMetadataCommand::InterpretL(TInt aErrorCode, CUpnpAction* aAction) 
	{
	DEBUGSTRING(("Interpret action response %d", aErrorCode));	
    if (aErrorCode == EHttp200Ok) 
    	{

        if (iType == EPrepareMetadata) 
        	{
            iResult = aAction->ArgumentValue( KResult ).AllocL();       
            TInt num = iResult->Length();                                    
            TPckg<TInt> numPckg(num);       
            // size of  metadata information
            iMessage.WriteL(1, numPckg); 
            iMessage.Complete(KErrNone);
            // don't destroy and wait for next command           
        	}
        else 
        	{
            // for set metadata
            iMessage.Complete(KErrNone);  
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
// CUpnpMetadataCommand::ResolvedId
// -----------------------------------------------------------------------------
// 
void CUpnpMetadataCommand::ResolvedIdL(TInt aErrCode, CUpnpPathElement* aIdElement)
	{
    
    if (aErrCode == KErrNone)  
    	{
        iObjectId = aIdElement->Id().AllocL();       
        if (iType == EPrepareMetadata) 
        	{            
            iSessionId = iAVCPManager.CdsBrowseActionL(iPathResolver->UUID(), *iObjectId, KBrowseMetadata, KFilterFlag, 0, 0, KNullDesC8);                
        	}
        else //set metadata 
        	{            
            iSessionId = iAVCPManager.CdsUpdateObjectActionL(iPathResolver->UUID(), *iObjectId, *iOldMetadata, *iNewMetadata);    
            delete iOldMetadata;
            iOldMetadata = NULL;
            delete iNewMetadata;
            iNewMetadata = NULL;
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
