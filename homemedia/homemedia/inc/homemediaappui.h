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
* Description:      Main Ui class
*
*/






#ifndef __HOMEMEDIAAPPUI_H__
#define __HOMEMEDIAAPPUI_H__

// INCLUDES
#include <aknViewAppUi.h> 

// FORWARD DECLARATIONS
class CHomeMediaAppView;

// CLASS DECLARATION
/**
 * ChomemediaAppUi application UI class.
 * Interacts with the user through the UI and request message processing
 * from the handler class
 */
class CHomeMediaAppUi : public CAknViewAppUi
    {
public:
    // Constructors and destructor

    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();

    /**
     * CHomeMediaAppUi.
     * C++ default constructor. This needs to be public due to
     * the way the framework constructs the AppUi
     */
    CHomeMediaAppUi();

    /**
     * ~CHomeMediaAppUi.
     * Virtual Destructor.
     */
    virtual ~CHomeMediaAppUi();

private:
    // Functions from base classes

    /**
     * From CEikAppUi, HandleCommandL.
     * Takes care of command handling.
     * @param aCommand Command to be handled.
     */
    void HandleCommandL(TInt aCommand);

    /**
    * From CEikAppUi, handles key events.
    * @param aType The type of resources that have changed
    */    
    virtual void HandleResourceChangeL( TInt aType );  

    /** 
    * From CCoeAppUi
    */
    void HandleForegroundEventL( TBool aForeground );  

    
private:
    // Data

    /**
     * The application view
     * Owned by CHomeMediaAppUi
     */
    CHomeMediaAppView* iAppView;

    };

#endif // __HOMEMEDIAAPPUI_H__
// End of File
