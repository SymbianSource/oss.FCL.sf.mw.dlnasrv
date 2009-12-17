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
* Description:      SQL clause handling class
*
*/







#ifndef C_CMSQLDBCLAUSE_H
#define C_CMSQLDBCLAUSE_H

#include <e32base.h>

/**
 *  CCmSqlDbClause class
 *  Part of SQL wrapper. SQL wrapper is part of
 *  Content manager component.CCmSqlDbClause formats needed sql clauses
 *
 *  @lib cmsqlwrapper.lib
 *
 *  @since S60 v3.1
 */
class CCmSqlDbClause : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCmSqlDbClause* NewL();
    
    /**
     * Two-phased constructor.
     */
    static CCmSqlDbClause* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCmSqlDbClause();
    
public: 

    /**
     * Starts to generate sql query clause according to 
     * fill rule and fill method
     *
     * @since S60 5.1     
     * @param aRule, fill rule
     * @param aField, metadata field      
     * @param aMedia, media type
     * @return None
     */
    void FormatClauseL( CCmFillRule& aRule, 
        TCmMetadataField aField = ECmNone, TCmMediaType aMedia = ECmAll );

    /**
     * Starts to generate sql query clause according to 
     * property collector
     *
     * @since S60 5.1     
     * @param aPropertys, propertys to be filtered
     * @return None
     */
    void FormatFilteringClauseL( CCmSqlPropertyCollector& aPropertys );
    
    /**
     * Returns sql clause
     *
     * @since S60 5.1     
     * @param None
     * @return iClause, reference
     */    
    TDesC8& ClauseL() const;
    
     /**
     * Sets profile filters
     *
     * @since S60 5.1
     * @param aProfileIds, profile ids
     * @return None
     */
    void SetDlnaProfileFilters( const RArray<TInt64>& aProfileIds );
    
private:

    /**
     * Formats fill rules media type into sql clause
     * @param aRule, fill rule
     *
     * @since S60 5.1
     * @return None
     */
    void FormatMediatypeL( CCmFillRule& aRule );
    
    /**
     * Formats fill rule params into sql clause
     *
     * @since S60 5.1
     * @param aRule, fill rule
     * @return None
     */
    void FormatParamsL( CCmFillRule& aRule );

    /**
     * Formats fill rule media servers into sql clause
     *
     * @since S60 5.1
     * @param aRule, fill rule
     * @return None
     */
    void FormatDevicesL( CCmFillRule& aRule );

    /**
     * Formats fill rule query result ordering into sql clause
     *
     * @since S60 5.1
     * @param aMethod, ordering method
     * @return None
     */
    void FormatOrderingL( TCmFillMethod aMethod );

    /**
     * Formats fill rule query amount into sql clause
     *
     * @since S60 5.1
     * @param aRule, fill rule
     * @return None
     */
    void FormatAmountL( CCmFillRule& aRule );

    /**
     * Formats fill rule metadata field into sql clause
     *
     * @since S60 5.1
     * @param aField, metadata field to be set in query
     * @param aMediaType, media type of the fill list 
     * @return None
     */ 
    void FormatMetadataColumnL( TCmMetadataField aField, 
        TCmMediaType aMediaType );

    /**
     * Formats fill rule operator into sql clause
     *
     * @since S60 5.1
     * @param aMetadataColum, metadata field to be set in query
     * @param aOper, operator of query results
     * @return None
     */    
    void FormatOperatorL( TCmMetadataField aMetadataColum, 
        TCmOperatorType aOper );

    /**
     * Formats property values query
     *
     * @since S60 5.1
     * @param aField, identified property
     * @param aMedia, identified media type
     * @return None
     */     
    void FormatProperyQueryL( TCmMetadataField aField, 
        TCmMediaType aMedia = ECmAll );
        
    /**
     * Formats profile filters into sql clause
     *
     * @since S60 5.1
     * @param None
     * @return None
     */ 
    void FormatProfileFiltersL();    

    /**
     * Formats property filters into sql clause
     *
     * @since S60 5.1
     * @param aPropertys, property collector
     * @return None
     */     
    void FormatPropertyFilterL( 
        CCmSqlPropertyCollector& aPropertys );

    /**
     * Adds property filters into sql clause
     *
     * @since S60 5.1
     * @param aContainer, property container
     * @param aField, property field
     * @param aClause, clause to be modified
     * @return None
     */         
    void AddPropertyFilterL( 
        CCmSqlPropertyContainer& aContainer, const TDesC8& aField,
        HBufC8*& aClause  );
        
private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmSqlDbClause();

    /**
     * Performs the second phase construction.
     */
    void ConstructL();
    

private:
    
    /**
     * Formated clause
     */
    HBufC8* iClause;                                // Owned
    
    /** 
     * Temporal free text clause
     */
    HBufC8* iFreeText;                              // Owned
    
    /**
     * Profile ids which need to be filtered out
     */
    RArray<TInt64> iProfileFilters;
    
    };

#endif //  C_CMSQLDBCLAUSE_H
