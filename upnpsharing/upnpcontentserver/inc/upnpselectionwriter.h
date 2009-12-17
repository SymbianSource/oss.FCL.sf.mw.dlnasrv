/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUpnpSelectionWriter class definition
 *
*/






#ifndef UPNPSELECTIONWRITER_H
#define UPNPSELECTIONWRITER_H

// INCLUDES

#include <badesca.h>    // for CDesCArray
#include <centralrepository.h>
#include "upnpcontentserverdefs.h"


using namespace UpnpContentServer;

// CLASS DECLARATION

/**
 * CUpnpSelectionWriter utility class
 * @since S60 3.1
 */
class CUpnpSelectionWriter: public CBase
    {
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     * @since S60 3.1
     * @param aMediaType Type of selections currently active
     */
    static CUpnpSelectionWriter* NewL(
        TUpnpMediaType aMediaType );

    /**
     * Destructor.
     */
    virtual ~CUpnpSelectionWriter();

    /**
     * Append identifier of the container which is shared next
     * @since S60 3.1
     * @param aNewObject, name of the object
     */
    void AppendItemL( const TPtrC aNewObject );

    /**
     * Append index of succesfully shared container
     * @since S60 3.1
     * @param aIndex, Index of the container
     */
    void AppendStatusL( const TInt aIndex );

    /**
     * Saves sharing state to central repository and updates shared albums
     * @since S60 3.1
     * @param aShareAllSelection, The selections type in UI( Share all \
/ None / Many )
     * @return TInt, error code
     */
    TInt SaveSharingStateL( TInt aShareAllSelection );

private:  // New functions
    /**
     * 2nd phase constructor.
     * @since S60 3.1
     * @param aMediaType Type of selections currently active
     */
    void ConstructL( TUpnpMediaType aMediaType );

    /**
     * Save the containers to file
     * @since S60 3.1
     */
    TInt StoreSharedContainerIDsL() const;

    /**
     * C++ default constructor.
     * @since S60 3.1
     */
    CUpnpSelectionWriter();

private: //data
    /**
     * Mediatype selected from listbox
     */
    TInt                            iMediaType;

    /**
     * selection type
     */
    TInt                            iShareAllSelection;

    /**
     * array of succesfully shared containers.
     */
    RArray<TInt>           iOkItems;

    /**
     * array for albums/playlist
     * owned
     */
    CDesCArrayFlat*                 iFileArray;

    /**
     * Pointer to Central repository
     * owned
     */
    CRepository*                    iRepository;
    };

#endif
// End of File
