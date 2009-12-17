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
* Description:      Definition of a MUpnpProfiler interface class.
*
*/






#ifndef M_UPNPPROFILER_H
#define M_UPNPPROFILER_H


// system includes
#include <e32base.h>

// user includes
// NONE

// forward declarations
class CDesC16ArrayFlat;

// constants
// NONE

/**
 *  MUpnpProfiler interface class
 *
 *  MUpnpProfiler defines an interface for media solver plugins. It contains
 *  methods for querying the profiles supported by this solver plugin and
 *  for resolving a DLNA profile of a given file.
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
class MUpnpProfiler
    {

public:

    virtual ~MUpnpProfiler() {};
    
    /**
     * SupportedProfilesL returns DLNA profiles that are currently supported.
     *
     * @since S60 v3.1
     * @param aProfiles Descriptor array where supported DLNA profiles are 
     *                  added.
     * @return TInt Error code, KErrNone if successfull.
     */
    virtual IMPORT_C TInt SupportedProfilesL( 
                                    CDesC16ArrayFlat* aProfiles ) const = 0;

    /**
     * ProfileForFileL is for resolving a DLNA profile of a given file.
     * Besides of file name, also mime type of the file is passed as a 
     * parameter in order to avoid re-opening of the file.
     *
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     * @param aMimeType Mime type of the file which is to be resolved.
     * @param aFile Handle to opened file specified by aFilename
     * @return DLNA profile of the file as a HBufC*, 
     *         NULL if could not be resolved
     */
    virtual IMPORT_C HBufC* ProfileForFileL( const TDesC& aFilename, 
                                             const TDesC8& aMimetype, 
                                             RFile& aFile 
                                           ) = 0;

    };


#endif // M_UPNPPROFILER_H
