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
* Description:  UPnP Error - this is internal class that converts 
*                TInt to TUpnpErrorCode it helps to avoid excessive casting                    
*
*/



#ifndef C_TUPNPERROR_H
#define C_TUPNPERROR_H

//  INCLUDES
#include <e32base.h>
#include "upnperrors.h"

// CLASS DECLARATION

/**
*  Convers TInt to TUpnpErrorCode
*
*  @lib 
*  @since Series60 2.6
*/
class TUpnpError
{
public: 
    /**
    * Constructor
    */
    TUpnpError(TInt aErrCode);
        
    /**
    * Convertion operator
    */
    operator TUpnpErrorCode();
    
private: 
    TUpnpErrorCode iUpnpErrCode;
};

#endif      // C_TUPNPERROR_H   
        
// End of File
