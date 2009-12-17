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
* Description:  Container UpdateID
*
*/



#ifndef C_CUPNPCONTAINERUPDATEID_H
#define C_CUPNPCONTAINERUPDATEID_H

#include <e32std.h>




/**
*  Storage Struct for conainerupdateId
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class TUpnpContainerUpdateId
{
public:
    // Constructors and destructor
    
    
    /*
    * C++ default constructor.
    */
    TUpnpContainerUpdateId();

public:
    // Data

    
    // Container Id
    HBufC8* iContainerId;
    

    TInt    iUpdateId;
};

#endif      // C_CUPNPCONTAINERUPDATEID_H   
        
// End of File