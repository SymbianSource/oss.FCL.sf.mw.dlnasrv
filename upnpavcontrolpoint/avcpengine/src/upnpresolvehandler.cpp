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
* Description:  CUpnpResolveHandler
*
*/


#include "upnpresolvehandler.h"
#include "upnppathresolver.h"
#include "upnppathelement.h"

#include "upnpavcpmanager.h"
#include "upnpavcontrolpoint.h"


#include "upnpstring.h"

#include "upnpavcpenginehelper.h"
#include "upnpmdebug.h"
using namespace UpnpAVCPEngine;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpResolveHandler* CUpnpResolveHandler::NewL(CUpnpAVCPManager& aAVCPManager, CUpnpPathResolver& aResolver, MUpnpResolverObserver& aObserver) 
	{
    CUpnpResolveHandler* self = new( ELeave ) CUpnpResolveHandler(aAVCPManager,aResolver, aObserver);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;    
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::CUpnpResolveHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpResolveHandler::CUpnpResolveHandler(CUpnpAVCPManager& aAVCPManager,
                            CUpnpPathResolver& aResolver, 
                            MUpnpResolverObserver& aObserver):
                            iAVCPManager(aAVCPManager),
                            iResolver(aResolver),
                            iObserver(aObserver),
                            iPathIndex(-1)    
	{
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::~CUpnpResolveHandler
// -----------------------------------------------------------------------------
//
CUpnpResolveHandler::~CUpnpResolveHandler()
	{    
	iResolver.RemoveHandler(this);
	
	delete iPath;
	delete iLastId;
	iPathElements.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpResolveHandler::ConstructL() 
	{
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::SessionId
// -----------------------------------------------------------------------------
//
TInt CUpnpResolveHandler::SessionId() 
	{
    return iSessionId;    
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::ResolveL
// -----------------------------------------------------------------------------
//
void CUpnpResolveHandler::ResolveL(const TDesC8& aPath) 
	{

    delete iPath;
    iPath = NULL;   

    iPath = aPath.AllocL();
    DEBUGSTRING8(("\tCResolveHandler::ResolveL %S", iPath));

    UpnpAVCPEngine::ParsePathToElementsL(iPath->Des(),iPathElements);
    
    DEBUGSTRING8(("\tCount of path elements %d", iPathElements.Count()));
    if (iPathElements.Count() == 0 || (iPathElements.Count() == 1 && iPathElements[0]->Length() == 0))	 
    	{
        CUpnpPathElement* elem = CUpnpPathElement::NewL();
        CleanupStack::PushL(elem);
        elem->SetIdL(KRootId());  
        elem->SetNameL(KRootName());  
       
        iObserver.ResolvedIdL(KErrNone, elem);
        CleanupStack::PopAndDestroy(elem);
        delete this;
    	}
    else 
    	{
        iPathIndex = 0;
        iLastId = KRootId().AllocL();
        iResolver.LockReset();
        CheckNextElementL();
    	}
	}


// -----------------------------------------------------------------------------
// CUpnpResolveHandler::CheckNextElementL
// -----------------------------------------------------------------------------
//
void CUpnpResolveHandler::CheckNextElementL(TBool aSendAction) 
	{	
	DEBUGSTRING8(("\tCheckNextElementL %d", aSendAction));
    CUpnpPathElement* element = iResolver.GetElementL(*iLastId,*iPathElements[iPathIndex]);
    
    if (element) 
    	{
    	
        iPathIndex++;
                     
        delete iLastId;
        iLastId = NULL;
        
        iLastId = element->Id().AllocL();
        DEBUGSTRING8(("\tFind element for index  %d with id %S", iPathIndex,iLastId));
        if (iPathIndex == iPathElements.Count() ) 
        	{
            iObserver.ResolvedIdL(KErrNone, element);  
            iResolver.UnlockReset();
            delete this; 
        	}
        else 
        	{
            CheckNextElementL();
        	}
    	}	 
    else if (aSendAction) 
    	{
        DEBUGSTRING8(("\tSend browse action for %S", iLastId));
        iSessionId = iAVCPManager.CdsBrowseActionL(iResolver.UUID(),*iLastId, KBrowseDirectChildren, KDefaultBrowseFilter, iStartIndex, KRequestedCountLimit, KNullDesC8);                
        DEBUGSTRING8(("\tSend browse action for %S with session id %d", iLastId, iSessionId));
        iAVCPManager.RegisterForAction(*this);
    	}
    else 
    	{
        iObserver.ResolvedIdL(KErrNotFound, NULL); 
        iResolver.UnlockReset();
        delete this;
    	}
    
	}

// -----------------------------------------------------------------------------
// CUpnpResolveHandler::Interpret
// -----------------------------------------------------------------------------
//
void CUpnpResolveHandler::InterpretL(TInt aErrorCode, CUpnpAction* aAction) 
	{
	DEBUGSTRING(("\tInterpret %d", aErrorCode));
    if (aErrorCode == EHttp200Ok) 
    	{

        TInt startIndex = StrToIntL( aAction->ArgumentValue( KStartingIndex ) );         
        TInt requestCount = StrToIntL( aAction->ArgumentValue( KRequestedCount ) );         
        
        TInt numberReturned = StrToIntL( aAction->ArgumentValue( KNumberReturned ) ); 
        TInt totalMatches = StrToIntL( aAction->ArgumentValue( KTotalMatches ) );         
                                                  
        iResolver.ParseBrowseResponseL(aAction->ArgumentValue( KResult ));  
        
        if ( startIndex+numberReturned < totalMatches)  
        	{
            iStartIndex +=  KRequestedCountLimit;
            iSessionId = iAVCPManager.CdsBrowseActionL(iResolver.UUID(),*iLastId, KBrowseDirectChildren, KDefaultBrowseFilter, iStartIndex, KRequestedCountLimit, KNullDesC8);                
            iAVCPManager.RegisterForAction(*this);
        	}
        else 
        	{
            CheckNextElementL(EFalse);
        	}
        
    	}
    else 
    	{
        iObserver.ResolvedIdL(KErrNotFound, NULL); 
        iResolver.UnlockReset();
        delete this;  
    	}            
	}
	
// -----------------------------------------------------------------------------
// CUpnpResolveHandler::RunError
// -----------------------------------------------------------------------------
//
void CUpnpResolveHandler::RunError(TInt aErrorCode) 
	{
	DEBUGSTRING(("\tRunError %d", aErrorCode));	
	TRAP_IGNORE( iObserver.ResolvedIdL(KErrNotFound, NULL) ); 
	delete this;
	}  	

//  End of File
