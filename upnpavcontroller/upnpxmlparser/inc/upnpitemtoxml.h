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
* Description:      Generates XML from an UPNP Item
*
*/






#ifndef C_UPNPITEMTOXML_H
#define C_UPNPITEMTOXML_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CONSTANTS
// None

// MACROS
// None

// FORWARD DECLARATIONS
class CUpnpItem;

/**
 * Converts an UPnP item to xml document
 *
 * @since Series 60 3.2
 * @lib upnpxmlparser.lib
 */
NONSHARABLE_CLASS( CUpnpItemToXML ) : public CBase
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     *
     * @param aItem
     * @return new instance
     */
    static CUpnpItemToXML* NewL( const CUpnpItem& aItem );
    
    /**
     * Two-phased constructor.
     *
     * @param aItem
     * @return new instance
     */
    static CUpnpItemToXML* NewLC( const CUpnpItem& aItem );

    /**
     * Destructor.
     */
    virtual ~CUpnpItemToXML();


public: // Functions from CUPnPObject

    /**
     * Returns object's XML description
     * Leaves in case of errors.
     * @since Series 60 3.1
     * @param none
     * @return HBufC8 pointer to buffer containing XML data
     */
    HBufC8* AsXmlL( const TBool aIncludeChilds = ETrue );

    /**
     * Returns object's XML description. This version of the method
     * is used to create the XML with empty <res> tag
     * Leaves in case of errors.
     * @since Series 60 3.1
     * @param None
     * @return HBufC8 pointer to buffer containing XML data
     */
    HBufC8* AsXmlEmptyL();


    /**
     * CUpnpItemToXML::AsResultArgumentL
     * Returns object's XML description that is embedded inside a
     * DIDL-LITE tag. The <res> tag of xml description is empty.
     * The returned value is xml encoded can therefore be used
     * for example when creating a CreateObject action.
     * @since Series 60 3.1
     * @param none
     * @return TDesC8& the date string
     */
    HBufC8* AsResultArgumentL();

    /**
     * Creates valid XML headers to source data. Do not decode the XML.
     *
     * @since Series 60 3.1
     * @param TDesC8 reference to source data
     * @return HBufC8 buffer to valid XML
     */
    HBufC8* CreateUnDecodedXmlL( const TDesC8& aData );

private:

    /**
     * Return value from an element
     *
     * @param aElemenName (const TDesC8&) name of the element
     * @return (const TDesC8&) value of the element
     */
    const TDesC8& GetValueFromElement(
                                const TDesC8& aElementName );

    /**
     * Validates format of dc:date
     *
     * @param aDate dc:date string
     * @return ETrue if format of dc:date is valid
     */
    TBool ValidateDateL( const TDesC8& aDate );
    
    /**
     * Validates format of res@duration
     *
     * @param aDuration res@duration string
     * @return ETrue if format of res@duration is valid
     */
    TBool ValidateDurationL( const TDesC8& aDuration );
    
    /**
     * Parse to a delimeter
     *
     * @param aLexer lexer
     * @param aDelimeter delimeter character
     */
    void ParseToDelimeter( TLex8& aLex, TChar aDelimeter );    

private:

    /**
     * C++ default constructor.
     */
    CUpnpItemToXML( const CUpnpItem& aItem );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data

    CUpnpItem*  iItem; // Not own
    };

#endif      // C_UPNPITEMTOXML_H
            
// End of File
