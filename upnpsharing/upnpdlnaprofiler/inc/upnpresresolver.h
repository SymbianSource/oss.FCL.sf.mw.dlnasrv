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
* Description:      Header file of CUpnpResResolver class. CUpnpResResolver is 
*                a class used for resolving res field attributes for a given 
*                file.
*
*/






#ifndef C_UPNPRESRESOLVER_H
#define C_UPNPRESRESOLVER_H

// system includes
#include <e32base.h>
#include <mdaaudiosampleplayer.h> // CMdaAudioPlayerUtility

// user includes
// NONE

// forward declarations
class RFile;
class CUpnpResParameters;

// data types
// NONE

// constants
// NONE

/**
 *  CUpnpResResolver is a class for resolving res field attributes for a 
 *  given file.
 *
 *  CUpnpResResolver class can be used to resolve a res field attributes for 
 *  a media file (image, AV, video). Attributes resolved for media files are:
 *     - Protocolinfo for all files.
 *     - Resolution and size for images.
 *     - Duration and size for videos.
 *     - Duration and size for music.
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
class CUpnpResResolver: public CBase, 
                        public MMdaAudioPlayerCallback
    {
public:

    IMPORT_C static CUpnpResResolver* NewL();

    IMPORT_C static CUpnpResResolver* NewLC();

    virtual ~CUpnpResResolver();

    /**
     * GetResParametersL resolves parameters that will be needed in 
     * res-element attributes.
     *
     * @since S60 v3.1
     * @param const TDesC& File for which to obtain res parameters.
     * @return CUpnpResParameters* Parameters needed for res-element.
     */  
    IMPORT_C CUpnpResParameters* GetResParametersL( const TDesC& aFilename );
    
private:

    CUpnpResResolver();

    void ConstructL();

    /**
     * GetMimetypeL returns the mime type of the file.
     *
     * @since S60 v3.1
     * @param RFile& File for which to obtain mime.
     * @return HBufC8* Mime type of the file.
     */  
    HBufC8* GetMimetypeL( RFile& aFile );
    
    /**
     * GetAudioDurationL obtains the duration of a audio file.
     * After call the duration is in iDuration member variable.
     *
     * @since S60 v3.1
     * @param const TDesC& The audio file for which to obtain duration.
     */  
    void GetAudioDurationL( const TDesC& aFilename );

    /**
     * GetVideoDurationL obtains the duration of a video file.
     * After call the duration is in iDuration member variable.
     *
     * @since S60 v3.1
     * @param RFile& The video file for which to obtain duration.
     */  
    void GetVideoDurationL( RFile& aFile );

    /**
     * GetFileSizeL returns the size of file in bytes.
     *
     * @since S60 v3.1
     * @param RFile& The file for which to obtain size
     * @return TInt File size.
     */  
    TInt GetFileSizeL( RFile& aFile );

    /**
     * GetImageResolutionL returns the resolution of the image.
     *
     * @since S60 v3.1
     * @param RFile& File for which to obtain resolution.
     * @param const TDesC8& Mime type of the file.
     * @return TSize The resolution of the image. 
     */  
    TSize GetImageResolutionL( RFile& aFile, const TDesC8& aMimetype );

// from base class MMdaAudioPlayerCallback

    /**
     * From MMdaAudioPlayerCallback.
     * MapcPlayComplete 
     *
     * @since S60 v3.1
     * @param aError error code
     */
    void MapcPlayComplete( TInt aError );

    /**
     * From MMdaAudioPlayerCallback.
     * MapcInitComplete 
     *
     * @since S60 v3.1
     * @param aError error code
     * @param aDuration duration of the file
     */
    void MapcInitComplete( TInt aError, 
                           const TTimeIntervalMicroSeconds& aDuration );
           
private: // data
    // attributes related to CMdaAudioPlayerUtility usage
    CActiveSchedulerWait*       iWait; // owned
    CMdaAudioPlayerUtility*     iAudioplayer; // owned. 
    
    // duration of the media file (video or audio)
    TTimeIntervalMicroSeconds   iDuration;
    };


#endif // C_UPNPRESRESOLVER_H
