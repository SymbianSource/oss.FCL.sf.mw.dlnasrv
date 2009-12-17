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
* Description:  Element table data handler
*
*/



#ifndef C_CUPNPSENDER_H
#define C_CUPNPSENDER_H

//  INCLUDES
#include "upnperrors.h"
#include "upnpcdssender.h"
#include <e32base.h>

class CUpnpAction;
class CUpnpHttpMessage;
class CUpnpContentDirectory;

// CLASS DECLARATION

/**
*  Stores the data of object table row.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpCdsSender: public CBase , public MUpnpCdsSender
{
    public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpCdsSender* NewLC(CUpnpContentDirectory* aCd);
    static CUpnpCdsSender* NewL(CUpnpContentDirectory* aCd);
    
    /**
    * Destructor.
    */
    virtual ~CUpnpCdsSender();

    public: // New functions
    /**
    * Sets 
    * @since Series S60 3.0
    * @param aRowSet
    */
    void SendL(CUpnpAction* aAction, TUpnpErrorCode aError);
    void SendL( CUpnpHttpMessage* aMessage );


 private: 
    /**
    * C++ default constructor.
    */
    CUpnpCdsSender(CUpnpContentDirectory* aCd);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

 private:    // Data
    CUpnpContentDirectory* iCd;
        
};

#endif      // C_CUPNPSENDER_H   
// End of File
