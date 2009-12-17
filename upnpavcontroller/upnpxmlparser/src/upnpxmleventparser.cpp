/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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






#include <xml/parser.h>
#include <xml/parserfeature.h>
#include <upnpstring.h>
#include <xml/matchdata.h>

#include "upnpxmleventparser.h"

_LIT( KComponentLogfile, "upnpxmlparser.txt");
#include "upnplog.h"

_LIT8( KXmlMimeType,    "text/xml"     );
_LIT8( KLIB2XML,        "libxml2" );

_LIT8( KEvent,          "Event"         );
_LIT8( KInstanceID,     "InstanceID"    );
_LIT8( KVolume,         "Volume"        );
_LIT8( KMute,           "Mute"          );
_LIT8( KDIDL,           "DIDL-Lite"     );
_LIT8( KDesc,           "desc"          );
_LIT8( KDlnaDoc,        "X_DLNADOC"     );
_LIT8( KVal,            "val"           );
_LIT8( KChannel,        "channel"       );
_LIT8( KMaster,         "Master"       );

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::CUPnPXMLEventParser()
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLEventParser::CUPnPXMLEventParser()
    {
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ConstructL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::ConstructL()
    {
    __LOG( "CUPnPXMLEventParser::CostructL" );
    
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::NewL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPXMLEventParser* CUPnPXMLEventParser::NewL()
    {
    __LOG( "CUPnPXMLEventParser::NewL" );
    CUPnPXMLEventParser* self = new( ELeave ) CUPnPXMLEventParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);    
    return self;
    }
    
// --------------------------------------------------------------------------
// CUPnPXMLEventParser::~CUPnPXMLEventParser
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLEventParser::~CUPnPXMLEventParser()
    {
    __LOG( "CUPnPXMLEventParser::~CUPnPXMLEventParser" );
    
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ParseResultDataL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C void CUPnPXMLEventParser::ParseResultDataL( const TDesC8& aData,
        TInt& aInstanceId, TInt& aVolume, TBool& aMute )
    {
    __LOG( "CUPnPXMLEventParser::ParseResultDataL, begin" );
    
    if ( !aData.Length() )
        {
        User::Leave( KErrArgument );
        }

    Reset();
        
    // Create parser 
    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL( KXmlMimeType ); 
    matchData->SetVariantL( KLIB2XML ); 
    CParser* parser = CParser::NewLC( *matchData, *this ); 
    parser->EnableFeature( Xml::EReportNamespaceMapping );
    
    Xml::ParseL( *parser, aData );    
    
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( matchData );
    
    if( iInstanceID != KErrNotFound )
        {
        aInstanceId = iInstanceID;
        if( iVolume != KErrNotFound )
            {
            aVolume = iVolume;    
            }
        if( iMute != KErrNotFound )
            {
            aMute = iMute;    
            }
        }
    else
        {
        User::Leave( KErrArgument );
        }    
    
    __LOG( "CUPnPXMLEventParser::ParseResultDataL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnStartDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnStartDocumentL( 
                                const RDocumentParameters& /*aDocParam*/, 
                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnEndDocumentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnEndDocumentL( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnStartElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnStartElementL( const RTagInfo& aElement, 
                                      const RAttributeArray& aAttributes,
                                      TInt aErrorCode )
    {
    __LOG1( "CUPnPXMLEventParser::OnStartElementL, error: %d", aErrorCode );
    if ( aErrorCode != KErrNone )
        {
        return;
        }
    const TDesC8& desName = aElement.LocalName().DesC();
    //const TDesC8& prefix = aElement.Prefix().DesC();
      
    if ( !desName.CompareF( KEvent ) )
        {
        iParserState = EEvent;
        }
    else if ( !desName.CompareF( KInstanceID ) )
        {
        iParserState = EInstanceID;
        SetAttributesL( aAttributes );
        }
    else if( !desName.CompareF( KVolume ) )
        {
        iParserState = EVolume;
        SetAttributesL( aAttributes );
        }
    else if( !desName.CompareF( KMute ) )
        {
        iParserState = EMute;
        SetAttributesL( aAttributes );
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
        __LOG( "OnStartElementL - unknown element!" );
        __LOG8( desName );
        }
    
    __LOG( "CUPnPXMLEventParser::OnStartElementL, end" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnEndElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnEndElementL( const RTagInfo& /*aElement*/, 
                                    TInt /*aErrorCode*/ )
    {
    __LOG( "CUPnPXMLSAXParser::OnEndElementL(), begin" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnContentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnContentL( const TDesC8& /*aBytes*/,
    TInt /*aErrorCode*/ )
    {
    __LOG( "CUPnPXMLSAXParser::OnContentL(), begin" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnStartPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnStartPrefixMappingL( const RString& /*aPrefix*/, 
                                               const RString& /*aUri*/, 
                                               TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnEndPrefixMappingL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnEndPrefixMappingL( const RString& /*aPrefix*/, 
                                             TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnIgnorableWhiteSpaceL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnIgnorableWhiteSpaceL( const TDesC8& /*aBytes*/, 
                                                TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnSkippedEntityL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnSkippedEntityL( const RString& /*aName*/, 
                                          TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnProcessingInstructionL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnProcessingInstructionL( const TDesC8& /*aTarget*/, 
                                                  const TDesC8& /*aData*/, 
                                                  TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnError
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnError( TInt /*aErrorCode*/ )
    {
    // No implementation needed
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::GetExtendedInterface
// See upnpxmlparser.h
// --------------------------------------------------------------------------
TAny* CUPnPXMLEventParser::GetExtendedInterface( const TInt32 /*aUid*/ )
    {    
    // No implementation needed
    return NULL;
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::SetAttributesL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::SetAttributesL(
    const RAttributeArray& aAttributes )
    {
    __LOG( "CUPnPXMLEventParser::SetAttributesL" );

    RAttribute attribute;
    TInt count = aAttributes.Count();
    TInt volume = KErrNotFound;
    iMasterVolumeState = EFalse;
    for ( TInt i = 0; i < count ; i++ )
        {
        attribute = aAttributes[i];
        const TDesC8& name = attribute.Attribute().LocalName().DesC();

        if ( iParserState == EVolume )
            {
            // assign the value of Volume to volume 
            if ( name.CompareF( KVal ) == KErrNone )
                {
                __LOG( "SetAttributesL - \"val\" found!" );
                TLex8 lexer( attribute.Value().DesC() );
                User::LeaveIfError( lexer.Val(volume) );
                __LOG1( "SetAttributesL - volume = %d", volume );
                }
            else if ( name.CompareF( KChannel ) == KErrNone )
                {
                // channel is found, check if is Master
                const TDesC8& channelname = attribute.Value().DesC();
                if ( channelname.CompareF( KMaster ) == KErrNone )
                    {
                    __LOG( "SetAttributesL - MasterVolume found!" );
                    iMasterVolumeState = ETrue;
                    }
                }
            }
        else
            {

            if ( name.Compare( KVal ) == KErrNone )
                {
                __LOG( "SetAttributesL - \"val\" found!" );

                TLex8 lexer( attribute.Value().DesC() );
                if ( iParserState == EInstanceID )
                    {
                    User::LeaveIfError( lexer.Val( iInstanceID ) );
                    }
                else
                    if ( iParserState == EMute )
                        {
                        User::LeaveIfError( lexer.Val( iMute ) );
                        }
                    else
                        {
                        __LOG( "SetAttributesL - unknown state!" );
                        }
                }
            }
        }
    
    // check Mastervolume if was found, and volume if was found
    if ( iParserState == EVolume && 
         iMasterVolumeState && 
         volume != KErrNotFound )
        {
        // all is found ,so assign the iVolume
        __LOG1( "SetAttributesL - set iVolume : %d", volume );
        iVolume = volume;       
        }
    
    __LOG( "CUPnPXMLEventParser::SetAttributesL - End" );
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::Reset
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::Reset()
    {
    iInstanceID = KErrNotFound;
    iMute = KErrNotFound;
    iVolume = KErrNotFound;
    iParserState = ENotSupported;
    iMasterVolumeState = EFalse;
    }
      
// end of file

