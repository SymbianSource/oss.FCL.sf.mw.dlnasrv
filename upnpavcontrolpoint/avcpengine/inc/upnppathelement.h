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
* Description:  Describe a path element
*
*/


#ifndef C_CUPNPPATHELEMENT_H
#define C_CUPNPPATHELEMENT_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
*  Describe a path element
*
*  @since Series 60 3.1
*/
class CUpnpPathElement: public CBase
	{
	public:  // Constructors and destructor        
		/**
		* Two-phased constructor.
		*/
		static CUpnpPathElement* NewL();        
		/**
		* Destructor.
		*/
		virtual ~CUpnpPathElement();

	public: // New functions     
		/**
	    * Get value of parent id
	    * @since Series 60 3.1
	    * @return parent id
	    */      	   
		const TDesC8& ParentId() const;   
		/**
	    * Set value of parent id
	    * @since Series 60 3.1
	    * @param aParentId new value of parent id
	    */		             
		void SetParentIdL(const TDesC8& aParentId);
		/**
	    * Get value of id
	    * @since Series 60 3.1
	    * @return id value
	    */   		
		const TDesC8& Id() const;  
		/**
	    * Set value of id
	    * @since Series 60 3.1
	    * @param aId new value of id
	    */				      
		void SetIdL(const TDesC8& aId);
		/**
	    * Get value of name
	    * @since Series 60 3.1
	    * @return name
	    */   		
		const TDesC8& Name() const;   
		/**
	    * Set value of name
	    * @since Series 60 3.1
	    * @param aName new value of name
	    */			             
		void SetNameL(const TDesC8& aName);        
		
		/**
	    * Get value of ImportURI
	    * @since Series 60 3.2
	    * @return ImportURI
	    */      	   
		const TDesC8& ImportURI() const;   
		/**
	    * Set value of ImportURI
	    * @since Series 60 3.2
	    * @param aImportURI a new value of importURI
	    */		             
		void SetImportURIL(const TDesC8& aImportURI);        		
		
		/**
	    * Get value of original name
	    * @since Series 60 3.2
	    * @return OriginalName
	    */      	   
		const TDesC8& OriginalName() const; 
		  
		/**
	    * Set value of original name
	    * @since Series 60 3.2
	    * @param aIOriginalName a new value of original name
	    */		             
		void SetOriginalNameL(const TDesC8& aOriginalName);    
		
		/**
	    * Deletes value of original name
	    * @since Series 60 3.2	   
	    */		             
		void DeleteOriginalName();    
		
		/**
		Compares if two path elements have the same name and parent id.
		@param aFirst First message to compare.
		@param aSecond Second message to compare.
		@result ETrue if same session id, EFalse otherwise.
		**/
		static TBool MatchName(const CUpnpPathElement& aElement1, 
                            const CUpnpPathElement& aElement2);        
		/**
		Compares if two path elements have the same name, id and parent id.
		@param aFirst First message to compare.
		@param aSecond Second message to compare.
		@result ETrue if same session id, EFalse otherwise.
		**/                            
		static TBool MatchAll(const CUpnpPathElement& aElement1, 
                            const CUpnpPathElement& aElement2);                              

	private:
		/**
		* C++ default constructor.
		*/
		CUpnpPathElement();

		/**
		* Symbian 2nd phase constructor
		*/
		void ConstructL();
        
	private:
		// id attrbute of UPnP object
		HBufC8* iId;
		// parent id attrbute of UPnP object
		HBufC8* iParentId;
		// name path element, name is taken from title element, 
		// but if duplication occurs, name is followed by index string
		HBufC8* iName;   
		//hidden import URI
		HBufC8* iImportURI;
		//original name if change by creating resource   	
		HBufC8* iOriginalName;
	};

#endif      // C_CUPNPPATHELEMENT_H   
            
// End of File
