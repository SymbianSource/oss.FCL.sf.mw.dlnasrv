/*
* Copyright (c) 2006,2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "upnpavtevent.h"

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
_LIT8( KTransportState, "TransportState" );
_LIT8( KTransportURI,   "AVTransportURI" );

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::CUPnPXMLEventParser()
// See upnpxmlparser.h
// --------------------------------------------------------------------------
CUPnPXMLEventParser::CUPnPXMLEventParser()
    {
    // No implementation required
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ConstructL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::ConstructL()
    {
    __LOG( "CUPnPXMLEventParser::CostructL" );
    
    iAvtEvent = CUPnPAVTEvent::NewL();
    iAvtResultEvent = CUPnPAVTEvent::NewL();
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
    
    delete iAvtEvent;
    delete iAvtResultEvent;
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ParseRcEventDataL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPAVTEvent* CUPnPXMLEventParser::ParseRcEventDataL(
    const TDesC8& aData, const TInt aInstanceId )
    {
    __LOG( "CUPnPXMLEventParser::ParseRcEventDataL, begin" );
    
    if ( !aData.Length() )
        {
        User::Leave( KErrArgument );
        }

    Reset();
    ResetResult();
    iSessionInstanceID = aInstanceId;
        
    // Create parser 
    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL( KXmlMimeType ); 
    matchData->SetVariantL( KLIB2XML ); 
    CParser* parser = CParser::NewLC( *matchData, *this ); 
    parser->EnableFeature( Xml::EReportNamespaceMapping );
    
    Xml::ParseL( *parser, aData );    
    
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( matchData );
    
    if( iAvtResultEvent->InstanceID() == KErrNotFound )
        {
        __LOG1( "CUPnPXMLEventParser::ParseRcEventDataL \
instanceid not matching %d",  iSessionInstanceID );
        User::Leave( KErrNotFound );
        }  
        
    return CUPnPAVTEvent::CloneL( *iAvtResultEvent );
    }


// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ParseAVTEventDataL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
EXPORT_C CUPnPAVTEvent* CUPnPXMLEventParser::ParseAvtEventDataL(
    const TDesC8& aData, const TInt aInstanceId )
    {
    __LOG( "CUPnPXMLEventParser::ParseAvtEventDataL, begin" );
    
    if ( !aData.Length() )
        {
        User::Leave( KErrArgument );
        }
    
    Reset();
    ResetResult();
    iSessionInstanceID = aInstanceId;
        
    // Create parser 
    CMatchData* matchData = CMatchData::NewLC();
    matchData->SetMimeTypeL( KXmlMimeType ); 
    matchData->SetVariantL( KLIB2XML ); 
    CParser* parser = CParser::NewLC( *matchData, *this ); 
    parser->EnableFeature( Xml::EReportNamespaceMapping );
    
    Xml::ParseL( *parser, aData );    
    
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( matchData );
    
    if( iAvtResultEvent->InstanceID() == KErrNotFound )
        {
        __LOG1( "CUPnPXMLEventParser::ParseAvtEventDataL \
instanceid not matching %d", iSessionInstanceID );
        User::Leave( KErrNotFound );
        }
    
    return CUPnPAVTEvent::CloneL( *iAvtResultEvent );
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
    if ( aErrorCode != KErrNone )
        {
        __LOG1( "CUPnPXMLEventParser::OnStartElementL, error: %d",
                                                                aErrorCode );
        return;
        }
    const TDesC8& desName = aElement.LocalName().DesC();
    __LOG8_1("CUPnPXMLEventParser::OnStartElementL name = %S", &desName );
  
    if ( !desName.CompareF( KEvent ) )
        {
        iParserState = EEvent;
        }
    else if ( !desName.CompareF( KInstanceID ) )
        {
        iParserState = EInstanceID;
        SetAttributesL( aAttributes );
        }
    //Rc events
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
    // Avt events 
    else if( !desName.CompareF( KTransportState ) )
        {
        iParserState = ETransportState;
        SetAttributesL( aAttributes );
        }
    else if ( !desName.CompareF( KTransportURI ) )
        {
        iParserState = ETransportURI;
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
        // just print attribute values
        iParserState = ENotSupported;
        SetAttributesL( aAttributes );
        }
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnEndElementL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnEndElementL( const RTagInfo& aElement, 
                                    TInt /*aErrorCode*/ )
    {
    // if we have finished parsing one event,
    // check that it belongs to our session
    const TDesC8& desName = aElement.LocalName().DesC();
    
    if ( !desName.CompareF( KInstanceID ) )
        {
        if( iAvtEvent->InstanceID() == iSessionInstanceID )
            {
            iAvtResultEvent->Reset();
            iAvtResultEvent->SetInstanceID( iAvtEvent->InstanceID() );
            iAvtResultEvent->SetMute( iAvtEvent->Mute() );
            iAvtResultEvent->SetVolume( iAvtEvent->Volume() );
            iAvtResultEvent->SetTransportState( iAvtEvent->TransportState() );
            iAvtResultEvent->SetTransportURIL( iAvtEvent->TransportURI() );
            
            __LOG( "CUPnPXMLEventParser::OnEndElementL() valid event" );
            }
        else
            {
            __LOG2( "CUPnPXMLEventParser OnEndElementL ERROR instanceid not \
matching session %d, event %d", iSessionInstanceID, iAvtEvent->InstanceID());
            }
            
        Reset();
        }
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::OnContentL
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::OnContentL( const TDesC8& /*aBytes*/,
    TInt /*aErrorCode*/ )
    {
    // No implementation needed
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
void CUPnPXMLEventParser::OnProcessingInstructionL(
    const TDesC8& /*aTarget*/, const TDesC8& /*aData*/, TInt /*aErrorCode*/ )
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
    RAttribute attribute;
    TInt count = aAttributes.Count();
    TInt volume = KErrNotFound;
    iMasterVolumeState = EFalse;
    
    for ( TInt i = 0; i < count ; i++ )
        {
        attribute = aAttributes[i];
        const TDesC8& name = attribute.Attribute().LocalName().DesC();
        const TDesC8& value = attribute.Value().DesC();
        if( value.Length() )
            {
            __LOG8_1( "CUPnPXMLEventParser::SetAttributesL value = %S",
                    &value );
            }
        
        // volume & channel
        if ( iParserState == EVolume )
            {
            // assign the value of Volume to volume 
            if ( name.CompareF( KVal ) == KErrNone )
                {
                TLex8 lexer( value );
                User::LeaveIfError( lexer.Val(volume) );
                }
            else if ( name.CompareF( KChannel ) == KErrNone )
                {
                // channel is found, check if is Master
                if ( value.CompareF( KMaster ) == KErrNone )
                    {
                    __LOG( "CUPnPXMLEventParser::SetAttributesL - \
MasterVolume found!" );
                    iMasterVolumeState = ETrue;
                    }
                }
            }
          
          
                     
              
        // other values
        else if ( name.Compare( KVal ) == KErrNone )
            {
            TLex8 lexer( value );
            
            if ( iParserState == EInstanceID )
                {
                TInt id = KErrNotFound;
                User::LeaveIfError( lexer.Val( id ) );
                iAvtEvent->SetInstanceID( id );
                }
            else if ( iParserState == EMute )
                {
                TInt mute = KErrNotFound;
                User::LeaveIfError( lexer.Val( mute ) );
                iAvtEvent->SetMute( mute );
                }
            else if ( iParserState == ETransportState )
                {
                iAvtEvent->SetTransportState( value );
                }
            else if ( iParserState == ETransportURI )
                {
                iAvtEvent->SetTransportURIL( value );
                }
            else
                {
                __LOG( "CUPnPXMLEventParser::SetAttributesL - \
unknown state" );
                }
            }
        }
    
    // check Mastervolume and volume
    if ( iParserState == EVolume && 
         iMasterVolumeState && 
         volume != KErrNotFound )
        {
        // all is found ,so assign the iVolume
        __LOG1( "CUPnPXMLEventParser::SetAttributesL - set iVolume : %d",
                                                                     volume );
        iAvtEvent->SetVolume( volume );       
        }
    }
    
// --------------------------------------------------------------------------
// CUPnPXMLEventParser::Reset
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::Reset()
    {
    iParserState = ENotSupported;
    iMasterVolumeState = EFalse;
    
    iAvtEvent->Reset();
    }

// --------------------------------------------------------------------------
// CUPnPXMLEventParser::ResetResult
// See upnpxmlparser.h
// --------------------------------------------------------------------------
void CUPnPXMLEventParser::ResetResult()
    {
    iSessionInstanceID = KErrNotFound;
    iParserState = ENotSupported;
    iMasterVolumeState = EFalse;
    
    iAvtResultEvent->Reset();
    }     
         
// end of file

