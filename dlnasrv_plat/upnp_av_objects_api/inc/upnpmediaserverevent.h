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
* Description:  Upnp Media Server event
*
*/


#ifndef C_CUPNPMEDIASERVEREVENT_H
#define C_CUPNPMEDIASERVEREVENT_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION
/**
*  Base class for Media Server events
*  @since Series 60 3.1
*/
class CUpnpMediaServerEvent : public CBase
{
public:
    /**
    * Constructor
    */
    CUpnpMediaServerEvent(TInt aType);

    /**
    * Destructor
    */
    virtual ~CUpnpMediaServerEvent();

    /**
    * Gets type of the event
    */
    TInt GetEventType();

    /**
    * Internalizes object
    */
    virtual void InternalizeL( RReadStream& aStream ) = 0;

    /**
    * Externalizes object
    */
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;


protected:

    //Event type of derived class
    TInt iEventType;

};

#endif // C_CUPNPMEDIASERVEREVENT_H
