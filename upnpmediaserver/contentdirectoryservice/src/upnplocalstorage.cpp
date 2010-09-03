/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Local interface
 *
*/


// INCLUDE FILES
#include "upnpcontainer.h"
#include "upnpitem.h"
#include "upnpelement.h"
#include "upnpattribute.h"
#include "upnpitemlist.h"
#include "upnpcontainerlist.h"
#include "upnpbrowsecriteria.h"
#include "upnplocalstorage.h"
#include "upnpobjectbean.h"
#include "upnpelementbean.h"
#include "upnpattributebean.h"
#include "upnpelementbean.h"
#include "upnpmetadatastorage.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpresourcesbean.h"
#include "upnpcdutils.h"
#include "upnpstatehandler.h"
#include "upnpstring.h"
#include "upnpcontentdirectory.h"
#include "upnpautodestroyobjectlist.h"
#include "upnpautodestroyobject.h"
#include <uriutils.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::CUpnpLocalStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpLocalStorage::CUpnpLocalStorage( CUpnpMetadataStorage* aMetadataStorage )
    {
    iMetadataStorage = aMetadataStorage; 
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpLocalStorage* CUpnpLocalStorage::NewLC(
        CUpnpMetadataStorage* aMetadataStorage )
    {
    CUpnpLocalStorage* self = new ( ELeave ) CUpnpLocalStorage( aMetadataStorage );
    
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

CUpnpLocalStorage* CUpnpLocalStorage::NewL(
        CUpnpMetadataStorage* aMetadataStorage )
    {
    CUpnpLocalStorage* self = CUpnpLocalStorage::NewLC( aMetadataStorage );
    
    CleanupStack::Pop();

    return self;
    }

void CUpnpLocalStorage::SetStateHandler( CUpnpStateHandler* aStateHandler )
    {
    iStateHandler = aStateHandler;
    }

// Destructor
CUpnpLocalStorage::~CUpnpLocalStorage()
    {

    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ShareObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ShareObjectL( CUpnpObject* aObj )
    {
    TLex8 lexer( aObj->ParentId() );
    TPtrC8 parentId = aObj->ParentId();
    TInt containerId;
    User::LeaveIfError( lexer.Val( containerId ) );
    // check parent
    ValidateContainerL( containerId );

    // start transaction
    iMetadataStorage->BeginTransactionL();

    TRAPD(err, DoShareObjectL( containerId, aObj ) );
    if ( err )
        { // rollback if error
        iMetadataStorage->RollbackTransactionL();    
        User::Leave( err );
        }
    iMetadataStorage->CommitTransactionL();
    
    // eventing
    if (iStateHandler)
        {
        iStateHandler->IncreaseSystemUpdateIdL();     
        iStateHandler->IncreaseContainerUpdateIdL( parentId );   
        }
    
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ShareReferenceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ShareReferenceL( CUpnpItem* aItem )
    {	    
	// check whether container exists and is not restricted
    TLex8 lexer( aItem->ParentId() );
    TPtrC8 parentId = aItem->ParentId();
    TInt containerId;
    User::LeaveIfError( lexer.Val( containerId ) );
    
    ValidateContainerL( containerId );    
   
    // if everything is OK -> start transaction
    iMetadataStorage->BeginTransactionL();
        
    TRAPD(err, DoShareReferenceL( containerId, aItem ) );
    if ( err )
        { // rollback if error
        iMetadataStorage->RollbackTransactionL();    
        User::Leave( err );
        }
    iMetadataStorage->CommitTransactionL();
    
    // eventing
    if (iStateHandler)
        {
        iStateHandler->IncreaseSystemUpdateIdL();                
        iStateHandler->IncreaseContainerUpdateIdL( parentId );   
        }               
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ShareReferenceListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ShareReferenceListL( CUpnpItemList* aItemList, 
											TInt* aExecutionStatus )
	{
	//----- firstly - validate all containers upon which references are to be created.
	for ( TInt i = 0; i < aItemList->ObjectCount(); i++ )
	    {	    
	    TLex8 lex((*aItemList)[i]->ParentId());
	    TInt containerId;
	    User::LeaveIfError( lex.Val(containerId) );
	    
	    ValidateContainerL(containerId);	    
	    }
	
	//----- if containers are OK, do the following ...
	
	// varaible used to do the rest of eventing
	TInt itemCommited=0; 
	    
    // start transaction
    iMetadataStorage->BeginTransactionL();
    
    // reset status
    *aExecutionStatus = 0;

    // for each item in the list ...
    for ( TInt i = 0; i < aItemList->ObjectCount(); i++ )
        {               
        // get parentId field value [upon which 'reference item' will be created]
        TLex8 lexer((*aItemList)[i]->ParentId());
        TInt parentId = -1;
        User::LeaveIfError( lexer.Val(parentId) );            
                     
        // commit every KCommitEveryNum inserts 
        if ( !(i%KCommitEveryNum) ) 
            {
            // ignore if i == 0;
            if ( i )
                {
                itemCommited=0;
                // commit last transaction
                iMetadataStorage->CommitTransactionL();
               
                // success - update status
                *aExecutionStatus = i;
                if ( iStateHandler )
                    {                                          
                    for ( TInt j = 0; j < KCommitEveryNum; j++ )
                        {
                        iStateHandler->IncreaseSystemUpdateIdL();    
                        iStateHandler->IncreaseContainerUpdateIdL( ( (*aItemList)[j]->ParentId() ) );    
                        }                       
                    }
                   // begin new transaction
                   iMetadataStorage->BeginTransactionL();                   
                }
            }
        
        // insert object
        CUpnpObject* object = ( *aItemList )[i];
        CUpnpItem* item = dynamic_cast<CUpnpItem*>( object );
        
        TRAPD( err, DoShareReferenceL( parentId, item ) );
        if ( err )
            { // rollback if error
            iMetadataStorage->RollbackTransactionL();    
            User::Leave(err);
            }
        itemCommited++;
        }
    
        // commit
        iMetadataStorage->CommitTransactionL();
        
        // success - update status
        *aExecutionStatus = aItemList->ObjectCount();
       
        //eventing
        if ( iStateHandler )
            {
            // do the rest of eventing if number of items were greater than KCommitEveryNum
            for ( TInt j = 0; j < itemCommited; j++ ) 
                {
                iStateHandler->IncreaseSystemUpdateIdL();     
                iStateHandler->IncreaseContainerUpdateIdL( ( ( *aItemList )[j]->ParentId() ) );    
                }            
            }			
	}

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::UnshareItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::UnshareItemL( TInt aId )
    {
    // check id
    ValidateItemL( aId );
    HBufC8* containerId = HBufC8::NewLC( KMaxIntegerLen );
    TInt containerIdInt = iMetadataStorage->GetParentIdL( aId );
    if ( containerIdInt == KErrGeneral )
        {
        User::Leave( KErrNotFound );
        }
    
    containerId->Des().Num( containerIdInt );
        
    if (aId <= KDownloadContId)
        {
        User::Leave( KErrArgument );
        }
    RArray<TInt> refList;
    CleanupClosePushL(refList);
    if ( iMetadataStorage->DeleteObjectL( aId, refList, ETrue, ETrue ) == ENoSuchObject )
        {
        User::Leave( KErrNotFound );
        }

    // eventing
    if ( iStateHandler )
        {
        TBuf8<KMaxIntegerLen> parentRefID;
        iStateHandler->IncreaseSystemUpdateIdL();     
        iStateHandler->IncreaseContainerUpdateIdL( *containerId );
        for (TInt i(0);i<refList.Count();i++)
            {
            iStateHandler->IncreaseSystemUpdateIdL();                
            parentRefID.Num( refList[i] );     
            iStateHandler->IncreaseContainerUpdateIdL( parentRefID );
            }
        }

    // clean up
    CleanupStack::PopAndDestroy( &refList );
    CleanupStack::PopAndDestroy( containerId );
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::UnshareContainerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::UnshareContainerL( TInt aId )
    {
    // check id
    ValidateContainerL( aId );
    HBufC8* containerId = HBufC8::NewLC( KMaxIntegerLen );
    containerId->Des().Num(iMetadataStorage->GetParentIdL( aId ) );
        
    if ( aId <= KDownloadContId )
        {
        User::Leave( KErrArgument );
        }
    RArray<TInt> refList;
    CleanupClosePushL(refList);    
    if ( iMetadataStorage->DeleteObjectL( aId,refList, ETrue, ETrue ) == ENoSuchObject )
        {
        User::Leave( KErrNotFound );
        }
    
    // eventing
    if ( iStateHandler )
        {            
        iStateHandler->IncreaseSystemUpdateIdL();     
        iStateHandler->IncreaseContainerUpdateIdL( *containerId );
        TBuf8<KMaxIntegerLen> parentRefID;    
        for (TInt i(0);i<refList.Count();i++)
            {
            parentRefID.Num( refList[i] );                 
            if ( containerId->Compare( parentRefID ) )
                {
                iStateHandler->IncreaseSystemUpdateIdL();                           
                iStateHandler->IncreaseContainerUpdateIdL( parentRefID );   
                }            
            }
        refList.Reset();
        }
    CleanupStack::PopAndDestroy( &refList );
    CleanupStack::PopAndDestroy( containerId );
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::GetSharedItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpItem* CUpnpLocalStorage::GetSharedItemL( TInt aItemId )
    {
    // new item
    CUpnpItem* item = CUpnpItem::NewL();
    CleanupStack::PushL( item );
    
    // get object
    GetObjectL( aItemId, item );
    
    // clean up
    CleanupStack::Pop( item );
    
    return item;
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ShareItemListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ShareItemListL(CUpnpItemList* aItemList,
        TInt* aExecutionStatus )
    {
    TInt itemCommited=0;
    // check parent    
    TLex8 lexer((*aItemList)[0]->ParentId());
    TInt parentId;
    User::LeaveIfError( lexer.Val(parentId) );
    ValidateContainerL(parentId);
    
      // start transaction
       iMetadataStorage->BeginTransactionL();
       // reset status
       *aExecutionStatus = 0;

    // for each item
    for ( TInt i = 0; i < aItemList->ObjectCount(); i++ )
        {
        // commit every KCommitEveryNum inserts
        if ( !(i%KCommitEveryNum) ) 
            {
            // ignore if i == 0;
            if ( i )
                {
                itemCommited=0;
                // commit last transaction
                iMetadataStorage->CommitTransactionL();
               
                // success - update status
                *aExecutionStatus = i;
                if ( iStateHandler )
                    {                                          
                    for ( TInt j =0; j<KCommitEveryNum; j++ )
                        {
                        iStateHandler->IncreaseSystemUpdateIdL();    
                        iStateHandler->IncreaseContainerUpdateIdL( ( (*aItemList)[0]->ParentId() ) );    
                        }                       
                    }
                   // begin new transaction
                   iMetadataStorage->BeginTransactionL();                   
                }
            }
        
        // insert object
        TRAPD( err, DoShareObjectL( parentId, ( *aItemList )[i]) );
        if ( err )
            { // rollback if error
            iMetadataStorage->RollbackTransactionL();    
            User::Leave(err);
            }
        itemCommited++;
        }
    
        // commit
        iMetadataStorage->CommitTransactionL();
        // success - update status
        *aExecutionStatus = aItemList->ObjectCount();
       
        //eventing
        if ( iStateHandler )
            {
            for ( TInt j =0; j<itemCommited; j++ )
                {
                iStateHandler->IncreaseSystemUpdateIdL();     
                iStateHandler->IncreaseContainerUpdateIdL( ( ( *aItemList )[0]->ParentId() ) );    
                }            
            }                       

    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ValidateContainerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ValidateContainerL( TInt aContainerId )
    {
    if ( !iMetadataStorage->IsContainerL( aContainerId ) )
        {
        User::Leave( KErrNotFound );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ValidateItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::ValidateItemL( TInt aItemId )
    {
    if ( iMetadataStorage->IsContainerL( aItemId ) )
        {
        User::Leave( KErrNotFound );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::UnshareItemListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::UnshareItemListL( RArray<TInt>& aObjList, 
                                          TInt* aExecutionStatus )
    {
    // list of resources
    RArray<TInt> refList;
    RArray<TInt> resList;
    RArray<TInt> objCommitedList;
    CleanupClosePushL(objCommitedList);
    CleanupClosePushL(resList);
    CleanupClosePushL(refList);
    TInt parentId(0);
    // start transaction
    iMetadataStorage->BeginTransactionL();
    // reset status
    *aExecutionStatus = 0;

    // for each item
    for (TInt i = 0; i < aObjList.Count(); i++ )
        {
        parentId = iMetadataStorage->GetParentIdL( aObjList[i] );
        if ( parentId>=0 )
            {
            objCommitedList.AppendL(parentId);
            // commit every KCommitEveryNum inserts
            if( !( i%KCommitEveryNum ) ) 
                {
                // ignore if i == 0;
                if ( i )
                    {                
                    // commit last transaction
                    iMetadataStorage->CommitTransactionL();

                    // everything went OK, delete files
                    iMetadataStorage->DeleteResourcesL(resList);
                    resList.Reset();
                   
                    // success - update status
                    *aExecutionStatus = i;
                   
                    // eventing
                    if (iStateHandler)
                        {                   
                        for (TInt j =0;j<objCommitedList.Count();j++)
                            {
                            iStateHandler->IncreaseSystemUpdateIdL();
                             TBuf8<KMaxIntegerLen> containerId;                       
                            containerId.Num( objCommitedList[j] );
                            iStateHandler->IncreaseContainerUpdateIdL( containerId );                    
                            }
                        objCommitedList.Reset();
                        TBuf8<KMaxIntegerLen> parentRefID;    
                           for ( TInt i(0); i<refList.Count(); i++ )
                            {   
                            iStateHandler->IncreaseSystemUpdateIdL();                
                            parentRefID.Num( refList[i] );     
                            iStateHandler->IncreaseContainerUpdateIdL( parentRefID );
                            }
                        refList.Reset();
                        }                                    
                
                    // begin new transaction
                    iMetadataStorage->BeginTransactionL();
                    }
                }
        
        // insert object
        TRAPD( err, iMetadataStorage->DoDeleteObjectL( aObjList[i],refList, resList,
                                                       ETrue, ETrue) );
        if ( err )
            { // rollback if error
            iMetadataStorage->RollbackTransactionL();    
            User::Leave(err);
                }
            }
        }

    // commit
    iMetadataStorage->CommitTransactionL();

    // everything went OK, delete files
    iMetadataStorage->DeleteResourcesL( resList );

    // success - update status
    *aExecutionStatus = objCommitedList.Count();

    // eventing       
    if ( iStateHandler )
       {
       for ( TInt j =0; j<objCommitedList.Count(); j++ )
            {
            if ( objCommitedList[j]>=0 )
                {                    
                TBuf8<KMaxIntegerLen> containerId;      
                containerId.Num(objCommitedList[j]);
                iStateHandler->IncreaseSystemUpdateIdL();
                iStateHandler->IncreaseContainerUpdateIdL( containerId );               
                }  
            }   
        objCommitedList.Reset(); 
        TBuf8<KMaxIntegerLen> parentRefID;    
        for ( TInt i(0); i<refList.Count(); i++ )
            {
            iStateHandler->IncreaseSystemUpdateIdL();                
            parentRefID.Num( refList[i] );     
            iStateHandler->IncreaseContainerUpdateIdL( parentRefID );
            }
        refList.Reset();
       }
                                                               
    // clean up
    CleanupStack::PopAndDestroy( &refList );
    CleanupStack::PopAndDestroy( &resList );
    CleanupStack::PopAndDestroy( &objCommitedList );
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::GetItemListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::GetItemListL(TInt aContainerId,
        CUpnpBrowseCriteria* aBrowseCriteria, TInt* aTotalMatches,
        CUpnpItemList* aItemList )
    {
    // filter
    TBool all = aBrowseCriteria->Filter().Length();

    // get children ids
    RArray<TInt> items;
    CleanupClosePushL( items );
    if ( iMetadataStorage->GetItemListL( aContainerId, items ) == ENoSuchObject || !items.Count() )
        {
        User::Leave(KErrNotFound);
        }

    // total matches
    *aTotalMatches = items.Count();

    // add wanted children
    for ( TInt i = aBrowseCriteria->StartingIndex();
          i < *aTotalMatches  && 
          i < aBrowseCriteria->StartingIndex() 
          + aBrowseCriteria->RequestedCount() ; 
          i++ 
        )
        {
        // new item
        CUpnpItem* item = CUpnpItem::NewL();
        CleanupStack::PushL(item);

        // get object
        GetObjectL(items[i], item, all);
        aItemList->AppendObjectL(*item);

        // pop item
        CleanupStack::Pop(item);
        }

    // clean up
    CleanupStack::PopAndDestroy(&items);
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::GetContainerListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::GetContainerListL(TInt aContainerId,
        CUpnpBrowseCriteria* aBrowseCriteria, TInt* aTotalMatches,
        CUpnpContainerList* aList )
    {
    // filter
    TBool all = aBrowseCriteria->Filter().Length();

    // get children ids
    RArray<TInt> contrs;
    CleanupClosePushL( contrs );
    if( iMetadataStorage->GetContainerListL( aContainerId, contrs ) == ENoSuchObject )
        {
        User::Leave( KErrNotFound );
        }

    // total matches
    *aTotalMatches = contrs.Count();

    // add wanted children
    for ( TInt i = aBrowseCriteria->StartingIndex();
          i < *aTotalMatches  && 
          i < aBrowseCriteria->StartingIndex() 
          + aBrowseCriteria->RequestedCount() ; 
          i++ 
        )
        {
        // new item
        CUpnpContainer* contr = CUpnpContainer::NewL();
        CleanupStack::PushL( contr );

        // get object
        GetObjectL( contrs[i], contr, all );
        aList->AppendObjectL( *contr );

        // pop item
        CleanupStack::Pop( contr );
        }

    // clean up
    CleanupStack::PopAndDestroy( &contrs );
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::GetSingleContainerL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpContainer* CUpnpLocalStorage::GetSingleContainerL(TInt aContainerId )
    {
    // new item
    CUpnpContainer* cont = CUpnpContainer::NewL();
    CleanupStack::PushL(cont);

    // get object
    GetObjectL( aContainerId, cont );

    // clean up
    CleanupStack::Pop( cont );

    return cont;
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::CreateObjectBeanLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpObjectBean* CUpnpLocalStorage::CreateObjectBeanLC(CUpnpObject* aItem )
    {
    CUpnpObjectBean* objBn = CUpnpObjectBean::NewLC();

    // restricted
    objBn->SetObjRestricted( aItem->Restricted() );

    // searchable
    objBn->SetObjSearchable( ETrue );

    // class
    objBn->SetObjClassL( aItem->ObjectClass() );

    // title
    objBn->SetObjTitleL( aItem->Title() );

    return objBn;
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::CreateElementBeanLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpElementBean* CUpnpLocalStorage::CreateElementBeanLC(CUpnpElement& aElement )
    {
    CUpnpElementBean* elmBn = CUpnpElementBean::NewLC();

    // name
    elmBn->SetElmNameL( aElement.Name() );

    // value
    elmBn->SetElmValueL( aElement.Value() );

    // is required
    elmBn->SetElmIsRequired( aElement.IsRequired() );

    TBool hasAttribute = aElement.GetAttributes().Count() > 0;
    elmBn->SetElmHasAttribute( hasAttribute );
    return elmBn;
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::CreateAttributeBeanLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean* CUpnpLocalStorage::CreateAttributeBeanLC(
        CUpnpAttribute& aAttr )
    {
    CUpnpAttributeBean* atrBn = CUpnpAttributeBean::NewLC();
    
    // name
    atrBn->SetAtrNameL( aAttr.Name() );
    
    // value
    atrBn->SetAtrValueL( aAttr.Value() );
    
    // set required
    if ( aAttr.Name() == KprotocolInfo )
        {
        atrBn->SetAtrIsRequired( ETrue );
        }
    
    return atrBn;
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::DoShareItemL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::DoShareObjectL(TInt aContainer, CUpnpObject* aObject )
    {

    // object table
    CUpnpObjectBean* objBn = CreateObjectBeanLC( aObject );
    TInt objId = iMetadataStorage->InsertIntoObjectTableL( aContainer, objBn );
    CleanupStack::PopAndDestroy( objBn );

    // set id
    HBufC8* id = HBufC8::NewLC( KMaxIntegerLen );
    id->Des().Num( objId );    
    aObject->SetIdL( *id );
    CleanupStack::PopAndDestroy( id ); 

    // add resources
    HandleResourcesL( aObject );

    // elements
    const RUPnPElementsArray& elms = aObject->GetElements ( );
    TInt elmId = iMetadataStorage->GetNextKeyL(KElementTableName8,elms.Count());
    for (TInt i = 0; i < elms.Count ( ); i++ )
        {
        CUpnpElementBean* elm = CreateElementBeanLC( *elms[i] );
        iMetadataStorage->InsertIntoElementTableL( objId, elm, elmId );
        CleanupStack::PopAndDestroy( elm );

        // attributes
        const RUPnPAttributesArray& attrs = elms[i]->GetAttributes ( );
        TInt atrId = iMetadataStorage->GetNextKeyL( KAttributeTableName8, attrs.Count() );
        for( TInt j = 0; j < attrs.Count(); j++ )
            {
            CUpnpAttributeBean* attr = CreateAttributeBeanLC (*attrs[j] );
            iMetadataStorage->InsertIntoAttributeTableL(elmId, attr, objId,atrId);
            atrId++;
            CleanupStack::PopAndDestroy (attr );
            }
        elmId++;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::DoShareReferenceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::DoShareReferenceL( TInt aContainerId, CUpnpItem* aRefItem ) 
	{    			
    // prepare for a while main object to get some data from it
    CUpnpItem* mainObj = CUpnpItem::NewL();
    CleanupStack::PushL( mainObj );
    
	TLex8 lexer( aRefItem->RefId() );
	TInt mainObjId;
	User::LeaveIfError(lexer.Val(mainObjId));

	// try to get 'real object', not e.g reference to reference
	mainObjId = ReferedObjectIdL( mainObjId );
	
	// set new refID, pointing onto real object
	TBuf8<KMaxIntegerLen> newRefIdBuf;
    newRefIdBuf.Num( mainObjId );               
    aRefItem->SetRefIdL( newRefIdBuf );
	// get all data from main object
    GetObjectL( mainObjId, mainObj ); 
    
    // setting: title and class in reference object, basing on info from main object 
    aRefItem->SetTitleL( mainObj->Title() );
    aRefItem->SetObjectClassL( mainObj->ObjectClass() );
    
    CleanupStack::PopAndDestroy( mainObj );
                                 
    // setting 'restricted' field
    aRefItem->SetRestricted(EFalse);    
                  
    // creating new 'object bean' basing on 'reference object' 
    CUpnpObjectBean* refBn = CreateObjectBeanLC( aRefItem );        
    refBn->SetObjRefId( mainObjId ); 
    refBn->SetObjParentId( aContainerId );
    
    // inserting newly created reference object onto 'object table'
    TInt id = iMetadataStorage->InsertIntoObjectTableL( aContainerId, refBn );
    CleanupStack::PopAndDestroy( refBn );                   
    
    // setting new id in created reference object     
	TBuf8<KMaxIntegerLen> idBuf;
    idBuf.Num( id );               
    aRefItem->SetIdL( idBuf );
    }	    

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::HandleResourcesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::HandleResourcesL(CUpnpObject* aItem )
    {
    //
    const RUPnPElementsArray& elms = aItem->GetElements();
    for ( TInt i = 0; i < elms.Count(); i++ )
        {
        if ( elms[i]->Name() == KRes )
            {

            // get file name
            TParse fp;
            fp.Set( elms[i]->FilePath(), 0, 0 );

            TPtrC8 itemId = aItem->Id();
            HBufC8* contUri = UpnpCdUtils::BuildContentUriL(
                    iMetadataStorage->GetNextKeyL( KImportUriId8 ), fp.Ext(), 
                    itemId );
            CleanupStack::PushL(contUri);

            // add resource to the database
            CUpnpResourcesBean* resBn = CUpnpResourcesBean::NewLC();
            resBn->SetId( UpnpCdUtils::ResIdFromUriL( *contUri ) );
            resBn->SetPathL( elms[i]->FilePath() );
            resBn->SetIsReadonly( ETrue );
            resBn->SetIsThumbnail( EFalse );

            TInt objectId;
            TLex8 lexer( aItem->Id() );
            User::LeaveIfError( lexer.Val( objectId ) );
            iMetadataStorage->AddResourceL( resBn, objectId );

            // update object
            HBufC8* uri = ValAsUriL( *contUri );
            CleanupStack::PushL( uri );
            elms[i]->SetValueL( *uri );
            CleanupStack::PopAndDestroy( uri );
            CleanupStack::PopAndDestroy( resBn );
            CleanupStack::PopAndDestroy( contUri );
            }

        }
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::GetObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::GetObjectL( TInt aObjId, CUpnpObject* aObj, TBool aAll )
    {
    // get view
    RDbView view = iMetadataStorage->GetObjViewL( aObjId );
    CleanupClosePushL( view );
    
    if ( view.FirstL() )
        {
        // get row
        view.GetL(); 

        // create bean
        CUpnpObjectBean* objBn = CUpnpObjectBean::NewLC( view );
        
        // add values
        objBn->CopyToUpnpObjectL( aObj );
        
        if ( aAll )
            { // add elements
            AddElementsL( aObjId, aObj );
            }
        
        CleanupStack::PopAndDestroy( objBn );
        } 
    else
        { // no such object
        User::Leave( KErrNotFound );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::AddElementsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpLocalStorage::AddElementsL( TInt aObjectID, CUpnpObject* aObj )
    {
    RDbView view = iMetadataStorage->GetElementsViewL( aObjectID, ETrue );
    CleanupClosePushL( view );
    // Iterate through elements
    while( view.NextL() )
        {
        view.GetL();
        CUpnpElementBean* elBean = CUpnpElementBean::NewLC( view );
        
        // add new element
        CUpnpElement* elm = elBean->AttachElmL( aObj );
        
        // add attributes
        AddAttributesL( elBean->ElmId(), elm );
        
        if( elBean->ElmName() == KRes && 
            elBean->ElmValue().Length() ) // ignore if empty res
            { // add file path
            CUpnpResourcesBean* resBn =     
                iMetadataStorage->GetResourceL(
                    UpnpCdUtils::ResIdFromUriL( elBean->ElmValue() ) );
            CleanupStack::PushL(resBn);
            
            // set file path
            elm->SetFilePathL( resBn->Path() );
            
            CleanupStack::PopAndDestroy( resBn );
            }
        
        // clean up
        CleanupStack::PopAndDestroy( elBean );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }
// -----------------------------------------------------------------------------
// CUpnpElementBean::ValAsUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpLocalStorage::ValAsUriL( const TDesC8& aElmValue ) const 
    {    
    // uris have to be properly escaped
    HBufC8* uriDes = NULL;
    HBufC* tmp = UpnpCdUtils::Des8ToDesLC( aElmValue );        
    TUriParser uripar;
    uripar.Parse( *tmp );
    CUri8* uri = uri = UriUtils::ConvertToInternetFormL( uripar );             
    CleanupStack::PopAndDestroy( tmp );          
    CleanupStack::PushL(uri); 
    if ( uri->Uri().UriDes().Length() )
        {                
        uriDes = UpnpString::StringReplaceL( uri->Uri().UriDes(), KHash8, KHashASCII8 );                                                       
        }
    else
        {
        uriDes = uri->Uri().UriDes().AllocL();    
        }    
    CleanupStack::PopAndDestroy( uri );                
    return uriDes;        
    }
// -----------------------------------------------------------------------------
// CUpnpLocalStorage::AddAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CUpnpLocalStorage::AddAttributesL( TInt aElmId, CUpnpElement* aElm )
    {
    RDbView view = iMetadataStorage->GetAttributesViewL( aElmId, ETrue );
    CleanupClosePushL(view);

    // Iterate through attributes
    while ( view.NextL() )
        {
        view.GetL();
        CUpnpAttributeBean* atr = CUpnpAttributeBean::NewLC( view );
        atr->AttachAttrL( aElm );
        CleanupStack::PopAndDestroy( atr );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }

// -----------------------------------------------------------------------------
// CUpnpLocalStorage::ReferedObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpLocalStorage::ReferedObjectIdL(TInt aObjectId)
{
    TInt ret = KErrNotFound;
    
    TInt refId = iMetadataStorage->ReferedObjectIdL(aObjectId);
    
    if( refId != KErrNotFound )
    {
        ret = ReferedObjectIdL( refId );
    }
    else
    {
        ret = aObjectId;
    }

    return ret;
}

//  End of File  

