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
* Description:      Main document class
*
*/






#ifndef __HOMEMEDIADOCUMENT_H__
#define __HOMEMEDIADOCUMENT_H__

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CHomeMediaAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * ChomemediaDocument application class.
 * An instance of class ChomemediaDocument is the Document part of the
 * AVKON application framework for the homemedia example application.
 */
class CHomeMediaDocument : public CAknDocument
    {
public:
    // Constructors and destructor

    /**
     * NewL.
     * Two-phased constructor.
     * Construct a CHomeMediaDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CHomeMediaDocument.
     */
    static CHomeMediaDocument* NewL(CEikApplication& aApp);

    /**
     * NewLC.
     * Two-phased constructor.
     * Construct a CHomeMediaDocument for the AVKON application aApp
     * using two phase construction, and return a pointer
     * to the created object.
     * @param aApp Application creating this document.
     * @return A pointer to the created instance of CHomeMediaDocument.
     */
    static CHomeMediaDocument* NewLC(CEikApplication& aApp);

    /**
     * ~CHomeMediaDocument
     * Virtual Destructor.
     */
    virtual ~CHomeMediaDocument();

public:
    // Functions from base classes

    /**
     * CreateAppUiL
     * From CEikDocument, CreateAppUiL.
     * Create a ChomemediaAppUi object and return a pointer to it.
     * The object returned is owned by the Uikon framework.
     * @return Pointer to created instance of AppUi.
     */
    CEikAppUi* CreateAppUiL();
    
public:
    // New functions
    
    /**
     * Registers the destructor key of ecom plugin implementation. 
     * Ecom plugin implementation will be destroyed in the destructor
     * of the document object, after the appui object (and its views)
     * has been destructed.
     * 
     * Method check if the given TUid is already registered. 
     * Then it is not registered.
     * 
     * @param aDestructorKey A key specifying the ecom plugin implementation.
     */
    void RegisterEcomDestructorKeyL( const TUid& aDestructorKey );

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CHomeMediaDocument.
     * C++ default constructor.
     * @param aApp Application creating this document.
     */
    CHomeMediaDocument(CEikApplication& aApp);
    
private:
    
    /**
     * List of ecom destructor keys.
     */
    RArray<TUid> iEcomDestructorKeys;

    };

#endif // __HOMEMEDIADOCUMENT_H__
// End of File
