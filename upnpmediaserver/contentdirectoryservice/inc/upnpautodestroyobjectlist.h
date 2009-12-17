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



#ifndef C_CUPNPAUTODESTROYOBJECTLIST_H
#define C_CUPNPAUTODESTROYOBJECTLIST_H

//  INCLUDES
#include <e32base.h>

//FORWARD DESCLARATIONS
class CUpnpAutoDestroyObject;

// CLASS DECLARATION

/**
*  Auto destroy object
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpAutoDestroyObjectList: public CBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpAutoDestroyObjectList* NewL();

    /**
    * Two-phased constructor.
    */
    static CUpnpAutoDestroyObjectList* NewLC();
    
    
    /**
    * Destructor.
    */
    virtual ~CUpnpAutoDestroyObjectList();

public: // New functions

    void AppendL(const CUpnpAutoDestroyObject* aObj);
    void RemoveAndDestroyL(TInt aIndex);
    TInt FindObjIndexByObjId(TInt aObjId);
    TInt FindObjIndexByResId(TInt aResId);
    CUpnpAutoDestroyObject* operator[](TInt aIndex);
    void TransferFailL(TInt aResId);
    TInt Count();


private:

    /**
    * C++ default constructor.
    */
    CUpnpAutoDestroyObjectList();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:

private:    // Data
    // main array
    RPointerArray<CUpnpAutoDestroyObject> iArray;
};

#endif      // C_CUPNPAUTODESTROYOBJECTLIST_H   
        
// End of File
