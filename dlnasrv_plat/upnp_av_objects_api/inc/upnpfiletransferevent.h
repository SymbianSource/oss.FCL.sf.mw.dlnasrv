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
* Description:  Upnp Media Server file transfer event
*
*/


#ifndef C_CUPNPFILETRANSFEREVENT_H
#define C_CUPNPFILETRANSFEREVENT_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include "upnpmediaserverevent.h"


// FORWARD DECLARATIONS
class CUpnpMediaServerEvent;


// CLASS DECLARATION

/**
 *  Class encapsulates file transfer message
 *
 *  @lib MediaServerClient.lib
 *  @since S60 v3.1
 */
class CUpnpFileTransferEvent : public CUpnpMediaServerEvent
{
public:
    enum TProgressState
        {
	    EStarted,
	    ECompleted,
	    EFailed,
	    EStopped
        };
        
    enum TDirection
        {   
        EIncoming,
        EOutgoing,        
        };    
public:
	
	/**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpFileTransferEvent* NewL();

    /**
    * Two-phased constructor.
    */
    IMPORT_C static CUpnpFileTransferEvent* NewLC();
    
    /**
    * Destructor.
    */
    virtual ~CUpnpFileTransferEvent();
    
    
    /**
    * Gets progress state of transfer
    * @since Series 60 3.1
    * @param none
    * @return progress state
    */
    IMPORT_C TProgressState ProgressState();
 
    /**
    * Sets progress state of transfer
    * @since Series 60 3.1
    * @param aState progress state
    * @return none
    */
    IMPORT_C void SetProgressState( TProgressState aState );
 
    /**
    * Gets transfer direction
    * @since Series 60 3.1
    * @param none
    * @return progress state
    */
    IMPORT_C TDirection Direction();
 
    /**
    * Sets transfer direction
    * @since Series 60 3.1
    * @param aState progress state
    * @return none
    */
    IMPORT_C void SetDirection( TDirection aState );
    
    /**
    * Gets content transfer identifier
    * @since Series 60 3.1
    * @param none
    * @return transfer identifier
    */    
    IMPORT_C TInt TransferId();

    /**
    * Sets transfer identifier
    * @since Series 60 3.1
    * @param aTransferId transfer identifier
    * @return none
    */    
    IMPORT_C void SetTransferId( TInt aTransferId );

    /**
    * Gets content directory object identifier
    * associated with transfer
    * @since Series 60 3.1
    * @param none
    * @return object identifier 
    */    
    IMPORT_C TInt ContentDirectoryObjectId();

    /**
    * Sets content directory object identifier
    * associated with transfer
    * @since Series 60 3.1
    * @param aObjectId object identifier
    * @return none  
    */    
    IMPORT_C void SetContentDirectoryObjectId( TInt aObjectId );

    /**
    * Gets file path 
    * @since Series 60 3.1
    * @param none
    * @return file path
    */    
    IMPORT_C TDesC& FilePath();

    /**
    * Gets file path 
    * @since Series 60 3.1
    * @param aFile file path
    * @return none 
    */    
    IMPORT_C void SetFilePathL(const TDesC& aFile);
    
    /**
    * Gets error code
    * @since Series 60 3.1
    * @param none
    * @return transport specific error code
    */    
    IMPORT_C TInt ErrorCode();
    
    /**
    * Sets error code
    * @since Series 60 3.1
    * @param aError transport specific error code
    * @return none
    */    
    IMPORT_C void SetErrorCode( TInt aError );
    
    /**
    * Sets error code
    * @since Series 60 3.1
    * @param aError transport specific error code
    * @return none
    */    
    IMPORT_C CUpnpFileTransferEvent* CloneL();
    
    
    /**
    * Externalizes transfer event information to a stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
        
    /**
    * Internalizes transfer event information from a stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    IMPORT_C void InternalizeL( RReadStream& aStream );

	
private:	

	 /**
    * By default Symbian 2nd phase constructor is private.
    */
	void ConstructL();
	
	/**
    * Constructor.
    */
	CUpnpFileTransferEvent();
	
private:
    //Transfer progress state 
	TInt iProgress;
    //Transfer direction
	TInt iDirection;
    //Content Directory object identifier
	TInt iContentDirectoryObjectId;
    //Local disk file path 
	HBufC*  iFilePath;
    //Transfer identifier
	TInt iTransferId;
    //Lower level error code, for failed transfers
	TInt iErrorCode;

};

#endif //C_CUPNPFILETRANSFEREVENT_H
