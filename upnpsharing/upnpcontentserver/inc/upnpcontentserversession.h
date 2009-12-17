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
* Description:      Header of CUpnpContentServerSession class
 *
*/






#ifndef __UPNPCONTENTSERVERSESSION_H__
#define __UPNPCONTENTSERVERSESSION_H__

#include <e32base.h>
#include <badesca.h>

#include "upnpcontentserver.h"
#include "upnpcontentsharingobserver.h"


// CLASS FORWARDS
class CUpnpContentServer;

/**
 *  A session class which creates an instance of Upnp content server
 *  when starting,
 *  and releases used memory when closing.
 *
 *  @since S60 3.1
 */
class CUpnpContentServerSession : public CSession2,
                                  public MUpnpContentSharingObserver
    {
public:
    /**
     * 2-phased constructor.
     * @since S60 3.1
     * @param aServer Pointer to server class
     */
    static CUpnpContentServerSession* NewL( CUpnpContentServer* aServer );

    /**
     * From CSession2, passes the request forward to DispatchMessageL.
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void ServiceL( const RMessage2& aMessage );

    /**
     * Handles the request from client.
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void DispatchMessageL( const RMessage2& aMessage );

    /**
     * Processes the album / playlist query
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void GetSelectionContentL(
        const RMessage2& aMessage );

    /**
     * Processes the old selections query
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void GetSelectionIndexesL(
        const RMessage2& aMessage );

    /**
     * Processes the sharing request
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void ChangeShareContentL(
        const RMessage2& aMessage );

    /**
     * Completes the album / playlist query
     * @param aMessage Reference to message from client
     * @since S60 3.1
     */
    void CompleteSelectionContentL(
        const CDesCArray& aMarkedItems );

    /**
     * Processes the progress query
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void RequestProgressInfoL(
        const RMessage2& aMessage );

    /**
     * Processes the refresh request
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void RefreshShareContentL(
        const RMessage2& aMessage );

    /**
     * Start connection monitor
     * @since S60 3.1
     * @param aMessage Reference to processed message
     */
    void StartConnectionMonitorL(
        const RMessage2& aMessage );


    /**
     * Stop connection monitoring
     * @since S60 3.1
     */
    void StopConnectionMonitorL( );

private:
    /**
     * 2nd phase constructor.
     * @since S60 3.1
     * @param aServer Pointer to server class
     */
    void ConstructL( CUpnpContentServer* aServer );

    /**
     * C++ constructor.
     * @since S60 3.1
     */
    CUpnpContentServerSession();

    /**
     * C++ destructor.
     * @since S60 3.1
     */
    virtual ~CUpnpContentServerSession();

private:
    /**
     * Handle to server
     * not owned
     */
    CUpnpContentServer* iServer;

    /** 
     * Handle to handler
     * not owned
     */
    CUpnpContentServerHandler* iHandler;

    /**
     * Result buffer for client messages
     * not owned
     */
    CBufFlat* iResultBuffer;

    /**
     * Possible pending message about for content selection
     */
    RMessage2 iSelectionContentMsg;

    /**
     * Possible pending message about sharing
     */
    RMessage2 iChangeShareContentMsg;

    };

#endif // __UPNPCONTENTSERVERSESSION_H__

