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
* Description:  Element table data handler
*
*/



#ifndef C_CUPNPAUTODESTROYOBJECT_H
#define C_CUPNPAUTODESTROYOBJECT_H

//  INCLUDES
#include <e32base.h>
#include <upnpnotifytimer.h>

//FORWARD DECLARATIONS
class CUpnpContentDirectory;
class CUpnpNotifyTimer;


// CLASS DECLARATION

/**
*  Auto destroy object
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpAutoDestroyObject: public CBase, public MUpnpNotifyTimerObserver
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpAutoDestroyObject* NewL(CUpnpContentDirectory* aCd, TInt aObjId, TInt aResId );

    /**
    * Two-phased constructor.
    */
    static CUpnpAutoDestroyObject* NewLC(CUpnpContentDirectory* aCd, TInt aObjId, TInt aResId );
    
    
    /**
    * Destructor.
    */
    virtual ~CUpnpAutoDestroyObject();

public: // New functions
    virtual void TimerEventL(CUpnpNotifyTimer* aTimer);
    TInt GetResId();
    TInt GetObjId();
    void SetTransferStartedL(TBool aVal);
    void RenewTimerL();    

private:

    /**
    * C++ default constructor.
    */
    CUpnpAutoDestroyObject(CUpnpContentDirectory* aCd, TInt aObjId, TInt aResId);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:    // Data
    CUpnpContentDirectory*      iCd;        // not owned
    CUpnpNotifyTimer*           iEventTimer;
    TInt                        iResId;
    TInt                        iObjectId;
    TBool                       iTransferStarted;
    TBool                       iTimeElapsed;
};

#endif      // C_CUPNPAUTODESTROYOBJECT_H   
        
// End of File
