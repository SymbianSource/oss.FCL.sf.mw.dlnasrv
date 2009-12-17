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
* Description:      XML SAX Parser for UPnP.
*
*/






// INCLUDE FILES
// xml parser api
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>
#include <xml/xmlparsererrors.h>

// upnp stack api
#include "upnpstring.h"
#include <upnpcontainer.h>
#include <upnpitem.h>
#include "upnpelement.h"
#include "upnpxmlparser.h"

// upnpframework / internal api's
#include "upnpcdsreselementutility.h"

// xmlparser internal
#include "upnpobjectstack.h"
#include "upnpitemtoxml.h"
#include "upnpxmlstringutility.h"
#include "upnpxmlparser.h"

_LIT( KComponentLogfile, "upnpxmlparser.txt");
#include "upnplog.h"


_LIT8( KXmlMimeType,    "text/xml"     );
_LIT8( KLIB2XML,        "libxml2" );

_LIT8( KContainer,      "container"    );
_LIT8( KItem,           "item"         );

_LIT8( KTitle,          "title"        );

_LIT8( KClass,          "class"        );
_LIT8( KContentURI,     "res"          );
_LIT8( KId,             "id"           );
_LIT8( KParentId,       "parentId"     );
_LIT8( KRestricted,     "restricted"   );
_LIT8( KComma,          ":"            );
_LIT8( KTrue,           "1"            );
_LIT8( KDIDL,           "DIDL-Lite"    );
_LIT8( KDesc,           "desc"         );
_LIT8( KDlnaDoc,        "X_DLNADOC"    );
_LIT8( KSpace,          " "            );
_LIT8( KImportUri,      "importUri"    );

// --------------------------------------------------------------------------
// CUPnPXMLParser::CUPnPXMLParser()
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLParser::CUPnPXMLParser()
    {
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::ConstructL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::ConstructL()
    {
    __LOG( "CUPnPXMLParser::CostructL" );
    iStack = CUPnPObjectStack::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::NewL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPXMLParser* CUPnPXMLParser::NewL()
    {
    CUPnPXMLParser* self = CUPnPXMLParser::NewLC();
    CleanupStack::Pop( self );    
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPXMLParser::NewLC
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPXMLParser* CUPnPXMLParser::NewLC()
    {   
    CUPnPXMLParser* self = new( ELeave ) CUPnPXMLParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// --------------------------------------------------------------------------
// CUPnPXMLParser::~CUPnPXMLParser
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLParser::~CUPnPXMLParser()
    {
    __LOG( "CUPnPXMLParser::~CUPnPXMLParser" );
    if ( iStack )
        {
        iStack->ResetAndDestroy();
        delete iStack;
        }
    
    delete iElementValue;
    delete iNewElement;
    
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::ResetMember
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::ResetMember()
    {
    iResultRoot->ResetAndDestroy();
    iResultRoot = NULL;
    iStack->ResetAndDestroy(); 
    delete iStack;
    iStack = NULL;

    delete iElementValue; iElementValue = NULL;
    delete iNewElement; iNewElement = NULL;
    iTitle = 0;
    iObjectClass = 0;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::ParseResultDataL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C void CUPnPXMLParser::ParseResultDataL( 
    RPointerArray<CUpnpObject>& aResultArray,
    const TDesC8& aData )
    {
    __LOG( "CUPnPXMLParser::ParseResultDataL, begin" );
    
    if ( !aData.Length() )
        {
        User::Leave( KErrArgument );
        }

    iResultRoot = &aResultArray;

    // Create parser 
    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL( KXmlMimeType ); 
    matchData->SetVariantL( KLIB2XML ); 
    CParser* parser = CParser::NewLC( *matchData, *this );
    parser->EnableFeature( Xml::EReportNamespaceMapping );
    
    TRAPD( err, Xml::ParseL( *parser,aData ) );
    // if the xml contained control character
    if( EXmlInvalidToken == err )
        {
        __LOG1 ( "CUPnPXMLParser::ParseResultDataL Can not Parse \
        		error code %d " ,err );
        HBufC8* FiltrateBuffer = 
            UpnpXmlStringUtility::RemoveXmlControlCharactersL( aData );
        //if remove some control characters.
        if( FiltrateBuffer )
            {
            CleanupStack::PushL( FiltrateBuffer );
            ResetMember();

            iStack = CUPnPObjectStack::NewL();
            iResultRoot = &aResultArray;

            Xml::ParseL( *parser, *FiltrateBuffer );
            CleanupStack::PopAndDestroy( FiltrateBuffer );
            }
        else
            {
            User::Leave( err );
            }
        }
    else if ( KErrNone != err )
        {
        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( matchData );
    
    iResultRoot = NULL;           
    __LOG( "CUPnPXMLParser::ParseResultDataL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::ItemAsXMLLC
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C HBufC8* CUPnPXMLParser::ItemAsXmlLC( const CUpnpItem& aItem )
    {
    CUpnpItemToXML* tmpXmlItem =  CUpnpItemToXML::NewLC( aItem );
    HBufC8* raw  = tmpXmlItem->AsXmlL();
    CleanupStack::PushL( raw );
    
    HBufC8* encoded = UpnpString::EncodeXmlStringL( raw );
    CleanupStack::PopAndDestroy( raw );
    CleanupStack::PushL( encoded );
    
    HBufC8* tagged = tmpXmlItem->CreateUnDecodedXmlL( *encoded );
    CleanupStack::PopAndDestroy( encoded );
    CleanupStack::PopAndDestroy( tmpXmlItem );
    CleanupStack::PushL( tagged );
    
    return tagged;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::XmlForCreateObjectLC
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C HBufC8* CUPnPXMLParser::XmlForCreateObjectLC(
    const CUpnpItem& aItem )
    {
    CUpnpItemToXML* tmpXmlItem = CUpnpItemToXML::NewLC( aItem );
    HBufC8* tempXmlDataBlock = tmpXmlItem->AsResultArgumentL();
    CleanupStack::PopAndDestroy( tmpXmlItem );
    CleanupStack::PushL( tempXmlDataBlock );
    
    return tempXmlDataBlock;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::ContainerToXmlLC
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C HBufC8* CUPnPXMLParser::ContainerToXmlLC(
    const CUpnpContainer& /*aContainer*/ )
    {
    // Not implemented
    User::Leave( KErrNotSupported );
    
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnStartDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnStartDocumentL( 
                                const RDocumentParameters& /*aDocParam*/, 
                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnEndDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnStartElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnStartElementL( const RTagInfo& aElement, 
                                      const RAttributeArray& aAttributes,
                                      TInt aErrorCode )
    {
    __LOG1( "CUPnPXMLParser::OnStartElementL, error code: %d", aErrorCode );
    if ( aErrorCode != KErrNone )
        {
        return;
        }
    const TDesC8& desName = aElement.LocalName().DesC();
    const TDesC8& prefix = aElement.Prefix().DesC();
    
    // Delete content, since there may be some stuff between the elements
    // (comments, whitespace etc.)
    delete iElementValue; iElementValue = NULL;    
      
    if ( !desName.CompareF( KContainer ) ) // Container element
        {
        CUpnpContainer* tmpContainer = CUpnpContainer::NewL();
        CleanupStack::PushL( tmpContainer );

        SetAttributesL( *tmpContainer, aAttributes );
        //push into the stack, ownership is transferred                      
        iStack->PushL( tmpContainer ); 
        
        CleanupStack::Pop( tmpContainer );
        }
    else if ( !desName.CompareF( KItem ) ) // Item element
        {
        CUpnpItem* tmpItem = CUpnpItem::NewL();
        CleanupStack::PushL( tmpItem );

        SetAttributesL( *tmpItem, aAttributes );                      
        //push into the stack, ownership is transferred                      
        iStack->PushL( tmpItem );
        
        CleanupStack::Pop( tmpItem );
        }
    else if( !desName.CompareF( KTitle ) ) // Title element
        {
        // check that we have item or container. cause leave if not
        if ( iStack->Count() == 0)
            {
            User::Leave( KErrArgument );
            }
        // We should have an item or a container already!
        //__ASSERTD( iStack->Top(), __FILE__, __LINE__ );
        iTitle = ETrue;
        }
    else if( !desName.CompareF( KClass ) ) // Object class element
        {
        // check that we have item or container. cause leave if not
        if ( iStack->Count() == 0)
            {
            User::Leave( KErrArgument );
            }
        // We should have an item or a container already!
       // __ASSERTD( iStack->Top(), __FILE__, __LINE__ );
        iObjectClass = ETrue;
        }
    // Ignore DIDL-Lite, desc and X_DLNADOC -elements (DLNA req)
    else if( desName.Compare( KDIDL ) == KErrNone ||
             desName.Compare( KDesc ) == KErrNone ||
             desName.Compare( KDlnaDoc ) == KErrNone    
           )
        {
        // Ignore
        }
    else 
        {
        // check that we have item or container. cause leave if not
        if ( iStack->Count() == 0)
            {
            User::Leave( KErrArgument );
            }
        
        // We should have an item or a container already!    
        //__ASSERTD( iStack->Top(), __FILE__, __LINE__ );
        
        if( prefix.Length() > 0 ) // If there is a namespace (upnp: etc)
            {
            HBufC8* name = HBufC8::NewLC( prefix.Length() +
            KComma().Length() + desName.Length() ) ;
            name->Des().Copy( prefix );
            name->Des().Append( KComma );
            name->Des().Append( desName );
            
            // Create a new element
            __ASSERTD( !iNewElement, __FILE__, __LINE__ );
            iNewElement = CUpnpElement::NewL( *name );
        
            CleanupStack::PopAndDestroy( name );
            }
        else
            {
            // Create a new element
            __ASSERTD( !iNewElement, __FILE__, __LINE__ );
            iNewElement = CUpnpElement::NewL( desName );
            }
        
        // Set attributes for element
        SetAttributesL( aAttributes );
        }
    __LOG( "CUPnPXMLParser::OnEndElementL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnEndElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnEndElementL( const RTagInfo& aElement, 
                                    TInt aErrorCode )
    {
    __LOG( "CUPnPXMLSAXParser::OnEndElementL(), begin" );
    if ( aErrorCode != KErrNone )
        {
        return;
        }
        
    const TDesC8& desName = aElement.LocalName().DesC();    
    if ( !desName.CompareF( KContainer ) || !desName.CompareF( KItem ) )
        {
        iResultRoot->AppendL( iStack->Top() );
        iStack->Pop(); // Remove object from stack.
        }
    else if( KErrNone != desName.CompareF( KDIDL ))
        {        
        CUpnpObject* obj = (CUpnpObject*)iStack->Top();
        __ASSERTD( obj, __FILE__, __LINE__ );
             
        if ( iElementValue )
            {
            SetValueToElementL( *obj, *iElementValue );
            }
        else
            {
            SetValueToElementL( *obj, KNullDesC8 );
            }    
        
        delete iElementValue; iElementValue = NULL;    
        }    
    __LOG( "CUPnPXMLSAXParser::OnEndElementL(), end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnContentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    __LOG( "CUPnPXMLSAXParser::OnContentL(), begin" );
    if ( !iStack->Count() || aErrorCode != KErrNone )
        {
        return;
        }
    
    if( !iElementValue ) //if 1st time
        {
        iElementValue = HBufC8::NewL(aBytes.Length());
        iElementValue->Des().Copy(aBytes); 
        }
    else
        {
        HBufC8* previousValue = iElementValue;
        iElementValue = HBufC8::NewL( previousValue->Des().Length() +
                                      aBytes.Length() );
        iElementValue->Des().Append( *previousValue );
        iElementValue->Des().Append( aBytes );
        delete previousValue;
        } 
    __LOG( "CUPnPXMLSAXParser::OnContentL(), end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnStartPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
                                               const RString& /*aUri*/, 
                                               TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnEndPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnEndPrefixMappingL( const RString& /*aPrefix*/, 
                                             TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnIgnorableWhiteSpaceL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
                                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnSkippedEntityL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnSkippedEntityL( const RString& /*aName*/, 
                                          TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnProcessingInstructionL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnProcessingInstructionL( const TDesC8& /*aTarget*/, 
                                                  const TDesC8& /*aData*/, 
                                                  TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::OnError
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::OnError( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::GetExtendedInterface
// See upnpxmlparser.h
// --------------------------------------------------------------------------
TAny* CUPnPXMLParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {    
    // No implementation needed
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParser::SetAttributesL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::SetAttributesL( CUpnpObject& aObject, 
    const RAttributeArray& aAttributes )
    {
    __LOG( "CUPnPXMLParser::SetAttributesL" );
    
    if ( iStack->Count() )
        {
        // Object has a parent.
        // Should not really happen, but implemented anyways
        __LOG( "Object has a parent!" );
        
        if ( iStack->Top()->ObjectType() != EUPnPContainer )
            {
            User::Leave( KErrArgument );
            }
        CUpnpContainer* container = (CUpnpContainer*)iStack->Top();
        container->AppendObjectL( aObject );
        }
        
    RAttribute attribute;
    TInt count = aAttributes.Count();
    for ( TInt i = 0 ; i < count; i++ ) // Read attributes.
        {
        attribute = aAttributes[ i ];
        const TDesC8& name = attribute.Attribute().LocalName().DesC();
        
        if( !name.CompareF( KId ) )
            {
            aObject.SetIdL( attribute.Value().DesC() );
            }
        if( !name.CompareF( KParentId ) )
            {
            aObject.SetParentIdL( attribute.Value().DesC() );
            }            
        if( !name.CompareF( KRestricted ) ) 
            {
            if ( !attribute.Value().DesC().CompareF( KTrue ) ) // restricted
                {
                aObject.SetRestricted( ETrue );
                }
            else // no restriction
                {
                aObject.SetRestricted( EFalse );
                }
            }
        }
    __LOG( "CUPnPXMLParser::SetAttributesL - End" );
    }
    
// --------------------------------------------------------------------------
// CUPnPXMLParser::SetValueToElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::SetValueToElementL( CUpnpObject& aItem,
    const TDesC8& aValue )
    {
    __LOG( "CUPnPXMLParser::SetValueToElementL()" );

    if( iTitle )
        {
        __LOG( "SetValueToElementL() - title element" );
        aItem.SetTitleL( aValue );
        iTitle = EFalse;
        }
    else if( iObjectClass )
        {
        __LOG( "SetValueToElementL() - objectclass element" );
        aItem.SetObjectClassL( aValue );  
        iObjectClass = EFalse;
        }
    // See if it's the res-element   
    else if( iNewElement && // NULL Check
             iNewElement->Name().Compare( KContentURI ) == KErrNone )
        {
        __LOG( "SetValueToElementL() - res element" ); 
                
        // Check that uri is absolute
        if( UpnpCdsResElementUtility::IsUriAbsolute( aValue ) )
            {
            __LOG( "valid res-element" );
            iNewElement->SetValueL( aValue );
            aItem.AddElementL( iNewElement ); // Ownership is transferred
            iNewElement = NULL;                
            }
        else
            { 
            // if CreateObjectResponse, there is importuri
            // no need to check res-element
            TInt found = EFalse;
            RUPnPAttributesArray elArray = iNewElement->GetAttributes();
            TInt count = elArray.Count(); 
            for( TInt i = 0; i < count; i++ )
                {
                if( elArray[ i ]->Name() == KImportUri )
                    {
                    i = count;
                    found = ETrue;
                    __LOG( "SetValueToElementL() - ImportUri found" );
                    }
                }
                              
            if( found )     
                {
                __LOG( "no res-element but import uri found" );
                iNewElement->SetValueL( KNullDesC8 );
                aItem.AddElementL( iNewElement ); // Ownership is transferred
                iNewElement = NULL;                   
                }  
            else
                {
                __LOG( "invalid res-element" );
                delete iNewElement;
                iNewElement = NULL;                
                }           
            }
  
        }
    else 
        {
        __LOG( "SetValueToElementL() - unknown element" );
        if( iNewElement )
            {
            // Check for empty values
            if( aValue == KNullDesC8 || aValue == KSpace )
                {
                // Do not add empty values
                delete iNewElement; iNewElement = NULL;                
                }
            else
                {
                iNewElement->SetValueL( aValue );
                aItem.AddElementL( iNewElement ); // Ownership is transferred
                iNewElement = NULL;                
                }    
            }
        }
        
    __LOG( "CUPnPXMLParser::SetValueToElementL() End" ); 
    }

    
// --------------------------------------------------------------------------
// CUPnPXMLParser::SetAttributesL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParser::SetAttributesL( const RAttributeArray& aAttributes )
    {
    __LOG( "CUPnPXMLParser::SetAttributesL" );
    
    RAttribute attribute;
    TInt count = aAttributes.Count();
    for ( TInt i = 0; i < count ; i++ )
        {
        attribute = aAttributes[i];
        const TDesC8& name = attribute.Attribute().LocalName().DesC();
        CUpnpAttribute* att = CUpnpAttribute::NewLC();
        att->SetNameL( name );
        att->SetValueL( attribute.Value().DesC() );
        __ASSERTD( iNewElement, __FILE__, __LINE__ );
        iNewElement->AddAttributeL( att );
        CleanupStack::Pop( att );     
        }
    __LOG( "CUPnPXMLParser::SetAttributesL End" );    
    }
    
// end of file

