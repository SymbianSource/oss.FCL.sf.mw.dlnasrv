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
* Description:  Local interface
*
*/



#ifndef UPNPLOCALSTORAGE_H
#define UPNPLOCALSTORAGE_H

//  INCLUDES
#include <e32base.h>
#include <upnpobject.h>

// FORWARD DECLARATIONS
class CUpnpMetadataStorage;
class CUpnpItem;
class CUpnpItemList;
class CUpnpContainerList;
class CUpnpBrowseCriteria;
class CUpnpContainer;
class CUpnpObjectBean;
class CUpnpElement;
class CUpnpElementBean;
class CUpnpAttributeBean;
class CUpnpAttribute;
class CUpnpObject;
class CUpnpContentDirectory;
class CUpnpStateHandler;

// CLASS DECLARATION

/**
*  Local interface
*
*  @lib avcontentdirectory.lib
*  @since Series 60 3.0
*/
class CUpnpLocalStorage : public CBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpLocalStorage* NewL(CUpnpMetadataStorage* aMetadataStorage);
    static CUpnpLocalStorage* NewLC(CUpnpMetadataStorage* aMetadataStorage);
    
    /**
    * Destructor.
    */
    virtual ~CUpnpLocalStorage();

public: // New functions
    
		void ShareObjectL(CUpnpObject* aItem);
		void ShareReferenceL( CUpnpItem* aItem );
		void ShareReferenceListL( CUpnpItemList* aItemList, TInt* aExecutionStatus );
		void UnshareItemL(TInt aItemId);
		void UnshareContainerL(TInt aItemId);
		CUpnpItem* GetSharedItemL(TInt aItemId);
		void ShareItemListL(CUpnpItemList* aItemList, TInt* aExecutionStatus);
		void UnshareItemListL(RArray<TInt>& aItemList, TInt* aExecutionStatus);
		void GetItemListL(TInt aContainerId, CUpnpBrowseCriteria* aBrowseCriteria, TInt* aTotalMatches, CUpnpItemList* aItemList);
		void GetContainerListL(TInt aContainerId, CUpnpBrowseCriteria* aBrowseCriteria, TInt* aTotalMatches, CUpnpContainerList* aList);
		void SetStateHandler(CUpnpStateHandler* aStateHandler);
		CUpnpContainer* GetSingleContainerL(TInt aContainerId);

    private:

        /**
        * C++ default constructor.
        */
        CUpnpLocalStorage(CUpnpMetadataStorage* aMetadataStorage);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void AddAttributesL( TInt aElmId, CUpnpElement* aElm );
        void AddElementsL( TInt aObjectID, CUpnpObject* aObj );
        void GetObjectL(TInt aObjId, CUpnpObject* aObj, TBool aAll = ETrue);
        void HandleResourcesL(CUpnpObject* aItem);
		void DoShareObjectL(TInt aContainer, CUpnpObject* aItem);
		void DoShareReferenceL( TInt aContainerId, CUpnpItem* aRefItem ); 
		CUpnpObjectBean* CreateObjectBeanLC(CUpnpObject* aItem);
		CUpnpElementBean* CreateElementBeanLC(CUpnpElement& aItem);
		CUpnpAttributeBean* CreateAttributeBeanLC(CUpnpAttribute& aAttr);
		void ValidateContainerL(TInt aContainerId);
		void ValidateItemL(TInt aItemId);
		HBufC8* ValAsUriL(const TDesC8& aElmValue) const ;
		TInt ReferedObjectIdL(TInt aObjectId);

private:    // Data
    CUpnpMetadataStorage*   iMetadataStorage; // not owned
    CUpnpStateHandler* iStateHandler; // not owned

};

#endif      // UPNPLOCALSTORAGE_H
        
// End of File
