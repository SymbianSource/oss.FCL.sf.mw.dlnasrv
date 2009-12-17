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



// INCLUDE FILES
#include "upnptablebean.h"
#include <d32dbms.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpTableBean::CUpnpTableBean
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpTableBean::CUpnpTableBean()
{

}
// -----------------------------------------------------------------------------
// CUpnpTableBean::~CUpnpTableBean
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpTableBean::~CUpnpTableBean()
{

}


// -----------------------------------------------------------------------------
// CUpnpTableBean::ReadLongTextColL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpTableBean::ReadLongTextColL(const RDbRowSet& aRowSet, const TInt aColNo)
{
    // prepare buffer
    TInt colLen = aRowSet.ColLength(aColNo);
    HBufC8* tmp = HBufC8::NewLC(colLen);
    TPtr8 tmpPtr(tmp->Des());
    
    // read 
    RDbColReadStream colReadStream;
    colReadStream.OpenLC(aRowSet, aColNo);
    colReadStream.ReadL(tmpPtr, colLen); // as the length of the HBufC can be grater than requested
    
    // clean up 
    CleanupStack::PopAndDestroy(&colReadStream);
    CleanupStack::Pop(tmp);
    
    return tmp;
}

//  End of File  
