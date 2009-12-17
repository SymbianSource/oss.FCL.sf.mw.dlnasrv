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






#ifndef C_UPNPXMLPARSERLITE_H_
#define C_UPNPXMLPARSERLITE_H_

// INCLUDES
#include <e32base.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATIONS
class CUpnpObjectLite;
class CUPnPObjectStackLite; 
//class CUpnpItem;
//class CUpnpContainer;

using namespace Xml;

/**
 * XML SAX Parser for UPnP.
 *
 * @since s60 3.1
 * @lib upnpxmlparser.lib
 */
class CUPnPXMLParserLite :  public CBase,
                            public MContentHandler
    {

public:

    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUPnPXMLParserLite* NewL();
    
    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUPnPXMLParserLite* NewLC();
    
    /**
     * Destructor.
     */
    virtual ~CUPnPXMLParserLite();
    
public:

    /**
     * Parses xml data to the array of objects.
     * @param aResultArray, An array for objects in xml root.
     * @param aData, xml data.
     * @param aFormatString the format of title. If KNullDesC title will be
     * stored as it is. It's expected that the string is in the following
     * format: _L( "0\t%S---" ); where "0" is a placeholder for icon index
     * and %S is a placeholder for title
     * 
     */
    IMPORT_C void ParseResultDataL( RPointerArray<CUpnpObjectLite>&
        aResultArray, const TDesC8& aData, const TDesC& aFormatString = KNullDesC );
                    
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
    CUPnPXMLParserLite();
    
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
    void SetAttributesL( CUpnpObjectLite& aObject, 
                         const RAttributeArray& aAttributes );
                         
    void SetIconIndex( const CUpnpObjectLite& aObject );                     
                                         
private: // data

    /**
     * A pointer to result array from the client.
     * Not own.
     */      
    RPointerArray<CUpnpObjectLite>* iResultRoot;
    
    /**
     * A stack for UPnP objects.
     * Own.
     */
    CUPnPObjectStackLite*           iStack;
    
    /**
     * Element value
     * Own.
     */
    HBufC8*                     iElementValue;
    
    HBufC8*                     iTitleBuf;
    
    HBufC*                      iFormatString;
            
    TBool                       iTitle;
    TBool                       iObjectClass;
    };

#endif // C_UPNPXMLPARSER_H_

// End of File
