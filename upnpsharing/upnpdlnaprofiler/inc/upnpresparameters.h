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
* Description:      Header file of CUpnpResParameters class. CUpnpResParameters 
*                is created to encapsulate information that is needed in a 
*                contentdirectory res element.
*
*/






#ifndef C_UPNPRESPARAMETERS_H
#define C_UPNPRESPARAMETERS_H

// system includes
#include <e32base.h>

// user includes
// NONE

// forward declarations
// NONE

// data types
// NONE

// constants
// NONE

/**
 *  CUpnpResParameters is created to encapsulate information that is needed
 *  in a contentdirectory res element.
 *
 *  @lib dlnaprofiler.lib
 *  @since S60 v3.1
 */
class CUpnpResParameters: public CBase
    {
public:

    IMPORT_C static CUpnpResParameters* NewL();

    IMPORT_C static CUpnpResParameters* NewLC();

    virtual ~CUpnpResParameters();

    // getters

    /**
     * ProtocolInfo returns the protocolinfo string.
     *
     * @since S60 v3.1
     * @return HBufC8* Protocolinfo
     */  
    IMPORT_C const TDesC8& ProtocolInfo() const;

    /**
     * FileSizeL returns the size of the file in bytes as descriptor.
     *
     * @since S60 v3.1
     * @return HBufC8* File Size
     */  
    IMPORT_C HBufC8* FileSizeL() const;

    /**
     * DurationL returns the duration in the following form:
     *     duration = hours ":" minutes ":" seconds
     *     hours = 1*5 DIGIT; 0-99999
     *     minutes = 2 DIGIT ; 00-59
     *     seconds = 2 DIGIT ["." 3 DIGIT] ; 00-59 (.000-.999) 
     *
     * @since S60 v3.1
     * @return HBufC8* Duration
     */  
    IMPORT_C HBufC8* DurationL() const;

    /**
     * ResolutionL returns the resolution in form widthXheight.
     *
     * @since S60 v3.1
     * @return HBufC8* Resolution
     */  
    IMPORT_C HBufC8* ResolutionL() const;

    /**
     * MimeType is a getter for mime type.
     *
     * @since S60 v3.1
     * @return HBufC8* Mime type
     */  
    const TDesC8& MimeType();
    
    // Methods for querying file type

    /**
     * IsImageFile returns true if file is image file, false otherwise.
     *
     * @since S60 v3.1
     * @return TBool
     */  
    IMPORT_C TBool IsImageFile() const ;

    /**
     * IsAudioOrVideoFile returns true if file is audio or video file, 
     * false otherwise.
     *
     * @since S60 v3.1
     * @return TBool 
     */  
    IMPORT_C TBool IsAudioOrVideoFile() const;

    // setters

    /**
     * SetProtocolInfoL sets the protocolinfo attribute.
     *
     * @since S60 v3.1
     * @param const TDesC8& Protocolinfo
     */  
    void SetProtocolInfoL( const TDesC8& aProtocolInfo );

    /**
     * SetMimetypeL sets the mime type attribute.
     *
     * @since S60 v3.1
     * @param const TDesC8& Mime type
     */  
    void SetMimetypeL( const TDesC8& aMimetype );

    /**
     * SetFileSize sets the file size attribute.
     *
     * @since S60 v3.1
     * @param const TInt File size
     */  
    void SetFileSize( const TInt aFileSize );

    /**
     * SetResolution sets the resolution attribute.
     *
     * @since S60 v3.1
     * @param const TSize Resolution
     */  
    void SetResolution( const TSize aResolution );

    /**
     * SetDurationInSeconds sets the duration attribute.
     *
     * @since S60 v3.1
     * @param const TInt64 Duration
     */  
    void SetDurationInSeconds( const TInt64 aDuration );


private:

    CUpnpResParameters();

    void ConstructL();

private: // data
    HBufC8* iProtocolInfo;   // owned
    HBufC8* iMimetype;       // owned
    TInt iFileSize;
    TSize iResolution;
    TInt64 iDurationInSeconds;
    
    };


#endif // C_UPNPRESPARAMETERS_H
