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
* Description:  Store image ja video file processing
*
*/


#include <mdesession.h>
#include <mdeconstants.h>
#include "upnpfileutility.h"
#include <f32file.h>
#include "cmstorelistitem.h"
#include "cmsmfilemngr.h"
#include "cmsmvideoandimagemngr.h"
#include "msdebug.h"


// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::NewL
// ---------------------------------------------------------------------------
// 
CCmSmVideoAndImageMngr* CCmSmVideoAndImageMngr::NewL( 
    MCmSmVideoAndImageMngrObserver& aObserver, CCmSmFileMngr& aFileMngr,
    CMdESession& aSession, RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::NewL() start"));    
    CCmSmVideoAndImageMngr* self = CCmSmVideoAndImageMngr::NewLC( aObserver,
        aFileMngr, aSession, aItems );
    CleanupStack::Pop( self );
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::NewL() end")); 
    return self;
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::NewLC
// ---------------------------------------------------------------------------
//     
CCmSmVideoAndImageMngr* CCmSmVideoAndImageMngr::NewLC( 
    MCmSmVideoAndImageMngrObserver& aObserver, CCmSmFileMngr& aFileMngr, 
    CMdESession& aSession, RPointerArray<CCmStoreListItem>& aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::NewLC() start"));     
    CCmSmVideoAndImageMngr* self = new ( ELeave ) CCmSmVideoAndImageMngr(
        aObserver, aFileMngr, aSession, aItems );
    CleanupStack::PushL( self );
    self->ConstructL();
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::NewLC() end"));     
    return self;  
    }    

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::~CCmSmVideoAndImageMngr
// ---------------------------------------------------------------------------
// 
CCmSmVideoAndImageMngr::~CCmSmVideoAndImageMngr()
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::\
        ~CCmSmVideoAndImageMngr() start"));
    Cancel();
    delete iTitleDef;
    delete iSizeDef;
    delete iDateDef;    
    delete iQuery;
    delete iMediaTypeDef;        
    delete iObjectDef;
    delete iNamespace;
    iObjects.ResetAndDestroy();
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::\
        ~CCmSmVideoAndImageMngr() end"));            
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::CCmSmVideoAndImageMngr
// ---------------------------------------------------------------------------
//            
CCmSmVideoAndImageMngr::CCmSmVideoAndImageMngr( 
    MCmSmVideoAndImageMngrObserver& aObserver, CCmSmFileMngr& aFileMngr,
    CMdESession& aSession, RPointerArray<CCmStoreListItem>& aItems )
    :CActive( EPriorityStandard ), iObserver( aObserver ), 
    iFileMngr( aFileMngr ), iSession( aSession ), iItems( aItems )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::CCmSmVideoAndImageMngr"));    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::ConstructL
// ---------------------------------------------------------------------------
//  
void CCmSmVideoAndImageMngr::ConstructL()
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::ConstructL()"));
    }    

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::ProcessMedia
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::ProcessMedia( TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::ProcessMedia"));    
    
    TRAPD( err, SetQueryAndStartFindL( aType ) );
    iQueriedMedia = aType;
    if( err )
        {
        TRACE(Print(_L("SetQueryAndStartFindL err = %d"), err ));
        iObserver.ImagesAndVideosReady( err );
        }
    }
            
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::CancelListRefresh
// ---------------------------------------------------------------------------
//
void CCmSmVideoAndImageMngr::CancelListRefresh()
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::CancelListRefresh()"));
    ResetQuery();              
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::CancelOperation
// ---------------------------------------------------------------------------
//    
void CCmSmVideoAndImageMngr::CancelOperation()
    {   
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::CancelOperation"));    
    
    Cancel();    
    CancelListRefresh();
    iFileMngr.ResetArrays();    
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::HandleQueryNewResults
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::HandleQueryNewResults(CMdEQuery& aQuery,
                                       TInt aFirstNewItemIndex,
                                       TInt aNewItemCount)
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::HandleQueryNewResults"));
    
    CMdEObjectQuery& query = (CMdEObjectQuery &) aQuery;
    TRACE(Print(_L("[FILL MNGR]\t aNewItemCount %d"), aNewItemCount ));
    if( &query ) 
        {
        for ( TInt i=aFirstNewItemIndex; 
              i < aFirstNewItemIndex+aNewItemCount; 
              i++ ) 
            {
            CMdEObject* object = (CMdEObject*)query.TakeOwnershipOfResult(i);
            iObjects.AppendL(object);         
            }   
        }
                   
    }
          
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::HandleQueryCompleted
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::HandleQueryCompleted( CMdEQuery& aQuery, 
                                        TInt aError )
    {    
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::\
    HandleQueryCompleted error = %d"), aError ));
    if (aError == KErrNone) 
        {
        if( &aQuery )
            {
            CompleteRequest( ECmSmQueryCompleted );                                          
            }
        else
            {
            CompleteRequest( ECmSmQueryFailed );
            }                      
        }
    else if ( aError == KErrCancel )
        {
        // Query canceled
        }
    else
        {
        CompleteRequest( ECmSmQueryFailed );
        }     
    }
       
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::FilterItems
// ---------------------------------------------------------------------------
//    
void CCmSmVideoAndImageMngr::FilterItems()
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::FilterItems"));
    
    TInt err( KErrNone );
    RArray<TInt> ids;
    
    TRAP( err, iFileMngr.MediaTypesL( ids, iQueriedMedia ) );
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::FilterItems \
         mediatypes err = %d"), err ));
    TRAP( err, FilterNewItemsL( ids, iQueriedMedia ) );
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::FilterItems \
          filter new items err = %d"), err ));
    ids.Reset();
         
    CompleteRequest( ECmSmFilteringReady );
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::FilterNewItemsL
// ---------------------------------------------------------------------------
//
void CCmSmVideoAndImageMngr::FilterNewItemsL( RArray<TInt>aDeviceIds, 
    TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::FilterNewItemsL"));
    
    RArray<TInt>storeIds;
    RArray<TInt>storedIds;

    
    // Collecting mde object propertys into CCmFillListItem package         
    for( TInt i = 0; i < iObjects.Count(); i++ )
        {
        // Copy ids into temporary array
        for( TInt ll = 0; ll < aDeviceIds.Count(); ll++ )
            {
            storeIds.Append(aDeviceIds[ll]);
            }          
        
        if ( !UPnPFileUtility::IsFileProtectedL( iObjects[i]->Uri( ) ) )
            {
            if( iFileMngr.CheckIsFilledFileL(  iObjects[i]->Uri( ) ) )
                {
                storeIds.Reset();              
                storeIds.Close();
                }
            iFileMngr.IsAlreadyStoreL( iObjects[i]->Uri(),
                                       storedIds,
                                       storeIds );
            if( KErrNone < storeIds.Count() )
                {        
                TCmListItemStatus status;
                CCmStoreListItem* item = CCmStoreListItem::NewL();
                CleanupStack::PushL( item );
                item->SetPathL( iObjects[i]->Uri( ) );
                item->SetListId( iFileMngr.RetrieveListId( aType ) );
                // Ms id values are binded with file status values 
                iFileMngr.RetrieveListStatusValues( aType, status );    
                CMdEProperty* prop;
                CMdETextProperty* textprop;
                TInt propIndex = iObjects[i]->Property( *iTitleDef, prop );
                if( propIndex != KErrNotFound )
                    {
                    textprop = (CMdETextProperty*) prop;
                    item->SetPrimaryTextL( textprop->Value() );
                    }
                else
                    {
                    TParsePtrC parser( iObjects[i]->Uri());                  
                    item->SetPrimaryTextL( parser.Name() );
                    LOG(_L("[STORE MNGR]\t Title not found file name used"));
                    }
                            
                propIndex = iObjects[i]->Property( *iSizeDef, prop );
                if( propIndex != KErrNotFound )
                    {
                    item->SetSize( prop->Uint32ValueL() );
                    }
                else
                    {
                    LOG(_L("[STORE MNGR]\t Size not found"));
                    }                            
                propIndex = iObjects[i]->Property( *iDateDef, prop );
                if( propIndex != KErrNotFound )
                    {
                    CMdETimeProperty* dateprop = (CMdETimeProperty*)prop;
                    item->SetDate( dateprop->Value() );
                    }
                else
                    {
                    LOG(_L("[STORE MNGR]\t Date not found"));
                    }
                // Where item will be stored 
                for( TInt s = 0 ; s < storeIds.Count(); s++ )
                    {
                    item->SetDevId( storeIds[s], status );    
                    }
                // Where item has already been stored
                for( TInt d = 0; d < storedIds.Count(); d++ )
                    {
                    item->SetDevId( storedIds[d], ECmStored );
                    }                                                              
                iItems.AppendL( item );
                CleanupStack::Pop( item );
                }            
            }
        storedIds.Reset();
        storedIds.Close();
        storeIds.Reset();
        storeIds.Close();
        }
    }
    
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::SetQueryAndStartFindL
// ---------------------------------------------------------------------------
//
void CCmSmVideoAndImageMngr::SetQueryAndStartFindL( TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::SetQueryAndStartFindL"));
    
    ResetQuery();
    if( !iNamespace )
        {
        iNamespace = &iSession.GetDefaultNamespaceDefL();
        }
    if( iNamespace )
        {
        iObjectDef = 
           &iNamespace->GetObjectDefL( MdeConstants::Object::KBaseObject );
        }
    if( ECmImage == aType || ECmOtherImage == aType )
        {
        TRACE(Print(_L("ECmImage == aType")));
        iMediaTypeDef = 
           &iNamespace->GetObjectDefL( MdeConstants::Image::KImageObject );
        }
    else if( ECmVideo == aType || ECmOtherVideo == aType )
        {
        TRACE(Print(_L("ECmVideo == aType")));
        iMediaTypeDef = 
           &iNamespace->GetObjectDefL( MdeConstants::Video::KVideoObject );
        }
    else
        {
        User::Leave( KErrArgument );
        }        
    if( !iObjectDef )
        {
        User::Leave( KErrArgument );
        }
    if( !iMediaTypeDef )
        {
        User::Leave( KErrArgument );
        }        
        
    iQuery = iSession.NewObjectQueryL( *iNamespace, *iMediaTypeDef, this );
    if( !iQuery )
        {
        User::Leave( KErrArgument );
        }
    /** Set logic condition */
    SetLogicConditionL( aType );  
    /** Set property filters */
    SetPropertyFiltersL();
    iQuery->SetResultMode( EQueryResultModeItem );
    /** Start query */ 
    iQuery->FindL();   
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::SetPropertyFiltersL
// ---------------------------------------------------------------------------
//    
void CCmSmVideoAndImageMngr::SetPropertyFiltersL( )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::SetPropertyFiltersL"));
    
    iTitleDef = &iObjectDef->GetPropertyDefL( 
        MdeConstants::Object::KTitleProperty );
    iSizeDef = &iObjectDef->GetPropertyDefL( 
        MdeConstants::Object::KSizeProperty );
    iDateDef = &iObjectDef->GetPropertyDefL(  
        MdeConstants::Object::KCreationDateProperty );    
    iQuery->AddPropertyFilterL( iTitleDef );      
    iQuery->AddPropertyFilterL( iSizeDef );      
    iQuery->AddPropertyFilterL( iDateDef ); 
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::SetLogicConditionL
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::SetLogicConditionL( TCmMediaType aType )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::SetLogicConditionL"));
           
    CMdELogicCondition& rootCondition = iQuery->Conditions();
    CMdELogicCondition* contentCondition = &rootCondition;
    CMdELogicCondition& logicCondition = 
        contentCondition->AddLogicConditionL( ELogicConditionOperatorOr );
    logicCondition.AddObjectConditionL( *iMediaTypeDef );
    
    CMdEPropertyDef* originProperty = NULL;
    if( aType == ECmOtherVideo || aType == ECmOtherImage )
        {
        originProperty = &iObjectDef->GetPropertyDefL( _L("Origin") );
        if( originProperty )
            {
            CMdELogicCondition& origLogicCondition = 
                contentCondition->AddLogicConditionL( 
                ELogicConditionOperatorAnd );            
            origLogicCondition.AddPropertyConditionL( 
                *originProperty, TMdEUintNotEqual( 
                MdeConstants::Object::ECamera ));
            }        
        }
    if( aType == ECmVideo || aType == ECmImage )
        {
        originProperty = &iObjectDef->GetPropertyDefL( _L("Origin") );
        if( originProperty )
            {
            CMdELogicCondition& origLogicCondition = 
                contentCondition->AddLogicConditionL( 
                ELogicConditionOperatorAnd );            
            origLogicCondition.AddPropertyConditionL( 
                *originProperty, TMdEUintEqual( 
                MdeConstants::Object::ECamera ));
            }         
        }
               
    }
    
// ---------------------------------------------------------------------------
// RCCmSmVideoAndImageMngr::ResetQuery
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::ResetQuery()
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::ResetQuery"));
    
    iObjects.ResetAndDestroy();    

	if ( iQuery )
		{
		iQuery->Cancel();
	    delete iQuery;
    	iQuery = NULL; 
		}
    }    
    
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::CompleteRequest
// ---------------------------------------------------------------------------
// 
void CCmSmVideoAndImageMngr::CompleteRequest( 
    TCmSmFileProcessingStatus aStatus )
    {
    LOG(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::CompleteRequest"));
    
    __ASSERT_DEBUG(!IsActive(),\
        User::Panic( KCmSmManager, KErrInUse ));          
    SetActive();        
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, aStatus );     
    }
    
// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::RunL
// ---------------------------------------------------------------------------
//                 
void CCmSmVideoAndImageMngr::RunL()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::RunL\
         status = %d"), iStatus.Int() ));        
    switch( iStatus.Int() )
        {
        case ECmSmQueryCompleted:
            {
            FilterItems();
            break;
            }
        case ECmSmFilteringReady:
            {
            iObserver.ImagesAndVideosReady( KErrNone );
            break;
            }
        case ECmSmQueryFailed:
            {
            iObserver.ImagesAndVideosReady( KErrGeneral );
            break;
            }        
        default:
            {
            LOG(_L("[STORE MNGR]\t Store file processing RunL default"));
            User::Invariant();
            break;
            }
        }
    }    

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::DoCancel
// ---------------------------------------------------------------------------
//    
void CCmSmVideoAndImageMngr::DoCancel()
    {
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::DoCancel()")));    
    }

// ---------------------------------------------------------------------------
// CCmSmVideoAndImageMngr::RunError
// ---------------------------------------------------------------------------
//
#ifdef _DEBUG
TInt CCmSmVideoAndImageMngr::RunError( TInt aError )    
    {
#else
TInt CCmSmVideoAndImageMngr::RunError( TInt /*aError*/ )    
    {    
#endif    
    TRACE(Print(_L("[STORE MNGR]\t CCmSmVideoAndImageMngr::RunError\
         aError = %d"), aError ));
    iObserver.ImagesAndVideosReady( KErrGeneral );     
    return KErrNone;     
    }
                        
// End of file



