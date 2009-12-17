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
* Description:  File transfer event observer
*
*/




#ifndef C_CUPNPCONTENTDIRECTORYEVENTOBSERVER_H
#define C_CUPNPCONTENTDIRECTORYEVENTOBSERVER_H



// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CUpnpFileTransferEvent;

// CLASS DECLARATION

/** 
  File transfer event observer
  */
class MUpnpContentDirectoryEventObserver 
    {
public:
    virtual void FileTransferEvent(CUpnpFileTransferEvent *aMessage) = 0;   
    
    };


#endif // C_CUPNPCONTENTDIRECTORYEVENTOBSERVER_H