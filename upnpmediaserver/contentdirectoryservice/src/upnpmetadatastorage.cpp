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
* Description:  Metadata storage
 *
*/


// INCLUDE FILES
#include <upnpdominterface.h>
#include "upnpmetadatastorage.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpobjectbean.h"
#include "upnpelementbean.h"
#include "upnpcdutils.h"
#include "upnpresourcesbean.h"
#include "upnpitem.h"
#include "upnpelement.h"
#include "upnpattributebean.h"
#include "upnpcddbfactory.h"

#define KLogFile _L("ContentDirectoryDb.log")
#include "upnpcustomlog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CUpnpMetadataStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMetadataStorage::CUpnpMetadataStorage()
    {

    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::ConstructL( const TDesC& aDatabaseFileName )
    {
    User::LeaveIfError( iFsSession.Connect( ) );
    OpenDbL( aDatabaseFileName );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMetadataStorage* CUpnpMetadataStorage::NewLC(
    const TDesC& aDatabaseFileName )
    {
    CUpnpMetadataStorage* self = new( ELeave ) CUpnpMetadataStorage();

    CleanupStack::PushL( self );
    self->ConstructL( aDatabaseFileName );

    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMetadataStorage* CUpnpMetadataStorage::NewL(
    const TDesC& aDatabaseFileName )
    {
    CUpnpMetadataStorage* self =
            CUpnpMetadataStorage::NewLC( aDatabaseFileName );

    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::~CUpnpMetadataStorage
// Destructor
// -----------------------------------------------------------------------------
//    
CUpnpMetadataStorage::~CUpnpMetadataStorage()
    {
    if ( iIsOpen )
        {
        iDatabase.Compact( );
        }
    iDatabase.Close( );
    delete iFileStore;
    iFsSession.Close( );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::OpenDbL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::OpenDbL( const TFileName& aDatabaseFileName )
    {
    iFileStore = CPermanentFileStore::OpenL( iFsSession, aDatabaseFileName,
        EFileRead|EFileWrite );
    iFileStore->SetTypeL( iFileStore->Layout( ) );
    iDatabase.OpenL( iFileStore, iFileStore->Root( ) );
    CheckDatabaseL( );
    iIsOpen = ETrue;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DbChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DbChangedL( TInt aWeight )
    {
    iCompactCounter += aWeight;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DbChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::CompactDbL()
    {
    if ( iCompactCounter >= KCompactWeight )
        {
        iCompactCounter = 0;
        User::LeaveIfError( iDatabase.Compact( ) );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::BeginTransactionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::BeginTransactionL()
    {
    iDatabase.Begin( );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CommitTransactionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::CommitTransactionL()
    {
    TInt err = iDatabase.Commit( );
    if ( err != KErrNone )
        {
        iDatabase.Rollback( );
        User::LeaveIfError( iDatabase.Recover( ) );
        User::Leave( err );
        }
    CompactDbL( );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::RollbackTransactionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::RollbackTransactionL()
    {
    iDatabase.Rollback( );
    User::LeaveIfError( iDatabase.Recover( ) );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjectListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::GetObjectListL( TInt aParentId,
    RArray<TInt>& aList )
    {
    TUpnpErrorCode ret = CheckObjectRestrictionL( aParentId ); // make sure the parent exists
    if ( ret != ENoSuchObject && ret != ENoContainer )
        {
        // prepare query
        TBuf<sizeof(KGetObjectListSqlCmd)/KCharSize + KMaxIntegerLen> query;
        query.Format( KGetObjectListSqlCmd, aParentId );
        // prepare view
        RDbView view;
        User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( query ),
            view.EReadOnly ) );
        CleanupClosePushL( view );
        // evaluate
        User::LeaveIfError( view.EvaluateAll( ) );
        // for each object
        const TInt KIdColNo=1;

        while ( view.NextL( ) )
            {
            view.GetL( );
            // get object
            TInt childId = view.ColInt( KIdColNo );
            aList.AppendL( childId );
            }
        //clean up
        CleanupStack::PopAndDestroy( &view );
        ret = EUpnpOk;
        }
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjectListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::GetItemListL( TInt aParentId,
    RArray<TInt>& aList )
    {
    TUpnpErrorCode ret = CheckObjectRestrictionL( aParentId ); // make sure the parent exists
    if ( ret != ENoSuchObject )
        {
        // prepare query
        TBuf<sizeof(KGetItemListSqlCmd)/KCharSize + KMaxIntegerLen> query;
        query.Format( KGetItemListSqlCmd, aParentId );
        // prepare view
        RDbView view;
        User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( query ),
            view.EReadOnly ) );
        CleanupClosePushL( view );
        // evaluate
        User::LeaveIfError( view.EvaluateAll( ) );
        // for each object
        const TInt KIdColNo=1;
        while ( view.NextL( ) )
            {
            view.GetL( );
            // get object
            TInt childId = view.ColInt( KIdColNo );
            aList.AppendL( childId );
            }
        //clean up
        CleanupStack::PopAndDestroy( &view );
        ret = EUpnpOk;
        }
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetContainerListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::GetContainerListL( TInt aParentId,
    RArray<TInt>& aList )
    {
    TUpnpErrorCode ret = CheckObjectRestrictionL( aParentId ); // make sure the parent exists
    if ( ret != ENoSuchObject )
        {
        // prepare query
        TBuf<sizeof(KGetContainerListSqlCmd)/KCharSize + KMaxIntegerLen> query;
        query.Format( KGetContainerListSqlCmd, aParentId );
        // prepare view
        RDbView view;
        User::LeaveIfError( view.Prepare( iDatabase, TDbQuery( query ),
            view.EReadOnly ) );
        CleanupClosePushL( view );
        // evaluate
        User::LeaveIfError( view.EvaluateAll( ) );
        // for each object
        const TInt KIdColNo=1;
        while ( view.NextL( ) )
            {
            view.GetL( );
            // get object
            TInt childId = view.ColInt( KIdColNo );
            aList.AppendL( childId );
            }
        //clean up
        CleanupStack::PopAndDestroy( &view );
        ret = EUpnpOk;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CheckObjectRestrictionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::CheckObjectRestrictionL( TInt aObjId )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EReadOnly ) );
    User::LeaveIfError( table.SetIndex( KObjIdIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        CDbColSet* colSet = table.ColSetL( );
        TDbColNo colNo(colSet->ColNo( KObjRestrictedColName ) );
        if ( !table.ColUint8( colNo ) ) // is restricted?
            { // OK
            TDbColNo colNo(colSet->ColNo( KObjClassColName ) );
            if ( table.ColDes8(colNo).Find( KContainerClass8 ) == 0 ) // check whether the object is a container
                { // this is a container
                ret = EUpnpOk;
                }
            else
                { // there is such object but this is not a container
                ret = ENoContainer;
                }
            }
        else
            {
            ret = ERestrictedObject;
            }
        delete colSet;
        }
    else
        { // no such object
        ret = ENoSuchObject;
        }

    CleanupStack::PopAndDestroy( &table );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CheckObjectRestrictionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpMetadataStorage::IsContainerL( TInt aObjId )
    {
    TBool ret = EFalse;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EReadOnly ) );
    User::LeaveIfError( table.SetIndex( KObjIdIndexName ) ); // use index

    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        CDbColSet* colSet = table.ColSetL( );
        TDbColNo colNo(colSet->ColNo( KObjClassColName ) );
        if ( table.ColDes8(colNo).Find( KContainerClass8 ) == 0 ) // check whether the object is a container
            { // this is a container
            ret = ETrue;
            }
        delete colSet;
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddMainTagAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUpnpErrorCode CUpnpMetadataStorage::AddMainTagAttrL(
    TXmlEngElement aElement, TInt aId, const TDesC8& aName )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    // xml string of name

    // is the attr exist?
    TXmlEngAttr atr = aElement.AttributeNodeL( aName );
    if ( atr.IsNull( ) )
        { // attr does not exist - add
        TBuf<sizeof(KSelectFromObjectSqlCmd)/KCharSize +
        KMaxIntegerLen + KColNameMaxLenth> query;
        query.Format( KSelectFromObjectSqlCmd, &aName, aId );

        // Execute command
        RDbView view;
        CleanupClosePushL( view );
        TInt dbErr = view.Prepare( iDatabase, TDbQuery( query ),
            view.EReadOnly );
        LOGSQL( "CUpnpContentDirectoryDb::AddMainTagAttrL",
            "RDbView::Prepare", dbErr, &query );
        if ( dbErr == KErrNone )
            {
            CleanupClosePushL( view );
            User::LeaveIfError( view.EvaluateAll( ) );

            // if there is a row, add attribute
            if ( view.FirstL( ) )
                {
                // get row
                view.GetL( );
                // col's val as Des
                HBufC8* buff = GetColDesValFromRowSetL( view, 1 );
                CleanupStack::PushL( buff );
                // prepare xml string of value
                // add attributes
                aElement.AddNewAttributeL( aName, *buff );
                ret = EUpnpOk;
                // clean up
                CleanupStack::PopAndDestroy( buff );
                }
            else
                { // no such object in the database
                ret = ENoSuchObject;
                }
            }
        else
            { // probably the name of the attr was wrong - ignore
            ret = ENoSuchObject;
            }
        // clean up
        CleanupStack::PopAndDestroy( &view );
        }
    else // if(!atr.IsNull())
        { // already exists - it is OK, do not need to do anything
        ret = EUpnpOk;
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetElementViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
RDbView CUpnpMetadataStorage::GetElementViewL( TInt aObjId,
    const TDesC8& aElName )
    {
    // convert to TDesC
    HBufC* elName = HBufC::NewLC( aElName.Length( ) );
    elName->Des().Copy( aElName );

    // Prepare SQL command
    HBufC* query = HBufC::NewLC( KSelectElmFromElmSqlCmd().Length( ) 
                                + KNamedElFromElement().Length( ) 
                                + KMaxIntegerLen 
                                + elName->Length( ) );
    TPtr queryPtr(query->Des( ));
    queryPtr.Format( KSelectElmFromElmSqlCmd, aObjId );
    queryPtr.AppendFormat( KNamedElFromElement, elName );
    

    // Execute command
    RDbView view = GetEvaluatedViewL( queryPtr );
    LOGSQL( "CUpnpContentDirectoryDb::GetElementViewL", "RDbView::Prepare",
        0, &queryPtr );
    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( elName );

    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

RDbView CUpnpMetadataStorage::GetObjViewL( TInt aObjId )
    {
    // Get data from object table
    // Prepare SQL command
    TBuf<sizeof(KSelectObjectFromObjSqlCmd)/KCharSize
    + KMaxIntegerLen> query;
    query.Format( KSelectObjectFromObjSqlCmd, aObjId );

    LOGSQL( "CUpnpContentDirectoryDb::GetObjectL", "", 0, &query );

    return GetEvaluatedViewL( query );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetElementsViewL( TInt aObjectID, TBool aAll )
    {
    // Get data from element table
    // Prepare SQL command
    TBuf<sizeof(KSelectElmFromElmSqlCmd)/KCharSize
    + sizeof(KOnlyRequiredFromElement)/KCharSize + KMaxIntegerLen> query;
    query.Format( KSelectElmFromElmSqlCmd, aObjectID );

    query.Format( KSelectElmFromElmSqlCmd, aObjectID );
    if ( !aAll )
        { // only required properties
        query.Append( KOnlyRequiredFromElement );
        }

    LOGSQL( "CUpnpContentDirectoryDb::GetElementsViewL", "", 0, &query );

    return GetEvaluatedViewL( query );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetAttributesViewL( TInt aElementID, TBool aAll )
    {
    // Get data from element table
    // Prepare SQL command
    TBuf<sizeof(KSelectAtrFromAttrSqlCmd)/KCharSize
    + sizeof(KOnlyRequiredFromAtr)/KCharSize + KMaxIntegerLen> query;
    query.Format( KSelectAtrFromAttrSqlCmd, aElementID );
    if ( !aAll )
        { // only required attr are wanted
        query.Append( KOnlyRequiredFromAtr );
        }

    LOGSQL( "CUpnpContentDirectoryDb::GetAttributesViewL", "", 0, &query );

    return GetEvaluatedViewL( query );
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetAttributesViewByObjectIdL
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetAttributesViewByObjectIdL( TInt aObjectID,
    TBool aAll )
    {
    // Get data from element table
    // Prepare SQL command
    TBuf<sizeof(KSelectAtrFromAttrByObjSqlCmd)/KCharSize
    + sizeof(KOnlyRequiredFromAtr)/KCharSize + KMaxIntegerLen> query;
    query.Format( KSelectAtrFromAttrByObjSqlCmd, aObjectID );
    if ( !aAll )
        { // only required attr are wanted
        query.Append( KOnlyRequiredFromAtr );
        }

    LOGSQL( "CUpnpContentDirectoryDb::GetAttributesViewByObjectIdL", "", 0,
        &query );

    return GetEvaluatedViewL( query );
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertElementL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::InsertElementL( const TXmlEngElement& aElement,
    TInt aObjId )
    {
    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KElementTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // elm_id
    TInt elmId = GetNextKeyL( KElementTableName8 );
    table.SetColL( objColSet->ColNo( KElmIdColName ), elmId );

    // elm_name
    HBufC8 * elmName = UpnpCdUtils::GetElmNameWithNsL( aElement );
    CleanupStack::PushL( elmName );
    table.SetColL( objColSet->ColNo( KElmNameColName ), *elmName );
    CleanupStack::PopAndDestroy( elmName );

    // elm_value
    if ( aElement.Value().Length( ) )
        {
        TPtrC8 elmValue(aElement.Value( ) );
        if ( elmValue.Length( ) > KMaxUpnpLongStringLen )
            { // too long
            User::Leave( EActionFailed );
            }
        // use write stream
        RDbColWriteStream ws;
        ws.OpenLC( table, objColSet->ColNo( KElmValueColName ) );
        ws.WriteL( elmValue ); // write value
        ws.CommitL( );
        CleanupStack::PopAndDestroy( &ws );
        }

    // elm_has_attribute
    table.SetColL( objColSet->ColNo( KElmHasAttrColName ),
        aElement.HasAttributes( ) );

    // elm_obj_id
    table.SetColL( objColSet->ColNo( KElmObjIdColName ), aObjId );

    // elm_is_required
    table.SetColL( objColSet->ColNo( KElmIsRequiredColName ),
        UpnpCdUtils::IsElementRequiredL( aElement ) );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );

    return elmId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CreatePathValL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMetadataStorage::CreatePathValL( const TXmlEngElement& aElement )
    {
    TPtrC8
            parentIdPtr(UpnpDomInterface::GetAttrValueL( aElement, KParentID ));
    TLex8 lexer(parentIdPtr);
    TInt parentId;
    User::LeaveIfError( lexer.Val( parentId ) );

    return CreatePathValL( parentId );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CreatePathValL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMetadataStorage::CreatePathValL( TInt aParentId )
    {
    HBufC8* path = NULL;
    User::LeaveIfNull( path = GetObjectPathL( aParentId ) );

    CleanupStack::PushL( path );

    HBufC8* tmp = HBufC8::NewL( path->Length( ) 
                                + KPathSeparator().Length( ) 
                                + KMaxIntegerLen );
    TPtr8 tmpPtr(tmp->Des( ));
    tmpPtr.Copy( *path );
    tmpPtr.Append( KPathSeparator );

    TBuf<KMaxIntegerLen> num;
    num.Num( aParentId );
    tmpPtr.Append( num );

    CleanupStack::PopAndDestroy( path );

    return tmp;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::InsertAttributeL( const TXmlEngAttr aAttr,
    TInt aElmId, TBool aIsRequired, TInt aObjId )
    {

    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KAttributeTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // atr_id
    table.SetColL( objColSet->ColNo( KAtrIdColName ),
        GetNextKeyL( KAttributeTableName8 ) ); // leaves if error

    // atr_name
    table.SetColL( objColSet->ColNo( KAtrNameColName ), aAttr.Name( ) ); // name cannot be null

    // atr_value
    if ( aAttr.Value().Length( ) )
        {
        TPtrC8 atrValue(aAttr.Value( ) );
        if ( atrValue.Length( ) > KMaxUpnpLongStringLen )
            { // too long
            User::Leave( EActionFailed );
            }
        // use write stream
        RDbColWriteStream ws;
        ws.OpenLC( table, objColSet->ColNo( KAtrValueColName ) );
        ws.WriteL( atrValue ); // write value
        ws.CommitL( );
        CleanupStack::PopAndDestroy( &ws );
        }

    // atr_elm_id
    table.SetColL( objColSet->ColNo( KAtrElmIdColName ), aElmId );

    // atr_is_required
    table.SetColL( objColSet->ColNo( KAtrIsRequiredColName ), aIsRequired );

    // atr_obj_id
    table.SetColL( objColSet->ColNo( KAtrIdObjColName ), aObjId );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertObjectIntoObjectTableL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::InsertObjectIntoObjectTableL(
    const TXmlEngElement& aElement )
    {
    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // set attributes
    RXmlEngNodeList<TXmlEngAttr> attrList;
    CleanupClosePushL( attrList );
    aElement.GetAttributes( attrList );
    while ( attrList.HasNext( ) )
        {
        // get next
        TXmlEngAttr attr = attrList.Next( );

        // col ordinal
        HBufC* colName = HBufC::NewLC( attr.Name().Length( ) );
        colName->Des().Copy( attr.Name( ) );
        TDbColNo colNo = objColSet->ColNo( *colName );
        CleanupStack::PopAndDestroy( colName );

        // insert if exists
        if ( colNo != KDbNullColNo )
            { // (not any string attribute in the main element)
            HBufC8* tmp = attr.Value().AllocLC( );
            UpnpCdUtils::ReplaceTrueFalse( tmp );// true -> 1, false -> 0
            TLex8 lexer(*tmp);
            TInt num;
            User::LeaveIfError( lexer.Val( num ) ); // string -> integer
            // set value
            table.SetColL( colNo, num );
            CleanupStack::PopAndDestroy( tmp );
            }
        }
    CleanupStack::Check( &attrList );

    // set title
    TXmlEngElement objTitle;
    UpnpDomInterface::GetElementL( aElement, objTitle, KObjTiltleColName8( ) );
    table.SetColL( objColSet->ColNo( KObjTiltleColName ), objTitle.Value( ) );
    CleanupStack::Check( &attrList );

    // set class
    TXmlEngElement objClass;
    UpnpDomInterface::GetElementL( aElement, objClass, KObjClassColName8( ) );
    table.SetColL( objColSet->ColNo( KObjClassColName ), objClass.Value( ) );
    CleanupStack::Check( &attrList );

    // set path
    HBufC8* path = CreatePathValL( aElement );
    CleanupStack::PushL( path );
    table.SetColL( objColSet->ColNo( KObjPathColName ), *path );
    CleanupStack::PopAndDestroy( path );
    CleanupStack::Check( &attrList );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( &attrList );
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );

    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetNextKeyForTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetNextKeyL( const TDesC8& aName )
    {
    TInt ret = KErrNotFound;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aName);
    User::LeaveIfError( table.Open( iDatabase, KSequencesTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KSeqColNameIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );
        CDbColSet* colSet = table.ColSetL( );
        CleanupStack::PushL( colSet );
        TDbColNo colNo(colSet->ColNo( KSeqNextKeyColName ) );
        ret = table.ColInt( colNo );
        table.UpdateL( );
        table.SetColL( colNo, ret + 1 );
        table.PutL( );

        DbChangedL( KExecuteWeight );

        CleanupStack::PopAndDestroy( colSet );
        }
    else
        { // no such object
        User::Leave( KErrCorrupt );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetNextKeyForTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetNextKeyL( const TDesC8& aName, TInt aAmount )
    {
    TInt ret = KErrNotFound;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aName);
    User::LeaveIfError( table.Open( iDatabase, KSequencesTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KSeqColNameIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );
        CDbColSet* colSet = table.ColSetL( );
        CleanupStack::PushL( colSet );
        TDbColNo colNo(colSet->ColNo( KSeqNextKeyColName ) );
        ret = table.ColInt( colNo );
        table.UpdateL( );
        table.SetColL( colNo, ret + aAmount );
        table.PutL( );

        DbChangedL( KExecuteWeight );

        CleanupStack::PopAndDestroy( colSet );
        }
    else
        { // no such object
        User::Leave( KErrCorrupt );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::NextKeyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::NextKeyL( const TDesC8& aName )
    {
    TInt ret = KErrNotFound;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aName);
    User::LeaveIfError( table.Open( iDatabase, KSequencesTableName,
        table.EReadOnly ) );
    User::LeaveIfError( table.SetIndex( KSeqColNameIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );
        CDbColSet* colSet = table.ColSetL( );
        CleanupStack::PushL( colSet );
        TDbColNo colNo(colSet->ColNo( KSeqNextKeyColName ) );
        ret = table.ColInt( colNo );

        CleanupStack::PopAndDestroy( colSet );
        }
    else
        { // no such object
        User::Leave( KErrCorrupt );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::ReferedObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::ReferedObjectIdL( TInt aObjectId )
    {
    TInt ret = KErrNotFound;
    TBuf<sizeof(KSelectFromObjectSqlCmd)/KCharSize
    + KMaxIntegerLen + KColNameMaxLenth> query;
    query.Format( KSelectFromObjectSqlCmd, &KObjRefIdColName, aObjectId );

    // Execute command
    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( query ), view.EReadOnly );
    LOGSQL( "CUpnpContentDirectoryDb::ReferedObjectIdL", "RDbView::Prepare",
        dbErr, &query );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.Evaluate( ) );
        if ( view.FirstL( ) )
            {
            // get col number -  there can by only one column but just in case get by name
            CDbColSet* colSet = view.ColSetL( );
            const TInt colNo = colSet->ColNo( KObjRefIdColName );
            delete colSet;

            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                ret = view.ColInt( colNo );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetElmIdForAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetElmIdForAttrL( const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    // convert to unicode
    HBufC* attrName = UpnpCdUtils::Des8ToDesLC( aAttrName );
    HBufC* attrVal = UpnpCdUtils::Des8ToDesLC( aAttrVal );

    TInt elmId = KErrNotFound;
    HBufC* query = HBufC::NewLC( aAttrVal.Length( ) 
                                 + KSelectFromSqlCmd().Length( ) 
                                 + KAtrElmIdColName().Length( ) 
                                 + KAttributeTableName().Length( ) 
                                 + KAtrValueColName().Length( ) 
                                 + KAndConditionSqlCmd().Length( ) 
                                 + KAtrNameColName().Length( ) 
                                 + aAttrName.Length( ) );
    TPtr queryPtr(query->Des( ));
    // prepare format string
    HBufC* fmt = HBufC::NewLC( KSelectFromSqlCmd().Length( ) + KAndConditionSqlCmd().Length( ) );
    TPtr fmtPtr(fmt->Des( ));
    fmtPtr.Copy( KSelectFromSqlCmd );
    fmtPtr.Append( KAndConditionSqlCmd );
    // format query
    queryPtr.Format( fmtPtr, &KAtrElmIdColName, &KAttributeTableName,
        &KAtrNameColName, attrName, &KAtrValueColName, attrVal );
    // clean up format string
    CleanupStack::PopAndDestroy( fmt );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.Evaluate( ) );
        if ( view.FirstL( ) )
            {
            // get col number -  there can by only one column but just in case get by name
            CDbColSet* colSet = view.ColSetL( );
            const TInt colNo = colSet->ColNo( KAtrElmIdColName );
            delete colSet;

            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                elmId = view.ColInt( colNo );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( attrVal );
    CleanupStack::PopAndDestroy( attrName );
    return elmId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjIdForElmIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetObjIdForElmIdL( TInt aId )
    {
    TInt objId = KErrNotFound;
    HBufC* query = HBufC::NewLC( KMaxIntegerLen + KSelectFromNumSqlCmd().Length( ) 
                                + KElementTableName().Length( ) 
                                + KElmObjIdColName().Length( ) 
                                + KElmIdColName().Length( ) );
    TPtr queryPtr(query->Des( ));
    queryPtr.Format( KSelectFromNumSqlCmd, &KElmObjIdColName,
        &KElementTableName, &KElmIdColName, aId );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.Evaluate( ) );
        if ( view.FirstL( ) )
            {
            // get col number -  there can by only one column but just in case get by name
            CDbColSet* colSet = view.ColSetL( );
            const TInt colNo = colSet->ColNo( KElmObjIdColName );
            delete colSet;

            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                objId = view.ColInt( colNo );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    return objId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjIdByAttrLikeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetElmIdForAttrLikeL( const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    //convert to unicode 
    HBufC* attrVal = UpnpCdUtils::Des8ToDesLC( aAttrVal );
    HBufC* attrName = UpnpCdUtils::Des8ToDesLC( aAttrName );

    // query buf
    TInt elmId = KErrNotFound;
    HBufC* query = HBufC::NewLC( attrVal->Length( ) 
                                + KSelectFromSqlCmd().Length( ) 
                                + KAtrElmIdColName().Length( ) 
                                + KAttributeTableName().Length( ) 
                                + KAtrValueColName().Length( ) 
                                + KAndLikeConditionSqlCmd().Length( ) 
                                + KAtrNameColName().Length( ) 
                                + attrName->Length( ) );
    TPtr queryPtr(query->Des( ));
    // prepare format string
    HBufC* fmt = HBufC::NewLC( KSelectFromSqlCmd().Length( ) + KAndLikeConditionSqlCmd().Length( ) );
    TPtr fmtPtr(fmt->Des( ));
    fmtPtr.Copy( KSelectFromSqlCmd );
    fmtPtr.Append( KAndLikeConditionSqlCmd );
    // format query
    queryPtr.Format( fmtPtr, &KAtrElmIdColName, &KAttributeTableName,
        &KAtrNameColName, attrName, &KAtrValueColName, attrVal );
    // clean up format string
    CleanupStack::PopAndDestroy( fmt );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.Evaluate( ) );
        if ( view.FirstL( ) )
            {
            // get col number -  there can by only one column but just in case get by name
            CDbColSet* colSet = view.ColSetL( );
            const TInt colNo = colSet->ColNo( KAtrElmIdColName );
            delete colSet;

            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                elmId = view.ColInt( colNo );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( attrName );
    CleanupStack::PopAndDestroy( attrVal );
    return elmId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetParentIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetParentIdL( TInt aObjId )
    {
    TInt objId=KErrGeneral;
    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                + KObjParentIdColName().Length( ) 
                                + KObjectTableName().Length( ) 
                                + KObjIdColName().Length( ) 
                                + KMaxIntegerLen );
    TPtr queryPtr(query->Des( ));
    queryPtr.Format( KSelectFromNumSqlCmd, &KObjParentIdColName,
        &KObjectTableName, &KObjIdColName, aObjId );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.Evaluate( ) );
        if ( view.FirstL( ) )
            {
            CDbColSet* colSet = view.ColSetL( );
            const TInt colNo = colSet->ColNo( KObjParentIdColName );
            delete colSet;

            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                objId = view.ColInt( colNo );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    return objId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetChildCountL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::GetChildCountL( TInt aObjId )
    {
    TInt childCount = KErrGeneral;

    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                 + KObjIdColName().Length( ) 
                                 + KObjectTableName().Length( ) 
                                 + KObjParentIdColName().Length( ) 
                                 + KMaxIntegerLen );
    TPtr queryPtr(query->Des( ));
    queryPtr.Format( KSelectFromNumSqlCmd, &KObjIdColName, &KObjectTableName,
        &KObjParentIdColName, aObjId );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.EvaluateAll( ) );
        childCount = view.CountL( );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    return childCount;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DeleteResElL( const TDesC& aResVal, TInt ObjId )
    {
    HBufC* query = HBufC::NewLC( KDeleteResElSqlCmd().Length( ) + aResVal.Length( ) + KMaxIntegerLen );
    TPtr SqlCmd(query->Des( ));
    SqlCmd.Format( KDeleteResElSqlCmd, &aResVal, ObjId );

    // execute
    ExecuteL( SqlCmd );

    // clean up
    CleanupStack::PopAndDestroy( query );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::ExecuteL( const TDesC& aSqlCmd )
    {
    // execute
    TInt err = iDatabase.Execute( aSqlCmd );
    LOGSQL( "CUpnpContentDirectoryDb::DeleteResElL", "iDataBase.Execute",
        err, &aSqlCmd );
    User::LeaveIfError( err );

    // db compact
    DbChangedL( KExecuteWeight );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResElL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetViewOfObjectListForResL( const TDesC& aResVal )
    {
    HBufC* query = HBufC::NewLC( KSelectFromSqlCmd().Length( ) 
                                + KElmObjIdColName().Length( ) 
                                + KElementTableName().Length( ) 
                                + KElmValueColName().Length( ) 
                                + aResVal.Length( )
                                + KNamedElFromElement().Length()
                                + KRes16().Length());
    TPtr queryPtr(query->Des( ));
    queryPtr.Format( KSelectFromSqlCmd, &KElmObjIdColName,
        &KElementTableName, &KElmValueColName, &aResVal );
    queryPtr.AppendFormat( KNamedElFromElement, &KRes16 );

    RDbView view = GetEvaluatedViewL( queryPtr );

    CleanupStack::PopAndDestroy( query );

    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetViewOfElmIdByNameAndValLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetViewOfElmIdByNameAndValL(
    const TDesC& aElmName, const TDesC& aElmValue )
    {
    HBufC* resValReplaced16 = UpnpCdUtils::EscapeAposL( aElmValue );
    HBufC* query = HBufC::NewLC( KSelectFromSqlCmd().Length( ) 
                                + KElmIdColName().Length( ) 
                                + KElementTableName().Length( ) 
                                + KElmNameColName().Length( ) 
                                + aElmName.Length( ) 
                                + KAndConditionSqlCmd().Length( ) 
                                + KElmValueColName().Length( ) 
                                + resValReplaced16->Des().Length( ) );
    TPtr queryPtr(query->Des( ));

    // format query
    queryPtr.Format( KSelectFromSqlCmd, &KElmIdColName, &KElementTableName,
        &KElmNameColName, &aElmName );
    queryPtr.AppendFormat( KAndConditionSqlCmd, &KElmValueColName,
        resValReplaced16 );

    RDbView view = GetEvaluatedViewL( queryPtr );

    CleanupStack::PopAndDestroy( query );

    delete resValReplaced16;
    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetAttrViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetAttrViewL( const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    HBufC* attrNameBuf = UpnpCdUtils::Des8ToDesLC( aAttrName );
    HBufC* attrValueBuf = UpnpCdUtils::Des8ToDesLC( aAttrVal );

    HBufC* query = HBufC::NewLC( KSelectAttributeByValueSqlCmd().Length( ) + aAttrName.Length( ) + aAttrVal.Length( )

    );
    TPtr queryPtr(query->Des( ));

    // format query
    queryPtr.Format( KSelectAttributeByValueSqlCmd( ), attrValueBuf,
        attrNameBuf );

    RDbView view = GetEvaluatedViewL( queryPtr );

    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( attrValueBuf );
    CleanupStack::PopAndDestroy( attrNameBuf );

    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetAttrViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetAttrViewL( TInt aElmId,
    const TDesC8& aAttrName )
    {
    // convert ot unicode
    HBufC* attrNameBuf = UpnpCdUtils::Des8ToDesLC( aAttrName );

    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                 + KAtrValueColName().Length( ) 
                                 + KAttributeTableName().Length( ) 
                                 + KAtrElmIdColName().Length( ) 
                                 + KMaxIntegerLen 
                                 + KAndConditionSqlCmd().Length( ) 
                                 + KAtrNameColName().Length( ) 
                                 + aAttrName.Length( ) );
    TPtr queryPtr(query->Des( ));
    // prepare format string
    HBufC* fmt = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) + KAndConditionSqlCmd().Length( ) );
    TPtr fmtPtr(fmt->Des( ));
    fmtPtr.Copy( KSelectFromNumSqlCmd );
    fmtPtr.Append( KAndConditionSqlCmd );
    // format query
    queryPtr.Format( fmtPtr, &KAsterisk, &KAttributeTableName,
        &KAtrElmIdColName, aElmId, &KAtrNameColName, attrNameBuf );

    CleanupStack::PopAndDestroy( fmt );

    RDbView view = GetEvaluatedViewL( queryPtr );

    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( attrNameBuf );

    return view;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetAttrViewByObjectIdL
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetAttrViewByObjectIdL( TInt aObjectId,
    const TDesC8& aAttrName )
    {
    // convert ot unicode
    HBufC* attrNameBuf = UpnpCdUtils::Des8ToDesLC( aAttrName );

    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                 + KAtrValueColName().Length( ) 
                                 + KAttributeTableName().Length( ) 
                                 + KAtrIdObjColName().Length( ) 
                                 + KMaxIntegerLen 
                                 + KAndConditionSqlCmd().Length( ) 
                                 + KAtrNameColName().Length( ) 
                                 + aAttrName.Length( ) );
    TPtr queryPtr(query->Des( ));
    // prepare format string
    HBufC* fmt = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) + KAndConditionSqlCmd().Length( ) );
    TPtr fmtPtr(fmt->Des( ));
    fmtPtr.Copy( KSelectFromNumSqlCmd );
    fmtPtr.Append( KAndConditionSqlCmd );
    // format query
    queryPtr.Format( fmtPtr, &KAsterisk, &KAttributeTableName,
        &KAtrIdObjColName, aObjectId, &KAtrNameColName, attrNameBuf );

    CleanupStack::PopAndDestroy( fmt );

    RDbView view = GetEvaluatedViewL( queryPtr );

    CleanupStack::PopAndDestroy( query );
    CleanupStack::PopAndDestroy( attrNameBuf );

    return view;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::PrepareViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::PrepareViewL( const TDesC& aSqlQuery )
    {
    RDbView view;
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( aSqlQuery ),
        view.EReadOnly );
    LOGSQL( "CUpnpMetadataStorage::GetViewL", "RDbView::Prepare", dbErr,
        &aSqlQuery );
    User::LeaveIfError( dbErr );

    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetEvaluatedViewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RDbView CUpnpMetadataStorage::GetEvaluatedViewL( const TDesC& aSqlQuery )
    {
    RDbView view = PrepareViewL( aSqlQuery );

    // evaluate
    TInt dbErr = view.EvaluateAll( );
    LOGCD( "CUpnpMetadataStorage::GetViewLC", "RDbView::EvaluateAll", dbErr,
        "" );
    User::LeaveIfError( dbErr );

    return view;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjectPathL 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMetadataStorage::GetObjectPathL( TInt aObjId )
    {
    HBufC8* ret = NULL;
    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                + KObjPathColName().Length( ) 
                                + KObjectTableName().Length( ) 
                                + KObjIdColName().Length( ) 
                                + KMaxIntegerLen );
    TPtr queryPtr(query->Des( ));
    // format query
    queryPtr.Format( KSelectFromNumSqlCmd, &KObjPathColName,
        &KObjectTableName, &KObjIdColName, aObjId );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.EvaluateAll( ) );

        CDbColSet* colSet = view.ColSetL( );
        const TInt colNo = colSet->ColNo( KObjPathColName );
        delete colSet;

        if ( view.FirstL( ) )
            {
            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                ret = view.ColDes8( colNo ).AllocL( );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetObjectTitleL 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMetadataStorage::GetObjectTitleL( TInt aObjId )
    {
    HBufC8* ret = NULL;
    HBufC* query = HBufC::NewLC( KSelectFromNumSqlCmd().Length( ) 
                                + KObjTiltleColName().Length( ) 
                                + KObjectTableName().Length( ) 
                                + KObjIdColName().Length( ) 
                                + KMaxIntegerLen );
    TPtr queryPtr(query->Des( ));
    // format query
    queryPtr.Format( KSelectFromNumSqlCmd, &KObjTiltleColName,
        &KObjectTableName, &KObjIdColName, aObjId );

    RDbView view;
    CleanupClosePushL( view );
    TInt dbErr = view.Prepare( iDatabase, TDbQuery( queryPtr ),
        view.EReadOnly );
    if ( dbErr == KErrNone )
        {
        User::LeaveIfError( view.EvaluateAll( ) );

        CDbColSet* colSet = view.ColSetL( );
        const TInt colNo = colSet->ColNo( KObjTiltleColName );
        delete colSet;

        if ( view.FirstL( ) )
            {
            view.GetL( );
            if ( !view.IsColNull( colNo ) )
                {
                ret = view.ColDes8( colNo ).AllocL( );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean* CUpnpMetadataStorage::GetResourceL( TInt64 aResId )
    {
    CUpnpResourcesBean* resBn = NULL;
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aResId);
    User::LeaveIfError( table.Open( iDatabase, KResourcesTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KRscIdIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );
        // table bean
        resBn = CUpnpResourcesBean::NewL( table );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );
    return resBn;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DeleteResourceByObjIdL( TInt aObjId,
    TBool aDeleteFile )
    {
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KResourcesTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KRscIdObjIndexName ) ); // use index
    while ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        // table bean
        CUpnpResourcesBean* resBn = CUpnpResourcesBean::NewLC( table );

        if ( aDeleteFile && !resBn->IsReadonly( ) && resBn->IsThumbnail( ) )
            { // delete the file
            DeleteFileL( resBn->Path( ) );
            }

        // clean up 
        CleanupStack::PopAndDestroy( resBn );

        // delete the row
        table.DeleteL( );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResourceByResIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DeleteResourceByResIdL( TInt64 aResId,
    TBool aDeleteFile )
    {
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aResId);
    User::LeaveIfError( table.Open( iDatabase, KResourcesTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KRscIdIndexName ) ); // use index
    while ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        // table bean
        CUpnpResourcesBean* resBn = CUpnpResourcesBean::NewLC( table );

        if ( aDeleteFile && !resBn->IsReadonly( ) && resBn->IsThumbnail( ) )
            { // delete the file
            DeleteFileL( resBn->Path( ) );
            }

        // clean up 
        CleanupStack::PopAndDestroy( resBn );

        // delete the row
        table.DeleteL( );
        }
    // clean up
    CleanupStack::PopAndDestroy( &table );

    }
//
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteObjectsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpMetadataStorage::DeleteObjectsL( TInt aObjId )
    {
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KObjIdIndexName ) ); // use index
    if ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        // delete the row
        table.DeleteL( );
        }
    CleanupStack::PopAndDestroy( &table );
    }
//
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteReferencesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpMetadataStorage::DeleteReferencesL( TInt aObjId,
    RArray<TInt>& aRefList )
    {
    RDbTable table;
    TInt ret( KErrNotFound );
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KRefIdIndexName ) ); // use index
    while ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );
        CDbColSet* colSet = table.ColSetL( );
        CleanupStack::PushL( colSet );
        TDbColNo colNo(colSet->ColNo( KObjParentIdColName ) );
        ret = table.ColInt( colNo );
        aRefList.AppendL( ret );
        CleanupStack::PopAndDestroy( colSet );
        // delete the row
        table.DeleteL( );
        }
    CleanupStack::PopAndDestroy( &table );
    }
//
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteElementsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpMetadataStorage::DeleteElementsL( TInt aObjId )
    {
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KElementTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KElmObjIdIndexName ) ); // use index
    while ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        // delete the row
        table.DeleteL( );
        }
    CleanupStack::PopAndDestroy( &table );
    }
//
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpMetadataStorage::DeleteAttributesL( TInt aObjId )
    {
    RDbTable table;
    CleanupClosePushL( table );
    TDbSeekKey seekKey(aObjId);
    User::LeaveIfError( table.Open( iDatabase, KAttributeTableName,
        table.EUpdatable ) );
    User::LeaveIfError( table.SetIndex( KAtrElmIdIndexName ) ); // use index
    while ( table.SeekL( seekKey ) ) // find object
        { // object found
        table.GetL( );

        // delete the row
        table.DeleteL( );
        }
    CleanupStack::PopAndDestroy( &table );
    }
//
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::GetColDesValFromRowSetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
HBufC8* CUpnpMetadataStorage::GetColDesValFromRowSetL( RDbRowSet aRowset,
    TDbColNo aColNo )
    {
    TDbColType colType = aRowset.ColType( aColNo );
    TBuf8<KMaxIntegerLen> val;
    switch ( colType )
        // so far only 2 types are possible
        {
        case EDbColBit:
            val.Num( aRowset.ColUint8( aColNo ) );
            break;
        case EDbColInt32:
        case EDbColInt16:
        case EDbColInt8:
            val.Num( aRowset.ColInt( aColNo ) );
            break;
        default:
            User::Leave( KErrGeneral );
        }
    return val.AllocL( );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DeleteFileL( const TDesC& aPath )
    {
    iFsSession.Delete( aPath );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertIntoObjectTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::InsertIntoObjectTableL( TInt aContainerId,
    CUpnpObjectBean* aObjBn )
    {
    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KObjectTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // object id
    TInt objId = GetNextKeyL( KObjectTableName8 );
    table.SetColL( objColSet->ColNo( KObjIdColName ), objId );

    // parent id
    table.SetColL( objColSet->ColNo( KObjParentIdColName ), aContainerId );

    // restricted
    table.SetColL( objColSet->ColNo( KObjRestrictedColName ),
        aObjBn->ObjRestricted( ) );

    // set title
    table.SetColL( objColSet->ColNo( KObjTiltleColName ), aObjBn->ObjTitle( ) );

    // set class
    table.SetColL( objColSet->ColNo( KObjClassColName ), aObjBn->ObjClass( ) );

    table.SetColL( objColSet->ColNo( KObjRefIdColName ), aObjBn->ObjRefId( ) );
    // set path
    HBufC8* path = CreatePathValL( aContainerId );
    CleanupStack::PushL( path );
    table.SetColL( objColSet->ColNo( KObjPathColName ), *path );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( path );
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );

    return objId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertIntoElementTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::InsertIntoElementTableL( TInt aObjId,
    CUpnpElementBean* aElmBn, TInt aElmId )
    {
    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KElementTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // elm_id
    table.SetColL( objColSet->ColNo( KElmIdColName ), aElmId );

    // elm_name
    table.SetColL( objColSet->ColNo( KElmNameColName ), aElmBn->ElmName( ) );

    // elm_value
    if ( aElmBn->ElmValue().Length( ) )
        {
        // use write stream
        RDbColWriteStream ws;
        ws.OpenLC( table, objColSet->ColNo( KElmValueColName ) );
        ws.WriteL( aElmBn->ElmValue( ) ); // write value
        ws.CommitL( );
        CleanupStack::PopAndDestroy( &ws );
        }

    // elm_has_attribute
    table.SetColL( objColSet->ColNo( KElmHasAttrColName ),
        aElmBn->ElmHasAttribute( ) );

    // elm_obj_id
    table.SetColL( objColSet->ColNo( KElmObjIdColName ), aObjId );

    // elm_is_required
    table.SetColL( objColSet->ColNo( KElmIsRequiredColName ),
        aElmBn->ElmIsRequired( ) );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );

    return aElmId;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::InsertIntoAttributeTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::InsertIntoAttributeTableL( TInt aElmId,
    CUpnpAttributeBean* aAtrBn, TInt aObjId, TInt aAtrId )
    {
    // open table
    RDbTable table;
    User::LeaveIfError( table.Open( iDatabase, KAttributeTableName,
        table.EUpdatable ) );
    CleanupClosePushL( table );

    // CdbColSet
    CDbColSet* objColSet = table.ColSetL( );
    CleanupStack::PushL( objColSet );

    // rowset cursor to the beginning position
    table.Reset( );

    // insert empty row
    table.InsertL( );

    // atr_id
    table.SetColL( objColSet->ColNo( KAtrIdColName ), aAtrId ); // leaves if error

    // atr_name
    table.SetColL( objColSet->ColNo( KAtrNameColName ), aAtrBn->AtrName( ) ); // name cannot be null

    // atr_value
    if ( aAtrBn->AtrValue().Length( ) )
        {
        // use write stream
        RDbColWriteStream ws;
        ws.OpenLC( table, objColSet->ColNo( KAtrValueColName ) );
        ws.WriteL( aAtrBn->AtrValue( ) ); // write value
        ws.CommitL( );
        CleanupStack::PopAndDestroy( &ws );
        }

    // atr_elm_id
    table.SetColL( objColSet->ColNo( KAtrElmIdColName ), aElmId );

    // atr_is_required
    table.SetColL( objColSet->ColNo( KAtrIsRequiredColName ),
        aAtrBn->AtrIsRequired( ) );

    // atr_obj_id
    table.SetColL( objColSet->ColNo( KAtrIdObjColName ), aObjId );

    // complete insertion
    table.PutL( );

    DbChangedL( KExecuteWeight );

    // clean up
    CleanupStack::PopAndDestroy( objColSet );
    CleanupStack::PopAndDestroy( &table );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::DeleteObjectL( TInt aObjId,
    RArray<TInt>& aRefList, TBool aDeleteResource, TBool aLocal,
    TBool aDeleteRef )
    {
    TUpnpErrorCode ret = ENoSuchObject;

    // start transaction
    BeginTransactionL( );

    RArray<TInt> resList;
    CleanupClosePushL( resList );
    TInt delNumber = NULL;
    TRAPD( err, delNumber = DoDeleteObjectL( aObjId, aRefList, resList,
        aDeleteResource, aLocal, aDeleteRef ) );
    if ( err )
        { // rollback - error
        RollbackTransactionL( );
        User::Leave( err );
        }
    // commit if success
    CommitTransactionL( );

    // everything went OK, delete files
    DeleteResourcesL( resList );

    // clean up
    CleanupStack::PopAndDestroy( &resList );

    if ( delNumber )
        {
        ret = EUpnpOk;
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DoDeleteObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::DoDeleteObjectL( TInt aObjId,
    TBool aDeleteResource, TBool aDeleteRef )
    {
    TUpnpErrorCode ret = ENoSuchObject;
    RArray<TInt> ignore;
    CleanupClosePushL( ignore );
    if ( DoDeleteObjectL( aObjId, ignore, ignore, aDeleteResource, EFalse,
        aDeleteRef ) )
        {
        ret = EUpnpOk;
        }
    CleanupStack::PopAndDestroy( &ignore );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DoDeleteObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::DoDeleteObjectL( TInt aObjId,
    RArray<TInt>& aRefList, RArray<TInt>& aResList, TBool aDeleteResource,
    TBool aLocal, TBool aDeleteRef )
    {
    TInt numberDeleted = NULL;
    RArray<TInt> objList;
    CleanupClosePushL( objList );

    // prepare list of object and resources in the object to be deleted
    if ( aDeleteResource )
        {
        AddSubObjectsL( aObjId, objList, aResList, aLocal );
        }
    else
        {
        objList.AppendL( aObjId );
        }

    // error if nothing can be deleted
    if ( !objList.Count( ) )
        {
        User::Leave( ERestrictedObject );
        }

    // delete each object
    for ( TInt i = 0; i < objList.Count( ); i++ )
        {
        DeleteObjectsL( objList[i] );
        DeleteElementsL( objList[i] );
        DeleteAttributesL( objList[i] );
        if ( aDeleteRef )
            {
            DeleteReferencesL( objList[i], aRefList );
            }

        numberDeleted++;
        }
    CleanupStack::PopAndDestroy( &objList );

    return numberDeleted;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::DeleteResourcesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::DeleteResourcesL( RArray<TInt>& aObjList )
    {
    for ( TInt i = 0; i < aObjList.Count( ); i++ )
        {
        DeleteResourceByObjIdL( aObjList[i] );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddSubObjectsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::AddSubObjectsL( TInt aObjId,
    RArray<TInt>& aObjList, RArray<TInt>& aResList, TBool aLocal )
    {
    // number of restricted objects in the subtree 
    TInt mustRemain = 0;

    // direct children
    RArray<TInt> children;
    CleanupClosePushL( children );
    GetObjectListL( aObjId, children );

    // for each children
    for ( TInt i = 0; i < children.Count( ); i++ )
        {
        // next level
        mustRemain
                += AddSubObjectsL( children[i], aObjList, aResList, aLocal );
        }

    // clean up
    CleanupStack::PopAndDestroy( &children );

    if ( !aLocal ) // local api can delete even restricted objects
        {
        // restricted?
        if ( CheckObjectRestrictionL( aObjId ) == ERestrictedObject )
            {
            mustRemain++;
            }

        // restricted parent?
        if ( CheckParentRestrictionL( aObjId ) == ERestrictedObject )
            {
            mustRemain++;
            }
        }

    // can be deleted?
    if ( !mustRemain )
        { // delete the object
        // add currrent object
        aObjList.AppendL( aObjId );
        aResList.AppendL( aObjId );
        }
    return mustRemain;
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CheckParentRestrictionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpMetadataStorage::CheckParentRestrictionL( TInt aObjId )
    {
    TInt parentId = GetParentIdL( aObjId );
    return CheckObjectRestrictionL( parentId );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::AddResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::AddResourceL( CUpnpResourcesBean* aResBean,
    TInt aObjId )
    {
    HBufC* resValReplaced16 = UpnpCdUtils::EscapeAposL( aResBean->Path( ) );

    // prepare command
    HBufC* sqlCommand = HBufC::NewLC( KInsertInto().Length( ) 
                                      + KResourcesTableName().Length( ) 
                                      + KOpenParant().Length( ) 
                                      + KRscIdColName().Length( ) 
                                      + KRscPathColName().Length( ) 
                                      + KRscReadonlyColName().Length( ) 
                                      + KRscThumbnailColName().Length( ) 
                                      + KRscIdObjColName().Length( ) 
                                      + KCloseParant().Length( ) 
                                      + KValuesOpen().Length( ) 
                                      + KMaxIntegerLen 
                                      + resValReplaced16->Des().Length( ) 
                                      + KMaxBoolLen 
                                      + KMaxBoolLen 
                                      + KMaxLongIntegerLen
                                      + KCloseParant().Length( )  
                                      + 2 * KQuot().Length( ) 
                                      + 8 * KCommaStr().Length( ) ); // we need 4 commas and 2 quotation-marks

    TPtr sqlCmdPtr(sqlCommand->Des( ));

    TBuf<KMaxIntegerLen> num;
    TBuf<KMaxLongIntegerLen> numLong;

    sqlCmdPtr.Append( KInsertInto );
    sqlCmdPtr.Append( KResourcesTableName );
    sqlCmdPtr.Append( KOpenParant );
    sqlCmdPtr.Append( KRscIdColName );
    sqlCmdPtr.Append( KCommaStr );
    sqlCmdPtr.Append( KRscPathColName );
    sqlCmdPtr.Append( KCommaStr );
    sqlCmdPtr.Append( KRscReadonlyColName );
    sqlCmdPtr.Append( KCommaStr );
    sqlCmdPtr.Append( KRscThumbnailColName );
    sqlCmdPtr.Append( KCommaStr );
    sqlCmdPtr.Append( KRscIdObjColName );
    sqlCmdPtr.Append( KCloseParant );
    sqlCmdPtr.Append( KValuesOpen );
    numLong.Num( aResBean->Id( ) );
    sqlCmdPtr.Append( numLong );
    sqlCmdPtr.Append( KCommaStr );
    sqlCmdPtr.Append( KQuot );
    sqlCmdPtr.Append( *resValReplaced16 );
    sqlCmdPtr.Append( KQuot );
    sqlCmdPtr.Append( KCommaStr );
    num.Num( aResBean->IsReadonly( ) );
    sqlCmdPtr.Append( num );
    sqlCmdPtr.Append( KCommaStr );
    num.Num( aResBean->IsThumbnail( ) );
    sqlCmdPtr.Append( num );
    sqlCmdPtr.Append( KCommaStr );
    num.Num( aObjId );
    sqlCmdPtr.Append( num );
    sqlCmdPtr.Append( KCloseParant );

    delete resValReplaced16;
    // execute
    ExecuteL( sqlCmdPtr );
    // clean up
    CleanupStack::PopAndDestroy( sqlCommand );

    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::RecoverDatabase
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::RecoverDatabase()
    {
    return iDatabase.Recover( );
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::RecreateDatabaseFileL
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::RecreateDatabaseFileL()
    {
    iDatabase.Close( );
    iIsOpen = EFalse;

    delete iFileStore;
    iFileStore = NULL;

    CUpnpCdDbFactory* cdf = CUpnpCdDbFactory::NewLC( );

    // database file name
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect( ) );

    TFileName path;
    User::LeaveIfError( fs.PrivatePath( path ) );
    TParse fp;
    fp.Set( KDatabaseFileName( ), &path, 0 );
    path = fp.FullName( );

    // recreate database file
    cdf->CreateDatabaseFileL( path );

    CleanupStack::PopAndDestroy( &fs );
    CleanupStack::PopAndDestroy( cdf );

    OpenDbL( path );
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CompareDbColSetsL
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::CompareDbColSetsL( CDbColSet *aColSet1,
    CDbColSet *aColSet2 )
    {
    if ( !aColSet1 || !aColSet1 )
        User::Leave( KErrCorrupt );
    if ( aColSet1->Count( ) != aColSet1->Count( ) )
        User::Leave( KErrCorrupt );

    for ( TInt i=1; i<=aColSet1->Count( ); i++ )
        {
        TBool foundColumn = EFalse;
        const TDbCol& col1 = (*aColSet1)[i];

        for ( TInt j=1; j<=aColSet2->Count( ); j++ )
            {
            const TDbCol& col2 = (*aColSet2)[j];
            TInt cmpResult = col1.iName.Compare( col2.iName );

            if ( cmpResult == 0 && !foundColumn )
                {
                foundColumn = ETrue;
                if ( col1.iType != col2.iType )
                    User::Leave( KErrCorrupt );
                if ( !col1.IsLong( col1.iType ) && col1.iMaxLength
                        != col2.iMaxLength )
                    User::Leave( KErrCorrupt );
                if ( col1.iAttributes != col2.iAttributes )
                    User::Leave( KErrCorrupt );
                }
            else if ( cmpResult == 0 && foundColumn )
                User::Leave( KErrCorrupt );
            }
        if ( !foundColumn )
            User::Leave( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::CheckDatabaseL
// -----------------------------------------------------------------------------
//
void CUpnpMetadataStorage::CheckDatabaseL()
    {
    CDbTableNames *tableNames = iDatabase.TableNamesL( );
    CleanupStack::PushL( tableNames );

    if ( !tableNames )
        User::Leave( KErrCorrupt );
    CUpnpCdDbFactory* cdf = CUpnpCdDbFactory::NewLC( );

    for ( TInt i=0; i<tableNames->Count( ); i++ )
        {
        CDbColSet *colSetRef = NULL;

        if ( (*tableNames)[i].Compare( KObjectTableName ) == 0 )
            {
            colSetRef = cdf->CreateObjectTableColumnSequenceLC( );
            }
        else if ( (*tableNames)[i].Compare( KElementTableName ) == 0 )
            {
            colSetRef = cdf->CreateElementTableColumnSequenceLC( );
            }
        else if ( (*tableNames)[i].Compare( KAttributeTableName ) == 0 )
            {
            colSetRef = cdf->CreateAttributeTableColumnSequenceLC( );
            }
        else if ( (*tableNames)[i].Compare( KResourcesTableName ) == 0 )
            {
            colSetRef = cdf->CreateResourceTableColumnSequenceLC( );
            }
        else if ( (*tableNames)[i].Compare( KSequencesTableName ) == 0 )
            {
            colSetRef = cdf->CreateSequenceTableColumnSequenceLC( );
            }
        else
            User::Leave( KErrCorrupt );

        CDbColSet *colSet = iDatabase.ColSetL( (*tableNames)[i] );
        CleanupStack::PushL( colSet );

        CompareDbColSetsL( colSet, colSetRef );

        CleanupStack::PopAndDestroy( colSet );
        CleanupStack::PopAndDestroy( colSetRef );
        }

    CleanupStack::PopAndDestroy( cdf );
    CleanupStack::PopAndDestroy( tableNames );
    }
// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::HandleDbError
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadataStorage::HandleDbError( TInt aError )
    {
    TInt err = aError;
    if ( err == KErrCorrupt )
        {
        if ( iIsOpen )
            {
            err = RecoverDatabase( );
            }
        if ( err!=KErrNone && err!=KErrDiskFull && err!=KErrNoMemory )
            {
            TRAP( err, RecreateDatabaseFileL( ) );
            if ( err )
                {
                iIsOpen = EFalse;
                err = KErrCorrupt;
                }
            else
                {
                iIsOpen = ETrue;
                }
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadataStorage::IsDbCreated
// -----------------------------------------------------------------------------
//
TBool CUpnpMetadataStorage::IsDbCreated()
    {
    return iIsOpen;
    }

//  End of File  
