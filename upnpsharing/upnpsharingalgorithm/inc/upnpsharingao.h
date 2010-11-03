/**
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *  Description : CUpnpSharingAO class definiton
 *
 */

#ifndef UPNP_SHARING_AO_H
#define UPNP_SHARING_AO_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpFileSharing;

/**
 *  UpnpSharingAO class definition
 *
 *  @lib upnpsharingalgorithm.lib
 *  @since S60 5.2
 */
class CUpnpSharingAO  : public CActive
    {

    public:
        // Constructors and destructor.

        /**
         * Two-phased constructor.
         * @since S60 5.2
         */
        static CUpnpSharingAO* NewL();

        /**
         * Two-phased constructor.
         * @since S60 5.2
         */
        static CUpnpSharingAO* NewLC();

        /**
         * Destructor.
         * @since S60 5.2
         */
        virtual ~CUpnpSharingAO();

    public:

        /**
         * Shares file
         *
         * @since S60 5.2
         * @param aParentContainerId, parent container id where
         * item is shared
         * @param aItem, UpnpItem to be shared
         */
        void ShareFileL( const TDesC8& aParentContainerId,
                         CUpnpItem* aItem );

        /**
         * Shares reference item based on original items id.
         * Item data is filled on sharing.
         *
         * @since S60 5.2
         * @param aParentContainerId, parent container id where
         * item is shared
         * @param aOriginalItemId, original item id from
         *                         which reference is created
         * @param aItem, reference UpnpItem to be shared
         */
        void ShareReferenceL( const TInt aParentContainerId,
                              const TInt aOriginalItemId,
                              CUpnpItem* aItem );

        /**
         * Shares container
         *
         * @since S60 5.2
         * @param aContainer, Container object which is going to be shared
         */
        void ShareContainerL( CUpnpContainer* aContainer );

        /**
         * Unshares file
         *
         * @since S60 5.2
         * @param aItemId, itemid of UPnPItem to be shared
         */
        void UnshareFileL( const TInt aItemId );

        /**
         * Unshares container
         *
         * @since S60 5.2
         * @param aContainerId, id for container which is going to be unshared
         */
        void UnshareContainerL( const TInt aContainerId );

    protected:

        // From base class CActive

        /**
         * From CActive
         * See base class definition
         */
        void RunL();

        /**
         * From CActive
         * See base class definition
         */
        void DoCancel();

    private:

        /**
         * Second-phase constructor.
         *
         * @since S60 5.2
         */
        void ConstructL();

        /**
         * C++ default constructor.
         * @since S60 5.2
         */
        CUpnpSharingAO::CUpnpSharingAO() :
                            CActive( CActive::EPriorityStandard ) {}

        /**
         * Waits that request completes
         * This does not block other AO's
         *
         * @since S60 5.2
         */
        void WaitRequestToComplete();

        /**
         * Stops waiting request complete
         *
         * @since S60 5.2
         */
        void StopWaitingRequestComplete();

        /**
         * Sets the object active and waits for the request to complete
         *
         * @since S60 5.2
         */
        void CompleteRequestL();

    private:

        /**
         * MediaServer file sharing instance. Owned
         */
        CUpnpFileSharing*               iFileSharing;

        /**
         * Waits request to complete. Owned
         */
        CActiveSchedulerWait*           iWaitRequest;

        /**
         * Active scheduler pointer. Owned.
         */
        CActiveScheduler*               iScheduler;
    };

#endif // UPNP_SHARING_AO_H
// End of File
