/** @file
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  class for sending/receiving HTTP request/response 
*                using Symbian HTTP client 
*
*/


#ifndef C_CUPNPTRANSFERBASE_H
#define C_CUPNPTRANSFERBASE_H

// INCLUDES
#include <e32base.h>
#include "httptransferobserver.h"
#include "upnptransferobserver.h"

// FORWARD DECLARATIONS
class CUpnpContentDirectory;
class CHttpTransferBase;
class MUpnpTransferObserver;

const TInt KDefaultAccessPoint = 11;
const TInt KDefaultBuffer = 64000;
const TInt KNumOfConcurrentTransactions = 2;

class CUpnpTransferBase: public CBase , public MHttpTransferObserver
{
public:  // Constructors and destructor
    
    /**
    * Destructor.
    */
    virtual ~CUpnpTransferBase();

// from base class  MHttpTransferObserver

   virtual void ReadyForTransferL( TAny* aKey);
     
   /**
    * Gets transfer progress callback
    *
    * @since S60 v3.2
    * @param aKey the file identifier
    * @param aBytes transferred
    * @param aTotalBytes all bytes to be transferred
    */
   virtual void TransferProgress( TAny* aKey, 
                                  TInt aBytes, 
                                  TInt aTotalBytes );
                                   
   virtual void TransferCompleted( TAny* aKey, TInt aStatus );     




public: // New functions

     /**
     * Creates new CHttpFile object and adds it to the iFileQueue
     *
     * @since S60 v3.2
     * @param aUri Uri of the downloaded object
     * @param aTargetPath Local path of the object
     * @param aKey Identifier of the object
     */
    void CreateAndQueueHttpFileL( const TDesC8& aUri, 
                                  const TDesC& aTargetPath, 
                                  TInt aKey );
    
    /**
    * Sets a header
    *
    * @since S60 v3.2
    * @param aKey the file identifier
    * @param aFieldName The field name, e.g. 'Content-Type'
    * @param aFieldValue The field value. e.g. 'text/plain'
    */
    void SetHeaderL( TInt aKey,
                     const TDesC8& aFieldName, 
                     const TDesC8& aFieldValue );
    
    /**
    * Turns on/off transfer progress callbacks
    *
    * @since S60 v3.2
    * @param aKey the file identifier
    * @param aSwitchOn tuning on/off    
    */
    void TrackProgess(TInt aKey, TBool aSwitchOn);   
          

    /**
    * cancels transfer
    *
    * @since S60 v3.2
    * @param aKey the file identifier   
    */         
    virtual void CancelTransfer( TInt aKey);     
  
 protected: 
    /**
    * C++ default constructor.
    */
    CUpnpTransferBase( MUpnpTransferObserver* aCd );
    
    // new functions
    TInt MapStatus( TInt aErrorCode );
    
 protected:    // Data
    MUpnpTransferObserver* iObserver;
    CHttpTransferBase* iHttpTransferBase;
    
    
};

#endif // C_CUPNPTRANFERBASE_H
