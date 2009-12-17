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






// INCLUDE FILES
#include "homemediadocument.h"
#include "homemediaapplication.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CHomeMediaApplication::CreateDocumentL()
// Creates CApaDocument object
// --------------------------------------------------------------------------
//
CApaDocument* CHomeMediaApplication::CreateDocumentL()
    {
    // Create an homemedia document, and return a pointer to it
    return CHomeMediaDocument::NewL(*this);
    }

// --------------------------------------------------------------------------
// CHomeMediaApplication::AppDllUid()
// Returns application UID
// --------------------------------------------------------------------------
//
TUid CHomeMediaApplication::AppDllUid() const
    {
    // Return the UID for the homemedia application
    return KUidhomemediaApp;
    }

// End of File
