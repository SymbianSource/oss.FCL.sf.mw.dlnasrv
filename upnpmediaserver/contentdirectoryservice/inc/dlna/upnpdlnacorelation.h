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
* Description:  Dlna corelation variables
*
*/



#ifndef C_TUPNPDLNACORELATION_H
#define C_TUPNPDLNACORELATION_H

//  INCLUDES
#include <e32base.h>

//FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  Stores dlna corelation info
*
*  @since Series60 5.0
*/
struct TUpnpDlnaCorelation
    {
    TBool iStreamingSupport;
    TBool iInteractiveSupport;
    TBool iBackgrondSupport;
    TBool iGetContentFeaturesExist;
    TBool iGetContentFeaturesIsOK;
    TPtrC8 iFourthField;
    };

#endif      // C_TUPNPDLNACORELATION_H   
// End of File