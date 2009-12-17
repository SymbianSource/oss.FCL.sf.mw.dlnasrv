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
* Description:  UPnP Error - this is internal class that converts 
*                TInt to TUpnpErrorCode it helps to avoid excessive casting                    
*
*/

#ifndef M_UPNPCDSSENDER
#define M_UPNPCDSSENDER

#include "upnperrors.h"

class CUpnpAction;
class CUpnpHttpMessage;


// Sender interface
class MUpnpCdsSender
{
 public:
    virtual void SendL(CUpnpAction* aAction, TUpnpErrorCode aError)=0;
    virtual void SendL( CUpnpHttpMessage* aMessage )=0;
};

#endif