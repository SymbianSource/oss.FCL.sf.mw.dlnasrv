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
* Description:      Base DLNA profile resolver class for av files.
*
*/






#ifndef C_UPNPAVSOLVERBASE_H
#define C_UPNPAVSOLVERBASE_H


// system includes
#include <3gplibrary/mp4lib.h>

// user includes
#include "upnpprofiler.h"

// forward declarations
class CVideoRecorderUtility;

// constants
const TInt TMP4DecoderSpecificInfoSize = 16;

// NONE

/**
 *  Av file DLNA profile base resolver class.
 *
 *  This class is the base class for resolving DLNA profile of av files.
 *  It contains methods for querying the profiles supported by this solver 
 *  and for resolving a DLNA profile of a given av file.
 *
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CUpnpAvSolverBase ) : public CBase,
                                         public MUpnpProfiler
    {

public:

    static CUpnpAvSolverBase* NewL();

    static CUpnpAvSolverBase* NewLC();

    virtual ~CUpnpAvSolverBase();

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
     * @param aFile Handle to opened file specified by aFilename
     * @return DLNA profile of the file as a HBufC*, 
     *         NULL if could not be resolved
     */
    HBufC* ProfileForFileL( const TDesC& aFilename, 
                            const TDesC8& aMimetype,
                            RFile& aFile );


private:

    CUpnpAvSolverBase();

    void ConstructL();

    /**
     * GetVideoFileInformationL
     *
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     * @return Error code. KErrNone if successfull.
     */
    TInt GetVideoFileInformationL( RFile& aFile );

    /**
     * A generic data container to be used in parsing different
     * MPEG4 codec headers
     */
    class TMP4DecoderSpecificInfo
        {
        public:
            TUint32 iData[TMP4DecoderSpecificInfoSize];
        };


private: // data

    // Type of video codec within the AV file
    mp4_u32 iVideoType;

    // Video resolution X size
    mp4_u32 iVideoResolutionX;

    // Video resolution Y size
    mp4_u32 iVideoResolutionY;

    mp4_double iVideoFramerate;

    // Type of audio codec within the AV file
    mp4_u32 iAudioType;

    // stream bitrate
    mp4_u32 iStreamAverageBitrate;

    // codec-specific details
    TMP4DecoderSpecificInfo iCodecInfo;

    };

#endif // C_UPNPAVSOLVERBASE_H
