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
* Description:  store file manager observer for images and videos
*
*/



#ifndef __CMSMVIDEOANDIMAGEMNGROBSERVER_H
#define __CMSMVIDEOANDIMAGEMNGROBSERVER_H


// CLASS DECLARATION
 /**
  * Video and image manager observer class definition
  *
  * @lib cmstoremanager.lib
  *
  * @since S60 v3.1
  */      
class MCmSmVideoAndImageMngrObserver
    {
public:

    /**
     * Call back for observer
     *
     * @since S60 5.1
     * @param aStatus, KErrNone if no errors, otherwise
     *                 system wide error codes
     * @return  None
     */    
    virtual void ImagesAndVideosReady( const TInt aStatus ) = 0;     
    };

#endif //  __CMSMVIDEOANDIMAGEMNGROBSERVER_H

