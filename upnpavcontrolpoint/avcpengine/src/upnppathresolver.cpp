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
* Description:  CUpnpPathResolver
*
*/



// INCLUDE FILES
#include    "upnppathresolver.h"
#include    "upnppathelement.h"
#include    "upnpresolvehandler.h"
#include    "upnpavcpmanager.h"

#include "upnpavcpenginecommon.h"
#include "upnpavcpenginehelper.h"
using namespace UpnpAVCPEngine;

#include "upnpstring.h"

#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdomparser.h>

#include "upnpdominterface.h"

_LIT8(KIndexFormat, "(%d)");

const static TInt KMaxElementsLimit = 1000;
        			    	    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpPathResolver::CUpnpPathResolver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpPathResolver::CUpnpPathResolver(CUpnpAVCPManager& aManager):
    iManager(aManager), iUptodate(ETrue)
    {
    iSystemId = -1;
    }

// -----------------------------------------------------------------------------
// CUpnpPathResolver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpPathResolver::ConstructL(const TDesC8& aUUID)
    {
    iUUID = aUUID.AllocL();
    iDOMImpl.OpenL();
    }

// -----------------------------------------------------------------------------
// CUpnpPathResolver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpPathResolver* CUpnpPathResolver::NewL(CUpnpAVCPManager& aManager, const TDesC8& aUUID)
    {
    CUpnpPathResolver* self = new( ELeave ) CUpnpPathResolver(aManager);
    
    CleanupStack::PushL( self );
    self->ConstructL(aUUID);
    CleanupStack::Pop( self );

    return self;
    }

 
// -----------------------------------------------------------------------------
// CUpnpPathResolver::~CUpnpPathResolver
// Destructor
// -----------------------------------------------------------------------------
//   
CUpnpPathResolver::~CUpnpPathResolver()
    {
	delete iUUID;
	iAVCPSessions.Reset();
	iPathElements.ResetAndDestroy(); 
	iResolveHandlers.ResetAndDestroy(); 
	iDOMImpl.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpPathResolver::UUID
// -----------------------------------------------------------------------------
//   
const TDesC8& CUpnpPathResolver::UUID() const
    {
	return *iUUID;                
    }
    
// -----------------------------------------------------------------------------
// CUpnpPathResolver::Match
// -----------------------------------------------------------------------------
//   
TBool CUpnpPathResolver::Match(const CUpnpPathResolver& aElement1, const CUpnpPathResolver& aElement2) 
	{
	return   (aElement1.UUID().Compare(aElement2.UUID()) == 0); 
	}    

// -----------------------------------------------------------------------------
// CUpnpPathResolver::RegisterSession
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::RegisterSession(CUpnpAVCPEngineSession* aSession) 
	{    
	TInt index = iAVCPSessions.Find(aSession); 
    if (index == KErrNotFound) 
	    {
	    iAVCPSessions.AppendL(aSession); 
	    }
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::UnregisterSession
// -----------------------------------------------------------------------------
//   
TInt CUpnpPathResolver::UnregisterSession(CUpnpAVCPEngineSession* aSession) 
	{
  	TInt index = iAVCPSessions.Find(aSession); 
    if (index != KErrNotFound) 
	    {
	    iAVCPSessions.Remove(index); 
	    }  
    return iAVCPSessions.Count();
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::AddElementL
// -----------------------------------------------------------------------------
//   
const TDesC8& CUpnpPathResolver::AddElementL(const TDesC8& aParentId, const TDesC8& aId, const TDesC8& aName, const TDesC8& aImportURI) 
	{
	// if we exceed max capacity for our cache, set to destroy all elements
	if (iPathElements.Count() > KMaxElementsLimit)
		iUptodate = EFalse;		
		
	
    CUpnpPathElement* elem = CUpnpPathElement::NewL();
    CleanupStack::PushL(elem);
    elem->SetParentIdL(aParentId);
    elem->SetIdL(aId);
    elem->SetNameL(aName);
    
    TIdentityRelation<CUpnpPathElement> matcher( CUpnpPathElement::MatchAll );
    // check if exist the same element
	TInt idx = iPathElements.Find( elem, matcher ); 
	
	if (idx != KErrNotFound)   
		{
		if(aImportURI != KNullDesC8())
			{
			iPathElements[idx]->SetImportURIL(aImportURI);
			}
	    CleanupStack::PopAndDestroy(elem);
	    return iPathElements[idx]->Name();
		}
	else 
		{
		// check name duplication
		TIdentityRelation<CUpnpPathElement> matcherName( CUpnpPathElement::MatchName );
	    TInt index = 0;
	    TInt dupIndex = iPathElements.Find( elem, matcherName ); 
	    
	    while (dupIndex != KErrNotFound)
	    	{
	    	HBufC8* newName = HBufC8::NewLC(aName.Length() + 10 /*integer*/);
	    	*newName = aName;
           	newName->Des().AppendFormat(KIndexFormat, ++index ); 	
           	// change to new name
           	elem->SetNameL(*newName); 
           	CleanupStack::PopAndDestroy( newName );
           	dupIndex = iPathElements.Find( elem, matcherName );
           	// check if exists the same element
           	idx = iPathElements.Find( elem, matcher ); 
           	if (idx != KErrNotFound)   
				{
				if(aImportURI != KNullDesC8())
					{
					iPathElements[idx]->SetImportURIL(aImportURI);
					}
	    		CleanupStack::PopAndDestroy(elem);
	    		return iPathElements[idx]->Name();
				}
	    	} 
	    if(aImportURI != KNullDesC8())
			{
			elem->SetImportURIL(aImportURI);
			}
	    CleanupStack::Pop(elem);
        iPathElements.AppendL(elem);
        return elem->Name();
		}
	  
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::GetElementId
// -----------------------------------------------------------------------------
//   
CUpnpPathElement*  CUpnpPathResolver::GetElementL(const TDesC8& aParentId, const TDesC8& aName) const 
	{
    CUpnpPathElement* elem = CUpnpPathElement::NewL();
    CleanupStack::PushL(elem);
    elem->SetNameL(aName);
    elem->SetParentIdL(aParentId);
    TIdentityRelation<CUpnpPathElement> matcher( CUpnpPathElement::MatchName );
	TInt idx = iPathElements.Find( elem, matcher ); 
	CleanupStack::PopAndDestroy(elem);	
	if (idx != KErrNotFound)   
		{
	   	return  iPathElements[idx];   
		}
	else 
		{
	    return NULL;
		}  
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::RemoveElement
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::RemoveElementD(CUpnpPathElement* aElement)  
	{
	TInt index = iPathElements.Find(aElement);
	if (index != KErrNotFound)   
		{
		iPathElements.Remove(index);
		delete aElement;
		} 
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::ResolveIdL
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::ResolveIdL(const TDesC8& aPath, MUpnpResolverObserver& aObserver) 
	{
	CUpnpResolveHandler* handler = CUpnpResolveHandler::NewL(iManager,*this, aObserver);	
	CleanupStack::PushL(handler);
	iResolveHandlers.AppendL(handler);
	handler->ResolveL(aPath);	
	CleanupStack::Pop(handler);	
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::RemoveHandler
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::RemoveHandler(CUpnpResolveHandler* aHandler) 
	{
	TInt index = iResolveHandlers.Find(aHandler); 
    if (index != KErrNotFound) 
    	{
        iResolveHandlers.Remove(index); 
    	}
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::LockReset
// -----------------------------------------------------------------------------
//  
void CUpnpPathResolver::LockReset() 
	{
	DEBUGSTRING8(("-> Obtain lock for resets")); 
	iLocked = ETrue;	
	}


// -----------------------------------------------------------------------------
// CUpnpPathResolver::UnlockReset
// -----------------------------------------------------------------------------
//  
void CUpnpPathResolver::UnlockReset() 	
	{
	DEBUGSTRING8(("<- Release lock for resets")); 
	iLocked = EFalse;
	if (!iUptodate) 
		{
 		DEBUGSTRING8(("Remove all path elements"));        	
       	iPathElements.ResetAndDestroy();  				
       	iUptodate = ETrue;
		}	
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::StateChangedL
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::StateChangedL(const TDesC8& aSystemId) 
	{
	
    TLex8 updateidLex( aSystemId );
    TInt systemUpdate;
    TInt err = updateidLex.Val( systemUpdate );
    if (err == KErrNone) 
    	{
    	DEBUGSTRING8(("StateChangedL (old: %d)->(new: %d) ", iSystemId,systemUpdate));  
        if (iSystemId != -1 && iSystemId != systemUpdate) 
        	{
        	if (!iLocked) 
        		{
	     		DEBUGSTRING8(("Remove all path elements"));        	
    	       	iPathElements.ResetAndDestroy();        		
        		}
        	else 
        		{        		
        		iUptodate = EFalse;
        		}
        	}
        iSystemId =  systemUpdate;
    	}            
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::SetModifiedTimeL
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::SetModifiedTimeL(TXmlEngElement& aDateElement, TFileInfo& aEntry) 
	{	
	if (aDateElement.Value().Length())
		{
		HBufC8* date = aDateElement.Value().AllocLC(); 
				
		TPtrC8 datePtr =  date->Des();
		
		TInt position = datePtr.Locate('-');
		TInt step = 0;
		TInt dateParts[3];
		while (position != KErrNotFound && step <3) 
			{
			TLex8 lex(datePtr.Left(position));
			TInt value(0) ;
			lex.Val(value);
			
			dateParts[step] = value;
			step++;					
		
			
			datePtr.Set(datePtr.Mid(position+1));							   	
		   	position = datePtr.Locate('-');
			}
		
		TLex8 lex(datePtr);
		TInt value(0) ;
		lex.Val(value);
		
		dateParts[step] = value;
		
		// implement data validation	
	    aEntry.iModified = TDateTime(dateParts[0], TMonth(dateParts[1] -1), dateParts[2] -1, 0, 0, 0, 0); 
	   	CleanupStack::PopAndDestroy(date);
		}
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::ParseBrowseResponse
// -----------------------------------------------------------------------------
//   
void CUpnpPathResolver::ParseBrowseResponseL(const TDesC8& aResponse, RArray<TFileInfo>* aDirList, const TDesC8& aObjectId, HBufC8** aResUri) 
	{           
	
	RXmlEngDocument doc = ParseXmlL(aResponse);	
        	
	// Does return resorce URI?
	TBool isReturnUri = aObjectId.Compare(KNullDesC8) != 0;
	//Start browsing document tree
	TXmlEngElement root = doc.DocumentElement();
	CleanupClosePushL( doc );
		
	if(root.NotNull())
		{		
		
	    RArray<TXmlEngElement> elements;
		CleanupClosePushL( elements );
		// get list of containers	    
        UpnpDomInterface::GetElementListL(root, elements, KContainer, EFalse);  
        // get list of items
        UpnpDomInterface::GetElementListL(root, elements, KItem, EFalse);
                    
        for (TInt i = 0; i < elements.Count(); i++) 
        	{
            TXmlEngElement objElem = elements[i];   
            TXmlEngElement titleElem;
            // check title
            if ( UpnpDomInterface::GetElementL(objElem, titleElem, KTitle ) &&
            	titleElem.Value().Length()) 
            	{
            	// check if it's item or container
                TBool isItem = UpnpDomInterface::CheckTagL(objElem, KItem);
                
                TPtrC8 id = UpnpDomInterface::GetAttrValueL( objElem, KId);
                TPtrC8 parentId = UpnpDomInterface::GetAttrValueL( objElem, KParentId);
                TPtrC8 title = titleElem.Value();                    			    			    
			    
				// try to add new path element, if name is taken, new name will be given
				const TDesC8& newTitle = AddElementL(parentId, id, title);                       
				// check if return dir list
                if (aDirList) 
                	{                        
                    TFileInfo entry;   
                    // set name                                                                             
			        entry.iName = newTitle;			        
			        
					// set date			        		        
			        TXmlEngElement dateElem;
			        if (UpnpDomInterface::GetElementL(objElem, dateElem, KDate ) )
			        	{
			        	SetModifiedTimeL(dateElem, entry);    			        
			        	}
			  
			        if (isItem) // for items
			        	{
			            entry.iAtt |= KEntryAttNormal;  
			            // get size from res if exist 
			            RArray<TXmlEngElement> resElements;
						CleanupClosePushL( resElements );			            
			            if (UpnpDomInterface::GetElementListL(objElem, resElements, KRes, EFalse)) 
			            	{
			            	// get prefer resource
    	    				TXmlEngElement res = GetPreferResourceL(resElements);			            	
			            	// maybe res URI should be returned
			                if (isReturnUri && UpnpDomInterface::CheckAttributeValueL(objElem, KId, aObjectId)) 
			                	{
			                	if (res.NotNull() && res.Value().Length()) 
			                		{
			                		*aResUri = res.Value().AllocL();
			                		}			                    
			                	}			             
    			            // set size base on res@size
    			    	    TPtrC8 size = UpnpDomInterface::GetAttrValueL( res, KSize);	        			    	        			    	
    			    	    TLex8 intLex( size );                			    	
    			    	    intLex.Val( entry.iSize );
    			    	    
    			    	    //TPtrC8 importUri = UpnpDomInterface::GetAttrValueL( res, KImportUri);	
    			    	    TPtrC8 restricted = UpnpDomInterface::GetAttrValueL( objElem, KRestricted);	
							// check if read-only flag should be set
    			    	    if (restricted.Compare(KOne) == 0 || restricted.CompareF(KTrue) == 0)
    			    	    	{
    			    	    	entry.iAtt |= KEntryAttReadOnly;
    			    	    	}
    			    	    
			            	}
						CleanupStack::PopAndDestroy(&resElements);	    			    	    			            	
			        	}
			        else // for containers
			        	{
			            entry.iAtt |= KEntryAttDir;  
			        	}
			            			    				   				   			        			    				    
			        // add to dir result
			        aDirList->Append(entry);
                	}                    
            	}
        	}            
		CleanupStack::PopAndDestroy(&elements);	        
		}			
	
	//Cleanup
	CleanupStack::PopAndDestroy(&doc);
	}


// -----------------------------------------------------------------------------
// CUpnpPathResolver::GetResUri
// -----------------------------------------------------------------------------
//   
TBool CUpnpPathResolver::GetResUriL(const TDesC8& aResponse, const TDesC8& aId, HBufC8*& aResUri) 
	{
    TInt found(EFalse);
    delete aResUri;
    aResUri = NULL;
        
	RXmlEngDocument doc = ParseXmlL(aResponse);				
	CleanupClosePushL( doc );
				
	//Start browsing document tree
	TXmlEngElement root = doc.DocumentElement();
	
	if(root.NotNull())
		{		    	
        TXmlEngElement item;
        if ( UpnpDomInterface::GetDirectoryElementL(root, item, KItem, KId, aId) )
        	{
    	    RArray<TXmlEngElement> resElements;
    	    CleanupClosePushL(resElements);
    	    if (UpnpDomInterface::GetElementListL(item, resElements, KRes, EFalse) ) 
    	    	{
    	    	TXmlEngElement res = GetPreferResourceL(resElements);
                
                if (res.Value().Length()) 
                	{
	                found = ETrue;	               
    	            aResUri = res.Value().AllocL();                	
                	}                
    	    	}		
    	    CleanupStack::PopAndDestroy(&resElements);		  				    
        	}	              
		}				

	CleanupStack::PopAndDestroy(&doc);

	return found;	
	}
// -----------------------------------------------------------------------------
// CUpnpPathResolver::BrowseCreateObjectL
// -----------------------------------------------------------------------------
//   
TBool CUpnpPathResolver::BrowseCreateObjectL(const TDesC8& aResponse) 
	{
    TInt found(EFalse);   
	RXmlEngDocument doc = ParseXmlL(aResponse);			
	CleanupClosePushL( doc );
	//Start browsing document tree
	TXmlEngElement root = doc.DocumentElement();
	
	if(root.NotNull())
		{		
        TXmlEngElement item;
        if ( UpnpDomInterface::GetElementL(root, item, KItem) )
            {    	    
    	    
    	    TPtrC8 id = UpnpDomInterface::GetAttrValueL( item, KId);
            TPtrC8 parentid = UpnpDomInterface::GetAttrValueL( item, KParentId);    
                    
    	    if (id.Length() && parentid.Length()) 
        	    {        	    	        	    	
                    RArray<TXmlEngElement> resElements;
    	    		CleanupClosePushL(resElements);
                    
                    if(UpnpDomInterface::GetElementListL(item, resElements, KRes, EFalse)) 
                    	{
                    	TPtrC8 import;
                    	TXmlEngElement resElement = GetResourceWithImportURIL(resElements, import);
		                
		                
		                if(import.Length())
		                	{
		             
		    				TXmlEngElement titleElem;
		    				
		    				if(UpnpDomInterface::GetElementL(item, titleElem, KTitle))
		    					{
		    					if ( titleElem.Value().Length()) 
		                        	{
		                        	found = ETrue;   
		                        	TPtrC8 title = titleElem.Value();                         	                       	
		                        	AddElementL(parentid, id, title, import);                    	
		                        	}
		    					} 
		                	}		                
							
						CleanupStack::PopAndDestroy(&resElements);	                   	                                          

                    	}
        	    }				  				        	    
            }                         			    	      
		}			
	

	//Cleanup
	CleanupStack::PopAndDestroy(&doc);
	
    return found;	
	}

// -----------------------------------------------------------------------------
// CUpnpPathResolver::GetItemL
// -----------------------------------------------------------------------------
//   
TBool CUpnpPathResolver::GetItemL(const TDesC8& aResponse, const TDesC8& aId) 
	{
    TInt found(EFalse);    

	RXmlEngDocument doc = ParseXmlL(aResponse);			
	CleanupClosePushL( doc );
	//Start browsing document tree
	TXmlEngElement root = doc.DocumentElement();
	
	if(root.NotNull())
		{		
        TXmlEngElement item;
        if ( UpnpDomInterface::GetDirectoryElementL(root, item, KItem, KId, aId) )
            {
    	    RArray<TXmlEngElement> resElements;
    	    CleanupClosePushL(resElements);
    	    if (UpnpDomInterface::GetElementListL(item, resElements, KRes, EFalse) ) 
        	    {        	    	
                found = ETrue;                       
        	    }				  				    
    	    CleanupStack::PopAndDestroy(&resElements);	
            }                         			    	      
		}				

	//Cleanup
	CleanupStack::PopAndDestroy(&doc);
	
    return found;	
	}
	
// -----------------------------------------------------------------------------
// CUpnpPathResolver::ParseXmlL
// -----------------------------------------------------------------------------
//   
RXmlEngDocument CUpnpPathResolver::ParseXmlL(const TDesC8& aXml) 	
	{
		
	//Create a parser
	RXmlEngDOMParser parser;
	User::LeaveIfError( parser.Open(iDOMImpl) );
	CleanupClosePushL(parser);
		
	//Handler to the parsed document
	RXmlEngDocument doc;				
		
	doc = parser.ParseL(aXml);	
	CleanupClosePushL( doc );	
        	
	if(doc.IsNull()) 
		{
		User::Leave(KErrUnknown);
		}
		
	CleanupStack::Pop(&doc);
	CleanupStack::PopAndDestroy(&parser);			
	return doc;		
	}
	

// -----------------------------------------------------------------------------
// CUpnpPathResolver::GetPreferResource
// -----------------------------------------------------------------------------
//   
TXmlEngElement CUpnpPathResolver::GetPreferResourceL(const RArray<TXmlEngElement>& aElements) 	
	{
	//DEBUGSTRING8(("GetPreferResource "));   
	if (aElements.Count() > 0)	
		{
		for (TInt i = 0; i < aElements.Count(); i++) 
			{
			TXmlEngElement elem = aElements[i];
			TPtrC8 protocol = UpnpDomInterface::GetAttrValueL(elem, KProtocolInfo);
			if (protocol.Left(KProtocolInfoBegin().Length()).Compare(KProtocolInfoBegin) == 0) 
				{				
				return elem;
				}
			}
		DEBUGSTRING8(("GetPreferResource:   Return empty element (no http-get elements) "));   			
		}
	else 
		{
		DEBUGSTRING8(("GetPreferResource:   Return empty element (size of list = 0) "));   		
		}
	return TXmlEngElement();		
	}
// -----------------------------------------------------------------------------
// CUpnpPathResolver::GetResourceWithImportURIL
// -----------------------------------------------------------------------------
//   
TXmlEngElement CUpnpPathResolver::GetResourceWithImportURIL(const RArray<TXmlEngElement>& aElements, TPtrC8& aImportURI) 	
	{	
	aImportURI.Set(KNullDesC8);
	
	if (aElements.Count() > 0)	
		{
		for (TInt i = 0; i < aElements.Count(); i++) 
			{
			TXmlEngElement elem = aElements[i];
			
			aImportURI.Set(UpnpDomInterface::GetAttrValueL(elem, KImportUri));
			if (aImportURI != KNullDesC8) 
				{				
				return elem;
				}
			}
		}
	return TXmlEngElement();		
	}	
//  End of File  
