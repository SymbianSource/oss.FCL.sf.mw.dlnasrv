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
* Description:  Shrinker class in the Memory manager component
*
*/


#ifndef C_CMMMSHRINKER_H
#define C_CMMMSHRINKER_H

#include <e32base.h>
#include "cmcommon.h" // TCmListItemStatus

/* Forward declarations. */
class MCmMmObserver;
class CCmMmImageMetadataResolver;
class CFbsBitmap;
class CImageDecoder;
class CBitmapScaler;
class CImageEncoder;
class CCmDmMain;

/**
 *  CCmMmShrinker class
 *
 *  @lib cmmemorymanager.lib
 *  @since S60 v3.1
 */
class CCmMmShrinker : public CActive
    {

// States
    enum TImageState
        {
        EIdle = 0,
        EDecode,
        EScale,
        EEncode,
        EReplace
        };

public: // Constructors and destructor

    /**
     * Creates new CCmMmShrinker class and opens connection to database
     *
     * @since S60 3.1
     * @param aDbManager
     * @return pointer to CCmMmShrinker object
     */
    static CCmMmShrinker* NewL( CCmDmMain& aDbManager );

    /**
     * Creates new CCmMmShrinker class and opens connection to database and
     * leaves the instance in the cleanup stack
     *
     * @since S60 3.1
     * @param aDbManager
     * @return pointer to CCmMmShrinker object
     */
    static CCmMmShrinker* NewLC( CCmDmMain& aDbManager );

    /**
     * C++ destructor
     *
     * @since S60 3.1
     */
    virtual ~CCmMmShrinker();

public:     // New functions

    /**
     * Shrinks given images.
     *
     * @since S60 3.1
     * @param aFiles
     */
    void ShrinkImagesL( CDesCArray& aFiles );
    
    /**
     * Sets memory manager observer
     *
     * @since S60 3.1
     * @param aObserver
     */
    void SetObserver ( MCmMmObserver* aObserver );

protected: // Functions from base class

    /**
     * From CActive
     * Cancels any outstanding operation
     *
     * @since S60 3.1
     */
    void DoCancel();

    /**
     * Handles a leave occurring in the request
     * completion event handler RunL()
     *
     * @since S60 3.1
     * @param aError
     * @return error value
     */
    TInt RunError( TInt aError );

    /**
     * From CActive
     * Callback function
     * Invoked to handle responses from the server
     *
     * @since S60 3.1
     */
    void RunL();

 private: // New methods

    /**
     * Return screensize
     *
     * @since S60 3.1
     * @return screensize
     */
    TSize ScreenSizeL();
 
    /**
     * Returns the private path according to the specified file name
     *
     * @since S60 3.1
     * @param aFs handle to file server
     * @param aPrivatePath reference to descriptor where private path is 
                           modified.
     * @param aOriginal original file path used to define drive and file name
     */     
     void PrivatePath( RFs& aFs, 
                       TFileName& aPrivatePath, 
                       const TFileName& aOriginal );

    /**
     * Clear shrinker mermber variables
     *
     * @since S60 3.1
     * @param None
     */                       
     void ClearShrinker();                                              

private:

    /**
     * C++ constructor
     *
     * @since S60 3.1
     * @param aDbManager
     */
    CCmMmShrinker( CCmDmMain& aDbManager );

    /**
     * Second-phase constructor.
     *
     * @since S60 3.1
     * @param aFiles
     */
    void ConstructL();



private: // data
    RFs               iFileSession;
    TSize             iScreenSize;
    TInt              iShrinkIndex;
    TTime             iStartTime;
    TImageState       iState;
    TTime             iStopTime;
    MCmMmObserver*    iObserver;        // not owned
    CCmDmMain&        iDbManager;
    CFbsBitmap*       iBitmap;          // owned
    CBitmapScaler*    iBitmapScaler;    // owned
    CDesCArray*       iFiles;           // owned
    CImageDecoder*    iImageDecoder;    // owned
    CImageEncoder*    iImageEncoder;    // owned
    TFileName         iTempFilename;
    CCmMmImageMetadataResolver* iImageMetadataResolver;
    };

#endif //  C_CMMMSHRINKER_H
