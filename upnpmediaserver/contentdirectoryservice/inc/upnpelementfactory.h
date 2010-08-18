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
* Description:  Declares ElementFactory class.
*
*/


#ifndef C_CUPNPELEMENTFACTORY_H
#define C_CUPNPELEMENTFACTORY_H


// INCLUDES

#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdocument.h>
#include "upnpcontentdirectory.h"
#include "upnpfiletransfer.h"
#include "upnperrors.h" // Added by ClassView
#include "upnpdominterface.h"

// FORWARD DECLARATIONS

class CUpnpElementFactory;
class TUpnpContainerUpdateId;
class CUpnpFileTransferTimerObserver;
class CUpnpObject;
class CUpnpItem;


// FORWARD DECLARATIONS

class CUpnpContentDirectory;

// CLASS DECLARATION

/**
*  @brief Used to create content objects
*
*
*  @lib contentdirectory.lib
*  @since Series60 2.6
*/
class CUpnpElementFactory : public CBase
{
public: // Constructors and destructors
                         
    /**
    * Gets active element from XML document. An active element
    * is a first element different from <DIDL-Lite> element.
    * @param aDocument XML document which will be search for active element.
    * @return handler to active element.
    */
    TXmlEngElement ActiveElementL( const RXmlEngDocument& aDocument );
    
    /**
    * Extracts active element from XML document. 
    * An active element is a first element different from <DIDL-Lite> element.
    * IMPORTANT: Caller takes responsibility for returned element. 
    * @param aDocument XML document which will be search for active element.
    * @return handler to extracted active element.
    */
    RXmlEngDocument ExtractActiveElementL( const RXmlEngDocument& aDocument );
    
    /**
    * Checks if two elements are equal and have exactly the same subtrees.
    * @param aFirst - first element to compare
    * @param aSecond - second element to compare
    * @return ETrue if elemants are equal.
    */
    TBool ElementsMatchL( TXmlEngElement aFirst, TXmlEngElement aSecond );
    
    /**
    * Counts elements in array that have the same name.
    * @param aName - name of elements, which will be counted.
    * @param aArray - array with elements to count.
    * @return number of found elements.
    */ 
    TInt CountElementsL( const TDesC8& aName, RArray<RXmlEngDocument>& aArray );
    
    /**
    * Counts elements in array that have the same name.
    * @param aName - name of elements, which will be counted.
    * @param aArray - array with elements to count.
    * @return number of found elements.
    */ 
    TInt CountElementsL( const TDesC8& aName, RArray<TXmlEngElement>& aArray );   
    
    /**
    * Gets classes descriptions for given object.
    * @param aObject - element representing object, which classes 
    *   descriptions we wan to acquire
    * @param aArray - (OUT) reference to array which will be filled with classes
    *   descriptions. 
    * @param aObjType - object type [item or container]
    */
    void ClassesL( const TXmlEngElement& aObject, RArray<TXmlEngElement>& aArray, const TDesC8& aObjType = KNullDesC8() );
    
    /**
    * Constructs descriptor with element's name and namespace prefix separated with colon.
    * IMPORTANT: As the function name indicates, it leaves pointer to heap descriptor on 
    * cleanup stack.
    * @param aElement - element for which descriptor will be constructed.
    * @return  - pointer to descriptor, allocated on the heap, containing element's name with prefix.
    */ 
    HBufC8* NameWithNsLC( const TXmlEngElement& aElement );
    

    /**
    * Two-phased constructor for building CUpnpElementFactory.
    * @since Series60 2.6
    */
    static CUpnpElementFactory* NewL( const TDesC& aObjectsXmlPath );

    /**
    * Two-phased constructor for building CUpnpElementFactory.
    * @since Series60 2.6
    */
    static CUpnpElementFactory* NewLC( const TDesC& aObjectsXmlPath );

    /**
    * Destructor function; called automatically from the destructor of a 
    * derived class
    */
    virtual ~CUpnpElementFactory();

    /**
    * Function to validate a new object given by Control Point, especially 
    * upnp:class element field eg. object.item.musictrack
    * IMPORTANT: If objects's description is invalid due to missing elements
    * it tries to repair it by adding those missing elements.
    * @since Series60 2.6
    * @param aElement element (xml) given by Control Point
    * @param aUpdateItem 
    * @return Returns a error code that is to be sent to caller of action.
    */
    TUpnpErrorCode ValidateNewObjectL( const TXmlEngElement& aNewElement, TBool aNew = ETrue, TBool aLocalSharing = EFalse );
    
    /**     
    * Function gets pointer to CUpnpContentDirectory and puts it into CUpnpElementFactory object 
    * @param aCD - pointer to CUpnpContentDirectory object
    */ 
    void GetContentDirectoryReference(CUpnpContentDirectory* aCD);
    
private: // Constructors

    /**
    * C++ default constructor.
    */
    CUpnpElementFactory();

    /**
    * Second phase of the constructor.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL(const TDesC& aObjectsXmlPath);

private: // New functions
    
    void ValidateBigImageL( TXmlEngElement aObj);
    
    /**
    *
    * @since Series S60 3.1
    * @param aPattern
    * @return boolean 
    */
    TBool IsRequiredL(TXmlEngElement aPattern);
    
    /**
    * Getter
    * @since Series S60 3.1
    * @param aPropertyName
    * @param aClassList
    * @return TXmlEngElement
    */
    TXmlEngElement GetPatternForResAttrL(const TDesC8& aPropertyName, RArray<TXmlEngElement>& aClassList );
    
    /**
    *
    * @since Series S60 3.1
    * @param aElement
    * @param aClassList
    */
    void ValidateResElL( TXmlEngElement aElement, RArray<TXmlEngElement>& aClassList);
    
    /**
    * Validates element
    * @since Series S60 3.1
    * @param aElement
    * @param aClasList
    */
    void ValidateElementL(TXmlEngElement aElement, RArray<TXmlEngElement>& aClassList);
    
    /**
    *
    * @since Series S60 3.1
    * @param aObj
    * @parma aClassList
    */
    void ValidatePropertiesL(TXmlEngElement aObj, RArray<TXmlEngElement>& aClassList);
    
    /**
    *
    * @since Series S60 3.1
    * @param aObj
    * @param aClassList
    */
    void ValidateMainAttributesL(TXmlEngElement aObj, RArray<TXmlEngElement>& aClassList);
    
    /**
    *
    * @since Series S60 3.1
    * @param aAttr
    * @parma aPattern
    */
    void ValidateElWithPatternL(TXmlEngElement aAttr, TXmlEngElement aPattern);
    
    /**
    *
    * @since Series S60 3.1
    * @param aAttr
    * @param aPattern
    */
    void ValidateAttrWithPatternL(TXmlEngAttr aAttr, TXmlEngElement aPattern);
    
    /**
    *
    * @since Series S60 3.1
    * @param aPropertyName
    * @param aClassList
    * @return TXmlEngElement
    */
    TXmlEngElement GetPatternL( const TDesC8& aPropertyName, RArray<TXmlEngElement>& aClassList, const TDesC8& aType );
    
    /**
    *
    * @since Series S60 3.1
    * @param aElement
    * @return boolean
    */
    TBool IsClassElement( const TXmlEngElement& aElement );
    
    /**
    *
    * @since Series S60 3.1
    * @param aElement
    * @return boolean
    */
    TBool IsNotEmptyImportUriL( const TXmlEngElement& aElement );
    
    /**
    *
    * @since Series S60 3.1
    * @param aElement
    * @return boolean
    */
    TBool IsMainObjectTagL(TXmlEngElement aElement);

    /**
    *
    * @since Series S60 3.1
    * @param aProtocolInfo
    * @param aLocalSharing
    * @return descriptor pointer
    */        
    HBufC8* ValidateProtocolInfoInResL( const TDesC8& aProtocolInfo, TBool aLocalSharing );
    
    /**
    *
    * @since Series S60 3.1
    * @param aElement    
    */
    void CheckDurationOfResElementL(const TXmlEngElement& aElement);
    
    /**
    *
    * @since Series S60 3.2
    * @param aElement    
    */
    void CheckSizeOfResElementL(const TXmlEngElement& aElement);
                    
private: // Data

    // owned
    RXmlEngDocument               iDocument;
    // Pointer to content directory
    CUpnpContentDirectory*  iContentDirectory;
    RXmlEngDOMImplementation iDOMImpl;
};

#endif  // C_CUPNPELEMENTFACTORY_H

// End of File
