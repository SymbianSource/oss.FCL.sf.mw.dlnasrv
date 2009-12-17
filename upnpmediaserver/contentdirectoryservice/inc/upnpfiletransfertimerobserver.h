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
* Description:  Implements the upnpfiletransfertimerobserver class
*
*/



#ifndef C_CUPNPFILETRANSFERTIMEROBSERVER_H
#define C_CUPNPFILETRANSFERTIMEROBSERVER_H

//  INCLUDES
#include <e32std.h>
#include "upnpnotifytimer.h"

// FORWARD DECLARATIONS
class CUpnpContentDirectory;
class MUpnpContentDirectoryEventObserver;
class CUpnpFileTransferTimerObserver;

const TInt KFileTransferStorageTime = 30000000;

class MUpnpFileTransferTimerObserver
    {
public:
   /**
    * Callback for filetransfer timer events
    * @since Series 60 2.6
    * @return Error code
    */
    virtual void TransferEvent( CUpnpFileTransferTimerObserver* aTimer ) = 0;
    };


/**
*  This is a observer for timer events. This is needed since CDS allready
*  is a observer for statevariable events.
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class CUpnpFileTransferTimerObserver : public CBase, public MUpnpNotifyTimerObserver
{
public: // Constructors and destructors

    /**
    * Two-phased constructor for creating FiletransferTimerObsever
    * @since Series60 2.6
    * @param aService Parent service 
    */
    static CUpnpFileTransferTimerObserver* NewLC( MUpnpFileTransferTimerObserver* aCd, TInt aTransferId);

    /**
    * Two-phased constructor for creating FiletransferTimerObsever
    * @since Series60 2.6
    * @param aService Parent service 
    */
    //static CUpnpFileTransferTimerObserver* NewLC(CUpnpContentDirectory* aCd);

    /**
    * Destructor.
    */
    virtual ~CUpnpFileTransferTimerObserver();

private:
    /**
    * C++ default constructor.
    */
    CUpnpFileTransferTimerObserver( MUpnpFileTransferTimerObserver* aCd, TInt aTransferId );
    
    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL();

public: //from NotifyTimerObserver
    void TimerEventL( CUpnpNotifyTimer* aTimer );
    TInt TransferId();
    
private: //data
    // Notify timer, owned
    MUpnpFileTransferTimerObserver* iCd;
    CUpnpNotifyTimer*    iEventTimer;
    TInt iTransferId;
    
    };

#endif      // C_CUPNPFILETRANSFERTIMEROBSERVER_H   
        
// End of File