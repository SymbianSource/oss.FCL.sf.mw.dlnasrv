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






#ifndef __CMFILLRULE_H
#define __CMFILLRULE_H

// INCLUDES
#include <e32base.h>
#include <badesca.h> // CDesCArray typedef
#include "cmcommontypes.h"
#include "cmcommon.h"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmRule;
class CCmMediaServer;

/**
 *  CCmFillRule class
 *    Capsulating fill rules
 *  @lib cmcommon.lib
 *  @since S60 v3.0
 */
class CCmFillRule : public CBase
    {

public:

    /* Constructors and destructor. */

    /**
     * Creates new CCMFillRule class.
     * @param None
     * @return  pointer to CFillRule class
     */
    IMPORT_C static CCmFillRule* NewL();
    
    /**
     * Creates new CCMFillRule class and 
     * leaves the instance in the cleanup stack.
     * @param None
     * @return  pointer to CCMFillRule class
     */
    IMPORT_C static CCmFillRule* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCmFillRule();

public:

    /**
     * AddRuleL.
     * @since Series 60 3.1     
     * @param aDataField ( metadatafield of the rule )
     * @param aOperator ( comparison type )
     * @returns index of the rule
     */
    IMPORT_C TInt AddRuleL( TCmMetadataField aDataField, 
        TCmOperatorType aOperator );

    /**
     * Rule.
     * @since Series 60 3.1     
     * @param aIndex rule index
     * @param aDataField metadatafield of the rule
     * @param aOperator rule operator
     * @param aParamCount param count on return
     * @returns index of the rule
     */    
    IMPORT_C void RuleL( TInt aIndex, TCmMetadataField* aDataField, 
        TCmOperatorType* aOperator, TInt* aParamCount );

    /**
     * Deletes rule.
     * @since Series 60 3.1     
     * @param aDataField metadatafield of the rule
     * @returns None
     */            
    IMPORT_C void DeleteRule( TCmMetadataField aDataField );

    /**
     * Deletes rule.
     * @since Series 60 3.1     
     * @param aDataField metadatafield of the rule
     * @param aOper, comparison method
     * @returns None
     */     
    IMPORT_C void DeleteRule( TCmMetadataField aDataField,
        TCmOperatorType aOper );    
    
    /**
     * AddRuleParamL.
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @param aParam parameter
     * @returns index of the rule
     */            
    IMPORT_C TInt AddRuleParamL( TInt aIndex, const TDesC8& aParam );

    /**
     * AddRuleParamL ( indexed param ).
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @param aParam parameter
     * @returns index of the rule
     */            
    IMPORT_C TInt AddRuleParamL( TInt aIndex, TInt aParam );
        
    /**
     * RuleParam.
     * @since Series 60 3.1     
     * @param aRuleIndex index to rule
     * @param aParamIndex index to param
     * @param aParam, where param is loaded
     * @returns None
     */    
    IMPORT_C void RuleParamL( TInt aRuleIndex, TInt aParamIndex, 
        TPtrC8* aParam );

    /**
     * RuleParam.
     * @since Series 60 3.1     
     * @param aRuleIndex index to rule
     * @param aParamIndex index to param
     * @param aParam, where param is loaded
     * @returns None
     */    
    IMPORT_C void RuleParamL( TInt aRuleIndex, TInt aParamIndex, 
        TInt& aParam );
        
    /**
     * AddMediaServerL.
     * @since Series 60 3.1     
     * @param aUDN
     * @returns index of the media server
     */    
    IMPORT_C TInt AddMediaServerL( const TDesC8& aUDN );

    /**
     * AddMediaServerL.
     * @since Series 60 3.1     
     * @param aDbId
     * @returns index of the media server
     */    
    IMPORT_C TInt AddMediaServerL( TInt aDbId );
    
    /**
     * DeleteMediaServer.
     * @since Series 60 3.1     
     * @param aUDN
     * @returns None
     */        
    IMPORT_C void DeleteMediaServer( const TDesC8& aUDN );

    /**
     * DeleteMediaServer.
     * @since Series 60 3.1     
     * @param aDbId
     * @returns None
     */        
    IMPORT_C void DeleteMediaServer( TInt aDbId );
     
    /**
     * AddExcAlbum.
     * @since Series 60 3.1     
     * @param aAlbum
     * @returns None
     */        
    IMPORT_C void AddExcAlbumL( const TDesC& aAlbum );
    
    /**
     * AddExcPlayList.
     * @since Series 60 3.1     
     * @param aPlayList
     * @returns None
     */        
    IMPORT_C void AddExcPlayListL( const TDesC& aPlayList );
    
    /**
     * RemoveExcAlbumL.
     * @since Series 60 3.1     
     * @param aAlbum
     * @returns None
     */        
    IMPORT_C void RemoveExcAlbum( const TDesC& aAlbum );
    
    /**
     * RemoveExcPlayListL.
     * @since Series 60 3.1     
     * @param aPlayList
     * @returns None
     */        
    IMPORT_C void RemoveExcPlayList( const TDesC& aPlayList );

    /**
     * ExcAlbumsL.
     * @since Series 60 3.1     
     * @returns array of albums
     */
    IMPORT_C CDesCArray& ExcAlbums() const;

    /**
     * ExcPlayListsL.
     * @since Series 60 3.1     
     * @returns array of play lists
     */
    IMPORT_C CDesCArray& ExcPlayLists() const;
        
    /**
     * MediaServer.
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @returns server uuid
     */    
    IMPORT_C const TDesC8& MediaServerL( TInt aIndex );    

    /**
     * MediaServer.
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @param aServer ref. to server
     * @returns None
     */    
    IMPORT_C void MediaServerL( TInt aIndex, TInt& aServer );
        
    /**
     * SetNameL.
     * @since Series 60 3.1     
     * @param aName
     * @returns KErrNone
     */    
    IMPORT_C TInt SetNameL( const TDesC8& aName );
    
    /**
     * Name.
     * @since Series 60 3.1     
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
     * @returns id of the fill list
     */        
    IMPORT_C TUint ListId() const;
    
    /**
     * SetAmount.
     * @since Series 60 3.1     
     * @param aAmount
     * @returns None
     */    
    IMPORT_C void SetAmount( TUint32 aAmount );

    /**
     * Amount.
     * @since Series 60 3.1     
     * @returns amount
     */
    IMPORT_C TUint32 Amount() const;
    
    /**
     * SetLimitType.
     * @since Series 60 3.1     
     * @param aLimitType
     * @returns None
     */    
    IMPORT_C void SetLimitType( TCmLimitType aLimitType );
    
    /**
     * LimitType.
     * @since Series 60 3.1     
     * @returns limit type
     */    
    IMPORT_C TCmLimitType LimitType() const;
        
    /**
     * SetMediaType.
     * @since Series 60 3.1     
     * @param aMediaType
     * @returns None
     */    
    IMPORT_C void SetMediaType( TCmMediaType aMediaType );
    
    /**
     * MediaType.
     * @since Series 60 3.1     
     * @returns media type ( video, image, music or what )
     */    
    IMPORT_C TCmMediaType MediaType() const;    
    
    /**
     * SetMethod.
     * @since Series 60 3.1     
     * @param aMethod
     * @returns None
     */    
    IMPORT_C void SetMethod( TCmFillMethod aMethod );
    
    /**
     * Method.
     * @since Series 60 3.1     
     * @returns method
     */    
    IMPORT_C TCmFillMethod Method() const;

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
     * Selected.
     * @since Series 60 3.1     
     * @returns status
     */    
    IMPORT_C TCmListItemStatus Status() const;
            
    /**
     * SetPriority.
     * @since Series 60 3.1     
     * @param aPriority
     * @returns None
     */        
    IMPORT_C void SetPriority( TUint8 aPriority );

    /**
     * Priority.
     * @since Series 60 3.1     
     * @returns Priority
     */        
    IMPORT_C TUint8 Priority() const;

    /**
     * SetTemplateId.
     * @since Series 60 3.1     
     * @param aTemplateId
     * @returns None
     */    
    IMPORT_C void SetTemplateId( TUint8 aTemplateId );

    /**
     * TemplateId.
     * @since Series 60 3.1     
     * @returns TemplateId
     */
    IMPORT_C TUint8 TemplateId() const;
    
    /**
     * SetListRealSizeInBytes.
     * @since Series 60 3.1     
     * @param aRealSize
     * @returns None
     */    
    IMPORT_C void SetListRealSizeInBytes( TUint32 aRealSize );

    /**
     * ListRealSizeInBytes.
     * @since Series 60 3.1     
     * @returns List's real size
     */
    IMPORT_C TUint32 ListRealSizeInBytes() const;
    
    /**
     * SetListRealCount.
     * @since Series 60 3.1     
     * @param aRealCount
     * @returns None
     */    
    IMPORT_C void SetListRealCount( TUint32 aRealCount );

    /**
     * ListRealCount.
     * @since Series 60 3.1     
     * @returns List's real count
     */
    IMPORT_C TUint32 ListRealCount() const;      
                
    /**
     * RuleCount.
     * @since Series 60 3.1     
     * @returns count
     */    
    IMPORT_C TInt RuleCount() const;
    
    /**
     * MediaServerCount.
     * @since Series 60 3.1     
     * @returns count
     */    
    IMPORT_C TInt MediaServerCount() const;
    
    /**
     * ParamCount.
     * @since Series 60 3.1     
     * @param aIndex index of the rule
     * @returns count
     */    
    IMPORT_C TInt ParamCountL( TInt aIndex ) const;
    
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
    CCmFillRule();

    /**
     * Second-phase constructor.
     */
    void ConstructL();
    

private: // data
    
    /** Rule name */    
    HBufC8* iName; // owned
    
    /** Amount */
    TUint32 iAmount;
    
    /** List's real size in MBs */
    TUint32 iRealSize;
    
    /** List's real item count */
    TUint32 iRealCount;
    
    /** Limit type */
    TCmLimitType iLimitType;
    
    /** Media type */
    TCmMediaType iMediaType;
    
    /** Method */
    TCmFillMethod iMethod;
    
    /** State of the fill rule ( Selected or not ) */
    TCmFillRuleStatus iSelected;
    
    /** Array rules included to fill rule */
    RPointerArray<CCmRule> iRuleArray; // items owned
    
    /** Array rules included to fill rule */
    RPointerArray<CCmMediaServer> iMediaServerArray; // items owned
    
    /** Priority of the fill rule */
    TUint8 iPriority;
    
    /** Id of the tempate that was used when creating rule */
    TUint8 iTemplateId;
    
    /** Id of the fill list */
    TUint iId;
    
    /** Excluded albums */
    CDesCArray* iExcAlbums; // owned.
    
    /** Excluded playlists */
    CDesCArray* iExcPlayLists; // owned.

    /** How to handle list items after fill */
    TCmListItemStatus iStatus;
    };

#endif //  __CMFILLRULE_H
