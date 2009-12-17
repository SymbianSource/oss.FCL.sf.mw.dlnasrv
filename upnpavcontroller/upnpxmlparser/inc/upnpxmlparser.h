/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:      XML SAX Parser for UPnP.
*
*/






#ifndef C_UPNPXMLPARSER_H_
#define C_UPNPXMLPARSER_H_

// INCLUDES
#include <e32base.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATIONS
class CUpnpObject;
class CUPnPObjectStack; 
class CUpnpElement;
class CUpnpItem;
class CUpnpContainer;

using namespace Xml;

/**
 * XML SAX Parser for UPnP.
 *
 * @since s60 3.1
 * @lib upnpxmlparser.lib
 */
class CUPnPXMLParser :  public CBase,
                        public MContentHandler
    {

public:

    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUPnPXMLParser* NewL();
    
    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUPnPXMLParser* NewLC();
    
    /**
     * Destructor.
     */
    virtual ~CUPnPXMLParser();
    
public:

    /**
     * Parses xml data to the array of objects.
     * @param aResultArray, An array for objects in xml root.
     * @param aData, xml data.
     */
    IMPORT_C void ParseResultDataL( RPointerArray<CUpnpObject>& aResultArray,
        const TDesC8& aData );
        
    /**
     * Creates an xml document from a given item 
     *
     * @param aItem item to convert
     * @return xml document as a heap descriptor
     */
    IMPORT_C static HBufC8* ItemAsXmlLC( const CUpnpItem& aItem );

    /**
     * Creates an xml document from a given item, for CreateObject-action. 
     *
     * @param aItem item to convert
     * @return xml document as a heap descriptor
     */
    IMPORT_C static HBufC8* XmlForCreateObjectLC( const CUpnpItem& aItem );

    /**
     * Creates an xml document from a given item container
     *
     * @param aContainer container to convert
     * @return xml document as a heap descriptor
     */
    IMPORT_C static HBufC8* ContainerToXmlLC( 
                                    const CUpnpContainer& aContainer );
            
protected: // from MContentHandler

    /**
     * From MContentHandler.
     * @param aDocParam, not used.
     * @param aErrorCode, not used.
     */
    void OnStartDocumentL( const RDocumentParameters& aDocParam, 
                           TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aErrorCode, not used.
     */
    void OnEndDocumentL( TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aElement, holds the element info.
     * @param aAttributes, holds the element's attributes.
     * @param aErrorCode, if not KErrNone, the method is ignored.
     */
    void OnStartElementL( const RTagInfo& aElement, 
                          const RAttributeArray& aAttributes, 
                          TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aElement, holds the element info.
     * @param aErrorCode, if not KErrNone, the method is ignored.
     */
    void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aBytes, The value of the content.
     * @param aErrorCode, if not KErrNone, the method is ignored.
     */
    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aPrefix, not used.
     * @param aUri, not used.
     * @param aErrorCode, not used.
     */    
    void OnStartPrefixMappingL( const RString& aPrefix, 
                                const RString& aUri, 
                                TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aPrefix, not used.
     * @param aErrorCode, not used.
     */
    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aBytes, not used.
     * @param aErrorCode, not used.
     */
    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aName, not used.
     * @param aErrorCode, not used.
     */
    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aTarget, not used.
     * @param aData, not used.
     * @param aErrorCode, not used. 
     */
    void OnProcessingInstructionL( const TDesC8& aTarget, 
                                   const TDesC8& aData, 
                                   TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aErrorCode
     */                               
    void OnError( TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aUid, not used.
     * @return None.
     */
    TAny* GetExtendedInterface( const TInt32 aUid );

                                    
private:

    /**
     * Constructor.
     */
    CUPnPXMLParser();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
private: 

    /**
     * Sets element's attributes to the object.
     *
     * @param aObject attributes will be set to this object
     * @param aAttributes attributes to set
     */
    void SetAttributesL( CUpnpObject& aObject, 
                         const RAttributeArray& aAttributes );
                             
    /**
     * Set value to a element
     * 
     * @since Series 60 3.1
     * @param aItem (CUpnpObject&) a item which owns the element
     * @param aValue element value
     */
    void SetValueToElementL( CUpnpObject& aItem,
        const TDesC8& aValue );
        
    /**
     * Sets element's attributes to the element
     *
     * @param aElement attributes will be set to this element
     * @param aAttributes attributes to set
     */
    void SetAttributesL( const RAttributeArray& aAttributes ); 

    /**
     * Reset member for parse again.
     */
	void ResetMember();
private: // data

    /**
     * A pointer to result array from the client.
     * Not own.
     */      
    RPointerArray<CUpnpObject>* iResultRoot;
    
    /**
     * A stack for UPnP objects.
     * Own.
     */
    CUPnPObjectStack*           iStack;
    
    /**
     * Element value
     * Own.
     */
    HBufC8*                     iElementValue;
        
    /**
     * Upnp element
     * Owned until ownership is transferred for an object
     */
    CUpnpElement*               iNewElement;
    
    TBool                       iTitle;
    TBool                       iObjectClass;
    };

#endif // C_UPNPXMLPARSER_H_

// End of File
