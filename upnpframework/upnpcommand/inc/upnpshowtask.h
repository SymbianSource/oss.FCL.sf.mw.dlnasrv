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
* Description:  Header file for the CUpnpShowTask class.
*
*/


#ifndef UPNP_SHOW_TASK_H
#define UPNP_SHOW_TASK_H

// INCLUDES
#include <e32base.h>
#include "upnpresourceselector.h" // a member located here
#include "upnptask.h" // base class
#include "upnpimagerenderingengineobserver.h" // base class

// FORWARD DECLARATIONS
class MUPnPAVRenderingSession;
class CUPnPCommonUI;
class CUpnpTaskResourceAllocator;
class CUpnpImageRenderingEngine;


/**
* This class defines the UpnpShowTask used in UpnpCommand component.
*
* @since S60 3.2
*/
class CUpnpShowTask : public CUpnpTask,
                      public MUpnpImageRenderingEngineObserver
    {
    public: // Methods from CUpnpTask

        /**
         * Creates a new UpnpShowTask object. Allocates Upnp Fw resources.
         * Resources will be released when the task is destroyed.
         *
         * @since S60 3.2
         * @return a new instance of CUpnpShowTask, casted to type CUpnpTask
         */
        static CUpnpTask* NewL();

        /**
         * Destructor.
         */
        virtual ~CUpnpShowTask();

        /**
         * Allocates the Upnp Fw resources.
         *
         * @since S60 3.2
         */
        void AllocateResourcesL();

        /**
         * Executes the task.
         *
         * @since S60 3.2
         */
        void ExecuteL();


    private: // Private construct/destruct methods

        /**
         * constructor
         */
        CUpnpShowTask();

        /**
         * Perform the second phase of two phase construction. Reserves the
         * Upnp Fw resources (they are released when the task is destroyed).
         */
        void ConstructL();
        
        /**
         * Cleans up used resources
         */
        void Cleanup();

    protected: // methods from MUpnpImageRenderingEngineObserver

        MUPnPItemResolver* GetMedia();

        TInt RenderAck(
            TInt aError,
            const CUpnpItem* aItem ); 

        void EngineShutdown(
            TInt aError );


    private: // Private methods

        /**
         * Launches the video player dialog for playing a video file on remote
         * @param aItem the item to play
         * @return an error code
         */
        TInt PlayVideo( const CUpnpItem& aItem );


    private: // Data members

        // Pointer to the UpnpAvController's rendering session.
        MUPnPAVRenderingSession*            iRenderingSession;
        
        // Pointer to the UpnpAvController's rendering session.
        MUPnPAVRenderingSession*            iVideoRenderingSession;

        // flag for video playing
        TBool                               iPlayingVideo;

        // Pointer to the UPnPCommonUI;
        CUPnPCommonUI*                      iCommonUI;

        // Default resource selector. Owned.
        TUPnPSelectDefaultResource          iSelector;

        // The resource allocator
        CUpnpTaskResourceAllocator*         iResourceAllocator;

        // the image rendering engine
        CUpnpImageRenderingEngine*          iRenderingEngine;
        
        // flag to show note when image showing failed
        TBool                               iShowPlaybackFailedNote;
        
        };


#endif // UPNP_SHOW_TASK_H

// End of File
