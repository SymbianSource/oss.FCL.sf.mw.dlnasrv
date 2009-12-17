/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMediaServantApp class implementation
*
*/


#include <eikstart.h>
#include "msdebug.h"
#include "msapp.h"
#include "msdocument.h"
#include "msconstants.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// CMediaServantApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CMediaServantApp::CreateDocumentL()
    {
    LOG(_L("[MediaServant]\t CMediaServantApp::CreateDocumentL"));
        
    return CMSDocument::NewL( *this );
    }

// ---------------------------------------------------------
// CMediaServantApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CMediaServantApp::AppDllUid() const
    {
    LOG(_L("[MediaServant]\t CMediaServantApp::AppDllUid"));
    
    return KUidMediaServant;
    }


// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// NewApplication() 
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    LOG(_L("[MediaServant]\t CApaApplication* NewApplication"));    
    
    return new CMediaServantApp;
    }

// ---------------------------------------------------------------------------
// Starts application
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    LOG(_L("[MediaServant]\t E32Main"));    
    
    return EikStart::RunApplication( NewApplication );
    }


// End of File  

