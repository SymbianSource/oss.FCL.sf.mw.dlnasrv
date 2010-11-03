/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP GStreamer wrapper constants
*
*/

#ifndef C_UPNPGSTWRAPPERCONSTS_H
#define C_UPNPGSTWRAPPERCONSTS_H

namespace Gst
    {
    
    enum TEvent
        {
        EUnknown                   = 1,
        EError                     = 2,
        EEOS                       = 3,
        EVideoStreamInfoAvailable  = 4,
        EAudioStreamInfoAvailable  = 5
        };
    
    }


#endif // C_UPNPGSTWRAPPERCONSTS_H

