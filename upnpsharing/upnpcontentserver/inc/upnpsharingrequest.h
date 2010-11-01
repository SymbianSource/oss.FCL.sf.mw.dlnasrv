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
* Description:      CUpnpSharingRequest class definition
 *
*/





#ifndef __UPNPSHARINGREQUEST_H__
#define __UPNPSHARINGREQUEST_H__

// Include Files
#include <e32base.h>
#include <badesca.h>

#include "upnpcontentserverdefs.h" //TUpnpMediaType

/**
 * Helper class to store information about ongoing/pending sharing request
 * @since S60 3.1
 */
class CUpnpSharingRequest : public CBase
    {
public :

    /**
     * 2 phased contructor
     * @since S60 5.2
     * @param aMediaType Type of sharing request (EImageAndVideo or EPlaylist)
     * @param aSharingType Type of sharing request (EShareNone, EShareAll, 
     *         EShareMany)
     */
    static CUpnpSharingRequest* NewL( 
            UpnpContentServer::TUpnpMediaType aMediaType, 
            TInt aSharingType );

    /**
     * Sets the share, unshare and clfids arrays
     * @since S60 5.2
     * @param aShareArr Files to be shared
     * @param aUnshareArr Files to be unshared
     * @param aClfIds Indexes of selection relative to albums or playlists
     */
    void SetSharingRequestInfo( 
            RArray<TFileName>* aShareArr,
            RArray<TFileName>* aUnshareArr,
            CDesCArray* aClfIds );

    /**
     * Destructor
     */
    virtual ~CUpnpSharingRequest();

private:

    /**
     * 2nd phase constructor.
     * @since S60 5.2
     */
    void ConstructL() {};

    /**
     * Constructor
     * @since S60 5.2
     * @param aMediaType Type of sharing request (EImageAndVideo or EPlaylist)
     * @param aSharingType Type of sharing request (EShareNone, EShareAll, 
     *         EShareMany)
     */
    CUpnpSharingRequest( 
            UpnpContentServer::TUpnpMediaType aMediaType,
            TInt aSharingType );

public :
// (These member variables are used without getters or setters in 
// contentserver, which is why they are public. CUpnpSharingRequest is
// only a data storage)

    /**
     * Determines the type of sharing:
     * images&videos or music
     */
    UpnpContentServer::TUpnpMediaType iMediaType;

    /**
     * Type of sharing request( EShareNone, EShareAll, EShareMany )
     */
    TInt iSharingType;

    /**
     * The progress of sharing if it is ongoing
     */
    TInt iProgress;

    /**
     * Files to be shared
     * owned
     */
    RArray<TFileName>* iShareArr;
      
    /**
     * Files to be unshared
     * owned
     */
    RArray<TFileName>* iUnshareArr;   
  
    /**
     * Ids of selection relative to albums or playlists
     * owned
     */
    CDesCArray* iClfIds;
  
    };

#endif // __UPNPSHARINGREQUEST_H__

// End of file
