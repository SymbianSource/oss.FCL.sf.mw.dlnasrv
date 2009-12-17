/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a data class containing browse-related data
*
*/






#ifndef C_CUPNPAVBROWSEREQUEST_H
#define C_CUPNPAVBROWSEREQUEST_H


#include <e32base.h>
#include <s32mem.h>

#include "upnpavbrowsingsession.h"
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  UPnP AV Controller Client/Server communication helper class
*  
*
*  @lib - 
*  @since Series 60 3.1
*/

class CUpnpAVBrowseRequest : public CBase
    { 
    
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static inline CUpnpAVBrowseRequest* NewLC();
    
    /**
     * Two-phased constructor.
     */
    static inline CUpnpAVBrowseRequest* NewL();
    
    /**
     * Destructor.
     */
    inline virtual ~CUpnpAVBrowseRequest();
    
public:

    /**
     * Externalizes device information to stream.
     * Leaves in case of errors.
     * @since Series 60 3.0
     * @param reference to RWriteStream
     * @return none
     */
    inline void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes device information from stream.
     * Leaves in case of errors.
     * @since Series 60 3.0
     * @param reference to RReadStream
     * @return none
     */
    inline void InternalizeL( RReadStream& aStream );

    /**
     * Externalizes information to stream and returns the object as a heap
     * desctiptor.
     */        
    inline HBufC8* ToDes8L() const;

private: //

    /**
     * Constructor
     */
    inline CUpnpAVBrowseRequest();

    /**
     * Perform the second phase construction
     */
    inline void ConstructL();

public: // New methods

    /**
     * Set Id
     * @param aId
     */
    inline void SetIdL( const TDesC8& aId );

    /**
     * Get Id
     * @return id
     */    
    inline const TDesC8& Id();

    /**
     * Set search criteria
     * @param aSearchCriteria
     */    
    inline void SetSearchCriteriaL( const TDesC8& aSearchCriteria );

    /**
     * Get search criteria
     * @return search criteria
     */    
    inline const TDesC8& SearchCriteria();

    /**
     * Set filter
     * @param aFilter
     */
    inline void SetFilterL( const TDesC8& aFilter );
        
    /**
     * Get filter
     * @return filter
     */    
    inline const TDesC8& Filter();

    /**
     * Set browse flag
     * @param aBrowseFlag
     */    
    inline void SetBrowseFlag( MUPnPAVBrowsingSession::TBrowseFlag
        aBrowseFlag );
    
    /**
     * Get browse flag
     * @return browse flag
     */    
    inline MUPnPAVBrowsingSession::TBrowseFlag BrowseFlag();

    /**
     * Set start index
     * @param aStartIndex
     */    
    inline void SetStartIndex( TInt aStartIndex );
    
    /**
     * Get start index
     * @return start index
     */    
    inline TInt StartIndex();

    /**
     * Set requested count
     * @param aRequestedCount
     */
    inline void SetRequestedCount( TInt aRequestedCount );

    /**
     * Get requested count
     * @return requested count
     */    
    inline TInt RequestedCount();

    /**
     * Set sort criteria
     * @param aSortCriteria
     */
    inline void SetSortCriteriaL( const TDesC8& aSortCriteria );
    
    /**
     * Get sort criteria
     * @return sort criteria
     */    
    inline const TDesC8& SortCriteria();

private:
    
    TInt                                iCriteriaLength;
    HBufC8*                             iId; // Owned
    HBufC8*                             iSearchCriteria; // Owned
    HBufC8*                             iFilter; // Owned
    MUPnPAVBrowsingSession::TBrowseFlag iBrowseFlag;
    TInt                                iStartIndex;
    TInt                                iRequestedCount;
    HBufC8*                             iSortCriteria; // Owned
    
    };

#include "upnpavbrowserequest.inl"

#endif // C_CUPNPAVBROWSEREQUEST_H
