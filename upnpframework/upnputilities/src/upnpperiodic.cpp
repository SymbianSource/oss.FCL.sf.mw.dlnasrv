/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      UPnP framework custom periodic timer
*
*/






#include "upnpperiodic.h"



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CUPnPPeriodic::CUPnPPeriodic
// Constructor.
// -----------------------------------------------------------------------------
//
CUPnPPeriodic::CUPnPPeriodic( TInt aPriority ) : 
    CPeriodic( aPriority )
    {
    }


// -----------------------------------------------------------------------------
// CUPnPPeriodic::CUPnPPeriodic
// Destructor.
// -----------------------------------------------------------------------------
//
CUPnPPeriodic::~CUPnPPeriodic()
    {
    Cancel();
    }


// -----------------------------------------------------------------------------
// CUPnPPeriodic::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUPnPPeriodic::ConstructL()
    {
    CPeriodic::ConstructL();
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CUPnPPeriodic::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUPnPPeriodic* CUPnPPeriodic::NewL( TInt aPriority )
    {
    CUPnPPeriodic* self = new( ELeave ) CUPnPPeriodic( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CUPnPPeriodic::RunError
// Called if CUPnPPeriodic function RunL() leaves.
// -----------------------------------------------------------------------------
//
TInt CUPnPPeriodic::RunError( TInt aError )

    {
  
    if( aError == KErrNoMemory )
        return KErrNone;
    else
        return aError;
    }
    
// End of file
