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
* Description:      Base DLNA profile resolver class for audio files.
*
*/






#ifndef C_UPNPAUDIOSOLVERBASE_H
#define C_UPNPAUDIOSOLVERBASE_H


// system includes
#include <mda/client/utility.h> // MMdaObjectStateChangeObserver
#include <mda/common/audio.h> // TMdaFileClipLocation
#include <mmf/common/mmfutilities.h> // TFourCC

// user includes
#include "upnpprofiler.h"

// forward declarations
class CMdaAudioConvertUtility;
class TMdaFileClipLocation;

// constants
// NONE

/**
 *  Audio file DLNA profile base resolver class.
 *
 *  This class is the base class for resolving DLNA profile of audio files.
 *  It contains methods for querying the profiles supported by this solver 
 *  and for resolving a DLNA profile of a given audio file.
 *
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CUpnpAudioSolverBase ) : 
                                        public CBase,
                                        public MUpnpProfiler, 
                                        public MMdaObjectStateChangeObserver
    {

public:

    static CUpnpAudioSolverBase* NewL();

    static CUpnpAudioSolverBase* NewLC();

    virtual ~CUpnpAudioSolverBase();

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

    CUpnpAudioSolverBase();

    void ConstructL();

    /**
     * GetAudioFileInformationL
     *
     * @since S60 v3.1
     * @param aFileName Filename of the file which is to be resolved.
     * @return Error code. KErrNone if successfull.
     */
    TInt GetAudioFileInformationL( const TDesC& aFilename );

    
// from base class MMdaObjectStateChangeObserver

    /**
     * From MMdaObjectStateChangeObserver.
     * MoscoStateChangeEvent 
     *
     * @since S60 v3.1
     * @param aObject
     * @param aPreviousState
     * @param aCurrentState
     * @param aErrorCode
     */
    void MoscoStateChangeEvent( CBase *aObject, TInt aPreviousState, 
                                TInt aCurrentState, TInt aErrorCode );

    /**
     * Determine3GppProfileL chooses the correct profile among supported 3gpp 
     * profiles. The decision is made based on audio file attributes that 
     * are stored in member variables of this class. 
     *
     * @since S60 v3.1
     * @return HBufC*. Pointer to string descriptor containing the profile.
     */
    HBufC* Determine3GppProfileL() const;

    /**
     * DetermineWmaProfileL chooses the correct profile among supported wma 
     * profiles. The decision is made based on audio file attributes that 
     * are stored in member variables of this class. 
     *
     * @since S60 v3.1
     * @return HBufC*. Ponter to string descriptor containing the profile.
     */
    HBufC* DetermineWmaProfileL() const;

    /**
     * DetermineMp4ProfileL chooses the correct profile among supported mp4 
     * profiles. The decision is made based on audio file attributes that 
     * are stored in member variables of this class. 
     *
     * @since S60 v3.1
     * @return HBufC*. Pointer to string descriptor containing the profile.
     */
    HBufC* DetermineMp4ProfileL() const;

private: // data
    CMdaAudioConvertUtility*    iAudioconverter; // owned
    CActiveSchedulerWait*       iWait; // owned
    
    // attributes related to audio file attributes. 
    TUint iChannelCount;
    TUint iSampleRate;
    TUint iBitrate;
    };

#endif // C_UPNPAUDIOSOLVERBASE_H
