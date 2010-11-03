/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      
*
*/

// This file defines the API for ThumbnailCreator.dll

#ifndef __UPNPTHUMBNAILCREATOR_H__
#define __UPNPTHUMBNAILCREATOR_H__

//  Include Files

#include <e32base.h>	// CBase
#include <e32std.h>	 // TBuf
#include <mdesession.h>
#include <mdeconstants.h>
#include <mdenamespacedef.h>
#include <mdequery.h>
#include <mdeobject.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#include "upnpthumbnailcreatorobserver.h"

class CMdeSession;
class CMdEObjectQuery;
class CFbsBitmap;
class CImageEncoder;
//  Constants

enum TThumbnailDlnaSize
    {
    ESmall = 0,
    EMedium,
    ELarge,
    EThumbnail,
    ESmallIcon,
    ELargeIcon
    };

//  Class Definitions

NONSHARABLE_CLASS( CUpnpThumbnailCreator) : private CActive,
                                          private MMdESessionObserver,
                                          private MMdEQueryObserver,
                                          private MThumbnailManagerObserver
    {
public:
    // states for this engine
    enum TThumbnailCreatorStates 
        {
        EIdle = 0,
        EInitializeMde,
        EQueryingImage,
        EFetchingThumbnail,
        EConvertingThumbnail
        };
        // new functions
    
    /**
     * static NewL
     *
     * @param MUpnpThumbnailCreatorObserver, thumbnail creator observer
     * @param aFilePath, file path of the source file
     * @return instance to CUpnpThumbnailCreator
     */
    IMPORT_C static CUpnpThumbnailCreator* NewL(
                    MUpnpThumbnailCreatorObserver& aMThumbnailCreatorObserver,
                    const TDesC& aFilePath,
                    TThumbnailDlnaSize aSize);
    
    /**
     * static NewLC
     *
     * @param MUpnpThumbnailCreatorObserver, thumbnail creator observer
     * @param aFilePath, file path of the source file 
     * @return instance to CUpnpThumbnailCreator
     */
    IMPORT_C static CUpnpThumbnailCreator* NewLC(
                    MUpnpThumbnailCreatorObserver& aMThumbnailCreatorObserver,
                    const TDesC& aFilePath,
                    TThumbnailDlnaSize aSize);


    /**
     * On completion of the ThumbnailCreatorReady callback with status KErrNone, 
     * this method MdeObjectId returns the MdE object Id for the image.
     * @return ObjectId for the object in MdE 
     */
    IMPORT_C TInt MdeObjectId() const;
    
    /**
     * On completion of the ThumbnailCreatorReady callback with status KErrNone, 
     * this method ThumbnailFilePath returns the thumbnail file path.
     * @return filepath of the thumbnail 
     */
    IMPORT_C const TDesC& ThumbnailFilePath() const;
    
    /**
     * On completion of the ThumbnailCreatorReady callback with status KErrNone, 
     * this method Thumbnail returns the thumbnail
     * @return reference to thumbnail created 
     */
    IMPORT_C CFbsBitmap& Thumbnail() const;

public:
    
    /**
     *  Destructor
     */
     ~CUpnpThumbnailCreator();
    
private:
    /********************************************************************
     * MMdESessionObserver  virtual methods
     ********************************************************************/
    void HandleSessionOpened(CMdESession& aSession, TInt aError);
    void HandleSessionError(CMdESession& aSession, TInt aError);
    
    /********************************************************************
     * MMdEQueryObserver virtual methods
     ********************************************************************/
    void HandleQueryNewResults(CMdEQuery& aQuery, TInt aFirstNewItemIndex,
                                           TInt aNewItemCount);
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);
    
    /********************************************************************
     * MThumbnailManagerObserver virtual methods
     ********************************************************************/
    // Callbacks from MThumbnailManagerObserver for getting thumbnails
    void ThumbnailPreviewReady(MThumbnailData& aThumbnail,
                                            TThumbnailRequestId aId );
    
    void ThumbnailReady(TInt aError, MThumbnailData& aThumbnail, 
                                            TThumbnailRequestId aId );    
    

private:
    // CActive
     void DoCancel();
     void RunL();    
     TInt RunError (TInt aError);
    
private:
    // new functions
    
    // CI: erkki missing doxygen comments @param ...
    
    /** CUpnpthumbnailCreator Constructor*/
    CUpnpThumbnailCreator(
            MUpnpThumbnailCreatorObserver& aMUpnpThumbnailCreatorObserver,
            TThumbnailDlnaSize aSize);
    
    /** Two Phase constructor -ConstructL method*/
    void ConstructL(const TDesC& aFilePath);
    
    TSize ConvertDlnaSizeToImageSize();
    
    /** Query to Metadata engine*/
    void QueryMdeL();
    
    /** GetThumbnail from Thumbnailmanager*/
    void GetThumbnail();
    
    /** Pepare the filepath for thumbnail storage*/
    void PrepareThumbnailFilePathL();

    /** Notifies observer*/
    void ThumbnailReady( TInt aErr );
    
    /** Uses ICL encoder to Encode and Save the thumbnail*/
    void EncodeAndSaveThumbnailL();
    
    /** Completes active object*/
    void CompleteRequest(TInt aError);
    


private:
    // data
    CMdESession* iMdESession;  //owned
    CMdEQuery* iMdEQuery;      //owned
    CFbsBitmap* iThumbnail;    //owned
    HBufC* iImageFilePath;     //owned
    HBufC* iThumbnailFilePath; //owned
    TInt iObjectid;
    CThumbnailManager* iThmbManager; //owned
    MUpnpThumbnailCreatorObserver& iMUpnpThumbnailCreatorObserver;
    RFs iFsSession;    //owned
    CImageEncoder*         iImageEncoder; //owned, encoder from ICL API    
    TThumbnailCreatorStates iState;
    TThumbnailDlnaSize iThumbnailDlnaSize;
    };

#endif  // __UPNPTHUMBNAILCREATOR_H__

