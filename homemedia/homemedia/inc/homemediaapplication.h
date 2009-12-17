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
* Description:      Main application class
*
*/






#ifndef __HOMEMEDIAAPPLICATION_H__
#define __HOMEMEDIAAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "homemedia.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidhomemediaApp =
    {
    _UID3
    };

// CLASS DECLARATION

/**
 * ChomemediaApplication application class.
 * Provides factory to create concrete document object.
 * An instance of ChomemediaApplication is the application part of the
 * AVKON application framework for the homemedia example application.
 */
class CHomeMediaApplication : public CAknApplication
    {
public:
    // Functions from base classes

    /**
     * From CApaApplication, AppDllUid.
     * @return Application's UID (KUidhomemediaApp).
     */
    TUid AppDllUid() const;

protected:
    // Functions from base classes

    /**
     * From CApaApplication, CreateDocumentL.
     * Creates ChomemediaDocument document object. The returned
     * pointer in not owned by the CHomeMediaApplication object.
     * @return A pointer to the created document object.
     */
    CApaDocument* CreateDocumentL();
    };

#endif // __HOMEMEDIAAPPLICATION_H__
// End of File
