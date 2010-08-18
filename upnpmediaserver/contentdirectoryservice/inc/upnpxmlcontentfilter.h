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
* Description:  Xml responses serialization filter
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengnodefilter.h>

//using namespace Xml;

/**
*  @brief Xml responses serialization filter
*
*  @lib ContentDirectory.lib
*  @since Series60 3.2
*/
class CUpnpXmlContentFilter : public CBase, MXmlEngNodeFilter
    {
public: // from MXmlEngNodeFilter

    /**
    * Filters node by type CDATA and comments are rejected
    * @since Series60 3.2
    * @param aNode node
    */    
    virtual TXmlEngNodeFilterResult AcceptNode(TXmlEngNode aNode);
    
public:
        
    /**
    * Aplies filter and serializes aElement to descriptor 
    * @since Series60 3.2
    * @param aService Parent service 
    */
    static HBufC8* SerializeLC(RXmlEngDocument& aDoc, TXmlEngElement& aElement );

    };
