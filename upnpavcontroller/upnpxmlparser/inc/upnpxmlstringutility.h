/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      String Utility for UPnPXMLParse.
*
*/





#ifndef UPNPXMLSTRINGUTILITY_H_
#define UPNPXMLSTRINGUTILITY_H_

// INCLUDES

// FORWARD DECLARATIONS
class HBufC8;
class TDesC8;

// CLASS DECLARATION

/**
* Utility for working with string operation.The class
* contains static methods that can be used to modify xmldata.
* @since S60 3.2
* @lib upnpxmlparser.lib
*/
class UpnpXmlStringUtility
    {

public:

    /**
     * Remove control characters from xml data
     * @param aXmlData, a buffer for xml data.
     * @return a buffer without control characters that are of form &#XXXX;
     *         where XXXX is an integer number.
     *         If no control character return NULL.
     */
    IMPORT_C static HBufC8* RemoveXmlControlCharactersL(
        const TDesC8& aXmlData );

   };

#endif // UPNPXMLSTRINGUTILITY_H_

// End of File