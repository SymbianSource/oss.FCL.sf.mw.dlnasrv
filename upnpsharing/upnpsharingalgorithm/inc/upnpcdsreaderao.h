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
 *  Description : CUpnpCdsReaderAO class definiton
 *
 */

#ifndef UPNP_CDS_READER_AO_H
#define UPNP_CDS_READER_AO_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpFileSharing;
class CUpnpCdsLiteObjectArray;

/**
 *  CUpnpCdsReaderAO class definition
 *
 *  @lib upnpsharingalgorithm.lib
 *  @since S60 5.2
 */
class CUpnpCdsReaderAO  : public CActive
    {

public:
    // Constructors and destructor.

    /**
     * Two-phased constructor.
     *
     * @param aUpnpCdsLiteObjects, reference to the array of cds objects
     */
    static CUpnpCdsReaderAO* NewL(
        CUpnpCdsLiteObjectArray &aUpnpCdsLiteObjects );

    /**
     * Destructor.
     */
    virtual ~CUpnpCdsReaderAO();

public:
    /**
     * Reads the shared stucture from content directory to the array passed
     * as parameter to the NewL
     *
     * @since S60 5.2
     */
    void ReadCdsStructureL();

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
     */
    void ConstructL();

    /**
     * C++ default constructor.
     * @param aUpnpCdsLiteObjects, reference to the array of cds objects
     */
    CUpnpCdsReaderAO( CUpnpCdsLiteObjectArray &aUpnpCdsLiteObjects  ):
        CActive( CActive::EPriorityStandard ),
        iUpnpCdsLiteObjects( aUpnpCdsLiteObjects ) {}

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
     * Get container and analyse container objects
     *
     * @since S60 5.2
     * @param aContainerId, where items and containers are searched
     */
    void GetContainersL( const TDesC8& aContainerId );

    /**
     * Gets all file objects of one container
     *
     * @since S60 5.2
     * @param aContainerId where items are searched
     */
    void GetItemsL( const TDesC8& aContainerId );

    /**
     * Sets the object active and waits for the request to complete
     *
     * @since S60 5.2
     */
    void CompleteRequestL();

private:

    /**
     * MediaServer file sharing instance - owned
     */
    CUpnpFileSharing*               iFileSharing;
    /**
     * Waits request to complete - owned
     */
    CActiveSchedulerWait*           iWaitRequest;
    /**
     * Object array
     */
    CUpnpCdsLiteObjectArray&        iUpnpCdsLiteObjects;
    /**
     * Active scheduler pointer
     */
    CActiveScheduler*               iScheduler;
    };

#endif // UPNPCDSREADERAO_H

// End of File
