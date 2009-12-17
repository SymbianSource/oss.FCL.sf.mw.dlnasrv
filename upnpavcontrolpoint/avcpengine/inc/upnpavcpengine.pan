/** @file
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of AVCPEngine panic codes
*
*/


#ifndef AVCPENGINE_PAN
#define AVCPENGINE_PAN

/** Panic Category */
_LIT(KAVCPEngine, "AVCPEngine");

/** AVCPEngine panic codes */
enum TAVCPEnginePanic
    {
    EAVCPEngineGenerealPanic = 0,
    EAVCPEngineBadRequest = 1    
    };
#endif // AVCPENGINE_PAN
