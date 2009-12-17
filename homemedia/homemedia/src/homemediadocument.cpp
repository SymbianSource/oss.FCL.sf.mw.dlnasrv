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






// INCLUDE FILES
#include "homemediaappui.h"
#include "homemediadocument.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CHomeMediaDocument::NewL()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CHomeMediaDocument* CHomeMediaDocument::NewL( CEikApplication& aApp )
    {
    CHomeMediaDocument* self = NewLC( aApp );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CHomeMediaDocument::NewLC()
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CHomeMediaDocument* CHomeMediaDocument::NewLC( CEikApplication& aApp )
    {
    CHomeMediaDocument* self = new ( ELeave ) CHomeMediaDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CHomeMediaDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CHomeMediaDocument::ConstructL()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CHomeMediaDocument::CHomeMediaDocument()
// C++ default constructor can NOT contain any code, that might leave.
// --------------------------------------------------------------------------
//
CHomeMediaDocument::CHomeMediaDocument( CEikApplication& aApp ) :
    CAknDocument( aApp ),
    iEcomDestructorKeys( 1 )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CHomeMediaDocument::~CHomeMediaDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CHomeMediaDocument::~CHomeMediaDocument()
    {
    // Destroyes registered ecom plugin implementations.
    TInt count = iEcomDestructorKeys.Count();
    if ( count )
        {
        for ( TInt i = 0; i < count; i++ )
            {
            REComSession::DestroyedImplementation( iEcomDestructorKeys[i] );
            }
        }
    iEcomDestructorKeys.Close();
    
    // Tears down ECom framework. Probably application enviroment does the same
    // thing but it doesn't cause any harm it it is here as well.
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CHomeMediaDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CHomeMediaDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return new ( ELeave )CHomeMediaAppUi;
    }

// ---------------------------------------------------------------------------
// CHomeMediaDocument::RegisterEcomDestructorKeyL()
// ---------------------------------------------------------------------------
//
void CHomeMediaDocument::RegisterEcomDestructorKeyL( const TUid& aDestructorKey )
    {
    // Checks the given destructor key, does it exist already.
    TInt idx = KErrNotFound;
    TInt count = iEcomDestructorKeys.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iEcomDestructorKeys[i] == aDestructorKey )
            {
            // Given TUid exists already.
            idx = i;
            break;
            }
        }
    
    if ( idx <= KErrNotFound )
        {
        // Appends new TUid in the list.
        iEcomDestructorKeys.AppendL( aDestructorKey );
        }
    }

// End of File
