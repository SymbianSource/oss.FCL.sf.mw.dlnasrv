/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Metadata resolver class in the Memory manager component
*
*/


#ifndef C_CMMMIMAGEMETADATARESOLVER_H
#define C_CMMMIMAGEMETADATARESOLVER_H

#include <e32base.h>
#include <f32file.h> // RFs

// Forward declarations
class CExifRead;
class CExifModify;

/**
 *  CCmMmImageMetadataResolver class
 *  Part of Memory manager
 *
 *  @lib cmmemorymanager.lib
 *  @since S60 v3.2
 */
class CCmMmImageMetadataResolver : public CBase
    {

public: // Symbian constructors and C++ destructor

    /**
     * Creates new CCmMmImageMetadataResolver class
     *
     * @since S60 3.2
     * @param aFileSession
     * @return pointer to CCmMmImageMetadataResolver object
     */
    static CCmMmImageMetadataResolver* NewL( RFs& aFileSession );

    /**
     * Creates new CCmMmImageMetadataResolver class and leaves the instance
     * in the cleanup stack
     *
     * @since S60 3.2
     * @param aFileSession
     * @return  pointer to CCmMmImageMetadataResolver object
     */
    static CCmMmImageMetadataResolver* NewLC( RFs& aFileSession );

    /**
     * @since S60 3.2
     *
     * C++ destructor
     */
    virtual ~CCmMmImageMetadataResolver();

public: // New methods

    /**
     * Captures orginal metadata
     * @since S60 3.2
     * @param aOrigFileName, orginal file name
     * @return None
     */
    void CaptureOrginalMetadataL( const TDesC& aOrigFileName );
    
    /**
     * Captures and resolves image metadata
     * @since S60 3.2
     * @param aShrinkedFileName, shrinked file name
     * @return None
     */
    void ResolveMetadataL( const TDesC& aShrinkedFileName );

private:

    /**
     * Fills orginal exif data to shrinked image
     * @since S60 3.2
     * @param aRead, exif reader
     * @param aModify, exif modifier
     * @return None
     */
    void FillExifL( CExifRead& aRead, CExifModify& aModify );
    
private: // Constructors

    /**
     * C++ constructor
     *
     * @since S60 3.2
     * @param None
     */
    CCmMmImageMetadataResolver( RFs& aFileSession );

    /**
     * Second-phase constructor
     *
     * @since S60 3.2
     */
    void ConstructL();

private: // data
    
    /** File session*/
    RFs& iFileSession;
    
    /** Orginal Metadata ( owned ) */
    HBufC8* iExifOrig;
    
    };

#endif // C_CMMMIMAGEMETADATARESOLVER_H
