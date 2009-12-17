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
* Description:      The factory for MdHClient
*
*/






#include "mdhclientfactory.h"
#include "mmdhclient.h"
#include "mdhclient.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
EXPORT_C MCmMdhClient* CCmMdhClientFactory::NewCmMdhClientL( MCmServiceObserver& aServer )
    {
    return CCmMdhClient::NewL( aServer );
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//    
EXPORT_C MCmMdhClient* CCmMdhClientFactory::NewCmMdhClientLC( MCmServiceObserver& aServer )
    {
    return CCmMdhClient::NewLC( aServer );
    }

// end of file
