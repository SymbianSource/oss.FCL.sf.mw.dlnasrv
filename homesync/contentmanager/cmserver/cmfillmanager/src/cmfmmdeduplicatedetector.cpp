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
* Description:  Dublicate detector for images and videos
*
*/

#include <mdelogiccondition.h>
#include <mdesession.h>
#include <mdccommon.h>
#include <mdeconstants.h>

#include "cmfilllistitem.h"
#include "cmfmitemhandlerobserver.h"
#include "cmfmmdeduplicatedetector.h"
#include "cmfmmpxduplicatedetector.h"
#include "cmfmmain.h"
#include "msdebug.h"    

    

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::NewL
// ---------------------------------------------------------------------------
// 
CCmFmMdEDuplicateDetector* CCmFmMdEDuplicateDetector::NewL( 
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::NewL() start"));    
    CCmFmMdEDuplicateDetector* self = 
        CCmFmMdEDuplicateDetector::NewLC( aObserver, aSession );
    CleanupStack::Pop( self );
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::NewL() end"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::NewLC
// ---------------------------------------------------------------------------
//    
CCmFmMdEDuplicateDetector* CCmFmMdEDuplicateDetector::NewLC( 
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::NewLC() start"));    
    CCmFmMdEDuplicateDetector* self = 
        new ( ELeave ) CCmFmMdEDuplicateDetector( aObserver, aSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::NewLC() end"));
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::~CCmFmMdEDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmMdEDuplicateDetector::~CCmFmMdEDuplicateDetector()
    {
    LOG(_L("[FILL MNGR]\t ~CCmFmMdEDuplicateDetector()"));
    delete iTitle;
    delete iDate;
    delete iSize;
    
    delete iQuery;
    delete iMediaTypeDef;
    delete iObjectDef;
    delete iNamespace;
    delete iCheckItem;
    iExistFiles.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::CCmFmMdEDuplicateDetector
// ---------------------------------------------------------------------------
// 
CCmFmMdEDuplicateDetector::CCmFmMdEDuplicateDetector( 
    MCmFmItemHandlerObserver& aObserver, CMdESession& aSession )
    : iObserver( aObserver ), iSession( aSession )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::\
    CCmFmMdEDuplicateDetector"));
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::ConstructL
// ---------------------------------------------------------------------------
//     
void CCmFmMdEDuplicateDetector::ConstructL()
    {
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::ConstructL"));
    }
    
// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::CheckImageOrVideoL
// ---------------------------------------------------------------------------
//
void CCmFmMdEDuplicateDetector::CheckImageOrVideoL( 
    const CCmFillListItem& aItem )
    {
    LOG(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::CheckImageOrVideoL"));
    TCmMediaType theType = aItem.MediaType();
    if( iExistFiles.Count() > 0 && iCurrentMediaType == theType )
        {
        CheckImageOrVideoLIfDuplicated( aItem );
        }
    else
        {
        delete iCheckItem;
        iCheckItem = NULL;
        iCheckItem = CCmFillListItem::NewL();
        iCheckItem->SetPrimaryTextL( aItem.PrimaryText() );
        iCheckItem->SetSize( aItem.Size() );
        iCheckItem->SetDate( aItem.Date() );
        delete iQuery;
        iQuery = NULL;
        
        if( !iNamespace )
            {
            iNamespace = &iSession.GetDefaultNamespaceDefL();
            }
        if( iNamespace )
            {
            iObjectDef = 
               &iNamespace->GetObjectDefL( MdeConstants::Object::KBaseObject );
            }
        
        iCurrentMediaType = theType;
        if( ECmImage == theType || ECmOtherImage == theType )
            {
            TRACE(Print(_L("ECmImage == aType")));
            iMediaTypeDef = 
               &iNamespace->GetObjectDefL( MdeConstants::Image::KImageObject );
            }
        else if( ECmVideo == theType || ECmOtherVideo == theType )
            {
            TRACE(Print(_L("ECmVideo == aType")));
            iMediaTypeDef = 
               &iNamespace->GetObjectDefL( MdeConstants::Video::KVideoObject );
            }
        else
            {
            User::Leave( KErrArgument );
            }        
        if( !iObjectDef || !iMediaTypeDef )
            {
            User::Leave( KErrArgument );
            }
    
        iQuery = iSession.NewObjectQueryL( 
                *iNamespace, *iMediaTypeDef, this );
        if( !iQuery )
            {
            User::Leave( KErrArgument );
            }
        SetLogicConditionL( aItem );    
        //iQuery->SetResultMode( EQueryResultModeId );
        iQuery->SetResultMode( EQueryResultModeItem );
        /** Start query, no need to query more than one item */ 
        iQuery->FindL();
        }
    }
        
// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::HandleQueryNewResults
// ---------------------------------------------------------------------------
// 
void CCmFmMdEDuplicateDetector::HandleQueryNewResults(CMdEQuery& aQuery,
                                       TInt aFirstNewItemIndex,
                                       TInt aNewItemCount)
    {
    TRACE(Print(_L("[FILL MNGR]\t CCmFmMdEDuplicateDetector::\
    HandleQueryNewResultsaNewItemCount %d"), aNewItemCount ));
    CMdEObjectQuery& query = (CMdEObjectQuery &) aQuery;
    iExistFiles.Reset();
    TRACE(Print(_L("[FILL MNGR]\t aNewItemCount %d"), aNewItemCount ));
    if( &query ) 
        {
        for ( TInt i = aFirstNewItemIndex; 
              i < aFirstNewItemIndex+aNewItemCount; 
              i++ )
            {
            CCmFillListItem *itm = CCmFillListItem::NewLC();
            CMdEObject* object = (CMdEObject*)query.TakeOwnershipOfResult(i);
            CMdEProperty* prop;
            TInt propIndex = object->Property( *iTitle, prop );
            if( propIndex != KErrNotFound )
                {
                CMdETextProperty* textprop = (CMdETextProperty*) prop;
                itm->SetPrimaryTextL( textprop->Value() );
                }
            else
                {
                TParsePtrC parser( object->Uri());
                itm->SetPrimaryTextL( parser.Name() );
                }
            
            propIndex = object->Property( *iSize, prop );
            if( propIndex != KErrNotFound )
                {
                itm->SetSize( prop->Uint32ValueL() );
                }
            iExistFiles.AppendL( itm );
            TRACE(Print(_L("[FILL MNGR]\t HandleQueryNewResults [%S][%d]"),
                            &itm->PrimaryText(), itm->Size() ));
            CleanupStack::Pop( itm );
            }
        
        // sort by size
        TLinearOrder<CCmFillListItem> key( CCmFmMain::CompareBySize );
        iExistFiles.Sort( key );
        }
    }
          
// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::HandleQueryCompleted
// ---------------------------------------------------------------------------
// 
#ifdef _DEBUG
void CCmFmMdEDuplicateDetector::HandleQueryCompleted( CMdEQuery& /*aQuery*/, 
                                        TInt aError )
#else
void CCmFmMdEDuplicateDetector::HandleQueryCompleted( CMdEQuery& /*aQuery*/, 
                                        TInt /*aError*/ )
#endif                                        
    {
    TRACE(Print(_L("[FILL MNGR]\t HandleQueryCompleted error = %d"), 
        aError ));
    if( iCheckItem )
        {
        CheckImageOrVideoLIfDuplicated( *iCheckItem );
        }
    }          

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::SetLogicConditionL
// ---------------------------------------------------------------------------
// 
void CCmFmMdEDuplicateDetector::SetLogicConditionL( 
    const CCmFillListItem& /*aItem*/ )
    {
    LOG(_L("[FILL MNGR]\t SetLogicConditionL"));
    CMdELogicCondition& rootCondition = iQuery->Conditions();
    CMdELogicCondition* contentCondition = &rootCondition;
    CMdELogicCondition& logicCondition = 
        contentCondition->AddLogicConditionL( ELogicConditionOperatorOr );
    logicCondition.AddObjectConditionL( *iMediaTypeDef );
   
    iTitle = &iMediaTypeDef->GetPropertyDefL( 
            MdeConstants::Object::KTitleProperty );
    iSize = &iMediaTypeDef->GetPropertyDefL( 
            MdeConstants::Object::KSizeProperty );
    iDate = &iMediaTypeDef->GetPropertyDefL(  
            MdeConstants::Object::KCreationDateProperty );
    iQuery->AddPropertyFilterL( iTitle );      
    iQuery->AddPropertyFilterL( iSize );      
    iQuery->AddPropertyFilterL( iDate );
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::CheckImageOrVideoLIfDuplicated
// ---------------------------------------------------------------------------
// 
void CCmFmMdEDuplicateDetector::CheckImageOrVideoLIfDuplicated( 
        const CCmFillListItem& aItem )
    {
    TInt count( iExistFiles.Count() );
    TBool dup( EFalse );
    for( TInt i = 0; i < count; i++ )
        {
        if( iExistFiles[i]->Size() > aItem.Size() )
            {
            //quit loop
            i = count;
            }
        else if( iExistFiles[i]->Size() == aItem.Size() && KErrNone == 
            iExistFiles[i]->PrimaryText().CompareF( aItem.PrimaryText() ) )
            {
            //quit loop
            i = count;
            dup = ETrue;
            }
        }
    iObserver.DuplicateL( dup );
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::AppendToArray
// ---------------------------------------------------------------------------
// 
void CCmFmMdEDuplicateDetector::AppendToArrayL( const CCmFillListItem& aItem )
    {
    CCmFillListItem* p = CCmFillListItem::NewLC();
    p->SetPrimaryTextL( aItem.PrimaryText() );
    p->SetSize( aItem.Size() );
    iExistFiles.AppendL( p );
    CleanupStack::Pop( p );
    
    // sort by size
    TLinearOrder<CCmFillListItem> key( CCmFmMain::CompareBySize );
    iExistFiles.Sort( key );
    }

// ---------------------------------------------------------------------------
// CCmFmMdEDuplicateDetector::ResetArrayL
// ---------------------------------------------------------------------------
// 
void CCmFmMdEDuplicateDetector::ResetArray()
    {
    iExistFiles.Reset();
    }

// End of file
