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
* Description:      Cds Sync Implementation Header
*
*/







#ifndef __CDSSYNCIMPL_H__
#define __CDSSYNCIMPL_H__

#include <e32cons.h>
#include <e32base.h> 
#include <f32file.h>
#include "cdssync.h"
#include <xml/parser.h>
#include <xml/contenthandler.h>

// Forward declarations
class CCdsSyncSqlAo;
class MCmSqlMain;
class CCmSqlPropertyItem;
class CCmSqlGenericItem;
class CCmSqlBaseItem;

typedef RPointerArray<CCmSqlPropertyItem> CCmSqlPropertyItemArray;

using namespace Xml;

/**
 *  CItemResource class definition
 *
 *  @lib cdssync.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CItemResource ) : public CBase 
{
public:
    
    /**
     * Two-phased constructor.
     */
    static CItemResource* NewLC();
   
    /**
     * Two-phased constructor.
     */    
    static CItemResource* NewL();
    
    /**
     * Destructor
     */
    virtual ~CItemResource();
    
private:
    /**
     * Performs the first phase of two phase construction.
     */
    CItemResource();

    /**
     * Performs the second phase construction.
     */    
    void ConstructL();

public:
    /**
     * Track duration
     */
    HBufC8* iDuration;   // owned
    /**
     * Track bitrate
     */    
    HBufC8* iBitrate;    // owned
    /**
     * File size
     */
    HBufC8* iSize;       // owned
    /**
     * Item resolution
     */
    HBufC8* iResolution; // owned
    /**
     * Item uri
     */
    HBufC8* iUri;        // owned
    /**
     * Item protocol
     */
    HBufC8* iProtocol;   // owned
};


/**
 * CCdsSyncImpl class definition
 *
 * @lib cdssync.lib
 *
 * @since S60 5.1
 */    
class CCdsSyncImpl : public CActive, 
                     public MContentHandler 
      {

private:

    // Syncronization status
    enum TCdsSyncStatus
        {
        ECdsSyncIdle,
        ECdsSyncInitializing,
        ECdsSyncReadyToParse,
        ECdsSyncParsing,
        ECdsSyncFinalizing
        };

public:

    /**
     * Two-phased constructor.
     */
    static CCdsSyncImpl* NewLC();

    /**
     * Two-phased constructor.
     */    
    static CCdsSyncImpl* NewL();
    
    /**
     * Destructor
     */    
    ~CCdsSyncImpl();

public: // new functions    

    /**
     * Initializes data
     *
     * @Since S60 5.1
     * @param aSourceDataArray, source data
     * @param aDeviceId, device id where data is retrieved
     * @param aObserver, process observer
     * @param aAddGranularity, data granularity
     */    
    void InitL( RPointerArray<HBufC8>& aSourceDataArray,
                         const TInt& aDeviceId, 
                         MCdsSyncObserver& aObserver,
                         TInt aAddGranularity);

    /**
     * Notifies when data is added
     *
     * @Since S60 5.1
     * @param aSourceDataComplete, indicates if all source data is added
     */
    void NotifySourceDataAddedL( 
        TBool aSourceDataComplete = EFalse );

    /**
     * Cancels operation and cleans data
     *
     * @Since S60 5.1     
     */
    void ResetL();
    
    /**
     * Called when operation is completed
     *
     * @Since S60 5.1
     * @param aErrCode, error code
     */    
    void OperationsCompleteL( TInt aErrCode );

    /**
     * Notifies observer about progress
     *
     * @Since S60 5.1
     * @param aItemCount, processed items
     */
    void ProgressL( TInt aItemCount );

    /**
     * Returns the chunk count to be processed including current chunk
     *
     * @Since S60 5.1
     * @return TInt, chunk count
     */    
    TInt ChunkCount();
    
    /**
     * Returns successfully processed item count. 
     *
     * @since S60 5.1
     * @return TInt, count of items that has been processed successfully.
     */
    TInt ProcessedItemCount();
    
    /**
     * Sets search index to cds sync
     *
     * @since S60 5.1
     * @param aSearchIndex, search index
     */    
    void SetSearchIndex( const TInt aSearchIndex );    
    
    /**
     * Chunk complete callback
     *
     * @since S60 5.1
     */    
    void ChunkCompleteL();

// From base class MContentHandler        

    /**
     * From MContentHandler
     * See base class definition
     */     
    void OnStartDocumentL( const RDocumentParameters& aDocParam, 
                           TInt aErrorCode );
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnEndDocumentL( TInt aErrorCode );
    
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnStartElementL( const RTagInfo& aElement, 
                          const RAttributeArray& aAttributes, 
                          TInt aErrorCode );
    
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );

    /**
     * From MContentHandler
     * See base class definition
     */
    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );
    
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnStartPrefixMappingL( const RString& aPrefix, 
                                const RString& aUri, 
                                TInt aErrorCode);
    
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

    /**
     * From MContentHandler
     * See base class definition
     */
    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * From MContentHandler
     * See base class definition
     */
    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

    /**
     * From MContentHandler
     * See base class definition
     */
    void OnProcessingInstructionL( const TDesC8& aTarget, 
                                   const TDesC8& aData, 
                                   TInt aErrorCode);
    
    /**
     * From MContentHandler
     * See base class definition
     */
    void OnError( TInt aErrorCode );

    /**
     * From MContentHandler
     * See base class definition
     */
    TAny* GetExtendedInterface( const TInt32 aUid );
    
// From base class CActive

    /**
     * From CActive
     * See base class definition
     */
    void RunL();

    /**
     * From CActive
     * See base class definition
     */    
    void DoCancel();
    
private:

    /**
     * Performs the first phase of two phase construction.
     */    
    CCdsSyncImpl();

    /**
     * Performs the second phase construction.
     */        
    void ConstructL();

    /**
     * Parses profile id
     *
     * @since S60 5.1
     * @param aItem, item data
     * @param aRes, item resource
     */
    void ParseProfileIdL( CCmSqlGenericItem& aItem,
                          const CItemResource& aRes ) const;
        
    /**
     * Starts background parsing
     *
     * @since S60 5.1
     */
    TInt DoBackgroundParseL();

    /**
     * Background parsing callback
     *
     * @since S60 5.1
     * @param aCdsSync, observer
     */
    static TInt BackgroundParseL( TAny *aCdsSync );

    /**
     * Finds attributes
     *
     * @since S60 5.1
     * @param aName, local name
     * @param aPref, prefix
     * @param aAttributes, attribute array
     */
    TInt FindAttribute( const TDesC8& aName, 
                        const TDesC8& aPref, 
                        const RAttributeArray& aAttributes ) const;
    
    /**
     * Parses resolution information
     *
     * @since S60 5.1
     * @param aDes, buffer to be parsed
     * @param aWidth, image widht
     * @param aHeight, image height
     */
    void ParseResolution( const TDesC8& aDes,
                          TUint& aWidth,
                          TUint& aHeight ) const;
    
    /**
     * Parses value from descriptor
     *
     * @since S60 5.1
     * @param aDes, buffer to be parsed
     * @return TUInt, parsed value     
     */
    TUint ParseUint( const TDesC8& aDes ) const;

    /**
     * Parses value from descriptor
     *
     * @since S60 5.1
     * @param aDes, buffer to be parsed
     * @return TInt64, parsed value     
     */
    TInt64 ParseInt64( const TDesC8& aDes ) const;

    /**
     * Parses track duration
     *
     * @since S60 5.1
     * @param aDes, buffer to be parsed
     * @return TReal, duration
     */
    TReal ParseDuration( const TDesC8& aDes ) const;

    /**
     * Parses time
     *
     * @since S60 5.1
     * @param aDes, buffer to be parsed
     * @return TTime, parsed time
     */    
    TTime ParseTime( const TDesC8& aDes ) const;

    /**
     * Calculates hash value
     *
     * @since S60 5.1  
     * @return HBufC, hash value
     */
    HBufC8* CalculateHashL() const;

    /**
     * Append escpace characters to hash value
     *
     * @since S60 5.1
     * @param aHash, hash value
     * @return HBufC, hash value with escape charachters
     */
    HBufC* EscapeHashLC( const TDesC8& aHash ) const;

    /**
     * Gets property ID
     *
     * @since S60 5.1
     * @param aMetadataIndex, index for metadata type table
     * @param aValue, property name
     * @return TInt64, property id
     */
    TInt64 GetPropertyIdL( TInt aMetadataIndex, const TDesC8& aValue ) const;

    /**
     * Clears item data
     *
     * @since S60 5.1
     */
    void CleanItemData();

    /**
     * Removes unchanged items
     *
     * @since S60 5.1
     */
    void RemoveUnchangedItems();

    /**
     * Parses uri
     *
     * @since S60 5.1
     */
    void ParseUri();
     
private:

    /**
     * Cds Sync observer
     */
    MCdsSyncObserver*   iObserver;                          // not owned
    /**
     * Parser
     */
    CParser*            iParser;                            // owned
    /**
     * Metadata database
     */
    MCmSqlMain*         iMetadataDb;                        // owned
    /**
     * Active object for low priority processing
     */
    CIdle*              iBackground;                        // owned
    /**
     * Cds Sync active object
     */
    CCdsSyncSqlAo*      iSqlAo;                             // owned
    /**
     * Data chuck
     */
    HBufC8* iCurrentDocument;                               // owned
    /**
     * Sync status
     */
    TCdsSyncStatus    iState;
    /**
     * Data buffer
     */
    HBufC8* iCurrentContent;                                // owned
    /**
     * Item id
     */    
    HBufC8* iItemId;                                        // owned
    /**
     * Artist name
     */
    HBufC8* iArtist;                                        // owned
    /**
     * Album name
     */
    HBufC8* iAlbum;                                         // owned
    /**
     * Title
     */
    HBufC8* iTitle;                                         // owned
    /**
     * Album class
     */
    HBufC8* iClass;                                         // owned
    /**
     * Music genre
     */    
    HBufC8* iGenre;                                         // owned
    /**
     * Album data
     */
    HBufC8* iDate;                                          // owned
    /**
     * Description field
     */
    HBufC8* iDescription;                                   // owned
    /**
     * Album art
     */
    HBufC8* iAlbumArtUri;                                   // owned
    /**
     * Item resource array
     */
    RPointerArray<CItemResource> iResources;                // items owned
    /**
     * Flag for indicating that source data is ready
     */
    TBool iSourceDataComplete;
    /**
     * Datachunk index
     */
    TInt iChunkIndex;
    /**
     * Amount of unchanged items
     */
    TInt iUnchangedItemCount;
    /**
     * Hash order
     */    
    TLinearOrder<CCmSqlBaseItem> iHashOrder;
    /**
     * Name order
     */
    TLinearOrder<CCmSqlPropertyItem> iNameOrder;
    /**
     * Source data array
     */
    RPointerArray<HBufC8>* iSourceDataArray;                // not owned
    /**
     * Name array
     */
    RPointerArray<CCmSqlPropertyItemArray> iNames;          // items owned
    /**
     * Array of items to add
     */
    RPointerArray<CCmSqlGenericItem> iItemsToAdd;           // items owned
    /**
     * Array of items in the database
     */
    RPointerArray<CCmSqlBaseItem> iItemsInDb;               // items owned
    /**
     * File server session
     */
    RFs iFs;
    /**
     * Search index
     */    
    TInt iSearchIndex;
    /**
     * Count of processed items
     */ 
    TInt iProcessedItems;
#ifdef _DEBUG
    /**
     * Hash time
     */ 
    TInt64 iHashTime;
#endif // _DEBUG
    };
#endif // __CDSSYNCIMPL_H__
