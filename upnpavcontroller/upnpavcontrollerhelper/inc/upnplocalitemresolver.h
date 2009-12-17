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
* Description:      Resolver for local items
*
*/






#ifndef UPNPLOCALITEMRESOLVER_H
#define UPNPLOCALITEMRESOLVER_H

//  INCLUDES
#include <e32base.h>
#include "upnpavbrowsingsessionobserver.h" // base class
#include "upnpitemresolver.h" // base class
#include <upnpmediaserverclient.h> // a member

// FORWARD DECLARATIONS
class MUPnPAVController;
class MUPnPAVBrowsingSession;
class CUpnpFileSharing;
class CUpnpItem;
class CUpnpSecAccessController;
class CUPnPPeriodic;

// CLASS DECLARATION

/**
* Resolves remote upnp items from a plain item id.
* This resolving is done by executing a upnp metadata browse, or
* possibly several ones of those.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPLocalItemResolver
    : public CActive
    , public MUPnPAVBrowsingSessionObserver
    , public MUPnPItemResolver
    {
public: // construction/destruction

    /**
     * static constructor
     *
     * @since Series 60 3.2
     * @param aFilePath, local item to be resolved
     * @param aAvController, AVController
     * @param aSelector, resource selector
     * @param aOptimisationFlags flags to optimise the algorithm
     * @return LocaltemResolver instance 
     */
    static CUPnPLocalItemResolver* NewL(
        const TDesC& aFilePath,
        MUPnPAVController& aAvController,
        MUPnPResourceSelector& aSelector,
        TInt aOptimisationFlags );

    /**
     * destructor
     * @since Series 60 3.2
     */
    virtual ~CUPnPLocalItemResolver();

private:

    /**
     * default constructor
     *
     * @since Series 60 3.2
     * @param aAvController, AVController
     * @param aSelector, resource selector
     */
    CUPnPLocalItemResolver(
        MUPnPAVController& aAvController,
        MUPnPResourceSelector& aSelector,
        TInt aOptimisationFlags );

    /**
     * 2nd phase constructor
     *
     * @param aFilePath, local item to be resolved
     */
    void ConstructL( const TDesC& aFilePath );

private: // Business logic methods

    /**
     * Allows/denied access to the files to which the given item's res-
     * elements are pointing.
     *
     * @since Series 60 3.2
     * @param aItem the item
     * @param aAccessAllowed ETrue to allow and EFalse to deny the access
     */
    void SetAccesstoItemResources( CUpnpItem& aItem, TBool aAccessAllowed );

public: // the interface

    /**
     * see UPnPItemResolver
     */
    void ResolveL(
        MUPnPItemResolverObserver& aObserver
        );

    /**
     * see UPnPItemResolver
     */
    const CUpnpItem& Item() const;


    /**
     * see UPnPItemResolver
     */
    const CUpnpElement& Resource() const;
    
protected: // From CActive

    /**
     * See CActive
     */
    void RunL();
    
    /**
     * See CActive
     */
    void DoCancel();
    
    /**
     * See CActive
     */
    TInt RunError( TInt aError );

protected: // overrides to CUPnPBrowsingSessionObserverProxy


    /**
     * see MUPnPAVSessionObserverBase
     */
    void ReserveLocalMSServicesCompleted( TInt aError );


private: // private methods

    /**
     * initiate the sharing process
     */
    void DoShareL();
    
    /**
     * initiate the unsharing process
     */
    void DoUnshareL();

    /**
     * Completes ongoing async operation
     */
    void Complete( TInt aError );

    /**
     * Clean up all resources
     */
    void Cleanup();

    /**
     * internal state machinery
     */
    enum TInternalState {
        EStateIdle,         // no data, ready for starting
        EStateCreatingItem, // CreateItemFromFileLC called
        EStateStartingMS,   // starting the local mediaserver
		EStateSharing,      // sharing the local file
		EStateUnsharing,    // unsharing the local file
        EStateReady,        // Ready. Can free.
		EStateCancel		// command to cancel ongoing process
    };        
        
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

    void MediaServerDisappeared(
        TUPnPDeviceDisconnectedReason /*aReason*/ ) {}
    
    
private: // Timer callback
    /**
     * timer callback
     * unsharing is sometimes jammed and timer is used for cancelling
     * unshare call
     */
   static TInt TimerExpired( TAny* aArg );

private: // members

    // local file path (Owned).
    HBufC* iFilePath;

    // avcontroller
    MUPnPAVController& iAvController;

    // temporary session for starting local mediaserver
    MUPnPAVBrowsingSession* iTempSession;

    // resource selector
    MUPnPResourceSelector& iSelector;

    // observer for this utility. Note: NOT OWNED
    MUPnPItemResolverObserver* iObserver;

    // state of the class
    TInternalState iState;

    // the first level browse result item (Owned).
    CUpnpItem* iSharedItem;

    // The selected resource within the last level item.
    const CUpnpElement* iResource;

    // The mediaserver (required to get the server IP address)
    RUpnpMediaServerClient iMediaServer;

    // optimisation flags
    TInt iOptimisationFlags;

    // S60 file sharing metadata interface
    CUpnpFileSharing* iFileSharing;

    // security access controller
    CUpnpSecAccessController* iAccessController;
    
    // wait until unshare finishes
    CActiveSchedulerWait*   iWait;
    
    // timer to cancel unshare
    CUPnPPeriodic*          iTimer;
    };


#endif  // UPNPLOCALITEMRESOLVER_H

// End of File
