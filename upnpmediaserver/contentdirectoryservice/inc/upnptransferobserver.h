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
* Description:  
*
*/



#ifndef C_CUPNPTRANSFEROBSERVER_H
#define C_CUPNPTRANSFEROBSERVER_H

//  INCLUDES
#include <e32std.h>

// Transfer interface for handling transfer
class MUpnpTransferObserver
    {
 public:                                                          
    virtual void TransferCompletedL( TInt akey, TInt aResultCode ) = 0;   
    
    virtual void TransferProgress( TInt akey, 
                                   TInt aBytes,
                                   TInt aTotalBytes) = 0;                                      
    };

#endif      // C_CUPNPTRANSFEROBSERVER_H 
        
// End of File