/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      AO for file sharing operations
*
*/






#ifndef C_UPNPFILESHARINGACTIVE_H
#define C_UPNPFILESHARINGACTIVE_H

// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CUPnPAVRenderingSessionImpl;
class CUpnpFileSharing;
class CUpnpItem;

/**
 *  Used to make an asynchronous file sharing operation to synchronous
 *  operation. Handles UPnP Security for a file.
 *
 *  @since S60 v3.1
 */
class CUPnPFileSharingActive : public CActive
    {

public:

    /**
     * Static 1st phase construct
     */
    static CUPnPFileSharingActive* NewL();
    
    /**
     * Destructor
     */
    virtual ~CUPnPFileSharingActive();
       
private:

    /**
     * Constructor
     */
    CUPnPFileSharingActive();
    
    void ConstructL();

private: // Private methods for accessing UPnP security

    /**
     * Allows/denied access to the files to which the given item's res-
     * elements are pointing.
     *
     * @since Series 60 3.2
     * @param aItem the item
     * @param aAccessAllowed ETrue to allow and EFalse to deny the access
     */
    void SetAccesstoItemResourcesL( CUpnpItem& aItem, TBool aAccessAllowed );
    
protected: // From CActive

    /**
     * See e32base.h
     */
    void RunL();
    
    /**
     * See e32base.h
     */
    void DoCancel();
    
    /**
     * See e32base.h
     */
    TInt RunError( TInt aError );

public: // New functions    

    /**
     * Shares an item
     *
     * @param item to share
     */
    void ShareItemL( CUpnpItem& aItem );
    
    /**
     * Unshares an item
     *
     * @param aId item to unshare (id)
     */
    void UnShareItemL( const TDesC8& aId );

private:
    
    CUpnpFileSharing* iFileSharing; // Own
    
    };

#endif // C_UPNPAVEVENTACTIVE_H
