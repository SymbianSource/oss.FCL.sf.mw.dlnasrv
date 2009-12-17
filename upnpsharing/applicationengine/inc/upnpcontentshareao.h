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
* Description:      file sharing engine class definition               
*
*/





#ifndef __UPNPCONTENTSHAREAO_H__
#define __UPNPCONTENTSHAREAO_H__

// INCLUDES
#include <e32base.h>
#include "upnpcontentserverclient.h"

// FORWARD DECLARATIONS
class CUPnPFileSharingEngine;

/**
 *  A class to send request to content server and get the responce.
 * *  @since S60 3.2
 */
NONSHARABLE_CLASS( CUpnpContentShareAO ) : public CActive
    {

private:

   /**
    * C++ constructor.
    * @since S60 3.2
    * @param aEngine the file sharing engine
    * @aContentServer the contentserver client
    */
    CUpnpContentShareAO( 
            CUPnPFileSharingEngine& aEngine,
            RUpnpContentServerClient& aContentServer );

    /**
     * Perform the second phase construction
     * @since S60 3.2
     */
    void ConstructL(); 

public:
            
    /**
     * Two-phased constructor.
     * @since S60 3.2
     * @param aEngine the file sharing engine
     * @aContentServer the contentserver client
     */
     static CUpnpContentShareAO* NewL( 
             CUPnPFileSharingEngine& aEngine, 
             RUpnpContentServerClient& aContentServer );

    /**
     * Two-phased constructor.
     * @since S60 3.2
     * @param aEngine the file sharing engine
     * @aContentServer the contentserver client
     */
     static CUpnpContentShareAO* NewLC( 
             CUPnPFileSharingEngine& aEngine, 
             RUpnpContentServerClient& aContentServer );

   /**
    * C++ destructor.
    */
    virtual ~CUpnpContentShareAO();   
    
    /**
     * Sends the selected indexes to server and starts sharing
     * @since S60 3.2
     * @param aMarkedItems List of selected indexes
     * @param aType Type of selection
     */
    void ChangeSharedContentL(
        UpnpContentServer::TUpnpMediaType aType,
        const CArrayFix<TInt>& aMarkedItems );

    /**
     * Updates shared objects in mediaserver
     * @since S60 3.2
     * @param aType Type of media to refresh
     */
    void RefreshSharedContentL( 
        UpnpContentServer::TUpnpMediaType aType );
    
protected:

   /**
    * Function is called when active request is ready
    * @since S60 3.2
    */
   void RunL();

   /**
    * Cancels active request
    * @since S60 3.2
    */
   void DoCancel();

   /**
    * Handles a leave occurring in the request completion event handler RunL()
    * @since S60 3.2
    */
   TInt RunError( TInt aError );
   
private: // New functions
    
    /**
     * Removes queued objects by given index.
     * @since S60 5.1
     * @param aIndex The index of objects to be removed.
     */
    void RemoveQueuedObject( TInt aIndex );
    
    /**
     * Removes Queued objects by type.
     * @since S60 5.1
     * @param aType The type of objects to be removed.
     */
    void RemoveQueuedObjectByType( UpnpContentServer::TUpnpMediaType aType );
    
    /**
     * Appends given Queued objects.
     * @since S60 5.1
     * @param aType The type to appended
     * @param aSelections The selection array to be appended. Can be NULL.
     */
    void AppendQueuedObjectL( 
            UpnpContentServer::TUpnpMediaType aType, 
            CArrayFix<TInt>* aSelections );

private: // data

    /**
     * File sharing engine
     */
    CUPnPFileSharingEngine& iEngine;

    /**
     * Content server session handle
     */
    RUpnpContentServerClient& iContentServer; 

    /**
     * Queued selection indexes
     */
    typedef CArrayFix<TInt> CSelections;
    RPointerArray<CSelections> iQueuedSelections;

    /**
     * Queued selection types
     */
    RArray<UpnpContentServer::TUpnpMediaType> iQueuedTypes;

    };

#endif // __UPNPCONTENTSHAREAO_H__
