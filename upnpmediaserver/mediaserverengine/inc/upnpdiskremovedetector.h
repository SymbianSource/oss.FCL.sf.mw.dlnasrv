/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Detects remove disk event.
*
*/


#ifndef C_CUPNPDISKREMOVEDETECTOR_H
#define C_CUPNPDISKREMOVEDETECTOR_H

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CUpnpMediaServer;


// CLASS DECLARATION
/**
 *  Class is responsible for detecting removal/unmounting of a disk from file system
 *  @since S60 v3.2
 */
class CUpnpDiskRemoveDetector : public CActive
    {
public: // New methods

    /**
    * Two-phased constructor.
    * @param aObserver callback interface
    */
    static CUpnpDiskRemoveDetector* NewLC( CUpnpMediaServer* aObserver );

    /**
    * Two-phased constructor.
    * @param aObserver callback interface
    */
    static CUpnpDiskRemoveDetector* NewL( CUpnpMediaServer* aObserver );

    /**
    * Destructor.
    */
    ~CUpnpDiskRemoveDetector();

    /**
     * Start watching for events - asynchronous method
     */	
    void StartL();

protected: // From CActive

    /**
     * From CActive, called when event occur.
     */
    void RunL();

    /**
     * From CActive, cancel watching for incoming events.
     */
    void DoCancel();

    /**
     * From CActive, called when RunL leaves.
     * @param aError error code
     */
    TInt RunError( TInt aError );

private: // New methods
    /**
    * C++ default constructor.
    * @param aObserver callback interface
    */
    CUpnpDiskRemoveDetector( CUpnpMediaServer* aObserver );

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Check if the upload direcotory is valid
     */
    void CheckUploadDirectoryL();
    
    /**
     * Subscribe this object for file system disk event
     */
    void SubscribeForDiskEvent();

private: // Data

    // Interface monitor
    RFs iFs;

    //Callback interface
    CUpnpMediaServer* iObserver;

    };

#endif // C_CUPNPDISKREMOVEDETECTOR_H
