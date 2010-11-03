/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP Renderer config parser
*
*/

#ifndef C_UPNPRENDERERCFGPARSER_H
#define C_UPNPRENDERERCFGPARSER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <xml/contenthandler.h>

class CUpnpRendererCfg;

namespace Xml 
    {
    class CParser;
    class CMatchData;
    }

class CUpnpRendererCfgParser : public CBase
                             , public Xml::MContentHandler
    {
    
    /*
     * Valid tags in renderer configuration xml file
     */
    enum TCfgElements
        {
        EUnknown,
        EConfig,
        ERenderer,
        ETranscoding,
        EPipeline,
        EProtocolInfo,
        ESizeMultiplier
        };

public:

    /**
     * Static NewL
     *
     * @param none
     * @return instance to CUpnpRendererCfgParser
     */
    IMPORT_C static CUpnpRendererCfgParser* NewL();
    
    /**
     * Static NewLC
     *
     * @param none
     * @return instance to CUpnpRendererCfgParser
     */
    IMPORT_C static CUpnpRendererCfgParser* NewLC();
    
    /**
     * Destructor
     */
    IMPORT_C ~CUpnpRendererCfgParser();
    
private:
    
    /**
     * Constructor
     */
    CUpnpRendererCfgParser();
   
    /**
     * 2nd phrase Constructor
     */
    void ConstructL();

public:
    
    /**
     * Parse renderer config.
     * 
     * @param aRendereConfig Reference to renderer config container; after 
     *        succesfull call, all the configs are stored in the container
     * 
     * @see upnprenderercfg.h
     */
    IMPORT_C void ParseRendererCfgL( CUpnpRendererCfg& aRendereConfig );         
    
private:
    
    /*
     * Start parsing.
     * 
     * @param aFile Renderer configuration xml file
     */
    void XMLParseL( const TDesC& aFile );
    
    /*
     * Reset memeber variables.
     */
    inline void Reset();
    
    /*
     * Fetch element enumeration.
     *  
     * @param aElement Reference to element's tag info
     *
     * @return Enumeration of the element
     */
    TCfgElements Element( const Xml::RTagInfo& aElement );
    
    /*
     * Checks if an attribute array contains an attribute with matching name
     * and matching value.
     *
     * @param aAttributes Reference to atttribute array
     * @param aAttributeName Reference to 8-bit name descriptor
     * @param aAttributeValue Reference to 8-bit value descriptor
     * @param aExactValueMatch Boolean for exact value match
     *
     * @param ETrue if found, otherwise EFalse
     */     
    TBool ContainsAttribute( const Xml::RAttributeArray& aAttributes, 
        const TDesC8& aAttributeName, const TDesC8& aAttributeValue, 
        TBool aExactValueMatch );

private: // from MContentHandler

    /**
     * From MContentHandler.
     * @param aDocParam, not used.
     * @param aErrorCode, not used.
     */
    void OnStartDocumentL( const Xml::RDocumentParameters& aDocParam, 
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
    void OnStartElementL( const Xml::RTagInfo& aElement, 
                          const Xml::RAttributeArray& aAttributes, 
                          TInt aErrorCode );
    
    /**
     * From MContentHandler.
     * @param aElement, holds the element info.
     * @param aErrorCode, if not KErrNone, the method is ignored.
     */
    void OnEndElementL( const Xml::RTagInfo& aElement, TInt aErrorCode );
    
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
    
    RFs              iFs;    
    TCfgElements     iCurrentElement;
    TBool            iCorrectModel;    
    TBool            iCorrectTranscoding;
    Xml::CParser*    iParser;             // owned    
    Xml::CMatchData* iMatchData;          // owned        
    HBufC8*          iModelName;          // not owned    
    HBufC8*          iSrcMimeType;        // not owned
                      
    // not owned if ParseRendererCfgL() doesn't LEAVE; otherwise owned
    HBufC8*          iPipeline;
    
    // not owned if ParseRendererCfgL() doesn't LEAVE; otherwise owned
    HBufC8*          iProtocolInfo;
    
    TReal            iSizeMultiplier;
    
    };


#endif // C_UPNPRENDERERCFGPARSER_H

