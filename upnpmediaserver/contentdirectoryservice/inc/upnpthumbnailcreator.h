/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declares ThumbnailCreator class.
*
*/



#ifndef C_CUPNPTHUMBNAILCREATOR_H
#define C_CUPNPTHUMBNAILCREATOR_H

//INCLUDE
#include <e32base.h>
#include <f32file.h>
#include <gulicon.h>
#include <mdaimageconverter.h>
#include "upnpcontentdirectoryglobals.h"


//FORWARD DECLARATIONS
class CImageEncoder;
class CImageDecoder;
class CBitmapScaler;
class CFbsBitmap;
class CUpnpThumbnailCreator;
class MUpnpThumbnailCreatorObserver;


/**
*  @brief Thumbnail Creator Observer
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class MUpnpThumbnailCreatorObserver
{ 
public:
    /*
    *
    * @since Series S60 3.1
    * @param aThCreator
    * @param Result
    */
    virtual void NotifyThumbnailResultL(CUpnpThumbnailCreator* aThCreator, TInt aResult ) = 0;
};



/**
*  @brief Thumbnail Creator
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
NONSHARABLE_CLASS( CUpnpThumbnailCreator ): public CActive
    {
public:

    enum TPhase
    {
        EDecode,
        EEncode,
        EScaling,
        ESleep,
        EDecodeObject
    };


    /**
    * Two-phased constructor for building CUpnpThumbnailCreator.
    * @since Series60 3.1
    * @param aObserver
    */
    static CUpnpThumbnailCreator* NewLC( MUpnpThumbnailCreatorObserver* aObserver );

    /**
    * Destructor function; called automatically from the destructor of a 
    * derived class
    **/
    ~CUpnpThumbnailCreator();


    /**
    * Function creates thumbnail for given image file
    * @since Series60 3.1
    * @param aSourceFile full file name
    **/
    void CreateL( const TDesC& aSourceFile, const TDesC& aThumbName );

    /**
    *
    * @since Sereries S60 3.1
    */
    TFileName GetFileName();

    /**
    *
    * @since Sereries S60 3.1
    */
    TPtrC GetFileExt();

    /**
    * Getter for file size 
    */
    TFileName GetPath();
    
    /**
    * Getter for file size 
    **/
    TSize GetSize(const TDesC& aSourceFile);

private:

    /**
    * By default Symbian 2nd phase constructor is private
    * @since Series60 3.1
    */     
    void ConstructL();

    /**
    * First phase constructor.
    * @param aObserver - pointer to observer
    * @since Series60 3.1
    **/
    CUpnpThumbnailCreator( MUpnpThumbnailCreatorObserver* aObserver );

    /**
    * Encode bitmap to jpeg and writes it to file.
    * @since Series60 3.1
    **/
    void EncodeAndConvertL();

    /**
    * Implementation of pure virtual from CActive
    * serves decoding phases completion
    * @since Series60 3.1
    */
    void RunL();

    /**
    * Implementation of virtual from CActive
    * serves decoding phases completion
    * @since Series60 3.1
    */
    TInt RunError(TInt aErr);

    /**
    * Implementation of pure virtual from CActive
    * @since Series60 3.1
    */
    void DoCancel();

    /**
    * Calculate resizing ratio (2,4 and 8) for decoder
    * in order not to create big bitmaps into memory 
    * @param aLoadSize - function stores computed size in it
    * @since Series60 3.1
    */
    void CalculateSize( TSize& aLoadSize );

    /**
    * Used internally by CalculateSize
    * @since Series60 3.1
    */
    TInt DivisionCeil( const TInt aVal, const TInt aDiv );


private:
    //Observer
    MUpnpThumbnailCreatorObserver* iObserver;
    //File server
    RFs                     iFs;
    //Image decoder
    CImageDecoder*          iDecoder;
    //Image encoder
    CImageEncoder*          iEncoder;
    //Scaler
    CBitmapScaler*          iScaler;
    //Bitmap
    CFbsBitmap*             iBitmap;
    //Bitmap default size
    TSize                   iSize;

    // Parser
    TParse                  iParse;
    // Extension
    TBuf<KExtensionLength>  iExtension;
    //State enum
    TPhase                  iPhase;
    //File name e.g. pic.jpg
    TFileName               iFileName;
    //Path
    TPath                   iPath;
    //Image type uid
    TUid                    iImageType;
    //Old image size 
    TSize                   iOldSize; 
    //frame info
    TFrameInfo              iInfo;
    };

#endif  // C_CUPNPTHUMBNAILCREATOR_H
