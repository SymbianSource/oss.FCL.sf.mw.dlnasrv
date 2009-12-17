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
* Description:  
*
*/



#ifndef C_CUPNPTRANSFERCONTROLLER_H
#define C_CUPNPTRANSFERCONTROLLER_H

//  INCLUDES
#include <e32std.h>
#include "upnpnotifytimer.h"
#include "upnperrors.h"
#include "upnpfiletransfer.h"
#include "upnpfiletransfertimerobserver.h"

// FORWARD DECLARATIONS
class CUpnpContentDirectory;
class CUpnpFileTransferEvent;
class CUpnpHttpMessage;

/**
*  This is a class controlling upload and downloads
*
*  @lib ContentDirectory
*  @since Series 60 2.6
*/
class CUpnpTransferController: public CBase, public MUpnpFileTransferTimerObserver
{
public: // Constructors and destructors

    /**
    * Two-phased constructor 
    * @since Series60 2.6
    * @param aService Parent service 
    */
    static CUpnpTransferController* NewL( CUpnpContentDirectory* aCd );


    /**
    * Destructor.
    */
    virtual ~CUpnpTransferController();

     /**
    * This functions is a entry point for http messages.
    * received from Symbian HTTP Stack
    * @since Series 60 3.2
    * @param aSessionId identifier of the HTTP request.
    * @param aHttpStatusCode HTTP error code.
    */
    void HttpResponseReceivedL( TInt aSessionId,
                                TInt aStatusCode);   
                                
    /** 
    * Function for stopping ongoing file transfer (import / export)
    * @since Series 60 2.6
    * @param aAction received action
    * @retrun upnperrorcode descriping situation
    **/
    TUpnpErrorCode StopTransferResourceL( TInt aId ); 
    
    
    /**
    * This function return a file transfer from CDs list.
    * @since Series 60 2.6
    * @param aTransferId transferid to be checked
    * @return TUpnpFileTransfer.
    */
    TInt Transfer( TInt aTransferId );
    
    /**
    * This functions returns export/import file transfers which are in progress state       
    * @param aResult number of file transfers in progress state 
    */
    TBool IsMaxImportExportTransfers();
        
    /**
    * This function removes a file transfer event from list and return it.
    * @since Series 60 2.6
    * @param aTransferId transferid to be checked
    * @return TUpnpFileTransfer.
    */
    CUpnpFileTransferEvent* RemoveTransferLC( TInt aTransferId );
    
    CUpnpFileTransferEvent* TransferL( TInt aTransferId );

    /**
    * Callback for filetransfer timer events
    * @since Series 60 2.6
    * @return Error code
    */
    void TransferEvent( CUpnpFileTransferTimerObserver* aTimer );
    
     /**
    * Append transfer to transfer list for eventing purpose        
    * @param aHttpMsg 
    */
    void AddIncomingTransferInternalL(CUpnpHttpMessage* aHttpMsg);
    
    void TransferFailL(CUpnpHttpMessage* aHttpMsg);

     /**
    * Sends transfer failed event to Media Server       
    * @param aHttpMsg 
    */
    void NotifyTransferFailL(CUpnpHttpMessage* aHttpMsg);
                                
private:
    /**
    * C++ default constructor.
    */
    CUpnpTransferController( CUpnpContentDirectory* aCd );
    
    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL();
    


public: 

    //array for filetransfers  
    RPointerArray<CUpnpFileTransfer> iFileTransfers;
    
    
    RPointerArray<CUpnpFileTransferTimerObserver> iStatusTimers;

    
private: 
    CUpnpContentDirectory* iContentDirectory;
           
    };

#endif      // C_CUPNPTRANSFERCONTROLLER_H 
        
// End of File