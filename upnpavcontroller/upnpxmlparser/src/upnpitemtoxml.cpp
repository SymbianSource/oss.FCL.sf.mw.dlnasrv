/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      generates XML from an UPNP item
*
*/






// INCLUDE FILES
// System
#include <e32base.h>

// upnp stack api
#include <upnpstring.h>
#include <upnpitem.h>
#include <upnpdlnaprotocolinfo.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp definitions
#include "upnpitemutility.h"

// xmlparser internal
#include "upnpitemtoxml.h"

_LIT( KComponentLogfile, "upnpxmlparser.txt");
#include "upnplog.h"

// CONSTANTS
_LIT8(KItemHeading1,            "<item id=\"");
_LIT8(KItemHeading2,            "\" parentID=\"");
_LIT8(KItemHeading3False,       "\" restricted=\"0\">");
_LIT8(KItemHeading3True,        "\" restricted=\"1\">");
_LIT8(KItemTitleBegin,          "<dc:title>");
_LIT8(KItemTitleEnd,            "</dc:title>");
_LIT8(KItemDateBegin,           "<dc:date>");
_LIT8(KItemDateEnd,             "</dc:date>");
_LIT8(KItemClassBegin,          "<upnp:class>");
_LIT8(KItemClassEnd,            "</upnp:class>");
_LIT8(KItemPInfoEmpty,          "<res protocolInfo=\"*:*:*:*\"></res>");
_LIT8(KItemPInfoEmptyDlna,      "<res protocolInfo=\"*:*:");
_LIT8(KDlnaPn,                  "DLNA.ORG_PN=" );
_LIT8(KColon,                   ":"); 
_LIT8(KItemPInfoBegin,          "<res protocolInfo=\"");
_LIT8(KItemPInfoMiddle,         ">");
_LIT8(KItemPInfoEnd,            "</res>");
_LIT8(KItemPInfoEnd2,            "\"></res>");
_LIT8(KItemEnd,                 "</item>");

_LIT8(KQuotationMark,           "\" ");
_LIT8(KItemSize,                "size=\"");
_LIT8(KItemDuration,            "duration=\"");
_LIT8(KItemResolution,          "resolution=\"");

// Music metadata
_LIT8(KItemArtistBegin,         "<upnp:artist>");
_LIT8(KItemArtistEnd,           "</upnp:artist>");
_LIT8(KItemCreatorBegin,        "<dc:creator>");
_LIT8(KItemCreatorEnd,          "</dc:creator>");
_LIT8(KItemAlbumBegin,          "<upnp:album>");
_LIT8(KItemAlbumEnd,            "</upnp:album>");
_LIT8(KItemGenreBegin,          "<upnp:genre>");
_LIT8(KItemGenreEnd,            "</upnp:genre>");
_LIT8(KItemAlbumArtURIBegin,    "<upnp:albumArtURI>");
_LIT8(KItemAlbumArtURIEnd,      "</upnp:albumArtURI>");

_LIT8(KDIDLBeginXmlEscaped,
    "&lt;DIDL-Lite "
    "xmlns=&quot;urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/&quot; "
    "xmlns:dc=&quot;http://purl.org/dc/elements/1.1/&quot; "
    "xmlns:upnp=&quot;urn:schemas-upnp-org:metadata-1-0/upnp/&quot;"
    "&gt;");
_LIT8(KDIDLEndXmlEscaped, "&lt;/DIDL-Lite&gt;");

_LIT8( KAsterisk, "*" );

const TInt KBufLen = 256;
const TInt KDateStringLength        = 10;
const TInt KDateTimeStringLength    = 19;
const TInt KMaxDateStringLength     = 30;
const TInt KSeparatorAscii          = 58;


// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpItemToXML::CUpnpItemToXML
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
CUpnpItemToXML::CUpnpItemToXML( const CUpnpItem& aItem ) :
    iItem( const_cast<CUpnpItem*>(&aItem) )
    {
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
void CUpnpItemToXML::ConstructL()
    {
    }


// --------------------------------------------------------------------------
// CUpnpItemToXML::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpItemToXML* CUpnpItemToXML::NewL( const CUpnpItem& aItem )
    {
    CUpnpItemToXML* self = CUpnpItemToXML::NewLC( aItem );
    
    CleanupStack::Pop();

    return self;
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::NewLC
// Two-phased constructor.
// --------------------------------------------------------------------------
CUpnpItemToXML* CUpnpItemToXML::NewLC( const CUpnpItem& aItem )
    {
    CUpnpItemToXML* self = new( ELeave ) CUpnpItemToXML( aItem );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }
    
// Destructor
CUpnpItemToXML::~CUpnpItemToXML()
    {
    }


// --------------------------------------------------------------------------
// CUpnpItemToXML::AsXmlL
// Returns XML buffer
// (other items were commented in a header).
// --------------------------------------------------------------------------
HBufC8* CUpnpItemToXML::AsXmlL( const TBool /*aIncludeChilds = ETrue */ ) 
    {
    __LOG( "CUpnpItemToXML::AsXmlL" );
    
    const TInt bufferSize = 64; // Buffer size, grows dynamicly in 64b steps
    CBufFlat *pBuf = CBufFlat::NewL( bufferSize );
    CleanupStack::PushL( pBuf );

    RBufWriteStream stream( *pBuf );
    CleanupClosePushL( stream );

    // Then add the actual data
    stream.WriteL( KItemHeading1() );
    // xml encoding added
    HBufC8* encodeTemp = HBufC8::NewLC( iItem->Id().Length() );
    encodeTemp->Des().Copy( iItem->Id() );

    HBufC8* tempPtr = NULL;
    tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
    CleanupStack::PushL( tempPtr );

    stream.WriteL( *tempPtr );
    CleanupStack::PopAndDestroy( tempPtr );
    tempPtr = NULL;

    CleanupStack::PopAndDestroy( encodeTemp );
    encodeTemp = NULL;

    stream.WriteL( KItemHeading2() );

    stream.WriteL( iItem->ParentId() );
    if (iItem->Restricted())
        {
        stream.WriteL( KItemHeading3True() );
        }
    else
        {
        stream.WriteL( KItemHeading3False() );
        }
    stream.WriteL( KItemTitleBegin() );

    // xml encoding added
    encodeTemp = HBufC8::NewLC( iItem->Title().Length() );
    encodeTemp->Des().Copy( iItem->Title() );
    
    tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
    CleanupStack::PushL( tempPtr );

    stream.WriteL( *tempPtr );
    CleanupStack::PopAndDestroy( tempPtr );
    tempPtr = NULL;
    
    CleanupStack::PopAndDestroy( encodeTemp );
    encodeTemp = NULL;    
    
    stream.WriteL( KItemTitleEnd() );
    stream.WriteL( KItemClassBegin() );

    stream.WriteL( iItem->ObjectClass() );
    stream.WriteL( KItemClassEnd() );
    
     // Music meta data information
    const TDesC8& artist = GetValueFromElement( KElementArtist ); 
    if ( artist != KNullDesC8 )

        {
        stream.WriteL( KItemArtistBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( artist.Length() );
        encodeTemp->Des().Copy( artist );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
        
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;

        stream.WriteL( KItemArtistEnd );
        }
    const TDesC8& creator = GetValueFromElement( KElementCreator ); 
    if ( creator != KNullDesC8 )
        {
        stream.WriteL( KItemCreatorBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( creator.Length() );
        encodeTemp->Des().Copy( creator );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
        
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;

        stream.WriteL( KItemCreatorEnd );
        }
    const TDesC8& album = GetValueFromElement( KElementAlbum ); 
    if ( album != KNullDesC8 )
        {
        stream.WriteL( KItemAlbumBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( album.Length() );
        encodeTemp->Des().Copy( album );
        
        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;
        //stream.WriteL( Album() );
        stream.WriteL( KItemAlbumEnd );
        }
    const TDesC8& genre = GetValueFromElement( KElementGenre );     
    if ( genre != KNullDesC8 )
        {
        stream.WriteL( KItemGenreBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( genre.Length() );
        encodeTemp->Des().Copy( genre );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;
        //stream.WriteL( Genre() );
        stream.WriteL( KItemGenreEnd );
        } 
    const TDesC8& albumarturi = GetValueFromElement( KElementAlbumArtUri );     
    if ( albumarturi != KNullDesC8 )
        {
        stream.WriteL( KItemAlbumArtURIBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( albumarturi.Length() );
        encodeTemp->Des().Copy( albumarturi );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;        
        stream.WriteL( KItemAlbumArtURIEnd );
        }
    const TDesC8& date = GetValueFromElement( KElementDate );         
    if ( date != KNullDesC8 )
        {        
        if( ValidateDateL( date ) )
            {
            stream.WriteL( KItemDateBegin );
            // xml encoding added
            encodeTemp = HBufC8::NewLC( date.Length() );
            encodeTemp->Des().Copy( date );

            tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
            CleanupStack::PushL( tempPtr );

            stream.WriteL( *tempPtr );
            CleanupStack::PopAndDestroy( tempPtr );
            tempPtr = NULL;

            CleanupStack::PopAndDestroy( encodeTemp );
            encodeTemp = NULL;
            stream.WriteL( KItemDateEnd );
            }
        }
 
    RUPnPElementsArray elArray;
    CleanupClosePushL( elArray );
    UPnPItemUtility::GetResElements( *iItem, elArray );
    TInt count = elArray.Count();
    for( TInt i = 0; i < count; i++ )
        {
        // Res-element starts
        stream.WriteL( KItemPInfoBegin );
        
        const CUpnpAttribute* attrproinfo = UPnPItemUtility
            ::FindAttributeByName( *elArray[ i ], KAttributeProtocolInfo );
        if ( attrproinfo )
            {        
            stream.WriteL( attrproinfo->Value() );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( KCriteriaSpace );
            }
        const CUpnpAttribute* attrsize = UPnPItemUtility
            ::FindAttributeByName( *elArray[ i ], KAttributeSize );            
        if ( attrsize )
            {
            stream.WriteL( KAttributeSize );
            stream.WriteL( KCriteriaEQ );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( attrsize->Value() );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( KCriteriaSpace );                   
            }         
        const CUpnpAttribute* attrresolution = UPnPItemUtility
            ::FindAttributeByName( *elArray[ i ], KAttributeResolution );
        if ( attrresolution )
            {
            stream.WriteL( KAttributeResolution );
            stream.WriteL( KCriteriaEQ );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( attrresolution->Value() );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( KCriteriaSpace );                        
            }        
        const CUpnpAttribute* attrduration = UPnPItemUtility
            ::FindAttributeByName( *elArray[ i ], KAttributeDuration );        
        if ( attrduration )
            {           
            if( ValidateDurationL( attrduration->Value() ) )
                {
                stream.WriteL( KAttributeDuration );
                stream.WriteL( KCriteriaEQ );
                stream.WriteL( KCriteriaQuot );
                stream.WriteL( attrduration->Value() );
                stream.WriteL( KCriteriaQuot );
                stream.WriteL( KCriteriaSpace );
                }
            else
                {
                // Format of duration is not valid, do not include it
                // Fixes ESLX-7AYFD6
                }    
            
            }        
        const CUpnpAttribute* attrbitrate = UPnPItemUtility
            ::FindAttributeByName( *elArray[ i ], KAttributeBitrate );
        if ( attrbitrate )
            {
            stream.WriteL( KAttributeBitrate );
            stream.WriteL( KCriteriaEQ );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( attrbitrate->Value() );
            stream.WriteL( KCriteriaQuot );
            stream.WriteL( KCriteriaSpace );
            } 
        stream.WriteL( KItemPInfoMiddle );
        
        // Get the content URI
        encodeTemp = HBufC8::NewLC( elArray[ i ]->Value().Length() );
        encodeTemp->Des().Copy( elArray[ i ]->Value() );

        // Encode the content URI
        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        // Write the encoded content URI
        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
        
        // Clean up
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;
        
        // Res-element ends
        stream.WriteL( KItemPInfoEnd );
        }
    
    CleanupStack::PopAndDestroy( &elArray );                  

    stream.WriteL( KItemEnd() );

    CleanupStack::PopAndDestroy(); // stream.Close();

    TPtrC8 start = pBuf->Ptr(0);

    // JLi: Do NOT decode retBuffer since it will be given to XML Parser
    HBufC8* retBuffer = start.AllocL();
    CleanupStack::PopAndDestroy( pBuf );
    
    return retBuffer;
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::AsXmlEmptyL
// Returns object's XML description. This version of the method is used to
// create the XML with empty <res> tag.
// (other items were commented in a header).
// --------------------------------------------------------------------------
HBufC8* CUpnpItemToXML::AsXmlEmptyL()
    {
    __LOG( "CUpnpItemToXML::AsXmlEmptyL" );
    
    const TInt bufferSize = 64; // Buffer size, grows dynamicly in 64b steps
    CBufFlat *pBuf = CBufFlat::NewL( bufferSize );
    CleanupStack::PushL( pBuf );

    RBufWriteStream stream( *pBuf );
    CleanupClosePushL( stream );

    // Then add the actual data
    stream.WriteL( KItemHeading1() );
    stream.WriteL( iItem->Id() );
    stream.WriteL( KItemHeading2() );
    stream.WriteL( iItem->ParentId() );
    stream.WriteL( KItemHeading3False() );
    //stream.WriteL( KItemEndTag() );
    stream.WriteL( KItemTitleBegin() );
    // xml encode name
    HBufC8* encodeTemp = HBufC8::NewLC( iItem->Title().Length() );
    encodeTemp->Des().Copy( iItem->Title() );
    HBufC8* tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
    CleanupStack::PushL( tempPtr ); 
    stream.WriteL( *tempPtr );
    CleanupStack::PopAndDestroy( tempPtr );
    CleanupStack::PopAndDestroy( encodeTemp );
    stream.WriteL( KItemTitleEnd() );
    stream.WriteL( KItemClassBegin() );
    
    stream.WriteL( iItem->ObjectClass() );
    stream.WriteL( KItemClassEnd() );
    
    // Music meta data information
    const TDesC8& artist = GetValueFromElement( KElementArtist ); 
    if ( artist != KNullDesC8 )
        {
        stream.WriteL( KItemArtistBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( artist.Length() );
        encodeTemp->Des().Copy( artist );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
        
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;

        stream.WriteL( KItemArtistEnd );
        }
    const TDesC8& creator = GetValueFromElement( KElementCreator ); 
    if ( creator != KNullDesC8 )
        {
        stream.WriteL( KItemCreatorBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( creator.Length() );
        encodeTemp->Des().Copy( creator );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
        
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;

        stream.WriteL( KItemCreatorEnd );        
        }
    const TDesC8& album = GetValueFromElement( KElementAlbum ); 
    if ( album != KNullDesC8 )
        {
        stream.WriteL( KItemAlbumBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( album.Length() );
        encodeTemp->Des().Copy( album );
        
        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;
        //stream.WriteL( Album() );
        stream.WriteL( KItemAlbumEnd );        
        }
    const TDesC8& genre = GetValueFromElement( KElementGenre );     
    if ( genre != KNullDesC8 )
        {
        stream.WriteL( KItemGenreBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( genre.Length() );
        encodeTemp->Des().Copy( genre );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;
        //stream.WriteL( Genre() );
        stream.WriteL( KItemGenreEnd );        
        }
    const TDesC8& albumarturi = GetValueFromElement( KElementAlbumArtUri );     
    if ( albumarturi != KNullDesC8 )
        {
        stream.WriteL( KItemAlbumArtURIBegin );
        // xml encoding added
        encodeTemp = HBufC8::NewLC( albumarturi.Length() );
        encodeTemp->Des().Copy( albumarturi );

        tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
        CleanupStack::PushL( tempPtr );

        stream.WriteL( *tempPtr );
        CleanupStack::PopAndDestroy( tempPtr );
        tempPtr = NULL;
    
        CleanupStack::PopAndDestroy( encodeTemp );
        encodeTemp = NULL;       
        stream.WriteL( KItemAlbumArtURIEnd );        
        }
    const TDesC8& date = GetValueFromElement( KElementDate );         
    if ( date != KNullDesC8 )
        {
        if( ValidateDateL( date ) )
            {
            stream.WriteL( KItemDateBegin );
            // xml encoding added
            encodeTemp = HBufC8::NewLC( date.Length() );
            encodeTemp->Des().Copy( date );

            tempPtr = UpnpString::EncodeXmlStringL( encodeTemp );
            CleanupStack::PushL( tempPtr );

            stream.WriteL( *tempPtr );
            CleanupStack::PopAndDestroy( tempPtr );
            tempPtr = NULL;

            CleanupStack::PopAndDestroy( encodeTemp );
            encodeTemp = NULL;
            stream.WriteL( KItemDateEnd );
            }       
        }
    
    // Create dlna compliant protocolinfo
    const RUPnPElementsArray& elms = iItem->GetElements();
    CUpnpDlnaProtocolInfo* pInfo = NULL; 
    HBufC8* duration  = NULL;
    HBufC8* size  = NULL;
    HBufC8* resolution  = NULL;
    HBufC8* info = NULL;
    for( TInt i = 0; i < elms.Count(); i++)
        {
        if( elms[ i ]->Name() == KElementRes )
            {
            const RUPnPAttributesArray& attr = elms[ i ]->GetAttributes();
            for( TInt j = 0; j < attr.Count(); j++ )
                {
                if( attr[ j ]->Name() == KAttributeProtocolInfo )
                    {
                    pInfo = CUpnpDlnaProtocolInfo::NewL( attr[ j ]->Value() );
                    if ( pInfo )
                        {
                        CleanupStack::PushL( pInfo );
                        info = HBufC8::NewLC( KBufLen );
                        }
                    }
                if ( attr[ j ]->Name() == KAttributeDuration )
                    {
                    duration= HBufC8::NewLC( attr[ j ]->Value().Length() );
                    duration->Des().Copy( attr[ j ]->Value() );
                    }
                if ( attr[ j ]->Name() == KAttributeSize )
                    {
                    size = HBufC8::NewLC( attr[ j ]->Value().Length() );
                    size->Des().Copy( attr[ j ]->Value() );
                    }
                if ( attr[ j ]->Name() == KAttributeResolution )
                    {
                    resolution = HBufC8::NewLC( attr[ j ]->Value().Length() );
                    resolution->Des().Copy( attr[ j ]->Value() );
                    }
                }
            i = elms.Count();
            }
        }
    
    if( pInfo )
        {
        info->Des().Copy( KItemPInfoEmptyDlna );
        info->Des().Append( pInfo->ThirdField() ); // Third param )
        info->Des().Append( KColon );
        if( pInfo->PnParameter().Length() > 0 )
            {
            info->Des().Append( KDlnaPn );
            info->Des().Append( pInfo->PnParameter() ); // Fourth param )
            }
        else
            {
            info->Des().Append( KAsterisk );
            }    
        if ( resolution )
            {
            info->Des().Append( KQuotationMark );
            info->Des().Append( KItemResolution );
            info->Des().Append( *resolution );
            CleanupStack::PopAndDestroy( resolution );
            resolution = NULL;
            }
        if ( duration )
            {
            info->Des().Append( KQuotationMark );
            info->Des().Append( KItemDuration );
            info->Des().Append( *duration );
            CleanupStack::PopAndDestroy( duration );
            duration = NULL;
            }
        if ( size )
            {
            info->Des().Append( KQuotationMark );
            info->Des().Append( KItemSize );
            info->Des().Append( *size );
            CleanupStack::PopAndDestroy( size );
            size = NULL;
            }
        
        info->Des().Append( KItemPInfoEnd2 );
        stream.WriteL( *info );
        CleanupStack::PopAndDestroy( info );
        CleanupStack::PopAndDestroy( pInfo );
        
        }
    else
        {
        stream.WriteL( KItemPInfoEmpty );    
        }    
            
    stream.WriteL( KItemEnd() );

    CleanupStack::PopAndDestroy( &stream );

    TPtrC8 start = pBuf->Ptr(0);

    HBufC8* tmpBuffer = start.AllocL();

    CleanupStack::PopAndDestroy( pBuf );

    return tmpBuffer;
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::AsResultArgumentL
// Returns object's XML description that is embedded inside a DIDL-LITE tag. 
// The <res> tag of xml description is empty.The returned value is xml encoded
// can therefore be used for example when creating a CreateObject action.
// (other items were commented in a header).
// --------------------------------------------------------------------------
HBufC8* CUpnpItemToXML::AsResultArgumentL()
    {
    __LOG( "CUpnpItemToXML::AsResultArgumentL" );
    
    HBufC8* asEmptyXml = this->AsXmlEmptyL();
    CleanupStack::PushL( asEmptyXml );
    
    // Xml must be encoded because eventually it will be embedded inside 
    // another Xml tag (Elements tag in CreateObject action).
    HBufC8* encodedItem = UpnpString::EncodeXmlStringL( asEmptyXml );
    CleanupStack::PopAndDestroy( asEmptyXml );
    CleanupStack::PushL( encodedItem );
    
    // Put item xml inside DIDL-LITE tag which must also be xml encoded.
    HBufC8* retval = CreateUnDecodedXmlL( *encodedItem );
    CleanupStack::PopAndDestroy( encodedItem );
    
    return retval;
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::CreateUnDecodedXmlL
// Fills common DIDL-Lite XML headers over the given XML fragment.
// --------------------------------------------------------------------------
HBufC8* CUpnpItemToXML::CreateUnDecodedXmlL( const TDesC8& aData )
    {
    __LOG( "CUpnpItemToXML::CreateUnDecodedXmlL" );
    
    const TInt bufferSize = 128; // Buffer size
    CBufFlat *pBuf = CBufFlat::NewL( bufferSize );
    CleanupStack::PushL( pBuf );

    RBufWriteStream stream( *pBuf );
    CleanupClosePushL( stream );

    stream.WriteL( KDIDLBeginXmlEscaped() );
    stream.WriteL( aData );
    stream.WriteL( KDIDLEndXmlEscaped() );

    CleanupStack::PopAndDestroy( &stream ); // stream.Close();

    TPtrC8 start = pBuf->Ptr(0);

    HBufC8* tmpBuffer = start.AllocL();

    CleanupStack::PopAndDestroy( pBuf );
    return tmpBuffer;
    }
    
// --------------------------------------------------------------------------
// CUpnpItemToXML::GetValueFromElement
// Returns the value of an element.
// --------------------------------------------------------------------------
const TDesC8& CUpnpItemToXML::GetValueFromElement(
    const TDesC8& aElementName )
    {
    const RUPnPElementsArray& elms = iItem->GetElements();
    TInt count = elms.Count();
    for( TInt i = 0; i < count; i++)
        {
        if( elms[ i ]->Name() == aElementName )
            {
            return elms[ i ]->Value();
            }
        }
    return KNullDesC8;              
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::ValidateDateL
// Validates dc:date
// --------------------------------------------------------------------------
TBool CUpnpItemToXML::ValidateDateL( const TDesC8& aDate )
    {
    TDateTime time;    
    TBuf<KMaxDateStringLength> formatDateString;
    HBufC* dateString = HBufC::NewL( aDate.Length() );
    dateString->Des().Copy( aDate );
    TInt err = KErrNone;
    TInt year = 0;
    TInt month = 1; 
    TInt day = 1;
    TInt hours = 0;
    TInt minutes = 0;
    TInt seconds = 0;

    if( aDate.Length() >= KDateStringLength )
    {
        TLex lex( dateString->Des().Left( 4 ) ); //Year
        err = lex.Val( year );
        if(  err == KErrNone )
        {
            lex.Assign( dateString->Des().Mid( 5,2 ) ); //Month
            TInt err = lex.Val( month );
            if(  err == KErrNone )
            {
                lex.Assign( dateString->Des().Mid( 8,2 ) ); //Day
                TInt err = lex.Val(day);
            }
        }
    }
        
    if (err)
    {
        return EFalse;
    }
        
    if( aDate.Length() >= KDateTimeStringLength )
    {
        TLex lex( dateString->Des().Mid( 11,2 ) ); //Hours
        err = lex.Val( hours );
        if(  err == KErrNone )
        {
            lex.Assign( dateString->Des().Mid( 14,2 ) ); //Minutes
            TInt err = lex.Val( hours );
            if(  err == KErrNone )
            {
                lex.Assign( dateString->Des().Mid( 17,2 ) ); //Seconds
                TInt err = lex.Val( hours );
            }
        }
    }

    if (err)
    {
        return EFalse;
    }
        
    TBool retVal = EFalse;
    //DateTime month and day are 0-based
    if( time.Set( year, TMonth(month - 1), 
            day - 1, hours, minutes, seconds,0 ) == KErrNone )
        {
        retVal = ETrue;
        }
    
    delete dateString;
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CUpnpItemToXML::ValidateDurationL
// Validates res@duration
// --------------------------------------------------------------------------
TBool CUpnpItemToXML::ValidateDurationL( const TDesC8& aDuration )
    {
    
    TBool retVal = ETrue;
    TLex8 input( aDuration );
    
    // Hours
    ParseToDelimeter( input, TChar( KSeparatorAscii ) );
    TInt hours = input.MarkedToken().Length();
    if( !input.Eos() )
        {
        input.SkipAndMark( 1 ); // Skip the delimeter
        }
    if( hours < 1 || hours > 5 ) // hours must be 1-5 digits long
        {
        retVal = EFalse;
        }
    else
        {
        // Minutes
        ParseToDelimeter( input, TChar( KSeparatorAscii ) );   
        TInt minutes = input.MarkedToken().Length();
        if( !input.Eos() )
            {
            input.SkipAndMark( 1 ); // Skip the delimeter
            }

        if( minutes != 2 ) // minutes must be 2 digits long
            {
            retVal = EFalse;
            }
        else
            {
            // Seconds
            ParseToDelimeter( input, TChar( KSeparatorAscii ) );
            TInt seconds = input.MarkedToken().Length();
            if( seconds < 2 || seconds > 6 ) // seconds must be 2-6 digits
            // long
                {
                retVal = EFalse;
                }

            if( !input.Eos() )
                {
                // Something is wrong, we should be in the end        
                retVal = EFalse;
                }            
            }    
        }    
  
    return retVal;
    }
    
// --------------------------------------------------------------------------
// CUpnpItemToXML::ParseToDelimeter
// Parse to a given delimeter
// --------------------------------------------------------------------------
void CUpnpItemToXML::ParseToDelimeter( TLex8& aLex, TChar aDelimeter )
    {
    aLex.Mark();
        
    TChar chr = 0;
    
    while( !aLex.Eos() )
        {
        chr = aLex.Peek();
        if( chr == aDelimeter )
            {
            break;
            }
            
        aLex.Inc();        
        }
    }    
    

//  End of File
