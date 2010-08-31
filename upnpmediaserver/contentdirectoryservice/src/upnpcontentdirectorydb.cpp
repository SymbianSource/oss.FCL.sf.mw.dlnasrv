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
* Description:  Database for MediaServer
 *
*/


// INCLUDE FILES
#include <barsread.h>
#include <barsc.h>   
#include <f32file.h>
#include <xml/dom/xmlengdom.h>
#include <sysutil.h>
#include <upnpdominterface.h>
#include <upnpstring.h>
#include "upnpprotocolinfo.h"

#include "upnpfileutils.h"
#include "upnpcontentdirectorydb.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpelementfactory.h"
#include "upnpelementbean.h"
#include "upnpobjectbean.h"
#include "upnpattributebean.h"
#include "upnpfilterelement.h"
#include "upnpresourcesbean.h"
#include "upnpcdutils.h"
#include "upnpcddbfactory.h"
#include "upnpmetadatastorage.h"

#define KLogFile _L("ContentDirectoryDb.log")
#include "upnpcustomlog.h"

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CleanupFilterElementPointerArray
// Used by TCleanupItem to clean a RPointerArray<CUpnpFilterElement>
// -----------------------------------------------------------------------------
//
void CleanupFilterElementPointerArray( TAny* aArray )
    {
    (reinterpret_cast<RPointerArray<CUpnpFilterElement>*> (aArray))->ResetAndDestroy( );
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::CUpnpContentDirectoryDb
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryDb::CUpnpContentDirectoryDb(
    CUpnpMetadataStorage* aMetadataStorage )
    {
    iMetadataStorage =aMetadataStorage;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::ConstructL()
    {
    iDOMImpl.OpenL( );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryDb* CUpnpContentDirectoryDb::NewLC(
    CUpnpMetadataStorage* aMetadataStorage )
    {
    CUpnpContentDirectoryDb* self = new ( ELeave ) CUpnpContentDirectoryDb(aMetadataStorage);
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryDb* CUpnpContentDirectoryDb::NewL(
    CUpnpMetadataStorage* aMetadataStorage )
    {
    CUpnpContentDirectoryDb* self = NewLC( aMetadataStorage );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::~~CUpnpContentDirectoryDb
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryDb::~CUpnpContentDirectoryDb()
    {
    iDOMImpl.Close( );
    LOGCD( "CUpnpContentDirectoryDb::~CUpnpContentDirectoryDb", "", 0,
        "DataBase closed" );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::InsertObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::InsertObjectL(
    RXmlEngDocument& aFragment, TInt aContainerId, TInt* aId )
    {
    TUpnpErrorCode ret( EUpnpUndefined );
    // get object element
    TXmlEngElement element = UpnpCdUtils::GetObjectElementL( aFragment );

    // start transaction
    iMetadataStorage->BeginTransactionL( );

    TRAPD( err, ret = DoInsertObjectL( element, aContainerId, aId ) );
    if ( err )
        { // rollback if error
        iMetadataStorage->RollbackTransactionL( );
        User::Leave( err );
        }
    iMetadataStorage->CommitTransactionL( );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::GetObjectListL( TInt aParentId,
    RArray<TInt>& aList )
    {
    return iMetadataStorage->GetObjectListL( aParentId, aList );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::GetObjectListL( const RArray<TInt>& aIds,
    const TDesC8& aFilter, RArray<RXmlEngDocument>& aObjs )
    {
    for ( TInt i = 0; i < aIds.Count( ); i++ )
        {
        RXmlEngDocument obj;
        CleanupClosePushL( obj );
        GetObjectL( aIds[i], obj, aFilter );
        aObjs.AppendL( obj );
        CleanupStack::Pop( &obj );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DeleteObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::DeleteObjectL( TInt aObjId,
    RArray<TInt>& aRefList, TBool aDeleteResource )
    {
    return iMetadataStorage->DeleteObjectL( aObjId, aRefList, aDeleteResource );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::UpdateObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::UpdateObjectL( TInt aObjId,
    RXmlEngDocument& aFragment )
    {
    TUpnpErrorCode ret = EUpnpUndefined;

    // // get object element
    TXmlEngElement element = UpnpCdUtils::GetObjectElementL( aFragment );

    // start transaction
    iMetadataStorage->BeginTransactionL( );

    TRAPD( err, ret = DoUpdateObjectL( aObjId, element ) );
    if ( err )
        { // rollback - error
        iMetadataStorage->RollbackTransactionL( );
        User::Leave( err );
        }
    // commit if success
    iMetadataStorage->CommitTransactionL( );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DoUpdateObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::DoUpdateObjectL( TInt aObjId,
    TXmlEngElement aFragment )
    {
    TUpnpErrorCode ret = EUpnpUndefined;

    // delete object
    ret = iMetadataStorage->DoDeleteObjectL( aObjId, EFalse, EFalse );
    TXmlEngElement el = aFragment;
    ret = InsertUpdatedL( aObjId, el );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::InsertUpdatedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::InsertUpdatedL( TInt aObjId,
    TXmlEngElement& aElement )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    iMetadataStorage->InsertObjectIntoObjectTableL( aElement );
    ret = EUpnpOk;
    RXmlEngNodeList<TXmlEngElement> elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    while ( elements.HasNext( ) )
        {
        TXmlEngElement el = elements.Next( );
        if ( el.Name().CompareF( KClassTagName ) && el.Name().CompareF( KTitleTagName ) )
            {
            ret = InsertElementL( el, aObjId );
            }
        else
            {
            UpnpCdUtils::IsElementRequiredL( el ); // just remove the marker attr
            }
        }
    CleanupStack::PopAndDestroy( &elements );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::InsertElementL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::InsertElementL(
    const TXmlEngElement& aElement, TInt aObjId )
    {
    return InsertAttributesL( aElement, iMetadataStorage->InsertElementL(
        aElement, aObjId ), aObjId );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::PrepareDidlDocumentL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RXmlEngDocument CUpnpContentDirectoryDb::PrepareDidlDocumentL()
    {
    RXmlEngDocument doc;
    doc.OpenL( iDOMImpl );
    TXmlEngElement el = doc.CreateDocumentElementL( KDidlLite( ) );
    el.AddNamespaceDeclarationL( KXmlnsNsUri( ), KCmlnsNsPref( ) );
    el.AddNamespaceDeclarationL( KDcNsUri( ), KDcNsPref( ) );
    el.AddNamespaceDeclarationL( KUpnpNsUri( ), KUpnpNsPref( ) );
    return doc;

    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::CheckObjectRestrictionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::CheckObjectRestrictionL( TInt aObjId )
    {
    return iMetadataStorage->CheckObjectRestrictionL( aObjId );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUpnpErrorCode CUpnpContentDirectoryDb::GetObjectL( TInt aObjectID,
    RXmlEngDocument& aDoc, const TDesC8& aFilterString )
    {
    TUpnpErrorCode ret = EUpnpUndefined;

    if ( aFilterString == KAsterisk8 )
        { // get the whole object
        ret = DoGetObjectL( aObjectID, aDoc, ETrue );
        }
    else if ( !aFilterString.Length( ) )
        { // get only required properties
        ret = DoGetObjectL( aObjectID, aDoc, EFalse );
        }
    else
        {
        RXmlEngDocument doc; // before the whole obj is created do not change the given reference,
        CleanupClosePushL( doc ); // it prevents from returning not completed object if something goes wrong 
        // during construction of response object
        // get required properties
        ret = DoGetObjectL( aObjectID, doc, EFalse );
        if ( ret == EUpnpOk ) // do not not continue if something went wrong
            {
            // prepare a list of filter elements
            RPointerArray<CUpnpFilterElement> filterElements;
            TCleanupItem cleanupItem( CleanupFilterElementPointerArray,
                &filterElements );
            CleanupStack::PushL( cleanupItem );
            // parse filter string
            ParseFilterStringL( aFilterString, filterElements );
            // add requested elements
            AddRequestedPropertiesL( aObjectID,
                UpnpCdUtils::GetObjectElementL( doc ), filterElements ); // ignore if error
            //clean up
            CleanupStack::PopAndDestroy( &filterElements );
            }
        if ( ret == EUpnpOk )
            { // object is completed
            aDoc = doc;
            CleanupStack::Pop( &doc );
            }
        else
            { // something went wrong
            CleanupStack::PopAndDestroy( &doc );
            }

        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUpnpErrorCode CUpnpContentDirectoryDb::AddRequestedPropertiesL(
    TInt aObjectID, TXmlEngElement aElement,
    const RPointerArray<CUpnpFilterElement>& aFilterElements )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    // aFilterElements must be sorted
    for ( TInt i = 0; i < aFilterElements.Count( ); i++ )
        {
        const CUpnpFilterElement* filter = aFilterElements[i];
        if ( filter->ElementName().Length( ) )
            { // element part defined
            AddElementL( aElement, aObjectID, filter );
            }
        else
            { // element part not defined - it is attribute of main element tag
            for ( TInt j = 0; j < filter->Count( ); j++ )
                {
                if ( (*filter)[j] == KChildCount )
                    {
                    SetChildCountL( aElement, aObjectID );
                    ret = EUpnpOk;
                    }
                else
                    {
                    ret = iMetadataStorage->AddMainTagAttrL( aElement,
                        aObjectID, (*filter)[j] );
                    }
                }
            }
        }
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::SetChildCountL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpContentDirectoryDb::SetChildCountL( TXmlEngElement aElement,
    TInt aId )
    {
    if ( aElement.Name( ) == KContainer )
        {
        // value
        TInt childCount = iMetadataStorage->GetChildCountL( aId );
        TBuf8<KMaxIntegerLen> num;
        num.Num( childCount );

        // is it already exist?
        TXmlEngAttr childAttr = aElement.AttributeNodeL( KChildCount( ) );
        if ( childAttr.IsNull( ) )
            { // not exist - add 
            aElement.AddNewAttributeL( KChildCount( ), num );
            }
        else
            { // already exists - update
            childAttr.SetValueL( num );
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::AddElementL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CUpnpContentDirectoryDb::AddElementL( TXmlEngElement aElement,
    TInt aObjectID, const CUpnpFilterElement* aReqElement )
    {
    // get elements from database
    RDbView view = iMetadataStorage->GetElementViewL( aObjectID,
        aReqElement->ElementName( ) );
    CleanupClosePushL( view );

    // for each element found
    while ( view.NextL( ) )
        {
        view.GetL( );
        CUpnpElementBean* elBean = CUpnpElementBean::NewLC( view );

        // add element
        TXmlEngElement newElement = elBean->AttachElmL( aElement );

        // add required attributes
        AddAttributesL( elBean->ElmId( ), aObjectID, newElement, EFalse );

        // for each requested attribute
        for ( TInt i = 0; i < aReqElement->Count( ); i++ )
            {
            TPtrC8 attrNamePtr((*aReqElement)[i]);

            TXmlEngAttr attr = newElement.AttributeNodeL( attrNamePtr );
            // add if not present
            // ignore if element has not any attribute
            if ( attr.IsNull( ) && elBean->ElmHasAttribute( ) )
                {
                AddAttributeL( newElement, attrNamePtr, elBean->ElmId( ),
                    aObjectID );
                }
            }
        // clean up
        CleanupStack::PopAndDestroy( elBean );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::ParseFilterStringL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::ParseFilterStringL( const TDesC8& aFilter,
    RPointerArray<CUpnpFilterElement>& aList )
    {
    // separate filter's parts
    CDesC8ArrayFlat* list = new(ELeave) CDesC8ArrayFlat(KDefaultGranularity);
    CleanupStack::PushL( list );
    TInt commaPos;
    TPtrC8 filter(aFilter);
    while ( (commaPos = filter.Find( KCommaStr8( ) ) ) != KErrNotFound )
        {
        HBufC8* buf = filter.Left(commaPos).AllocLC( );
        buf->Des().TrimAll( );
        list->AppendL( *buf );
        filter.Set( filter.Mid( commaPos+1 ) );
        CleanupStack::PopAndDestroy( buf );
        }
    // add last
    HBufC8* buf = filter.AllocLC( );
    buf->Des().TrimAll( );
    list->AppendL( *buf );
    filter.Set( filter.Mid( commaPos+1 ) );
    CleanupStack::PopAndDestroy( buf );

    // sort to simplify
    list->Sort( );

    TPtrC8 curr( KMinusOne8 );
    for ( TInt i = 0; i< list->Count( ); i++ ) // for each filter property
        {
        // split filter string - element@attribute
        TInt atPos = (*list)[i].Find( KAtStr8 );
        TPtrC8 attr;
        if ( atPos == KErrNotFound ) // is it element name
            { // it is element
            attr.Set( KNullDesC8 );
            atPos = (*list)[i].Length( );
            }
        else
            {
            attr.Set( (*list)[i].Mid( atPos+1 ) );
            }
        TPtrC8 element((*list)[i].Left( atPos ) );

        // do not add "class" and "title" elements - required by default
        if ( element.CompareF( KClassTagNameWithNs8 )
                && element.CompareF( KTitleTagNameWithNs8 ) )
            { // it is neither a class nor title element
            if ( element == curr )
                { // already exists, add next attribute
                if ( attr.Length( ) ) // ignore empty attr
                    aList[aList.Count()-1]->AppendAttributeL( attr );
                }
            else
                { // create new filter element
                CUpnpFilterElement* tmp = CUpnpFilterElement::NewLC( );
                tmp->SetElementNameL( element );
                if ( attr.Length( ) ) // ignore epmty attr
                    tmp->AppendAttributeL( attr );
                aList.AppendL( tmp );
                CleanupStack::Pop( tmp );
                curr.Set( element );
                }
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( list );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DoGetObjectL
// Function returns DIDL-Lite XML fragment with metadata for requested object.
// IMPORTANT: 1. Caller takes responsibility for returned document and is 
//              obliged to close it.
//           2. The root element of the document is <DIDL-Lite> element and
//              the object's element is its direct, onluy child.
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::DoGetObjectL( TInt aObjectID,
    RXmlEngDocument& aDoc, TBool aAll )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    RDbView view = iMetadataStorage->GetObjViewL( aObjectID );
    CleanupClosePushL( view );

    // Get result
    if ( view.FirstL( ) )
        {
        // get row
        view.GetL( );

        // prepare DIDL-Lite
        RXmlEngDocument doc;
        CleanupClosePushL( doc );
        doc = PrepareDidlDocumentL( );

        // object bean      
        CUpnpObjectBean* objBean = CUpnpObjectBean::NewLC( view );
        TXmlEngElement objElement;

        if ( aAll )
            { // all object's properties
            objElement
                    = objBean->AttachWholeObjectElL( doc.DocumentElement( ) );
            // childCount
            SetChildCountL( UpnpCdUtils::GetObjectElementL( doc ),
                objBean->ObjId( ) );
            }
        else
            { // only required
            objElement = objBean->AttachObjectElL( doc.DocumentElement( ) );
            }

        // clean up
        CleanupStack::PopAndDestroy( objBean );

        // aObjectID might be a reference object, so trying to get real object id        
        TInt realObjectId = ReferedObjectIdL( aObjectID );

        // add elements to the tree
        AddElementsL( realObjectId, objElement, aAll );

        // assign to the given reference
        aDoc = doc;
        // everything went right
        ret = EUpnpOk;
        // do not remove doc
        CleanupStack::Pop( &doc );
        }
    else
        { // the is no such object
        ret = ENoSuchObject;
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::AddElementsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::AddElementsL( TInt aObjectID,
    TXmlEngElement aObjectElement, TBool aAll )
    {
    RDbView view = iMetadataStorage->GetElementsViewL( aObjectID, aAll );
    CleanupClosePushL( view );
    // Iterate through elements
    while ( view.NextL( ) )
        {
        view.GetL( );
        CUpnpElementBean* elBean = CUpnpElementBean::NewLC( view );

        // add new element
        TXmlEngElement newElement = elBean->AttachElmL( aObjectElement );

        // add attributes
        AddAttributesL( elBean->ElmId( ), aObjectID, newElement, aAll );

        // clean up
        CleanupStack::PopAndDestroy( elBean );
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::AddAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CUpnpContentDirectoryDb::AddAttributesL( TInt aElementId,
    TInt aObjectID, TXmlEngElement& aElement, TBool aAll )
    {
    RDbView view = iMetadataStorage->GetAttributesViewByObjectIdL( aObjectID,
        aAll );
    CleanupClosePushL( view );

    CDbColSet* colSet = view.ColSetL( );
    const TInt elmIdColNo = colSet->ColNo( KAtrElmIdColName );
    delete colSet;

    // Iterate through attributes
    while ( view.NextL( ) )
        {
        view.GetL( );

        if ( view.ColInt( elmIdColNo ) == aElementId )
            {
            CUpnpAttributeBean* atr = CUpnpAttributeBean::NewLC( view );
            atr->AttachAttrL( aElement );
            CleanupStack::PopAndDestroy( atr );
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &view );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DoInsertObjectL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::DoInsertObjectL(
    TXmlEngElement& aElement, TInt aContainerId, TInt* aId )
    {
    TUpnpErrorCode ret = EUpnpUndefined;

    *aId = SetObjectIdL( aElement );
    UpnpCdUtils::SetContainerIdL( aElement, aContainerId );
    iMetadataStorage->InsertObjectIntoObjectTableL( aElement );
    ret = EUpnpOk;
    RXmlEngNodeList<TXmlEngElement> elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    while ( elements.HasNext( ) )
        {
        TXmlEngElement el = elements.Next( );
        if ( el.Name().CompareF( KClassTagName ) && el.Name().CompareF( KTitleTagName ) )
            {
            ret = InsertElementL( el, *aId );
            }
        else
            {
            UpnpCdUtils::IsElementRequiredL( el ); // just remove the marker attr
            }
        }
    // set object childCount
    SetChildCountL( aElement, *aId );

    CleanupStack::PopAndDestroy( &elements );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::InsertAttributesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::InsertAttributesL(
    const TXmlEngElement aElement, TInt aElmId, TInt aObjId )
    {
    TUpnpErrorCode ret = EUpnpUndefined;
    RXmlEngNodeList<TXmlEngAttr> list;
    CleanupClosePushL( list );
    aElement.GetAttributes( list );
    if ( list.Count( ) )
        {
        RArray<TXmlEngAttr> attributes; // to store real attr
        CleanupClosePushL( attributes );
        RArray<TXmlEngAttr> markers; // to store marker attr
        CleanupClosePushL( markers );
        // separate markers from attr
        while ( list.HasNext( ) )
            {
            TXmlEngAttr attr = list.Next( );
            if ( attr.Name().Match( KRequiredAtrSufPattern ) == KErrNotFound )
                {
                attributes.AppendL( attr );
                }
            else
                {
                markers.AppendL( attr );
                }
            }

        // insert attributes
        for ( TInt i = 0; i < attributes.Count( ); i++ )
            {
            iMetadataStorage->InsertAttributeL( attributes[i], aElmId,
                IsAttrRequiredL( attributes[i], markers ), aObjId );
            ret = EUpnpOk;
            }
        // remove markers from DOM tree
        for ( TInt i = 0; i < markers.Count( ); i++ )
            {
            markers[i].Remove( );
            }
        // clean up
        CleanupStack::PopAndDestroy( &markers );
        CleanupStack::PopAndDestroy( &attributes );
        }
    else
        {
        ret = EUpnpOk;
        }

    CleanupStack::PopAndDestroy( &list );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::IsAttrRequiredL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectoryDb::IsAttrRequiredL( const TXmlEngAttr& aAttr,
    RArray<TXmlEngAttr>& aMarkers )
    {
    TBool ret = EFalse;

    for ( TInt i = 0; i< aMarkers.Count( ); i++ )
        {
        if ( aMarkers[i].Name().Find( aAttr.Name( ) ) == 0 )
            { // attr required
            ret = ETrue;
            break;
            }
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::SetObjectIdL
// Sets the id val in the objected to be created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::SetObjectIdL( TXmlEngElement& aElement )
    {
    TInt ret = iMetadataStorage->GetNextKeyL( KObjectTableName8 );
    UpnpCdUtils::SetObjectIdL( aElement, ret );
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectIdL
// Gets the id val in the objected to be created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetObjectIdL()
    {
    TInt ret = iMetadataStorage->NextKeyL( KObjectTableName8 );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectIdL
// Gets the id val in the objected to be created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectoryDb::GetObjectTitleL( TInt aId )
    {
    return iMetadataStorage->GetObjectTitleL( aId );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::ReferedObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::ReferedObjectIdL( TInt aObjectId )
    {
    TInt ret = KErrNotFound;

    TInt refId = iMetadataStorage->ReferedObjectIdL( aObjectId );

    if ( refId != KErrNotFound )
        {
        ret = ReferedObjectIdL( refId );
        }
    else
        {
        ret = aObjectId;
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetElIdByAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetObjIdByAttrL( const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    TInt objId = KErrNotFound;

    TInt elmId = iMetadataStorage->GetElmIdForAttrL( aAttrName, aAttrVal );
    if ( elmId != KErrNotFound )
        {
        objId = iMetadataStorage->GetObjIdForElmIdL( elmId );
        }

    return objId;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjIdByAttrLikeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetObjIdByAttrLikeL( const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    TInt objId = KErrNotFound;

    TInt elmId = iMetadataStorage->GetElmIdForAttrLikeL( aAttrName, aAttrVal );
    if ( elmId != KErrNotFound )
        {
        objId = iMetadataStorage->GetObjIdForElmIdL( elmId );
        }

    return objId;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectByAttrL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::GetObjectByAttrL(
    RXmlEngDocument& aDoc, TInt* aObjId, const TDesC8& aAttrName,
    const TDesC8& aAttrVal )
    {
    TUpnpErrorCode ret = EUndefined;

    *aObjId = GetObjIdByAttrL( aAttrName, aAttrVal );
    if ( *aObjId >= 0 )
        {
        ret = GetObjectL( *aObjId, aDoc, KAsterisk8 );
        }
    else
        { // something went wrong
        ret = ENoSuchObject;
        }
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::CheckParentRestrictionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectoryDb::CheckParentRestrictionL( TInt aObjId )
    {
    return iMetadataStorage->CheckParentRestrictionL( aObjId );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DeleteResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::DeleteResourceL( const TDesC8& aResVal,
    RArray<TInt>& aContIds )
    {
    // start transaction
    iMetadataStorage->BeginTransactionL( );

    TRAPD( err, DoDeleteResourceL( aResVal, aContIds ) );
    if ( err != KErrNone )
        { // rollback - error
        iMetadataStorage->RollbackTransactionL( );
        User::Leave( err );
        }
    // commit if success
    iMetadataStorage->CommitTransactionL( );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DoDeleteResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::DoDeleteResourceL( const TDesC8& aResVal,
    RArray<TInt>& aContIds )
    {
    HBufC* resVal = HBufC::NewLC( aResVal.Length( ) );
    resVal->Des().Copy( aResVal );

    HBufC* resValReplaced16 = UpnpCdUtils::EscapeAposL( *resVal );
    CleanupStack::PushL( resValReplaced16 );
    HBufC8* resValReplaced8 = UpnpCdUtils::DesToDes8LC( *resValReplaced16 );
    RArray<TInt> objs;
    CleanupClosePushL( objs );
    GetObjectsByResourceL( objs, *resValReplaced16 );

    // not any obj?
    if ( !objs.Count( ) )
        {
        User::Leave( ENoSourceResource );
        }

    TUpnpErrorCode err = EUpnpOk;

    // for each object
    for ( TInt i = 0; i < objs.Count( ); i++ )
        {
        CUpnpObjectBean* objBn = GetObjBeanLC( objs[i] );
        if ( !objBn->ObjRestricted( ) )
            {
            if ( CheckObjectRestrictionL( objBn->ObjParentId( ) ) == EUpnpOk )
                {
                iMetadataStorage->DeleteResElL( *resValReplaced16,
                    objBn->ObjId( ) );
                aContIds.AppendL( objBn->ObjParentId( ) );
                }
            else
                {
                err = ERestrictedParentObject;
                }
            }
        else
            {
            err = ERestrictedObject;
            }
        CleanupStack::PopAndDestroy( objBn );
        }

    // was any deleted
    if ( !aContIds.Count( ) )
        {
        User::Leave( err );
        }

    // if each deleted?
    if ( aContIds.Count( ) == objs.Count( ) )
        {
        // try to fetch resource id
        TRAPD( err, UpnpCdUtils::ResIdFromUriL( *resValReplaced8 ) );
        // ignore if error - resource id could not be fetched
        if ( !err )
            { // OK
            // delete resource
            DeleteResourceByResIdL( UpnpCdUtils::ResIdFromUriL( *resValReplaced8 ) );
            }
        }

    CleanupStack::PopAndDestroy( &objs );
    CleanupStack::PopAndDestroy( resValReplaced8 );
    CleanupStack::PopAndDestroy( resValReplaced16 );
    CleanupStack::PopAndDestroy( resVal );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjBeanLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpObjectBean* CUpnpContentDirectoryDb::GetObjBeanLC( TInt aObjId )
    {
    CUpnpObjectBean* objBn = 0;

    RDbView view = iMetadataStorage->GetObjViewL( aObjId );
    CleanupClosePushL( view );

    if ( view.FirstL( ) )
        {
        // get values
        view.GetL( );
        objBn = CUpnpObjectBean::NewL( view );
        }

    // clean up 
    CleanupStack::PopAndDestroy( &view );

    CleanupStack::PushL( objBn );
    return objBn;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectIdsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::GetObjectsByResourceL( RArray<TInt>& aIds,
    const TDesC& aResVal )
    {

    RDbView view = iMetadataStorage->GetViewOfObjectListForResL( aResVal );
    CleanupClosePushL( view );

    CDbColSet* colSet = view.ColSetL( );
    const TInt colNo = colSet->ColNo( KElmObjIdColName );
    delete colSet;

    // for each row
    while ( view.NextL( ) )
        {
        view.GetL( );
        TInt no = view.ColInt( colNo );
        aIds.AppendL( no );
        }

    // clean up
    CleanupStack::PopAndDestroy( &view );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetProtocolInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetProtocolInfoL( const TDesC8& aContentUri,
    CUpnpDlnaProtocolInfo*& aProtocolInfo )
    {
    HBufC* uri = HBufC::NewLC( aContentUri.Length( ) );
    TPtr uriPtr(uri->Des( ));
    uriPtr.Copy( aContentUri );
    TInt ret = GetElmIdByNameAndValL( KRes16, uriPtr );
    if ( ret >= 0 )
        {
        HBufC8* attrVal;
        GetAttrValueByNameL( ret, KprotocolInfo, attrVal );
        CleanupStack::PushL( attrVal );
        aProtocolInfo = CUpnpDlnaProtocolInfo::NewL( *attrVal );
        CleanupStack::PopAndDestroy( attrVal );
        }
    CleanupStack::PopAndDestroy( uri );

    if ( ret >= 0 ) //item ID number is't meaningfull for function caller
        {
        ret = KErrNone;
        }
    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetElmIdByNameAndValL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetElmIdByNameAndValL( const TDesC& aElmName,
    const TDesC& aElmValue )
    {
    TInt ret = KErrNotFound;

    RDbView view = iMetadataStorage->GetViewOfElmIdByNameAndValL( aElmName,
        aElmValue );
    CleanupClosePushL( view );

    // col id
    CDbColSet* colSet = view.ColSetL( );
    const TInt colNo = colSet->ColNo( KElmIdColName );
    delete colSet;

    while ( view.NextL( ) )
        {
        view.GetL( );
        ret = view.ColInt( colNo );
        }

    // clean up
    CleanupStack::PopAndDestroy( &view );

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetAttrValueByNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

HBufC8* CUpnpContentDirectoryDb::GetProtocolInfoByImportUriL(
    const TDesC8& aImportUri )
    {
    CUpnpAttributeBean* attBean =
            GetAttrByValueL( KImportUri8( ), aImportUri );
    CleanupStack::PushL( attBean );

    HBufC8* attValBuf = NULL;
    if ( attBean )
        {
        //KprotocolInfo
        CUpnpAttributeBean* att = GetAttrByNameL( attBean->AtrElmId( ),
            KprotocolInfo( ), attValBuf );
        delete att;
        }
    CleanupStack::PopAndDestroy( attBean );
    return attValBuf;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetAttrValueByNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::GetAttrValueByNameL( TInt aElmId,
    const TDesC8& aAttrName, HBufC8*& aAttrVal )
    {
    CUpnpAttributeBean* atrBean =
            GetAttrByNameL( aElmId, aAttrName, aAttrVal );
    delete atrBean;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetAttrByValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean* CUpnpContentDirectoryDb::GetAttrByValueL(
    const TDesC8& aAttrName, const TDesC8& aAttrVal )
    {
    RDbView view = iMetadataStorage->GetAttrViewL( aAttrName, aAttrVal );
    CleanupClosePushL( view );

    TInt number = view.CountL( );

    CDbColSet* colSet = view.ColSetL( );
    const TInt colNo = colSet->ColNo( KAtrValueColName );
    delete colSet;

    CUpnpAttributeBean* atrBean = NULL;
    if ( view.FirstL( ) )
        {
        view.GetL( );
        atrBean = CUpnpAttributeBean::NewLC( view );
        CleanupStack::Pop( atrBean );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    // clean up
    CleanupStack::PopAndDestroy( &view );
    return atrBean;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetAttrByNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpAttributeBean* CUpnpContentDirectoryDb::GetAttrByNameL( TInt aElmId,
    const TDesC8& aAttrName, HBufC8*& aAttrVal )
    {
    RDbView view = iMetadataStorage->GetAttrViewL( aElmId, aAttrName );
    CleanupClosePushL( view );

    CDbColSet* colSet = view.ColSetL( );
    const TInt colNo = colSet->ColNo( KAtrValueColName );
    delete colSet;

    CUpnpAttributeBean* atrBean = NULL;
    if ( view.FirstL( ) )
        {
        view.GetL( );
        atrBean = CUpnpAttributeBean::NewLC( view );
        aAttrVal = atrBean->AtrValue().AllocL( );
        CleanupStack::Pop( atrBean );
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    // clean up
    CleanupStack::PopAndDestroy( &view );
    return atrBean;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetObjectIdByElementValueLikeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetObjectIdByElementValueLikeL(
    const TDesC8& aElmName, const TDesC8& aElmValue )
    {
    // convert to unicode
    HBufC* elmName = UpnpCdUtils::Des8ToDesLC( aElmName );
    HBufC* elmValue = UpnpCdUtils::Des8ToDesLC( aElmValue );

    // prepare query buf
    HBufC* query = HBufC::NewLC( KSelectFromSqlCmd().Length( ) + KElmObjIdColName().Length( ) + KElementTableName().Length( ) + KElmNameColName().Length( ) + elmName->Length( ) + KAndLikeConditionSqlCmd().Length( ) + KElmValueColName().Length( ) + elmValue->Length( ) );

    TPtr queryPtr(query->Des( ));

    // format query
    queryPtr.Format( KSelectFromSqlCmd, &KElmObjIdColName,
        &KElementTableName, &KElmNameColName, elmName );
    queryPtr.AppendFormat( KAndLikeConditionSqlCmd, &KElmValueColName,
        elmValue );

    // prepare
    RDbView view = iMetadataStorage->GetEvaluatedViewL( queryPtr );
    CleanupClosePushL( view );

    // col no
    CDbColSet* colSet = view.ColSetL( );
    const TInt colNo = colSet->ColNo( KElmObjIdColName );
    delete colSet;

    TInt ret = KErrNotFound;

    // for one row
    if ( view.NextL( ) )
        {
        view.GetL( );
        if ( !view.IsColNull( colNo ) )
            {
            ret = view.ColInt( colNo );
            }
        }

    CleanupStack::PopAndDestroy( &view );
    CleanupStack::PopAndDestroy( query );

    CleanupStack::PopAndDestroy( elmValue );
    CleanupStack::PopAndDestroy( elmName );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetParentIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetParentIdL( TInt aObjectId )
    {
    return iMetadataStorage->GetParentIdL( aObjectId );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::AddAttributeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

void CUpnpContentDirectoryDb::AddAttributeL( TXmlEngElement aElement,
    const TDesC8& aAttrName, TInt aElementId, TInt aObjectId )
    {
    RDbView view = iMetadataStorage->GetAttrViewByObjectIdL( aObjectId,
        aAttrName );
    CleanupClosePushL( view );

    CDbColSet* colSet = view.ColSetL( );
    const TInt elmIdColNo = colSet->ColNo( KAtrElmIdColName );
    delete colSet;

    // Iterate through attributes - usually there is only one
    while ( view.NextL( ) )
        {
        view.GetL( );

        if ( view.ColInt( elmIdColNo ) == aElementId )
            {
            CUpnpAttributeBean* atr = CUpnpAttributeBean::NewLC( view );
            atr->AttachAttrL( aElement );
            CleanupStack::PopAndDestroy( atr );
            }
        }

    // clean up
    CleanupStack::PopAndDestroy( &view );
    }
// -----------------------------------------------------------------------------
// CContentDirectoryDb::CanBeNestedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectoryDb::CanBeNestedL( TInt aObjId )
    {
    // get path
    HBufC8* path = iMetadataStorage->GetObjectPathL( aObjId );
    CleanupStack::PushL( path );

    // count levels
    TPtrC8 rest(path->Des( ));
    TInt levelNo(1); // root level
    TInt pos( KErrNotFound );
    while ( (pos = rest.Find( KPathSeparator8 )) != KErrNotFound )
        {
        rest.Set( rest.Mid( pos + KPathSeparator8().Length( ) ) );
        levelNo++;
        }
    // clean up
    CleanupStack::PopAndDestroy( path );
    return (levelNo < KAvailableNestedLevels);
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::AddResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::AddResourceL( CUpnpResourcesBean* aResBean,
    TInt aObjId )
    {
    iMetadataStorage->AddResourceL( aResBean, aObjId );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DeleteResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::DeleteResourceByResIdL( TInt64 aResId,
    TBool aDeleteFile )
    {
    iMetadataStorage->DeleteResourceByResIdL( aResId, aDeleteFile );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::DeleteResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::DeleteResourceByObjIdL( TInt aObjId,
    TBool aDeleteFile )
    {
    iMetadataStorage->DeleteResourceByObjIdL( aObjId, aDeleteFile );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetResourceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpResourcesBean* CUpnpContentDirectoryDb::GetResourceL( TInt64 aResId )
    {
    return iMetadataStorage->GetResourceL( aResId );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::ResNewDownloadDirectoryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::NewMediaDirectoryL( const TDesC& aOldDir )
    {
    //UPDATE resources SET rsc_readonly=1 WHERE KRscPathColName LIKE $aOldDir* AND KRscReadonlyColName=0
    // prepare command
    HBufC* query = HBufC::NewLC( KNewMediaDirSqlCmd().Length( ) + aOldDir.Length( ) );
    TPtr SqlCmd(query->Des( ));
    SqlCmd.Format( KNewMediaDirSqlCmd, &aOldDir );

    // execute
    iMetadataStorage->ExecuteL( SqlCmd );

    CleanupStack::PopAndDestroy( query );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::GetKeyForUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::GetKeyForUriL()
    {
    return iMetadataStorage->GetNextKeyL( KImportUriId8 );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::CheckDatabaseL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryDb::CheckDatabaseL()
    {
    iMetadataStorage->CheckDatabaseL( );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::RecreateDatabaseFile
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::RecreateDatabaseFile()
    {
    TRAPD( err, iMetadataStorage->RecreateDatabaseFileL( ) );
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::HandleDbError
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectoryDb::HandleDbError( TInt aError )
    {
    return iMetadataStorage->HandleDbError( aError );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::IsDbCreated
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectoryDb::IsDbCreated()
    {
    return iMetadataStorage->IsDbCreated( );
    }

//  End of File  
