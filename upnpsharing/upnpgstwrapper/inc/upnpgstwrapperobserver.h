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
* Description:  UPnP GStreamer wrapper observer
*
*/

#ifndef C_UPNPGSTWRAPPEROBSERVER_H
#define C_UPNPGSTWRAPPEROBSERVER_H

#include "upnpgstwrapperconsts.h"

class MUpnpGstWrapperObserver
    {       

public:

    /**
     * Indicates GStreamer event
     * @param Gst::TEvent A event to be handled
     *
     */
    virtual void HandleGstEvent( Gst::TEvent aEvent ) = 0;        
    
    };


#endif // C_UPNPGSTWRAPPEROBSERVER_H

