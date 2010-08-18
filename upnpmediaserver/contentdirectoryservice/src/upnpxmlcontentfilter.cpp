/** @file
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "upnpxmlcontentfilter.h"

#include <e32svr.h>

#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/dom/xmlengnodefilter.h>
#include <xml/dom/xmlengdomparser.h>

#include <charconv.h>
#include <utf.h>

using namespace Xml;
              
               
// -----------------------------------------------------------------------------
// CUpnpXmlContentFilter::AcceptNode
// -----------------------------------------------------------------------------
//               
TXmlEngNodeFilterResult CUpnpXmlContentFilter::AcceptNode( TXmlEngNode aNode )
    {
    if ( aNode.NodeType() == TXmlEngNode::ECDATASection ||
         aNode.NodeType() == TXmlEngNode::EComment )
        {
        return EReject;
        }
    return EAccept;
    }
    
// -----------------------------------------------------------------------------
// CUpnpXmlContentFilter::SerializeLC
// -----------------------------------------------------------------------------
//     
HBufC8* CUpnpXmlContentFilter::SerializeLC(RXmlEngDocument& aDoc, TXmlEngElement& aElement )
    {
    CUpnpXmlContentFilter* nodefilter = new (ELeave) CUpnpXmlContentFilter();
    CleanupStack::PushL(nodefilter);
    
    RBuf8 buf;
	    CleanupClosePushL( buf );               

    TXmlEngSerializationOptions options(  TXmlEngSerializationOptions::KOptionOmitXMLDeclaration
                          | TXmlEngSerializationOptions::KOptionIncludeNsPrefixes);	        			  
	options.SetNodeFilter(nodefilter);	    	  
    aDoc.SaveL( buf, aElement, options );
    
    HBufC8* res = buf.AllocL();
    CleanupStack::PopAndDestroy( &buf );
    CleanupStack::PopAndDestroy( nodefilter );
    CleanupStack::PushL(res);
    
    return res;        
    }

// end of file
