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



#ifndef C_CUPNPTABLEBEAN_H
#define C_CUPNPTABLEBEAN_H

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h>


// CLASS DECLARATION

/**
*  Base class for tables' bean classes.
*
*  @lib ContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpTableBean : public CBase
{
 public: 

    // marks the value of the member has not yet been set i.e. has no value
    static const TInt KUndef = -1;

 protected: // New functions

    /**
    * Destructor.
    */
    virtual ~CUpnpTableBean();
        
    /**
    * C++ default constructor.
    */
    CUpnpTableBean();

    /**
    * Reads the LONG VARCHAR (EDbColLongText) column's value.
    * @since Series ?XX ?SeriesXX_version
    * @param RowSet
    * @param aColNo
    * @return descriptors pointer
    */
    HBufC8* ReadLongTextColL(const RDbRowSet& aRowSet, const TInt aColNo);
};

#endif      // C_CUPNPTABLEBEAN_H
        
// End of File