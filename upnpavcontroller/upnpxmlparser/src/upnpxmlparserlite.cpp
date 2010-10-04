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
// xml parser
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>

// upnp stack api
#include <upnpstring.h>

// upnpframework / xmlparser api
#include "upnpxmlparserlite.h"
#include "upnpobjectlite.h"

// xmlparser internal
#include "upnpobjectstacklite.h"


_LIT( KComponentLogfile, "upnpxmlparser.txt");
#include "upnplog.h"

_LIT8( KXmlMimeType,    "text/xml"     );
_LIT8( KLIB2XML,        "libxml2" );

_LIT8( KContainer,      "container"    );
_LIT8( KItem,           "item"         );
_LIT8( KTitle,          "title"        );
_LIT8( KClass,          "class"        );
_LIT8( KId,             "id"           );
//_LIT8( KComma,          ":"            );
_LIT8( KDIDL,           "DIDL-Lite"    );
_LIT8( KDesc,           "desc"         );
_LIT8( KDlnaDoc,        "X_DLNADOC"    );

_LIT( KIndexContainer,  "1" );
_LIT( KIndexAudio,      "2" );
_LIT( KIndexVideo,      "3" );
_LIT( KIndexImage,      "4" );
_LIT( KIndexOther,      "5" );

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::CUPnPXMLParserLite()
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLParserLite::CUPnPXMLParserLite()
    {
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::ConstructL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::ConstructL()
    {
    __LOG( "CUPnPXMLParserLite::CostructL" );
    iStack = CUPnPObjectStackLite::NewL();
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::NewL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPXMLParserLite* CUPnPXMLParserLite::NewL()
    {
    CUPnPXMLParserLite* self = CUPnPXMLParserLite::NewLC();
    CleanupStack::Pop( self );    
    return self;
    }


// --------------------------------------------------------------------------
// CUPnPXMLParserLite::NewLC
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPXMLParserLite* CUPnPXMLParserLite::NewLC()
    {   
    CUPnPXMLParserLite* self = new( ELeave ) CUPnPXMLParserLite();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// --------------------------------------------------------------------------
// CUPnPXMLParserLite::~CUPnPXMLParserLite
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLParserLite::~CUPnPXMLParserLite()
    {
    __LOG( "CUPnPXMLParserLite::~CUPnPXMLParserLite" );
    if ( iStack )
        {
        iStack->ResetAndDestroy();
        delete iStack;
        }
    
    delete iElementValue;
    delete iFormatString;
    delete iTitleBuf;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::ParseResultDataL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C void CUPnPXMLParserLite::ParseResultDataL(
    RPointerArray<CUpnpObjectLite>& aResultArray, const TDesC8& aData,
    const TDesC& aFormatString )
    {
    __LOG( "CUPnPXMLParserLite::ParseResultDataL, begin" );
    
    delete iFormatString; iFormatString = NULL;
    if( aFormatString != KNullDesC )
        {
        iFormatString = aFormatString.AllocL();
        }
    
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
    
    Xml::ParseL( *parser, aData );    
    
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( matchData );
    
    iResultRoot = NULL;           
    __LOG( "CUPnPXMLParserLite::ParseResultDataL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnStartDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnStartDocumentL( 
                                const RDocumentParameters& /*aDocParam*/, 
                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnEndDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnStartElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnStartElementL( const RTagInfo& aElement, 
                                      const RAttributeArray& aAttributes,
                                      TInt aErrorCode )
    {
    __LOG1( "CUPnPXMLParserLite::OnStartElementL, error code: %d", aErrorCode );
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
        CUpnpObjectLite* tmpContainer = CUpnpObjectLite::NewL();
        CleanupStack::PushL( tmpContainer );

        SetAttributesL( *tmpContainer, aAttributes );
        //push into the stack, ownership is transferred                      
        iStack->PushL( tmpContainer ); 
        
        CleanupStack::Pop( tmpContainer );
        }
    else if ( !desName.CompareF( KItem ) ) // Item element
        {
        CUpnpObjectLite* tmpItem = CUpnpObjectLite::NewL();
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
        //__ASSERTD( iStack->Top(), __FILE__, __LINE__ );
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
        // Ignore rest of the elements
                }
    __LOG( "CUPnPXMLParserLite::OnStartElementL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnEndElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnEndElementL( const RTagInfo& aElement, 
                                    TInt aErrorCode )
    {
    __LOG( "CUPnPXMLParserLite::OnEndElementL(), begin" );
    if ( aErrorCode != KErrNone )
        {
        return;
        }
        
    const TDesC8& desName = aElement.LocalName().DesC();    
    if ( !desName.CompareF( KContainer ) || !desName.CompareF( KItem ) )
        {
        if( iTitleBuf )
            {
            CUpnpObjectLite* obj = iStack->Top();
            __ASSERTD( obj, __FILE__, __LINE__ );

            if( iFormatString )
                {
                HBufC* tempBuf = UpnpString::ToUnicodeL( *iTitleBuf );
                CleanupStack::PushL( tempBuf );
                HBufC* buf = HBufC::NewL( iFormatString->Length() +
                    tempBuf->Length() );
                CleanupStack::PushL( buf );
                buf->Des().Format( *iFormatString, tempBuf );
                obj->SetTitleL( buf );
                CleanupStack::Pop( buf );
                CleanupStack::PopAndDestroy( tempBuf );
                }
            else
                {
                HBufC* buf = HBufC::NewL( iTitleBuf->Length() );
                CleanupStack::PushL( buf );
                buf->Des().Copy( *iTitleBuf );
                obj->SetTitleL( buf );
                CleanupStack::Pop( buf );
                }
            delete iTitleBuf; iTitleBuf = NULL;
            }
               
        iResultRoot->AppendL( iStack->Top() );
        iStack->Pop(); // Remove object from stack.
        }
    else if( KErrNone != desName.CompareF( KDIDL ))
        {        
        CUpnpObjectLite* obj = iStack->Top();
        __ASSERTD( obj, __FILE__, __LINE__ );
             
        if ( iElementValue )
            {
            if( !desName.CompareF( KTitle ) )
                {
                delete iTitleBuf; iTitleBuf = NULL;
                iTitleBuf = iElementValue;
                iElementValue = NULL;
                }
            else if( !desName.CompareF( KClass ) )
                {
                obj->SetObjectClass( *iElementValue );                
                iObjectClass = EFalse;
                if( iFormatString )
                    {
                    SetIconIndex( *obj );
                    }                
                }
            else
                {               
                }          
            }
        else
            {
            }    
        
        delete iElementValue; iElementValue = NULL;    
        }    
    __LOG( "CUPnPXMLParserLite::OnEndElementL(), end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnContentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnContentL( const TDesC8& aBytes, TInt aErrorCode )
    {
    __LOG( "CUPnPXMLParserLite::OnContentL(), begin" );
    if ( !iStack->Count() || aErrorCode != KErrNone )
        {
        return;
        }
    
    if( iTitle || iObjectClass ) // We are only interested in title or
                                 // object class!
        {
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
        }
    
    __LOG( "CUPnPXMLParserLite::OnContentL(), end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnStartPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
                                               const RString& /*aUri*/, 
                                               TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnEndPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnEndPrefixMappingL( const RString& /*aPrefix*/, 
                                             TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnIgnorableWhiteSpaceL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
                                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnSkippedEntityL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnSkippedEntityL( const RString& /*aName*/, 
                                          TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnProcessingInstructionL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnProcessingInstructionL( const TDesC8& /*aTarget*/, 
                                                  const TDesC8& /*aData*/, 
                                                  TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::OnError
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::OnError( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::GetExtendedInterface
// See upnpxmlparser.h
// --------------------------------------------------------------------------
TAny* CUPnPXMLParserLite::GetExtendedInterface( const TInt32 /*aUid*/ )
    {    
    // No implementation needed
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPXMLParserLite::SetAttributesL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLParserLite::SetAttributesL( CUpnpObjectLite& aObject, 
    const RAttributeArray& aAttributes )
    {
    __LOG( "CUPnPXMLParserLite::SetAttributesL" );
    
    if ( iStack->Count() )
        {
        // Object has a parent.
        // Should not really happen
        __LOG( "Object has a parent!" );
        __PANICD(__FILE__, __LINE__);      
        }
        
    RAttribute attribute;
    TInt count = aAttributes.Count();
    for ( TInt i = 0 ; i < count; i++ ) // Read attributes.
        {
        attribute = aAttributes[ i ];
        const TDesC8& name = attribute.Attribute().LocalName().DesC();
        
        if( !name.CompareF( KId ) )
            {
            aObject.SetObjectIdL( attribute.Value().DesC() );
            }
        }
    __LOG( "CUPnPXMLParserLite::SetAttributesL - End" );
    }

void CUPnPXMLParserLite::SetIconIndex( const CUpnpObjectLite& aObject )
    {
    if( aObject.IsTypeOf( CUpnpObjectLite::EContainer ) )
        {
        iFormatString->Des().Replace( 0, 1, KIndexContainer );
        }
    else if( aObject.IsTypeOf( CUpnpObjectLite::EMusicItem ) )
        {
        iFormatString->Des().Replace( 0, 1, KIndexAudio );
        }
    else if( aObject.IsTypeOf( CUpnpObjectLite::EImageItem ) )
        {
        iFormatString->Des().Replace( 0, 1, KIndexImage );
        }
    else if( aObject.IsTypeOf( CUpnpObjectLite::EVideoItem ) )
        {
        iFormatString->Des().Replace( 0, 1, KIndexVideo );
        }
    else
        {
        // Other
        iFormatString->Des().Replace( 0, 1, KIndexOther );
        }    
    }
        
// end of file


