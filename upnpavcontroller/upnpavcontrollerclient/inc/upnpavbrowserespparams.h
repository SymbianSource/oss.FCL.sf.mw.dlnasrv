/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a data class containing browse and search response
*                parameters
*
*/






#ifndef T_TUPNPAVBROWSERESPPARAMS_H
#define T_TUPNPAVBROWSERESPPARAMS_H


#include <e32base.h>
#include <s32mem.h>

#include "upnpavbrowsingsession.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  UPnP AV Controller Client/Server communication helper class
*  
*
*  @lib - 
*  @since Series 60 3.1
*/

class TUpnpAVBrowseRespParams
    { 
       
public: // Public data members
    
    TInt iResponseSize;
    TInt iUpdateIdSize;
    TInt iMatches;
    TInt iTotalCount;
        
    };


#endif // T_TUPNPAVBROWSERESPPARAMS_H
