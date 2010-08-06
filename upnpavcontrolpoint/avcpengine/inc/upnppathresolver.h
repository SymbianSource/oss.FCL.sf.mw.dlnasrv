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



#ifndef C_CUPNPPATHRESOLVER_H
#define C_CUPNPPATHRESOLVER_H

//  INCLUDES
#include <e32base.h>

#include <xml/dom/xmlengdom.h>

// FORWARD DECLARATIONS
class CUpnpPathElement;
class CUpnpResolveHandler;
class MUpnpResolverObserver;
class CUpnpAVCPManager;
class TFileInfo;
class CUpnpAVCPEngineSession;

// CLASS DECLARATION
/**
* Class responsible for managing path element for one MS.
* It helps in translating pathname into id.
*
*  @since Series 60 3.1
*/
class CUpnpPathResolver : public CBase
	{
	public:  
		/**
		* Two-phased constructor.
		*/
		static CUpnpPathResolver* NewL(CUpnpAVCPManager& aManager,const TDesC8& aUUID);
		
		/**
		* Destructor.
		*/
		virtual ~CUpnpPathResolver();

  public: // New functions
		/**
		*  Return UDN of MS that resolver is related
		*/     
		const TDesC8& UUID() const;
		
		/**
		* Add new path element into resolver.
		* if resolver has element with the same name and ids ignore.
		* if resolver has element with the same naem but different id,
		*  perform duplicatione mechanism by adding '(inx)' at the end of name, new name is returned.
		*
		* @param aParentId parent id
		* @param aId id
		* @param aName name of element
		* @param aImportURI importURI of element
		* @return approved name, it may be the same as aName if we don't have duplication, ale is followed by index
		*/
		const TDesC8& AddElementL(const TDesC8& aParentId, const TDesC8& aId, const TDesC8& aName, const TDesC8& aImportURI = KNullDesC8);
		
		/**
		* Gets path element for object with given parent id and name.
		* Ownership is not passed to caller.
		* 
		* @param aParentId parent id 
		* @param aName name
		* @return NULL if not found, else instance describing path element
		*/
		CUpnpPathElement* GetElementL(const TDesC8& aParentId, const TDesC8& aName) const;
		/**
		* Remove given element from cache array (if found in cache it destroys also given instance)
		*
		* @param aElement param to remove
		*/
		void RemoveElementD(CUpnpPathElement* aElement);  
		/**
		* Request for resolving pathname and notify given observer after process will be finished.
		*/
		void ResolveIdL(const TDesC8& aPath, MUpnpResolverObserver& aObserver);
		/**
		* Remove instance of resolver's handler from array of active handlers
		*/
		void RemoveHandler(CUpnpResolveHandler* aHandler);							
		/**
		Compares if two path resolvers have the UDN
		@param aFirst First message to compare.
		@param aSecond Second message to compare.
		@result ETrue if same session id, EFalse otherwise.
		**/
		static TBool Match(const CUpnpPathResolver& aElement1, const CUpnpPathResolver& aElement2) ;        
		/**
		* Notify if event comes from CDS with system id, 
		* If system id is changed path elements will be removed
		*/
		void StateChangedL(const TDesC8& aSystemId);
		/**
		* Set modification time based on date element from CDS's object.
		*
		* @param aDateElement XML element with date
		* @param aEntry structure to be updated 
		*/
		void SetModifiedTimeL(TXmlEngElement& aDateElement, TFileInfo& aEntry); 		
		/**
		* Parse Browse response and updates information about path elements
		* If aDirList is defined, update array by parsed path element 
		*	(set also file/directory attributes)
		* If aObjectId is not equal KNullDesC8, then also try to find resource URI for aObjectId.
		*
		* @param aResponse response
		* @param aDirList array of directory entry elements
		* @param aObjectId object id for resource URI
		* @param	aResUri	found resource
		*/
		void ParseBrowseResponseL(const TDesC8& aResponse, RArray<TFileInfo>* aDirList = NULL, 
		        const TDesC8& aObjectId = KNullDesC8, HBufC8** aResUri = NULL);
		/**
		* Parse Browse response and find resourse URI for object with given id.
		* 
		* @param aResponse response
		* @param aId id of object
		* @param aResUri parameter to be set with found resoursce URI
		* @return ETrue if found, else EFalse
		*/
		TBool GetResUriL(const TDesC8& aResponse, const TDesC8& aId, HBufC8*& aResUri);
		/**
		* Parse Browse response and find resourse for object with given id.
		* 
		* @param aResponse response
		* @param aId id of object		
		* @return ETrue if found, else EFalse
		*/		
		TInt GetItemL(const TDesC8& aResponse, const TDesC8& aId);		
		
		/**
		* Parse Browse response of CreateObject
		* 
		* @param aResponse response				
		* @return ETrue if found, else EFalse
		*/
		TBool BrowseCreateObjectL(const TDesC8& aResponse ) ;
		
		/**
		* Register session that is interested of this resolver
		*
		* @param aSession session
		*/
		void RegisterSession(CUpnpAVCPEngineSession* aSession);
		/**
		* Unregister session that is not interested of this resolver
		*
		* @param aSession session
		* @return number of session still being interested
		*/		
		TInt UnregisterSession(CUpnpAVCPEngineSession* aSession);
		/**
		* Lock resolver to not accept resets as a result of state variable changes 
		* Reset will be postpone until unlock method is invoked
		*
		*/
		void LockReset();
		/**
		* Unlock reset.
		* If between lock-unlock operation occurs state variable change, 
		* it'll reset all entries.
		*/
		void UnlockReset(); 			
	private:

		/**
		* C++ default constructor.
		*/
		CUpnpPathResolver(CUpnpAVCPManager& aManager);
		
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL(const TDesC8& aUUID);        
		/**
		* Parse XML given as parameter and return DOM tree
		*
		* @param aXml buffer with XML
		* @return DOM document
		*/	
		RXmlEngDocument ParseXmlL(const TDesC8& aXml); 	
		/**
		* Get prefer resource element
		*
		* @param aElements array of elements
		* @return chosen one
		*/
		TXmlEngElement GetPreferResourceL(const RArray<TXmlEngElement>& aElements); 
		/**
		* Get first resource element with import URI
		*
		* @param aElements array of elements
		* @param aImportURI if found
		* @return chosen one
		*/
		TXmlEngElement GetResourceWithImportURIL(const RArray<TXmlEngElement>& aElements, TPtrC8& aImportURI); 
		
	private:    // Data
		// last known system id (statevariable of CDS)
		TInt iSystemId;
		// UDN of MS 
		HBufC8* iUUID;
		// manager
		CUpnpAVCPManager& iManager;
		// array of path elements
		RPointerArray<CUpnpPathElement> iPathElements;
		// array of resolver's handlers
		RPointerArray<CUpnpResolveHandler> iResolveHandlers;
		// array of session being interested of this resolver
		RPointerArray<CUpnpAVCPEngineSession> iAVCPSessions;
		// indicate whether path resolver is reseted to remove all entries 
		// when state variable changes
		TBool iLocked;
		// indicate whether entries are up-to-date
		TBool iUptodate;
		
		RXmlEngDOMImplementation iDOMImpl;
    };

#endif      // C_CUPNPPATHRESOLVER_H   
            
// End of File
