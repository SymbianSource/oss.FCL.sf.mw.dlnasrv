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






#ifndef C_UPNPXMLEVENTPARSER_H_
#define C_UPNPXMLEVENTPARSER_H_

// INCLUDES
#include <e32base.h>
#include <xml/contenthandler.h>

// FORWARD DECLARATIONS

using namespace Xml;

/**
 * XML SAX Parser for UPnP.
 *
 * @since s60 3.1
 * @lib upnpxmlparser.lib
 */
class CUPnPXMLEventParser :  public CBase,
                             public MContentHandler
    {

public:

    enum TParserState
        {
        EEvent = 0,
        EInstanceID,
        EVolume,
        EMute,
        ENotSupported // Brightness etc.
        };    

public:

    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUPnPXMLEventParser* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CUPnPXMLEventParser();
    
public:

    /**
     * Parses xml data to the array of objects.
     * @param aResultArray, An array for objects in xml root.
     * @param aData, xml data.
     */
    IMPORT_C void ParseResultDataL( const TDesC8& aData,
        TInt& aInstanceId, TInt& aVolume, TBool& aMute );
                    
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
    CUPnPXMLEventParser();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL();
    
private:    

    void SetAttributesL( const RAttributeArray& aAttributes );

    void Reset();
    
private: // data
    
    TParserState    iParserState;
        
    TInt            iInstanceID;
    
    TInt            iMute;

    TInt            iVolume;
    //ETrue:Master Volume is set.EFalse:Master Volume is not set.
    TBool           iMasterVolumeState;
    };

#endif // C_UPNPXMLEVENTPARSER_H_

// End of File
