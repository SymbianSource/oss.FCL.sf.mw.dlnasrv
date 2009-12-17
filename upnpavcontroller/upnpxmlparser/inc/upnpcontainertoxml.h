/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Generates XML from a container
*
*/






#ifndef C_UPNPCONTAINERTOXML_H
#define C_UPNPCONTAINERTOXML_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CUpnpContainer;

// CLASS DECLARATION

/**
 * Converts a container to xml document
 *
 * @since Series 60 3.0
 * @lib upnpxmlparser.lib
 */
class CUpnpContainerToXML : public CBase
    {
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     *
     * @param aContainer container to convert
     * @return new instance
     */
    static CUpnpContainerToXML* NewL( const CUpnpContainer& aContainer );
    
    /**
     * Two-phased constructor.
     *
     * @param aContainer container to convert
     * @return new instance
     */
    static CUpnpContainerToXML* NewLC( const CUpnpContainer& aContainer );
    
    /**
     * Destructor.
     */
    virtual ~CUpnpContainerToXML();

public: // New functions
             
    /**
     * Returns object's XML description
     * Leaves in case of errors.
     *
     * @return HBufC8 pointer to buffer containing XML data
     */
    IMPORT_C HBufC8* AsXmlL();

    /**
     * Returns object's XML description
     * Leaves in case of errors.
     *
     * @return HBufC8 pointer to buffer containing XML data
     */
    IMPORT_C HBufC8* AsEmptyXmlL();

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
     * C++ default constructor.
     */
    CUpnpContainerToXML( const CUpnpContainer& aContainer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data

    const CUpnpContainer& iContainer; // Not own        

    };

#endif      // C_UPNPCONTAINERTOXML_H
            
// End of File
