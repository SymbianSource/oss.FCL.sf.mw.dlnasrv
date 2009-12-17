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
* Description:      Base DLNA profile resolver class for image files.
*
*/






#ifndef C_UPNPIMAGESOLVERBASE_H
#define C_UPNPIMAGESOLVERBASE_H


// system includes
// NONE

// user includes
#include "upnpprofiler.h"

// forward declarations
// NONE

// constants
// NONE

/**
 *  Image file DLNA profile base resolver class.
 *
 *  This class is the base class for resolving DLNA profile of image files.
 *  It contains methods for querying the profiles supported by this solver 
 *  and for resolving a DLNA profile of a given image file.
 *
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CUpnpImageSolverBase ) : public CBase,
                                            public MUpnpProfiler
    {
public:

    static CUpnpImageSolverBase* NewL();

    static CUpnpImageSolverBase* NewLC();

    virtual ~CUpnpImageSolverBase();

// from base class MUpnpProfiler

    /**
     * From MUpnpProfiler.
     * SupportedProfilesL returns DLNA profiles that are currently supported.
     *
     * @since S60 v3.1
     * @param aProfiles Descriptor array where supported DLNA profiles are 
     *                  added.
     * @return TInt Error code, KErrNone if successfull.
     */
    TInt SupportedProfilesL( CDesC16ArrayFlat* aProfiles ) const;

    /**
     * From MUpnpProfiler.
     * ProfileForFileL is for resolving a DLNA profile of a given file.
     * Besides of file name, also mime type of the file is passed as a 
     * parameter in order to avoid re-opening of the file.
     *
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     * @param aMimeType Mime type of the file which is to be resolved.
     * @param aFile Handle to opened file specified by aFilename.
     * @return DLNA profile of the file as a HBufC*, 
     *         NULL if could not be resolved
     */
    HBufC* ProfileForFileL( const TDesC& aFilename, 
                            const TDesC8& aMimetype, 
                            RFile& aFile );
                            
private:

    CUpnpImageSolverBase();

    void ConstructL();
    };

#endif // C_UPNPIMAGESOLVERBASE_H
