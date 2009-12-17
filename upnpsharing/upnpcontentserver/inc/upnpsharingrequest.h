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
#include <e32def.h> //CArrayFix
#include <badesca.h> //CDesCArray


/**
 * Helper class to store information about ongoing/pending sharing request
 * @since S60 3.1
 */
class CUpnpSharingRequest : public CBase
    {
public :

    /**
     * 2 phased contructor
     * @since S60 3.1
     * @param aKind Type of sharing request( images&videos or music )
     * @param aArr Indexes of selection relative to albums or playlists
     * @param aNameArray Names of containers to share
     */
    static CUpnpSharingRequest* NewL(
        TInt aKind,
        const RArray<TInt>& aArr,
        CDesCArray* aIdArray = NULL,
        CDesCArray* aNameArray = NULL );

    /**
     * 2nd phase constructor.
     * @since S60 3.1
     * @param aArr Indexes of selection relative to albums or playlists
     * @param aNameArray Names of containers to share
     */
    void ConstructL( const RArray<TInt>& aArr,
                    CDesCArray* aIdArray,
                    CDesCArray* aNameArray );

    /**
     * Constructor
     * @since S60 3.1
     * @param aKind Type of sharing request( images&videos or music )
     */
    CUpnpSharingRequest( TInt aKind );

    /**
     * Destructor
     */
    virtual ~CUpnpSharingRequest();

public :
    /**
     * Determines the type of sharing:
     * images&videos or music
     */
    TInt iKind;

    /**
     * The number of items in selection
     */
    TInt iItemCount;

    /**
     * The progress of sharing if it is ongoing
     */
    TInt iProgress;

    /**
     * Selections for the sharing request
     */
    RArray<TInt> iSelections;

    /**
     * Array for object IDs
     */
    CDesCArray* iObjectIds;
    
    /**
     * Array for object names
     */
    CDesCArray* iObjectNames;
    };


#endif // __UPNPSHARINGREQUEST_H__
