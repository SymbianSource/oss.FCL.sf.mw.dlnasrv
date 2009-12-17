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
* Description:      Capsulating fill rule
*
*/






#ifndef __CMSTORERULE_H
#define __CMSTORERULE_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "cmcommontypes.h"
#include "cmcommon.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmMediaServer;

class TCmStoreRuleMediaServers
    {
public: // public member variables
    CCmMediaServer* iMediaServer; // Media server object
    TCmMediaType iMediaType;      // Media type
    };
    
/**
 *  CCmStoreRule class
 *    Capsulating store rules
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmStoreRule : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCmStoreRule class.
     * @param None
     * @return  pointer to CCmStoreRule class
     */
    IMPORT_C static CCmStoreRule* NewL();
    
    /**
     * Creates new CCmStoreRule class and 
     * leaves the instance in the cleanup stack.
     * @param None
     * @return  pointer to CCmStoreRule class
     */
    IMPORT_C static CCmStoreRule* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmStoreRule();

public:

    /**
     * AddStoreRuleL.
     * @since Series 60 3.1     
     * @param aMediaType
     * @returns index of the rule
     */
    IMPORT_C TInt AddStoreRuleL( TCmMediaType aMediaType );

    /**
     * StoreRule.
     * @since Series 60 3.1     
     * @param aIndex rule index
     * @param aMediaType
     * @returns index of the rule
     */    
    IMPORT_C void StoreRule( TInt aIndex, TCmMediaType* aMediaType );

    /**
     * AddMediaServerL.
     * @since Series 60 3.1     
     * @param aUDN
     * @returns index of the media server
     */    
    IMPORT_C TInt AddMediaServerL( const TDesC8& aUDN );
    
    /**
     * DeleteMediaServer.
     * @since Series 60 3.1     
     * @param aUDN
     * @returns None
     */        
    IMPORT_C void DeleteMediaServer( const TDesC8& aUDN );

    /**
     * AddExcAlbumL.
     * @since Series 60 3.1     
     * @param aAlbum
     * @returns None
     */        
    IMPORT_C void AddExcAlbumL( const TDesC& aAlbum );
    
    /**
     * AddExcPlayListL.
     * @since Series 60 3.1     
     * @param aPlayList
     * @returns None
     */        
    IMPORT_C void AddExcPlayListL( const TDesC& aPlayList );
    
    /**
     * RemoveExcAlbum.
     * @since Series 60 3.1     
     * @param aAlbum
     * @returns None
     */        
    IMPORT_C void RemoveExcAlbum( const TDesC& aAlbum );
    
    /**
     * RemoveExcPlayList.
     * @since Series 60 3.1     
     * @param aPlayList
     * @returns None
     */        
    IMPORT_C void RemoveExcPlayList( const TDesC& aPlayList );

    /**
     * ExcAlbums.
     * @since Series 60 3.1     
     * @param None
     * @returns array of albums
     */
    IMPORT_C CDesCArray& ExcAlbums();

    /**
     * ExcPlayLists.
     * @since Series 60 3.1     
     * @param None
     * @returns array of play lists
     */
    IMPORT_C CDesCArray& ExcPlayLists();
                
    /**
     * MediaServer.
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @returns server uuid
     */    
    IMPORT_C const TDesC8& MediaServerL( TInt aIndex );    
    
    /**
     * SetNameL.
     * @since Series 60 3.1     
     * @param aName
     * @returns None
     */    
    IMPORT_C void SetNameL( const TDesC8& aName );
    
    /**
     * Name.
     * @since Series 60 3.1     
     * @param None
     * @returns name of the fill rule
     */    
    IMPORT_C TDesC8& Name() const;
    
    /**
     * SetListId.
     * @since Series 60 3.1     
     * @param aId
     * @returns None
     */        
    IMPORT_C void SetListId( const TUint aId );
    
    /**
     * ListId.
     * @since Series 60 3.1     
     * @param None
     * @returns id of the store list
     */        
    IMPORT_C TUint ListId() const;
    
    /**
     * SetSelected.
     * @since Series 60 3.1     
     * @param aSelected
     * @returns None
     */    
    IMPORT_C void SetSelected( TCmFillRuleStatus aSelected );

    /**
     * Selected.
     * @since Series 60 3.1     
     * @param None
     * @returns status
     */        
    IMPORT_C TCmFillRuleStatus Selected() const;

    /**
     * SetStatus.
     * @since Series 60 3.1     
     * @param aStatus
     * @returns None
     */    
    IMPORT_C void SetStatus( TCmListItemStatus aStatus );

    /**
     * Status.
     * @since Series 60 3.1     
     * @param None
     * @returns status
     */    
    IMPORT_C TCmListItemStatus Status() const;
            
    /**
     * DefCount.
     * @since Series 60 3.1     
     * @param None
     * @returns count
     */    
    IMPORT_C TInt DefCount() const;
    
    /**
     * MediaServerCount.
     * @since Series 60 3.1     
     * @param None
     * @returns count
     */    
    IMPORT_C TInt MediaServerCount() const;
    
    /**
    * Externalizes container information to stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RWriteStream
    * @return none
    */
    void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
    * Internalizes container information from stream.
    * Leaves in case of errors.
    * @since Series 60 3.1
    * @param reference to RReadStream
    * @return none
    */
    void InternalizeL( RReadStream& aStream );        

private:

    /**
     * Constructor.
     */
    CCmStoreRule();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    /** Rule name */
    HBufC8* iName; // owned
    
    /** Selected or not */
    TCmFillRuleStatus iSelected;
    
    /** Rule defs */    
    RArray<TCmMediaType> iRuleDefs;
       
    /** Array rules included to fill rule*/
    RPointerArray<CCmMediaServer> iMediaServers; // items owned
    
    /** Id of the fill list */
    TUint iId;
    
    /** Excluded albums */
    CDesCArray* iExcAlbums; // owned
    
    /** Excluded playlists */
    CDesCArray* iExcPlayLists; // owned
    
    /** How to handle list items after store */
    TCmListItemStatus iStatus;

    };

#endif //  __CMSTORERULE_H
