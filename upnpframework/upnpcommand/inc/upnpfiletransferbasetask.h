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
* Description:  Header file for the CUpnpFileTransferBaseTask class.
*
*/


#ifndef UPNP_FILETRANSFER_BASE_TASK_H
#define UPNP_FILETRANSFER_BASE_TASK_H

// INCLUDES
#include <e32base.h>
#include "upnpavbrowsingsessionobserver.h"
#include "upnptask.h"

// FORWARD DECLARATIONS
class MUPnPAVBrowsingSession;
class MUPnPAVSessionBase;
class CUPnPCommonUI;
class CUpnpTaskResourceAllocator;



/**
* This class defines the CUpnpFileTransferBaseTask 
* used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpFileTransferBaseTask : public CUpnpTask, 
                            public MUPnPAVBrowsingSessionObserver
    {
    public: // Methods from CUpnpFileTransferBaseTask

        /**
         * Allocates the Upnp Fw resources.
         *
         * @since S60 3.2
         */
        void AllocateFileTransferResourcesL( TInt aMode );

        /**
         * Executes the task.
         *
         * @since S60 3.2
         */
        void ExecuteFileTransferL( TBool aRemoveFiles, 
                                TInt& aTransferredFiles);
        
        /**
         * return CUpnpTaskResourceAllocator
         *
         * @since S60 3.2
         */
        CUpnpTaskResourceAllocator* ResourceAllocator();
        
    public: // Methods from CUpnpTask, not used
    
        void AllocateResourcesL() {}
        void ExecuteL() {}

    public: // Public destructor

        /**
         * Destructor.
         */
        virtual ~CUpnpFileTransferBaseTask();

        /**
         * Perform the first phase of two phase construction.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpFileTransferBaseTask, 
         * casted to type CUpnpTask
         */
        CUpnpFileTransferBaseTask();
        
    
    protected:
 
        /**
         * Returns connection state
         */
        TBool IsWlanActive();
       
    private: // Call back methods of MUPnPAVBrowsingSessionObserver

        /* Not used */
        void BrowseResponse(
            const TDesC8& /*aBrowseResponse*/,
            TInt /*aError*/,
            TInt /*aMatches*/,
            TInt /*aTotalCount*/,
            const TDesC8& /*aUpdateId*/
            ) {}
        void SearchResponse( 
            const TDesC8& /*aSearchResponse*/,
            TInt /*aError*/,
            TInt /*aMatches*/,
            TInt /*aTotalCount*/,
            const TDesC8& /*aUpdateId*/
            ) {}
        void SearchCapabilitiesResponse( 
            TInt /*aError*/,
            const TDesC8& /*aSearchCapabilities*/ 
            ) {}
        void CreateContainerResponse( TInt /*aError*/, 
            const TDesC8& /*aObjectId*/ ) {}
        void DeleteObjectResponse( TInt /*aError*/ ) {}  
        void ReserveLocalMSServicesCompleted( TInt /*aError*/ ){}

        
        /**
         * Notifies that the Media Renderer we have a 
         * session which has disappeared.
         *
         * @since S60 3.2
         */
        void MediaServerDisappeared(
            TUPnPDeviceDisconnectedReason aReason ) ;
    

    private: // Data members
   
        /**
         * Connection state
         */       
        enum TConnectionState{
            EStateConnected = 0,
            EStateMSLost,
            EStateWLANLost
            };
               
                        
        /**
         * Pointer to the UpnpAvController's browsing session instance. Owned.
         */
        MUPnPAVBrowsingSession*             iBrowsingSession;
        
        /**
         * Pointer to the UPnPCommonUI. Owned.
         */
        CUPnPCommonUI*                      iCommonUI;
        
        /**
         * The resource allocator. Owned.
         */
        CUpnpTaskResourceAllocator*         iResourceAllocator;
        
        /**
         *  Connection state
         */        
         TConnectionState                   iState;

    };

#endif // UPNP_FILETRANSFER_BASE_TASK_H

// End of File
