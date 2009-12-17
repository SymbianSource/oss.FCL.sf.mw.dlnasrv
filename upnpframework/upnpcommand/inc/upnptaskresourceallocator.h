/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that allocates resources for a task
*
*/


#ifndef UPNP_TASKRESOURCEALLOCATOR_H
#define UPNP_TASKRESOURCEALLOCATOR_H

// INCLUDES
// System
#include <e32base.h>

// upnpframework / avcontroller api
#include "upnpavdeviceobserver.h" // base class
#include "upnpavbrowsingsessionobserver.h" // base class

// FORWARD DECLARATIONS
class CUPnPCommonUI;
class CUpnpNoteHandler;
class MUPnPAVController;
class CUpnpAVDevice;
class MUPnPAVBrowsingSession;
class CUpnpNoteHandler;

/**
 * A helper class that makes a method async.
 */
class CUpnpTaskResourceAllocator
    : public CAsyncOneShot
    , public MUPnPAVDeviceObserver
    , public MUPnPAVBrowsingSessionObserver
    {
    public:

        /**
         * List of upnp resource types available.
         * These types can be combined by masking.
         */
        enum TUpnpResourceTypes
            {
            // open AV controller resource
            EResourceAvController            = 0x0001,
            // start local mediaserver
            EResourceLocalMediaServer        = 0x0002,
            // select an image-capable renderer
            EResourceSelectImageRenderer     = 0x0100,
            // select a copy-capable server
            EResourceSelectCopyServer        = 0x0200,
            // select a copy-capable server (display move title)
            EResourceSelectMoveServer        = 0x0400,
            // a mask, USED ONLY INTERNALLY !!!
            EResourceSelectDevice            = 0xFF00,
            };

        /**
         * static constructor
         * @param aCommonUI common UI reference to use
         * @param aNoteHandler note handler reference to use
         * @param aMode which resources are needed
         */
        static CUpnpTaskResourceAllocator* NewL(
            CUPnPCommonUI& aCommonUI,
            TInt aMode );

        /**
         * destructor
         */
        virtual ~CUpnpTaskResourceAllocator();
         
    private:

        /**
         * constructor
         */
        CUpnpTaskResourceAllocator(
            CUPnPCommonUI& aCommonUI,
            TInt aMode );

    public: // the interface
        
        /**
         * Allocates the resources, returns when complete.
         * A wait note will be displayed during the process.
         * displays any necessary errors that occur during the process
         */
        void AllocateL();

        /**
         * returns the AVController resource after allocation.
         * Note: This method panics if EResourceAvController was not set.
         */
        MUPnPAVController& AVController();

        /**
         * returns the selected device
         * Note: the method panics if neither EResourceSelectRenderer nor
         * EResourceSelectServer was set.
         */
        const CUpnpAVDevice& SelectedDevice();
        
        
         /**
         * Sets the pointer to the note handler. The ownership of the pointer
         * is not transfered.
         *
         * Leaves with KErrArgument, if the provided pointer is invalid.
         *
         * @since S60 3.2
         * @param aNoteHandler (CUpnpNoteHandler*) the pointer to the note
         *        handler
         */
        void SetNoteHandlerL( CUpnpNoteHandler* aNoteHandler );
        
         
    public: // from CAsyncOneShot
        
        /**
         * Asynchronous execution
         */
        void RunL();

        /**
         * Execution of the error branch
         */
        TInt RunError( TInt aError );

    private: // own methods

        /**
         * cleans up all resources
         */
        void Cleanup();

        /**
         * creates the AVController resource
         */
        void StartAvControllerL();

        /**
         * Starts the local mediaserver
         */
        void StartLocalMediaServerL();


        /**
         * shows a device selection popup
         */
        void SelectDeviceL();

        /**
         * sets an asynchronous error code
         */
        void SetErrorCode( TInt aError );

    public: // Call back methods of MUPnPAVDeviceObserver

        void UPnPDeviceDiscovered( const CUpnpAVDevice& aDevice );

        void UPnPDeviceDisappeared( const CUpnpAVDevice& aDevice );

        void WLANConnectionLost();
       
        
    public: // Call back methods of MUPnPAVBrowsingSessionObserver

        void BrowseResponse(
            const TDesC8& aBrowseResponse,
            TInt aError,
            TInt aMatches,
            TInt aTotalCount,
            const TDesC8& aUpdateId
            );

        void SearchResponse( 
            const TDesC8& aSearchResponse,
            TInt aError,
            TInt aMatches,
            TInt aTotalCount,
            const TDesC8& aUpdateId
            );

        void SearchCapabilitiesResponse( 
            TInt aError,
            const TDesC8& aSearchCapabilities 
            );

        void CreateContainerResponse( TInt aError, 
            const TDesC8& aObjectId );

        void DeleteObjectResponse( TInt aError );

        void MediaServerDisappeared(
            TUPnPDeviceDisconnectedReason aReason );

        void ReserveLocalMSServicesCompleted( TInt aError );

    protected:

        /**
         * states of the allocator
         */
        enum TAllocatorState
            {
            EStateIdle = 0, // class constructed, allocation not started yet
            EStateAllocating, // allocation in progress
            EStateWaitingForLMS, // rest done,waiting for local media server
            EStateReady, // allocation succesful
            EStateError  // allocation failed
            };

    private:

        // internal state
        TAllocatorState               iState;
    
        // the common UI resource
        CUPnPCommonUI&                iCommonUI;

        // the note handler
        CUpnpNoteHandler*             iNoteHandler;

        // resource allocation mode
        TInt                          iMode;

        // avcontroller resource
        MUPnPAVController*            iAVController;

        // selected device
        CUpnpAVDevice*                iSelectedDevice;

        // the mediaserver keepalive browsing session
        MUPnPAVBrowsingSession*       iMediaServerSession;

        // error occurred in some asynchronous operation
        TInt                          iErrorCode;

        // flag for local mediaserver start state
        TBool                         iLocalMSStarted;

        // flag for local mediaserver callback
        TBool                         iLocalMSSCompleted;

        
    };


#endif // UPNP_TASKRESOURCEALLOCATOR_H

// End of File
