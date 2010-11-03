/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UPnP GStreamer wrapper
*
*/

// INCLUDES
#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <xml/matchdata.h>
#include <xml/xmlparsererrors.h>
#include <f32file.h>
#include <s32file.h>
#include <upnpaction.h>
#include <upnpstring.h>

#include "upnprenderercfg.h"
#include "upnprenderercfgparser.h"

_LIT( KComponentLogfile, "upnprenderercfgparser.txt");
#include "upnplog.h"

#define __FUNC_LOG __LOG8_1( "%s", __PRETTY_FUNCTION__ );

//tags
_LIT8( KTranscoding,             "transcoding" );
_LIT8( KPipeline,                "pipeline" );
_LIT8( KProtocolInfo,            "protocolInfo" );
_LIT8( KSizeMultiplier,          "sizeMultiplier" );
_LIT8( KRenderer,                "renderer" );
_LIT8( KConfig,                  "config" );

//attributes
_LIT8( KExactMatch,              "exactMatch" );
_LIT8( KModelName,               "modelName" );
_LIT8( KSrcMimeType,             "srcMimeType" );

_LIT8( KFalse,                    "0" );

_LIT8( KXmlMimeType,             "text/xml" );
_LIT8( KLIB2XML,                 "libxml2" );

const TUint8 KCharSpace = 32; // Space
const TUint8 KCharDel = 127;  // DEL

using namespace Xml;

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::NewL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRendererCfgParser* CUpnpRendererCfgParser::NewL()
    {
    __FUNC_LOG;
    
    CUpnpRendererCfgParser* self = CUpnpRendererCfgParser::NewLC();
    CleanupStack::Pop( self );    
    return self;
    }    

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::NewLC()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRendererCfgParser* CUpnpRendererCfgParser::NewLC()
    {
    __FUNC_LOG;
    
    CUpnpRendererCfgParser* self = new( ELeave ) CUpnpRendererCfgParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;    
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::~CUpnpRendererCfgParser()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C CUpnpRendererCfgParser::~CUpnpRendererCfgParser()
    {   
    __FUNC_LOG; 
    
    delete iParser;
    delete iMatchData;
    
    iFs.Close();
    }
    
// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::CUpnpRendererCfgParser()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CUpnpRendererCfgParser::CUpnpRendererCfgParser()
    : iSizeMultiplier( KErrNotFound )
    {
    __FUNC_LOG;
    // No implementation needed    
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::ConstructL()
// (See comments in header file)
// --------------------------------------------------------------------------
//    
void CUpnpRendererCfgParser::ConstructL()
    {
    __FUNC_LOG;
    
    iMatchData = CMatchData::NewL();
    iMatchData->SetMimeTypeL( KXmlMimeType ); 
    iMatchData->SetVariantL( KLIB2XML ); 
    iParser = CParser::NewL( *iMatchData, *this );    
    iParser->EnableFeature( ESendFullContentInOneChunk );
    
    User::LeaveIfError( iFs.Connect() );
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::ParseRendererCfgL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
EXPORT_C void CUpnpRendererCfgParser::ParseRendererCfgL( 
        CUpnpRendererCfg& aRendereConfig )
    {     
    __FUNC_LOG;             
    
    if( aRendereConfig.iPipeline || 
        aRendereConfig.iProtocolInfo || 
        aRendereConfig.iSizeMultiplier != KErrNotFound )
        {
        //these values should be still untouched
        User::Leave( KErrArgument );
        }
    
    iModelName = aRendereConfig.iModelName;
    iSrcMimeType = aRendereConfig.iSrcMimeType;
    
    //syncronous call -> returns when the file is fully parsed
    XMLParseL( *aRendereConfig.iConfigFile );

    if( !iPipeline || !iProtocolInfo || iSizeMultiplier == KErrNotFound )
        {
        __LOG8_1( "Couldn't find all the config elements for %S", 
                iModelName );
        
        //delete allocated bufs and reset vars
        delete iPipeline; iPipeline = NULL;        
        delete iProtocolInfo; iProtocolInfo = NULL;                
        
        //make sure that everything is in init state
        Reset();
        
        User::Leave( KErrNotFound );
        }
            
    aRendereConfig.iPipeline = iPipeline; //ownership transfer 
    aRendereConfig.iProtocolInfo = iProtocolInfo; //ownership transfer    
    aRendereConfig.iSizeMultiplier = iSizeMultiplier;    
    
    //make sure that everything is in init state
    Reset();
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::XMLParseL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::XMLParseL( const TDesC& aFile )
    {
    __FUNC_LOG;
   
    RFile file;
    User::LeaveIfError( file.Open( iFs, aFile , EFileRead ) );
    CleanupClosePushL( file );
    
    TInt size;
    User::LeaveIfError( file.Size( size ) );
    
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 ptr = buf->Des();
    User::LeaveIfError( file.Read( ptr ) );
    
    Xml::ParseL( *iParser, *buf );
    
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( &file );    
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::Reset()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::Reset()
    {
    __FUNC_LOG;
        
    iCurrentElement = EUnknown;
    iCorrectModel = EFalse;
    iCorrectTranscoding = EFalse;
    iModelName = NULL;
    iSrcMimeType = NULL;
    iPipeline = NULL;
    iProtocolInfo = NULL;
    iSizeMultiplier = KErrNotFound;
    }
    
// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::Element()
// (See comments in header file)
// --------------------------------------------------------------------------
//
CUpnpRendererCfgParser::TCfgElements CUpnpRendererCfgParser::Element(  
        const RTagInfo& aElement )
    {
    __FUNC_LOG;
    
    TCfgElements element = EUnknown;
    
    const TDesC8& desName = aElement.LocalName().DesC();   
    
    if( desName.CompareF( KTranscoding ) == 0 )
        {
        element = ETranscoding;
        }
    else if( desName.CompareF( KPipeline ) == 0 )
        {
        element = EPipeline;
        }
    else if( desName.CompareF( KProtocolInfo ) == 0 )
        {
        element = EProtocolInfo;
        }
    else if( desName.CompareF( KSizeMultiplier ) == 0 )
        {
        element = ESizeMultiplier;
        }
    else if( desName.CompareF( KRenderer ) == 0 )
        {
        element = ERenderer;
        }
    else if( desName.CompareF( KConfig ) == 0 )
        {
        element = EConfig;
        }
    else
       {
       __LOG8_1( "Error: Unknown element %S", &desName );
       __ASSERT( EFalse, __FILE__, __LINE__ );
       }
       
    return element;
    }
    
// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::ContainsAttribute()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TBool CUpnpRendererCfgParser::ContainsAttribute( 
        const RAttributeArray& aAttributes, const TDesC8& aAttributeName, 
        const TDesC8& aAttributeValue, TBool aExactValueMatch )
    {
    __FUNC_LOG;
      
    TInt attributeCount = aAttributes.Count();
    for( TInt i = 0; i < attributeCount; i++ )
        {
        Xml::RAttribute attr = aAttributes[i];                      

        if( attr.Attribute().LocalName().DesC() == aAttributeName )
            {
            if( aExactValueMatch )
                {
                if( aAttributeValue.CompareF( attr.Value().DesC() ) == 0 )
                    {
                    //requested exact value match found
                    return ETrue;
                    }
                }
            else if( aAttributeValue.FindF( attr.Value().DesC() ) >= 0 )
                {
                //requested non-exact value match (sub-string) found
                return ETrue;
                }
            }
        }
        
    return EFalse;
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnStartDocumentL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnStartDocumentL( 
        const RDocumentParameters& /*aDocParam*/,
        TInt /*aErrorCode*/ )
    {    
    __FUNC_LOG;
    
    // No implementation needed
    }    

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnEndDocumentL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }   

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnStartElementL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnStartElementL( const RTagInfo& aElement,
        const RAttributeArray& aAttributes, TInt aErrorCode )
    {    
    __FUNC_LOG;
    __ASSERT( !aErrorCode, __FILE__, __LINE__ );
        
    iCurrentElement = Element( aElement );
    
    switch( iCurrentElement )
        {
        case EConfig:         //fall through
        case EPipeline:       //fall through
        case EProtocolInfo:   //fall through
        case ESizeMultiplier:
            {
            break;
            }
        case ERenderer:
            {      
            TBool exactMatch = ETrue;
            if( ContainsAttribute( aAttributes, KExactMatch, KFalse, 
                    ETrue ) )
                {
                exactMatch = EFalse;
                }
                    
            if( ContainsAttribute( aAttributes, KModelName, *iModelName, 
                    exactMatch ) )
                {
                iCorrectModel = ETrue;
                }
            else
                {
                iCorrectModel = EFalse;
                } 
            break;
            }
        case ETranscoding:
            {
            if( iCorrectModel &&
                ContainsAttribute( aAttributes, KSrcMimeType, *iSrcMimeType,
                    ETrue ) )
                {
                // correct transcoding found 
                // -> assert if something is already set
                __ASSERT( !iPipeline, __FILE__, __LINE__ );        
                __ASSERT( !iProtocolInfo, __FILE__, __LINE__ );     
                __ASSERT( iSizeMultiplier == KErrNotFound, __FILE__, 
                        __LINE__ );     
                                            
                iCorrectTranscoding = ETrue;
                }
            break;
            }
        default:
            {
            __ASSERT( EFalse, __FILE__, __LINE__ );
            }
        }  
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnEndElementL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnEndElementL( const RTagInfo& aElement, 
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    if( iCurrentElement == EUnknown )
        {
        //at least second OnEndElementL callback in a row -> fetch enum
        iCurrentElement = Element( aElement );
        }
    
    switch( iCurrentElement )
        {
        case EPipeline:     //fall through
        case EProtocolInfo: //fall through
        case ESizeMultiplier:      
            {
            break;
            }
        case EUnknown:      //fall through
        case EConfig:       //fall through
        case ERenderer:
            {
            iCorrectModel = EFalse;
            //fall through
            }
        case ETranscoding:
            {
            iCorrectTranscoding = EFalse;
            break;
            }
        default:
            {
            __ASSERT( EFalse, __FILE__, __LINE__ );
            }            
        }    
        
    //exiting from element -> reset current element
    iCurrentElement = EUnknown;
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnContentL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnContentL( const TDesC8& aBytes, 
        TInt aErrorCode )
    {
    __FUNC_LOG;
    __ASSERT( !aErrorCode, __FILE__, __LINE__ );
    
    TUint8 firstChar = aBytes[0];    
    if( firstChar <= KCharSpace || firstChar == KCharDel )
        {
        //1st char of the content is a special char -> skip
        return;
        }
    
    switch( iCurrentElement )
        {
        case EConfig:   //fall through
        case ERenderer: //fall through
        case ETranscoding:
            break;
        case EPipeline:
            {
            if( iCorrectTranscoding )
                {
                //shouldn't contain any parsed value yet
                __ASSERT( !iPipeline, __FILE__, __LINE__ );
                       
                iPipeline = HBufC8::NewL( aBytes.Length() );
                iPipeline->Des().Copy( aBytes ); 
                }
            break;
            }
        case EProtocolInfo:
            {
            if( iCorrectTranscoding )
                {
                //shouldn't contain any parsed value yet
                __ASSERT( !iProtocolInfo, __FILE__, __LINE__ );
                
                iProtocolInfo = HBufC8::NewL( aBytes.Length() );
                iProtocolInfo->Des().Copy( aBytes ); 
                }
            break;
            }
        case ESizeMultiplier:
            {
            if( iCorrectTranscoding )
                {
                //shouldn't contain any parsed value yet
                __ASSERT( iSizeMultiplier == KErrNotFound, __FILE__, 
                        __LINE__ );
                
                TLex8 lex;
                lex.Assign( aBytes );                
                TInt err = lex.Val( iSizeMultiplier );
                
                __ASSERT( !err, __FILE__, __LINE__ );
                }
            break;
            }
        default:
            {
            __ASSERT( EFalse, __FILE__, __LINE__ );
            }            
        }
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnStartPrefixMappingL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnStartPrefixMappingL( 
        const RString& /*aPrefix*/, const RString& /*aUri*/, 
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }
// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnEndPrefixMappingL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnEndPrefixMappingL( const RString& /*aPrefix*/,
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnIgnorableWhiteSpaceL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnSkippedEntityL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnSkippedEntityL( const RString& /*aName*/, 
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnProcessingInstructionL()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnProcessingInstructionL( 
        const TDesC8& /*aTarget*/, 
        const TDesC8& /*aData*/,
        TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    } 

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::OnError()
// (See comments in header file)
// --------------------------------------------------------------------------
//
void CUpnpRendererCfgParser::OnError( TInt /*aErrorCode*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    }  

// --------------------------------------------------------------------------
// CUpnpRendererCfgParser::GetExtendedInterface()
// (See comments in header file)
// --------------------------------------------------------------------------
//
TAny* CUpnpRendererCfgParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {
    __FUNC_LOG;
    
    // No implementation needed
    return NULL;
    }


// end of file
