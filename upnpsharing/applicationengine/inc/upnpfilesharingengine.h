/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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





#ifndef __UPNPFILESHARINGENGINE_H__
#define __UPNPFILESHARINGENGINE_H__


//  Include Files
#include <e32base.h>    // CBase
#include <e32std.h>     // TBuf
#include <badesca.h>    // for CDesCArray

#include "upnpconnectionmonitorobserver.h"
#include "upnpcontentserverclient.h"
#include "upnpfilesharingengineobserver.h"

// FORWARD DECLARATIONS
class CUpnpContentRequestAO;
class CUPnPFileSharingEngineAO;
class CUpnpContentShareAO;
class CUpnpProgressWatcher;
class CUPnPConnectionMonitor;


// CONSTANTS


// ENUMERATIONS

enum THomeConnectMediaType
    {
    EImageAndVideo = 0,
    EPlaylist,
    EPhotoFile,
    EVideoFile,
    EMusicFile
    };

enum THomeConnectSharingStates
    {
    ENoChange = -99,
    EShareNone = 0,
    EShareMany,
    EShareAll
    };    

// CLASS DECLARATION

/**
 *  File sharing engine class for UPnP Home Connect Application
 *  @lib upnpapplicationengine.lib
 *  @since S60 3.1
 */
class CUPnPFileSharingEngine : public CBase,
                               public MUPnPConnectionMonitorObserver
    {

private:

    /**
     * C++ default constructor.
     */
    CUPnPFileSharingEngine();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @since S60 3.1
     */
    IMPORT_C static CUPnPFileSharingEngine* NewL();

    /**
     * Two-phased constructor.
     * @since S60 3.1
     */
    IMPORT_C static CUPnPFileSharingEngine* NewLC();

    /**
     * Destructor.
     * @since S60 3.1
     */
    IMPORT_C virtual ~CUPnPFileSharingEngine();

public:

    /**
     * Sets observer for active object
     * @since S60 3.1
     * @param aObserver, observer class
     */
    IMPORT_C void SetObserver( MUPnPFileSharingEngineObserver* aObserver );

    /**
     * Return handle to MUPnPFileSharingEngineObserver interface
     * @since S60 3.1
     */
    IMPORT_C MUPnPFileSharingEngineObserver* Observer();

    /**
     * Sets sharing state to on or off
     * Asynchronous, callback to 
     * MUPnPFileSharingEngineObserver::HandleSharingStatus
     * @since S60 3.1
     * @param aState ETrue to enable sharing, otherwise EFalse
     */
    IMPORT_C void SetSharingStateL( TBool aState );

    /**
     * Gets file sharing state from local MediaServer
     * @since S60 3.1
     * @return ETrue if sharing is ON otherwise EFalse
     */
    IMPORT_C TBool SharingStateL();

    /**
     * Starts sharing process
     * Asynchronous, callback to 
     * MUPnPFileSharingEngineObserver::HandleSharingDone and
     * MUPnPFileSharingEngineObserver::HandleSharingProgress
     * @since S60 3.1
     * @param aType, Type of media to share
     */
    IMPORT_C void ShareMediaL( THomeConnectMediaType aType );

    /**
     * Updates shared objects
     * Asynchronous, callback to 
     * MUPnPFileSharingEngineObserver::HandleSharingDone and
     * MUPnPFileSharingEngineObserver::HandleSharingProgress
     * @since S60 3.1
     * @param aType Type of media to refresh
     */
    IMPORT_C void RefreshSharedMediaL( THomeConnectMediaType aType );

    /**
     * Gets the selected albums/playlist indexes
     * Asynchronous, callback to 
     * MUPnPFileSharingEngineObserver::HandleSelectionIndexes
     * @since S60 3.1
     * @param aType Type of media selected
     */
    IMPORT_C void RequestSelectionIndexesL( 
        THomeConnectMediaType aType ) const;

    /**
     * Gets the selected albums/playlist indexes
     * @since S60 3.1
     * @param aMarkedItems The indexes of the selections user has made\
relative to selections received from GetSelectionContentL
     */
    IMPORT_C void GetSelectionIndexesL(
        CArrayFix<TInt>& aMarkedItems ) const;

    /**
     * Sets the user selections
     * @since S60 3.1
     * @param aType Type of media selected
     * @param aMarkedItems The indexes of the selections user has made\
relative to selections received from GetSelectionContentL
     */
    IMPORT_C void SetSelectionIndexesL( 
        THomeConnectMediaType aType,
        const CArrayFix<TInt>& aMarkedItems );

    /**
     * Requests the album /playlist names
     * Asynchronous, callback to 
     * MUPnPFileSharingEngineObserver::HandleSelectionContent
     * @since S60 3.1
     * @param aType Type of the media requested
     */
    IMPORT_C void RequestSelectionContentL( THomeConnectMediaType aType );

    /**
     * Gets the album /playlist names from upnpcontentserver once ready
     * @since S60 3.1
     * @param aType Container type
     * @param aArray Array containng the names for the UI
     */
    IMPORT_C void GetSelectionContentL( CDesCArray& aArray ) const;

    /**
     * Request information about sharing statuses / progress from the
     * contentserver. The information is received via callback
     * @since S60 3.1
     */
    IMPORT_C void RequestSharingProgressL();

    /**
     * Starts upload listener. Leave in case of error
     * @since S60 3.1
     */
    IMPORT_C void StartUploadListenerL();

    /**
     * Stops upload listener. Leave in case of error
     * @since S60 3.1
     */
    IMPORT_C void StopUploadListenerL();

    /**
     * Start or stop connection monitoring
     * @since S60 3.1
     * @param aState ETrue = Start, EFalse = Stop
     * @param aStatus TRequestStatus for async operation
     */
    void SetConMonStateL( TBool aState, TRequestStatus& aStatus );
	
     /**
     * Set share file completed flag
     * @since S60 3.2.3
     * @param aShareState set ETrue indicate imageandvideo or playlist shared completely, 
     * perhaps both of them shared completely.
     * EFalse  indicate imageandvideo or playlist shared incompletely,
     * perhaps both of them shared completely.
     */
    void SetShareFileComplete( TBool aShareState ) ;

private:

    /**
     * Helper function to copy TInts between arrays of CArrayFix<TInt>&
     * @since S60 3.1
     */
    void CopyTintArrayL( const CArrayFix<TInt>& aSrc,
                         CArrayFix<TInt>& aDst) const;

    /**
     * Helper function to copy descriptors between arrays of CDesCArrays
     * @since S60 3.1
     */
    void CopyCdescArrayL( const CDesCArray& aSrc,
                          CDesCArray& aDst ) const;

private : // From MUPnPConnectionMonitorObserver

    /**
     * Update the sharing state variable in case WLAN connection is lost
     * From MUPnPConnectionMonitorObserver
     * @since S60 3.1
     */
    void ConnectionLost();

private: // data

    /**
     * Active object for asynchronous file sharing
     */
    CUPnPFileSharingEngineAO* iAO;
    
    /**
     * Active object for asynchronous get responce from content server
     * Owned.
     */
    CUpnpContentShareAO* iContentSharer;

    /**
     * Active object class retrieving information about selections and
     * content from upnpcontentserver
     */
    CUpnpContentRequestAO* iContentRequestor;

    /**
     * Cached image selections
     */
    CArrayFixFlat<TInt>* iImageSelections;

    /**
     * Cached music selections
     */
    CArrayFixFlat<TInt>* iMusicSelections;

    /**
     * Client side handle to upnpcontentserver
     */
    RUpnpContentServerClient iContentServer;

    /**
     * Observer for file sharing state
     */
    MUPnPFileSharingEngineObserver* iObserver;
    
     /**
     * Set share file completed flag.      
     */
    TBool iShareFileState;

    /**
     * Connection monitor
     */
    CUPnPConnectionMonitor* iConnMon;

    /**
     * Active object handling querying progress
     */
    CUpnpProgressWatcher* iProgress;

    };

#endif  // __UPNPFILESHARINGENGINE_H__
// End of file

