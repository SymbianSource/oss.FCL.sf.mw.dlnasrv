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
* Description:      SQL database sql clause class
*
*/






#include <utf.h>
#include "cmfillrule.h"
#include "cmsqlpropertycollector.h"
#include "cmsqlpropertycontainer.h"
#include "cmsqlpropertyitem.h"
#include "cmsqldbclause.h"
#include "cmsqlclausedef.h"
#include "msdebug.h"                                                    

// CONSTANTS
// The size of buf that need to allot for store the rule amount
const TUint KCmRuleAmountSize = 5;

// ---------------------------------------------------------------------------
// CCmSqlDbClause::NewL
// ---------------------------------------------------------------------------
//
CCmSqlDbClause* CCmSqlDbClause::NewL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::NewL"));    
    CCmSqlDbClause* self = CCmSqlDbClause::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
 
// ---------------------------------------------------------------------------
// CCmSqlDbClause::NewLC
// ---------------------------------------------------------------------------
//    
CCmSqlDbClause* CCmSqlDbClause::NewLC()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::NewLC"));    
    CCmSqlDbClause* self = new ( ELeave ) CCmSqlDbClause();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSqlDbClause::~CCmSqlDbClause
// ---------------------------------------------------------------------------
//
CCmSqlDbClause::~CCmSqlDbClause()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::~CCmSqlDbClause"));    
    
    delete iClause;
    delete iFreeText;
    iProfileFilters.Reset();        
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatClauseL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatClauseL( CCmFillRule& aRule,
    TCmMetadataField aField, TCmMediaType aMedia )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatClauseL"));    
    
    delete iClause;
    iClause = NULL;   
    if( aField == ECmNone && &aRule )
        {
        if( ECmAudio != aRule.MediaType() )
            {
            iClause = KCmSqlBaseSelect().AllocL();    
            }
        else
            {
            iClause = KCmSqlMusicSelect().AllocL();
            }
        TRACE(Print(_L("[SQL wrapper]\t Formating media type...")));
        FormatMediatypeL( aRule );
        TRACE(Print(_L("[SQL wrapper]\t Formating params...")));
        FormatParamsL( aRule );
        TRACE(Print(_L("[SQL wrapper]\t Formating devices...")));
        FormatDevicesL( aRule );
        TRACE(Print(_L("[SQL wrapper]\t Formating profile filters...")));
        FormatProfileFiltersL();
        TRACE(Print(_L("[SQL wrapper]\t Formating ordering...")));
        FormatOrderingL( aRule.Method() );
        TRACE(Print(_L("[SQL wrapper]\t Formating amount...")));
        FormatAmountL( aRule );
        // Finish clause    
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlSemicolon().Length() );
        iClause->Des().Append( KCmSqlSemicolon() );     
        }
    else
        {
        FormatProperyQueryL( aField, aMedia );
        }        
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatFilteringClauseL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatFilteringClauseL( 
    CCmSqlPropertyCollector& aPropertys )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatFilteringClauseL"));    
    
    delete iClause;
    iClause = NULL;

    TInt count = aPropertys.PropertyContainerCount();

    if( count == 1 && aPropertys.PropertyContainer( 0 )->Type() == ECmTitle)
        {
        //video
        iClause = KCmSqlFilteredPropertySelectVideo().AllocL();
        }
    else
        {
        iClause = KCmSqlFilteredPropertySelect().AllocL();
        }

    FormatPropertyFilterL( aPropertys );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlDbClause::ClauseL
// ---------------------------------------------------------------------------
//
TDesC8& CCmSqlDbClause::ClauseL() const
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::ClauseL"));    
    
#ifdef _DEBUG    
    HBufC* data16bitBuf = HBufC::NewLC(iClause->Length());
    TPtr data16bitPtr = data16bitBuf->Des();
       
    TInt conversionError = 
        CnvUtfConverter::ConvertToUnicodeFromUtf8(data16bitPtr, 
                                                *iClause );
    TPtrC temp;                                                
    for( TInt i = 0; i < (data16bitPtr.Length())/100 + 1 ; i++ ) 
        {
        if( i == (data16bitPtr.Length())/100 )
            {
            temp.Set( data16bitPtr.Mid(100*i ) );
            TRACE( Print(_L("SQL clause %S"), &temp ) ); 
            }
        else
            {
            temp.Set( data16bitPtr.Mid(100*i, 100 ) );
            TRACE( Print(_L("SQL clause %S"), &temp ) ); 
            }        
        }
    CleanupStack::PopAndDestroy( data16bitBuf );
#endif    
    return *iClause;
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::SetDlnaProfileFilters
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::SetDlnaProfileFilters( 
    const RArray<TInt64>& aProfileIds )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::SetDlnaProfileFilters"));    
    
    iProfileFilters.Reset();
    for( TInt i = 0; i < aProfileIds.Count() ; i++ )
        {
        iProfileFilters.InsertInSignedKeyOrder( aProfileIds[i] );
        }
    }
        
// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatMediatypeL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatMediatypeL( CCmFillRule& aRule )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatMediatypeL"));    
    
    TBuf8<20>mediatype;
    mediatype.Format( KCmSqlSelectMediatype, aRule.MediaType() );
    iClause = iClause->ReAllocL( iClause->Length() + mediatype.Length() );
    iClause->Des().Append( mediatype );
    }
    
// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatParamsL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatParamsL( CCmFillRule& aRule )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatParamsL"));    
    
    TCmMetadataField metadataColum;
    TPtrC8 dataField;
    TInt dataFieldIndex;
    TCmOperatorType oper;
    TInt paramCount;
    TBool operatorAdded( EFalse );   
    for( TInt i = 0; i < aRule.RuleCount(); i++ )
        {
        aRule.RuleL( i, &metadataColum, &oper, &paramCount );
        if( paramCount )
            {            
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlAnd().Length() + KCmSqlLeftBracket().Length()  );
            iClause->Des().Append( KCmSqlAnd );
            iClause->Des().Append( KCmSqlLeftBracket() );                
            for( TInt j = 0; j < paramCount; j++ )
                {
                iClause = iClause->ReAllocL( iClause->Length() + 
                    KCmSqlLeftBracket().Length() );
                iClause->Des().Append( KCmSqlLeftBracket() );
                              
                FormatMetadataColumnL( metadataColum, aRule.MediaType() );
                // Data field may change to integer value ( index to table )
                aRule.RuleParamL( i, j, &dataField );
                aRule.RuleParamL( i, j, dataFieldIndex );
                
                // If rule is free text rule, operator is already appended 
                // into Sql clause
                operatorAdded = EFalse;
                if( ECmFreeText != metadataColum &&
                    ECmArtist != metadataColum &&
                    ECmAlbum != metadataColum &&
                    ECmResolution != metadataColum &&
                    ECmGenre != metadataColum && 
                    ECmUpnpClass != metadataColum && 
                    ECmTitle != metadataColum )
                    {
                    FormatOperatorL( metadataColum, oper );
                    operatorAdded = ETrue;
                    }
                      
                if( dataField.Size() && 
                    ( iFreeText || ECmDate == metadataColum || 
                    ECmFileSize == metadataColum || 
                    ECmDuration == metadataColum ) )
                    {
                    if( ECmDate != metadataColum && 
                        ECmFileSize != metadataColum && 
                        ECmDuration != metadataColum )
                        {
                        TInt multiplier( 1 );
                        if( aRule.MediaType() == ECmAudio && 
                            metadataColum == ECmFreeText )
                            {
                            // Three fields are used in audio free text search
                            multiplier = 3;
                            }
                        iFreeText = iFreeText->ReAllocL( iFreeText->Length() +
                            ( dataField.Length() * multiplier ) );  
                        HBufC8* temp = HBufC8::NewMaxL( iFreeText->Length() + 
                            ( dataField.Length() * multiplier ) );
                        CleanupStack::PushL( temp );
                        // Format dataField into clause
                        if( multiplier == 1 )
                            {                    
                            temp->Des().Format( *iFreeText, &dataField );
                            }
                        else
                            {
                            temp->Des().Format( *iFreeText, &dataField, 
                                &dataField, &dataField );   
                            } 
                        if( iFreeText )
                            {
                            delete iFreeText; 
                            iFreeText = NULL;
                            }
                        iClause = iClause->ReAllocL( iClause->Length() + 
                            temp->Length() );
                        iClause->Des().Append( *temp );
                        CleanupStack::PopAndDestroy( temp );                       
                        }
                    else
                        {
                        // Date is appended in TInt64 format
                        TLex8 lex( dataField );
                        TInt64 num;
                        lex.Val( num );
                        iClause = iClause->ReAllocL( iClause->Length() + 
                            dataField.Length() );                    
                        iClause->Des().AppendNum( num );
                        }            
                    }
                else
                    {
                    if( operatorAdded )
                        {
                        iClause = iClause->ReAllocL( iClause->Length() + 
                            sizeof( dataFieldIndex ) );
                            
                        HBufC8* temp = HBufC8::NewMaxLC( iClause->Length() );
                        temp->Des().Format( *iClause, dataFieldIndex );
                        delete iClause;
                        iClause = NULL;
                        iClause = temp->AllocL();
                        CleanupStack::PopAndDestroy( temp );
                        }
                    else
                        {
                        iClause = iClause->ReAllocL( iClause->Length() + 
                            sizeof( dataFieldIndex ) );                    
                        iClause->Des().AppendNum( dataFieldIndex );
                        }      
                    }    

                iClause = iClause->ReAllocL( iClause->Length() + 
                    KCmSqlRightBracket().Length() );
                iClause->Des().Append( KCmSqlRightBracket() );         
                
                if( j < paramCount - 1 )
                    {
                    iClause = iClause->ReAllocL( iClause->Length() + 
                        KCmSqlOr().Length() );
                    iClause->Des().Append( KCmSqlOr() );            
                    }            
                }
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlRightBracket().Length() );
            iClause->Des().Append( KCmSqlRightBracket() );             
            }            
        }      
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatDevicesL
// ---------------------------------------------------------------------------
//    
void CCmSqlDbClause::FormatDevicesL( CCmFillRule& aRule )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatDevicesL"));    
    
    TPtrC8 mediaServer;
    TInt msId(0);
    if( aRule.MediaServerCount() )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlAnd().Length() );
        iClause->Des().Append( KCmSqlAnd() );
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlLeftBracket().Length() );
        iClause->Des().Append( KCmSqlLeftBracket() );                
        }
    for( TInt k = 0; k < aRule.MediaServerCount(); k++ )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlLeftBracket().Length() );
        iClause->Des().Append( KCmSqlLeftBracket() );                
        aRule.MediaServerL( k, msId );
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlSelectDeviceId().Length() + sizeof( msId ) );
        iClause->Des().Append( KCmSqlSelectDeviceId );
        iClause->Des().AppendNum( msId );
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlRightBracket().Length() );
        iClause->Des().Append( KCmSqlRightBracket() );          
        if( k < aRule.MediaServerCount() - 1 )
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlOr().Length() );
            iClause->Des().Append( KCmSqlOr() );            
            }
        }
    if( aRule.MediaServerCount() )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlRightBracket().Length() );
        iClause->Des().Append( KCmSqlRightBracket() );                
        }                     
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatOrderingL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatOrderingL( TCmFillMethod aMethod )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatOrderingL"));    
    
    switch( aMethod )
        {
        case ECmRandom:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlOrderByRandom().Length() );
            iClause->Des().Append( KCmSqlOrderByRandom() );              
            break;
            }
        case ECmLatest:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlOrderByDate().Length() + KCmSqlDesc().Length() );
            iClause->Des().Append( KCmSqlOrderByDate() );
            iClause->Des().Append( KCmSqlDesc() );             
            break;
            }        
        case ECmOldest:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlOrderByDate().Length() + KCmSqlAsc().Length() );
            iClause->Des().Append( KCmSqlOrderByDate() );
            iClause->Des().Append( KCmSqlAsc() );
            break;
            }        
        default:
            {
            break;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatAmountL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatAmountL( CCmFillRule& aRule )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatAmountL"));    
    
    if( aRule.LimitType() == EPieces )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlLimit().Length() + KCmRuleAmountSize  );
        iClause->Des().Append( KCmSqlLimit() );
        iClause->Des().AppendNum( aRule.Amount() );        
        }
    else if( aRule.LimitType() == EMbits )
        {
        // If amount is wanted in MBytes, no possibility to 
        // LIMIT result set in this phase
        }
    else
        {
        // Unlimited rule
        // Fill quota is used as a maximun amount
        }    
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatMetadataColumnL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatMetadataColumnL( TCmMetadataField aField,
    TCmMediaType aMediaType )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatMetadataColumnL"));    
    
    if( iFreeText )
        {
        delete iFreeText; iFreeText = NULL;
        }
    TRACE(Print(_L("[SQL wrapper]\t Formating data field = %d"), 
        (TInt) aField ));             
    
    switch( aField )
        {       
        case ECmArtist:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectArtistById().Length() );
            iClause->Des().Append( KCmSqlSelectArtistById() );
            break;
            }                    
        case ECmAlbum:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectAlbumById().Length() );
            iClause->Des().Append( KCmSqlSelectAlbumById() );            
            break;
            }        
        case ECmGenre:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectGenreById().Length() );
            iClause->Des().Append( KCmSqlSelectGenreById() );               
            break;
            }        
        case ECmDuration:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectDuration().Length() );
            iClause->Des().Append( KCmSqlSelectDuration() );              
            break;
            }        
        case ECmTitle:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectTitle().Length() );
            iClause->Des().Append( KCmSqlSelectTitle() );
            break;
            }        
        case ECmUpnpClass:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectUpnpclassById().Length() );
            iClause->Des().Append( KCmSqlSelectUpnpclassById );    
            break;
            }        
        case ECmContentURI:
            {
            LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatMetadataColumnL \
            aField = ECmContentURI"));
            break;
            }        
        case ECmFileSize:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectSize().Length() );
            iClause->Des().Append( KCmSqlSelectSize() );            
            break;
            }        
        case ECmBitrate:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlSelectBitrate().Length() );
            iClause->Des().Append( KCmSqlSelectBitrate );    
            break;
            }        
        case ECmResolution:
            {
            iClause = iClause->ReAllocL( iClause->Length() +
                KCmSqlSelectResolutionById().Length() );
            iClause->Des().Append( KCmSqlSelectResolutionById );    
            break;
            }
        case ECmDate:
            {
            iClause = iClause->ReAllocL( iClause->Length() +
                KCmSqlSelectDate().Length() );
            iClause->Des().Append( KCmSqlSelectDate );
            break;
            }  
        case ECmFreeText:
            {
            switch( aMediaType )
                {
                case ECmAudio:
                    {
                    // Title, Artist or album field contains specified field
                    iFreeText = KCmSqlFreeTextSelectAudio().AllocL();
                    break;
                    }                
                case ECmImage:           
                case ECmVideo: 
                    {
                    // Title contains specified field
                    iFreeText = KCmSqlFreeTextSelectImageOrVideo().AllocL();
                    break;
                    }
                default:
                    {
                    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::\
                    FormatMetadataColumnL mediatype not found"));                    
                    break;
                    }
                }
            break;
            }
        default:
            {
            LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatMetadataColumnL \
            field not found"));
            break;
            }    
        }      
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatOperatorL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatOperatorL( TCmMetadataField aMetadataColum, 
    TCmOperatorType aOper )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatOperatorL"));    
    
    switch( aOper )
        {           
        case ECmEquals:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlEqual().Length() + KCmSqlAppendString().Length() );
            iClause->Des().Append( KCmSqlEqual() );
            iClause->Des().Append( KCmSqlAppendInteger() );
            break;
            }
        // Greater and smaller operator are always integer operators    
        case ECmGreater:
            {
            if( aMetadataColum != ECmDate && aMetadataColum != ECmFileSize && 
                aMetadataColum != ECmDuration )
                {            
                iClause = iClause->ReAllocL( iClause->Length() + 
                                             KCmSqlGreater().Length() + 
                                             KCmSqlAppendInteger().Length() );
                iClause->Des().Append( KCmSqlGreater() );
                iClause->Des().Append( KCmSqlAppendInteger() );
                }
            else
                {
                iClause = iClause->ReAllocL( iClause->Length() + 
                    KCmSqlGreater().Length() + KCmSqlAppendLong().Length() );
                iClause->Des().Append( KCmSqlGreater() );                
                }    
            break;
            } 
        case ECmSmaller:
            {
            if( aMetadataColum != ECmDate && aMetadataColum != ECmFileSize &&
                aMetadataColum != ECmDuration )
                {
                iClause = iClause->ReAllocL( iClause->Length() + 
                                             KCmSqlSmaller().Length() + 
                                             KCmSqlAppendInteger().Length() );
                iClause->Des().Append( KCmSqlSmaller() );
                iClause->Des().Append( KCmSqlAppendInteger() );
                }
            else
                {
                iClause = iClause->ReAllocL( iClause->Length() + 
                    KCmSqlSmaller().Length() + KCmSqlAppendLong().Length() );
                iClause->Des().Append( KCmSqlSmaller() );               
                }                  
            break;
            } 
        case ECmConsistOf:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlLike().Length() + KCmSqlConsistOf().Length() );
            iClause->Des().Append( KCmSqlLike() );
            iClause->Des().Append( KCmSqlConsistOf() );             
            break;
            } 
        case ECmBeginsWith:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlLike().Length() + KCmSqlBeginsWith().Length() );
            iClause->Des().Append( KCmSqlLike() );
            iClause->Des().Append( KCmSqlBeginsWith() );              
            break;
            }
        case ECmEndsWith:
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlLike().Length() + KCmSqlEndsWith().Length() );
            iClause->Des().Append( KCmSqlLike() );
            iClause->Des().Append( KCmSqlEndsWith() );              
            break;
            }
        default:
            {
            break;
            }
        }    
    }
    
// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatProperyQueryL
// ---------------------------------------------------------------------------
//     
void CCmSqlDbClause::FormatProperyQueryL( TCmMetadataField aField,
    TCmMediaType aMedia )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatProperyQueryL"));    
    
    if( iClause )
        {
        delete iClause;
        iClause = NULL;
        }
    
    switch( aField )
        {
        case ECmArtist:
            {           
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlArtist().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlArtist );
            break;
            }        
        case ECmAlbum:
            {
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlAlbum().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlAlbum );           
            break;
            }        
        case ECmGenre:
            {
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlGenre().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlGenre );
            break;
            }        
        case ECmUpnpClass:
            {
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlUpnpclass().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlUpnpclass );
            break;
            }
        case ECmTitle:
            {           
            iClause = HBufC8::NewL( KCmSqlSelectTitleAndId().Length() + 
                sizeof( aMedia ) );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectTitleAndId, aMedia );                 
            break;
            }
        case ECmResolution:
            {
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlResolutions().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlResolutions );
            break;
            }
        case ECmProfileId:
            {
            iClause = HBufC8::NewL( KCmSqlSelectPropertyValues().Length() + 
                KCmSqlResolutions().Length() );
            TPtr8 ptr( iClause->Des() );    
            ptr.Format( KCmSqlSelectPropertyValues, &KCmSqlUpnpProfiles );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatProfileFiltersL
// ---------------------------------------------------------------------------
//     
void CCmSqlDbClause::FormatProfileFiltersL(  )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatProfileFiltersL"));    
    
    if( iProfileFilters.Count() )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlAnd().Length() );
        iClause->Des().Append( KCmSqlAnd() );
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlLeftBracket().Length() );
        iClause->Des().Append( KCmSqlLeftBracket() );                
        }
    for( TInt k = 0; k < iProfileFilters.Count(); k++ )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlLeftBracket().Length() );
        iClause->Des().Append( KCmSqlLeftBracket() );                

        iClause = iClause->ReAllocL( iClause->Length() + 
                                     KCmSqlFilterUpnpProfileId().Length() + 
                                     sizeof( iProfileFilters[k] ) );
        iClause->Des().Append( KCmSqlFilterUpnpProfileId );
        iClause->Des().AppendNum( iProfileFilters[k] );
        
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlRightBracket().Length() );
        iClause->Des().Append( KCmSqlRightBracket() );          
        if( k < iProfileFilters.Count() - 1 )
            {
            iClause = iClause->ReAllocL( iClause->Length() + 
                KCmSqlAnd().Length() );
            iClause->Des().Append( KCmSqlAnd() );            
            }
        }
    if( iProfileFilters.Count() )
        {
        iClause = iClause->ReAllocL( iClause->Length() + 
            KCmSqlRightBracket().Length() );
        iClause->Des().Append( KCmSqlRightBracket() );                
        }
    iClause = iClause->ReAllocL( iClause->Length() + 
        KCmSqlRightBracket().Length() );
    iClause->Des().Append( KCmSqlRightBracket() );          
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::FormatPropertyFilterL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::FormatPropertyFilterL( 
    CCmSqlPropertyCollector& aPropertys )
    {    
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::FormatPropertyFilterL"));    
    
    CCmSqlPropertyContainer* container = NULL;
    HBufC8* temp = NULL;   
    for( TInt i = 0; i < aPropertys.PropertyContainerCount(); i++ )
        {
        container = aPropertys.PropertyContainer( i );        
        switch( container->Type() )
            {
            case ECmTitle:
                {
                AddPropertyFilterL( *container, 
                    KCmSqlSelectTitle(), temp );                   
                break;
                }
            case ECmArtist:
                {
                AddPropertyFilterL( *container, 
                    KCmSqlSelectArtistById(), temp );                               
                break;
                }
            case ECmAlbum:
                {
                AddPropertyFilterL( *container, 
                    KCmSqlSelectAlbumById(), temp );                                
                break;
                }
            case ECmGenre:
                {
                AddPropertyFilterL( *container, 
                    KCmSqlSelectGenreById(), temp );
                break;
                }
            default:
                {
                break;
                }                                                                
            }
        container = NULL;
        }
    
    if( temp )
        {
        iClause = iClause->ReAllocL( iClause->Length() + temp->Length() );
        iClause->Des().Append( *temp );
        delete temp;        
        }
    }

// ---------------------------------------------------------------------------
// CCmSqlDbClause::AddPropertyFilterL
// ---------------------------------------------------------------------------
//
void CCmSqlDbClause::AddPropertyFilterL( 
    CCmSqlPropertyContainer& aContainer, const TDesC8& aField, 
    HBufC8*& aClause )
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::AddPropertyFilterL"));    
    
    if( aClause && aContainer.PropertyItemCount() )
        {
        aClause = aClause->ReAllocL( aClause->Length() + 
            KCmSqlAnd().Length() + ( 2 * KCmSqlSpace().Length() ) );
        aClause->Des().Append( KCmSqlSpace() );
        aClause->Des().Append( KCmSqlAnd() );
        aClause->Des().Append( KCmSqlSpace() );                       
        }
    if( aContainer.PropertyItemCount() && !aClause )
        {
        aClause = KCmSqlAnd().AllocL();
        }
    if( aContainer.PropertyItemCount() && aClause )
        {
        aClause = aClause->ReAllocL( aClause->Length() +
        		KCmSqlLeftBracket().Length() );
        aClause->Des().Append( KCmSqlLeftBracket );
        
        for( TInt j = 0; j < aContainer.PropertyItemCount() ; j++ )
            {
            aClause = aClause->ReAllocL( aClause->Length() + 
                KCmSqlSelectTitle().Length() + 
                sizeof( aContainer.PropertyItem( j )->Id() ) );
            aClause->Des().Append( aField );
            aClause->Des().AppendNum( 
                aContainer.PropertyItem( j )->Id() );
           if( j < aContainer.PropertyItemCount() - 1 )
                {
                aClause = aClause->ReAllocL( aClause->Length() + 
                    KCmSqlOr().Length() + ( 2 * KCmSqlSpace().Length() ) );
                aClause->Des().Append( KCmSqlSpace() );
                aClause->Des().Append( KCmSqlOr() );
                aClause->Des().Append( KCmSqlSpace() );             
                }                                                         
            }
        
        aClause = aClause->ReAllocL( aClause->Length() +
        		KCmSqlRightBracket().Length() );
        aClause->Des().Append( KCmSqlRightBracket );
        
        for( TInt k = 0; k < aContainer.PropertyItemCount() ; k++ )
            {                        
            aContainer.DeletePropertyItem( k );
            k--;
            }
        }
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//    
CCmSqlDbClause::CCmSqlDbClause()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::CCmSqlDbClause"));    
    }
 
// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//   
void CCmSqlDbClause::ConstructL()
    {
    LOG(_L("[SQL Wrapper]\t CCmSqlDbClause::ConstructL"));    
    }    

// End of file

