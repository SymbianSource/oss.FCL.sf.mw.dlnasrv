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
* Description:      Header file of CUpnpDlnaProfiler class. CUpnpDlnaProfiler is 
*                a class used for resolving a DLNA profile of a given file.
*
*/






#ifndef C_UPNPDLNAPROFILER_H
#define C_UPNPDLNAPROFILER_H

// system includes
#include <e32base.h>

// user includes
// NONE

// forward declarations
class MUpnpProfiler;
class RFile;
class CDesC16ArrayFlat;
class CUpnpResParameters;

// data types
// NONE

// constants
const TInt KMimeStartLength = 5;
_LIT8( KAudio, "audio" );
_LIT8( KImage, "image" );
_LIT8( KVideo, "video" );

/**
 *  CUpnpDlnaProfiler is a class for resolving DLNA profile for a given file.
 *
 *  CUpnpDlnaProfiler class can be used to resolve a DLNA profile for a media 
 *  file (image, AV, video). User of this class can also query for a list of 
 *  currently supported DLNA profiles.
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
class CUpnpDlnaProfiler: public CBase
    {
public:

    IMPORT_C static CUpnpDlnaProfiler* NewL();

    IMPORT_C static CUpnpDlnaProfiler* NewLC();

    virtual ~CUpnpDlnaProfiler();

    /**
     * SupportedProfilesL returns DLNA profiles that are currently supported.
     *
     * @since S60 v3.1
     * @param aProfiles Descriptor array where supported DLNA profiles are 
     *                  added.
     * @return TInt Error code, KErrNone if successfull.
     */
    IMPORT_C TInt SupportedProfilesL( CDesC16ArrayFlat* aProfiles ) const;

    /**
     * ProfileForFileL is for resolving a DLNA profile of a given file.
     *
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     *        Filename must contain full path (drive+path+filename).
     * @return DLNA profile of the file as a HBufC*, 
     *         NULL if could not be resolved
     */
    IMPORT_C HBufC* ProfileForFileL( const TDesC& aFilename );
    
    /**
     * ProfileForFileL is for resolving a DLNA profile of a given file.
     * Takes RFile and CUpnpResParameters parameters in order to minimize 
     * the amount of file opening.
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     *        Filename must contain full path (drive+path+filename).
     * @param aFile Handle to opened file specified by aFilename.
     * @param aParameters Res parameters that are already known.
     * @return DLNA profile of the file as a HBufC*, 
     *         NULL if could not be resolved
     */
    IMPORT_C HBufC* ProfileForFileL( const TDesC& aFilename, 
                                     RFile& aFile,
                                     CUpnpResParameters& aParameters );
    
private:

    CUpnpDlnaProfiler();

    void ConstructL();

    /**
     * DlnaProfileFromSolversL checks the mime type given as parameter and
     * forwards the recognition task to solver plugins corresponding the
     * mimetype.
     * @param aFileName Filename of the file which is to be resolved.
     *        Filename must contain full path (drive+path+filename).
     * @param aMimeType Mime type of file specified by aFilename.
     * @param aFile Handle to opened file specified by aFilename.
     * @return pointer to descriptor containing resolved DLNA profile
     * @since S60 v3.1
     */
    HBufC* DlnaProfileFromSolversL( const TDesC& aFilename, 
                                    const TDesC8& aMimetype, 
                                    RFile& aFile );

private: // data

    /**
     * iAudioSolvers is a list of audio solver plugins. Base solver plugin is 
     * the first one on the list and the rest are product specific solvers.
     * These are used for resolving a DLNA profile of an audio file.
     */
    RPointerArray<MUpnpProfiler> iAudioSolvers; // elements owned

    /**
     * iAvSolvers is a list of AV solver plugins. Base solver plugin is 
     * the first one on the list and the rest are product specific solvers.
     * These are used for resolving a DLNA profile of an AV (video) file.
     */
    RPointerArray<MUpnpProfiler> iAvSolvers; // elements owned
    
    /**
     * iImageSolvers is a list of image solver plugins. Base solver plugin is 
     * the first one on the list and the rest are product specific solvers.
     * These are used for resolving a DLNA profile of an image file.
     */
    RPointerArray<MUpnpProfiler> iImageSolvers; // elements owned

    };


#endif // C_UPNPDLNAPROFILER_H
