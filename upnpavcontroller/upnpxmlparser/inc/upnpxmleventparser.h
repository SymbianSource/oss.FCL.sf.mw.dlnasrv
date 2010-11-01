/*
* Copyright (c) 2006-2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
class CUPnPAVTEvent;

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
        
private:

    /**
     * Internal parser state.
     */
    enum TParserState
        {
        EEvent = 0,
        EInstanceID,
        EVolume,
        EMute,
        ETransportState,
        ETransportURI,
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
     * Parses xml event data to values.
     * 
     * @param aData, xml data.
     * @param aInstanceId, instance id of current session
     * @return CUPnPAVTEvent*   contains event data. Ownership of event
     *                          is transferred.
     */
    IMPORT_C CUPnPAVTEvent* ParseRcEventDataL( const TDesC8& aData,
            TInt aInstanceId );
        
    /**
     * Parses xml event data to values.
     * @param aData, xml data.
     * @param aInstanceId, instance id of current session
     * @return CUPnPAVTEvent*   contains event data. Ownership of event
     *                          is transferred.
     */
    IMPORT_C CUPnPAVTEvent* ParseAvtEventDataL( const TDesC8& aData,
            TInt aInstanceId );
                    
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
  
    /**
     * Sets class member variables
     * @param RAttributeArray& aAttributes
     * @return None.
     */
    void SetAttributesL( const RAttributeArray& aAttributes );
    
    /**
     * Reset temporary variables that are used when parsing event
     */
    void Reset();
    
    /**
     * Resets variables where event values are stored
     */
     void ResetResult();
    
private: // data
    
    TParserState    iParserState;
    
    TInt            iSessionInstanceID;
    
    TInt            iMasterVolumeState;
    

    // variables used when parsing current event
    CUPnPAVTEvent*   iAvtEvent;          // own
    
    // variables where data from a valid event is stored
    CUPnPAVTEvent*   iAvtResultEvent;    // own
    };

#endif // C_UPNPXMLEVENTPARSER_H_

// End of File
