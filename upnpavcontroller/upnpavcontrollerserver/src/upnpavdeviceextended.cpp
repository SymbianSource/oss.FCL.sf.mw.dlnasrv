/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      device with extended information - used in AVC server
*
*/






// INCLUDES
#include <in_sock.h>

// dlnasrv / mediaserver api
#include <upnpdlnaprotocolinfo.h>
#include <upnpitem.h>

// dlnasrv / avcontroller helper api
#include "upnpitemutility.h"
#include "upnpconstantdefs.h" // for upnp-specific stuff

// dlnasrv / internal api's
#include "upnpcommonutils.h"

// dlnasrv / avcontroller internal
#include "upnpavdeviceextended.h"


_LIT8( KAudioSupport,        "audio/" );
_LIT8( KImageSupport,        "image/" );
_LIT8( KVideoSupport,        "video/" );
_LIT8( KDlnaPn,              "DLNA.ORG_PN" );


const TInt KProtocolInfoDelimeter = 44;
const TInt KUnicodeC0RangeStart = 0;// the begin character of C0 range
const TInt KUnicodeC0RangeEnd = 32;// the end character of C0 range 
const TInt KUnicodeC1RangeStart = 127;// the begin character of C1 range
const TInt KUnicodeC1RangeEnd = 159;// the end character of C1 range
const TInt KSlash = 92;
_LIT8( KProtocolInfo,       "protocolInfo" );
_LIT8( KAsterisk,           "*" );

const TInt KUrlIpBufLen = 16; // For IP address e.g. xxx.xxx.xxx.xxx
const TInt KUrlPortLen = 11; // For HTTP port e.g. yyyyy
_LIT8( KUrlColon, ":" );
_LIT8( KUrlSlash, "/" );
_LIT8( KUrlHttp, "http://" );

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

// --------------------------------------------------------------------------
// EnsureFinalSlash
// --------------------------------------------------------------------------
static void EnsureFinalSlash( TDes8& aUrl )
    {
    TInt len( aUrl.Length() );
    if ( len > 0 && aUrl[ len - 1 ] != KUrlSlash()[ 0 ] )
        {
        aUrl.Append( KUrlSlash );
        }
    }

// --------------------------------------------------------------------------
// RemoveInitial
// --------------------------------------------------------------------------
static TPtrC8 RemoveInitial( const TDesC8& aUrl, const TDesC8& aInitial )
    {
    return aUrl.Find( aInitial ) == 0 ? aUrl.Mid( aInitial.Length() ) : aUrl;
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::NewL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended* CUpnpAVDeviceExtended::NewL(
    const CUpnpAVDevice& aDevice )
    {
    CUpnpAVDeviceExtended* dev = new(ELeave) CUpnpAVDeviceExtended();
    CleanupStack::PushL( dev );
    
    dev->iDeviceType = aDevice.DeviceType();
    dev->SetFriendlyNameL( aDevice.FriendlyName() );
    dev->SetUuidL( aDevice.Uuid() );
    dev->iCopyCapability = aDevice.CopyCapability();
    dev->iSearchCapability = aDevice.SearchCapability();
    dev->iPauseCapability = aDevice.PauseCapability();
    dev->iVolumeCapability = aDevice.VolumeCapability();
    dev->iMuteCapability = aDevice.MuteCapability();
    dev->iAudioMediaCapability = aDevice.AudioCapability();
    dev->iImageMediaCapability = aDevice.ImageCapability();
    dev->iVideoMediaCapability = aDevice.VideoCapability();
    dev->iNextAVTransportUri = aDevice.NextAVTransportUri();
    dev->iMaxVolume = aDevice.MaxVolume();
    dev->iDlnaCompatible = aDevice.DlnaCompatible();
    dev->iSeekCapability = aDevice.SeekCapability();
           
    dev->ConstructL();
    CleanupStack::Pop();
    return dev;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::NewL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended* CUpnpAVDeviceExtended::NewL(
    const CUpnpAVDeviceExtended& aDevice )
    {
    CUpnpAVDeviceExtended* dev = new(ELeave) CUpnpAVDeviceExtended();
    CleanupStack::PushL( dev );
    
    dev->iDeviceType = aDevice.DeviceType();
    dev->SetFriendlyNameL( aDevice.FriendlyName() );
    dev->SetUuidL( aDevice.Uuid() );
    dev->iCopyCapability = aDevice.CopyCapability();
    dev->iSearchCapability = aDevice.SearchCapability();
    dev->iPauseCapability = aDevice.PauseCapability();
    dev->iVolumeCapability = aDevice.VolumeCapability();
    dev->iMuteCapability = aDevice.MuteCapability();
    dev->iAudioMediaCapability = aDevice.AudioCapability();
    dev->iImageMediaCapability = aDevice.ImageCapability();
    dev->iVideoMediaCapability = aDevice.VideoCapability();
    dev->iNextAVTransportUri = aDevice.NextAVTransportUri();
    dev->iMaxVolume = aDevice.MaxVolume();
    dev->iDlnaCompatible = aDevice.DlnaCompatible();
    dev->iSeekCapability = aDevice.SeekCapability();
        
    dev->SetSinkProtocolInfoL( aDevice.SinkProtocolInfo() );
    dev->SetSourceProtocolInfoL( aDevice.SourceProtocolInfo() );
    dev->iSubscriptionCount = aDevice.SubscriptionCount();   
    dev->iLocal = aDevice.Local();
    dev->iAudioUpload = aDevice.AudioUpload();
    dev->iImageUpload = aDevice.ImageUpload();
    dev->iVideoUpload = aDevice.VideoUpload(); 
    dev->iCreateChildContainer = aDevice.CreateChildContainer();
    dev->iDestroyObject = aDevice.DestroyObject();
    dev->iPInfoReceived = aDevice.PInfoReceived();
    dev->iDLNADeviceType = aDevice.DLNADeviceType();
    dev->iPrepareForConnection = aDevice.PrepareForConnection();
    
    dev->ConstructL();
    CleanupStack::Pop();
    return dev;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::NewL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended* CUpnpAVDeviceExtended::NewL()
    {
    CUpnpAVDeviceExtended* dev = new(ELeave) CUpnpAVDeviceExtended();
    CleanupStack::PushL( dev );        
    dev->ConstructL();
    CleanupStack::Pop();
    return dev;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::~CUpnpAVDeviceExtended
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended::~CUpnpAVDeviceExtended()
    {
    delete iIconUrl;
    iSinkProtocolInfo.ResetAndDestroy();
    iSourceProtocolInfo.ResetAndDestroy();
    }
    

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::CUpnpAVDeviceExtended
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended::CUpnpAVDeviceExtended() :
    CUpnpAVDevice()
    {     
    }
   
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::IncreaseSubscriptionCount
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TInt CUpnpAVDeviceExtended::IncreaseSubscriptionCount()
    {
    return ++iSubscriptionCount; 
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::DecreaseSubscriptionCount
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TInt CUpnpAVDeviceExtended::DecreaseSubscriptionCount()
    {
    iSubscriptionCount--;
    if( iSubscriptionCount < 0 )
        {
        iSubscriptionCount = 0;
        }
    return iSubscriptionCount;    
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SubscriptionCount
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TInt CUpnpAVDeviceExtended::SubscriptionCount() const
    {
    return iSubscriptionCount;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::ConstructL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetSinkProtocolInfoL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetSinkProtocolInfoL(
    const TDesC8& aProtocolInfo )
    {
    __LOG( "CUpnpAVDeviceExtended::SetSinkProtocolInfoL" );
    HBufC8* buffer = RemoveIllegalCharactersL( aProtocolInfo );
    if( buffer )
        {
        CleanupStack::PushL( buffer );
        TLex8 input( *buffer );
        
        while( !input.Eos() )
            {
            ParseToDelimeter( input, TChar( KProtocolInfoDelimeter ) );
            CUpnpDlnaProtocolInfo* tmpInfo = NULL;
            TRAPD( err, tmpInfo = CUpnpDlnaProtocolInfo::NewL(
                input.MarkedToken() ) );
             
            if( err == KErrNone && tmpInfo )
                {
                // Transfer ownership of tmpInfo
                iSinkProtocolInfo.Append( tmpInfo );
                }
            else
                {
                __LOG1( "CUpnpDlnaProtocolInfo::NewL failed: %d", err );
                }
                
            if( !input.Eos() )
                {
                input.SkipAndMark( 1 ); // Skip the delimeter
                }
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    __LOG( "CUpnpAVDeviceExtended::SetSinkProtocolInfoL end" );
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SinkProtocolInfo
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
const RPointerArray<CUpnpDlnaProtocolInfo>&
    CUpnpAVDeviceExtended::SinkProtocolInfo() const
    {
    return iSinkProtocolInfo;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetSourceProtocolInfoL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetSourceProtocolInfoL(
    const TDesC8& aProtocolInfo )
    {
    __LOG( "CUpnpAVDeviceExtended::SetSourceProtocolInfoL" );
    HBufC8* buffer = RemoveIllegalCharactersL( aProtocolInfo );
    if( buffer )
        {
        CleanupStack::PushL( buffer );
        TLex8 input( *buffer );
        
        while( !input.Eos() )
            {
            ParseToDelimeter( input, TChar( KProtocolInfoDelimeter ) );
            CUpnpDlnaProtocolInfo* tmpInfo = NULL;
            TRAPD( err, tmpInfo = CUpnpDlnaProtocolInfo::NewL(
                input.MarkedToken() ) );
               
            if( err == KErrNone && tmpInfo )
                {
                // Transfer ownership of tmpInfo
                iSourceProtocolInfo.Append( tmpInfo );
                }
            else
                {
                __LOG1( "CUpnpDlnaProtocolInfo::NewL failed: %d", err );
                }
                
            if( !input.Eos() )
                {
                input.SkipAndMark( 1 ); // Skip the delimeter
                }
            }
        CleanupStack::PopAndDestroy( buffer );
        }
    __LOG( "CUpnpAVDeviceExtended::SetSourceProtocolInfoL end" );
    }
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SourceProtocolInfo
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
const RPointerArray<CUpnpDlnaProtocolInfo>&
    CUpnpAVDeviceExtended::SourceProtocolInfo() const
    {
    return iSourceProtocolInfo;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetLocal
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetLocal( TBool aLocal )
    {
    iLocal = aLocal;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::Local
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::Local() const
    {
    return iLocal;
    }    

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchSinkProtocolInfo
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::MatchSinkProtocolInfo(
    const TDesC8& aInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchSinkProtocolInfo" );

    TBool match = EFalse;

    if( DlnaCompatible() && DLNADeviceType() == CUpnpAVDeviceExtended::EDMR )
        {
        // The device is DLNA compatible
        
        // Try try find PN parameter to determine if it's dlna content
        if( aInfo.Find( KDlnaPn ) != KErrNotFound )
            {
            __LOG( "MatchSinkProtocolInfo - DLNA content and the renderer \
is DLNA compatible, start matching..." );

            match = MatchSinkProfileId( aInfo );
            if( !match )
                {
                // if profile id was not found try matching mime 
                match = MatchSinkMime( aInfo );
                }
            }
        else
            {
            __LOG( "MatchSinkProtocolInfo - Non DLNA content and the \
renderer is DLNA compatible, start matching..." );            
            match = MatchSinkMime( aInfo );
            }    
        }
    else
        {
        __LOG( "MatchSinkProtocolInfo - Renderer is not DLNA compatible, \
start matching..." );            
        match = MatchSinkMime( aInfo );
        }    
        
    return match;
    }      

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchSourceProtocolInfo
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::ValidateTransfer(
    const TDesC8& aInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchSourceProtocolInfo" );

    // Try try find PN parameter to determine if it's dlna content
    TBool match = EFalse;
    if( aInfo.Find( KDlnaPn ) != KErrNotFound )
        {
        match = MatchSourceProfileId( aInfo );     
        }

    return match;
    }      

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchSinkProfileId
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::MatchSinkProfileId(
    const TDesC8& aInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchSinkProfileId" );
    
    TBool match = EFalse;
    CUpnpDlnaProtocolInfo* tmpInfo = NULL;
    TRAPD( err, tmpInfo = CUpnpDlnaProtocolInfo::NewL( aInfo ) );
    if ( err == KErrNone )
        {      
        // Match the first parameter and PN parameter
        TInt count = iSinkProtocolInfo.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( iSinkProtocolInfo[ i ]->PnParameter() ==
                tmpInfo->PnParameter() )
                {
                // PN parameter matches, try matching the first
                // parameter
                if( iSinkProtocolInfo[ i ]->FirstField() ==
                    tmpInfo->FirstField() ||
                    iSinkProtocolInfo[ i ]->FirstField() ==
                    KAsterisk )
                    {
                    __LOG( "MatchSinkProfileId - a match" );
                    
                    // We have a match!
                    i = count;
                    match = ETrue;
                    }
                }     
            }
        delete tmpInfo;
        }            
    return match;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchSourceProfileId
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::MatchSourceProfileId(
    const TDesC8& aInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchSourceProfileId" );
    
    TBool match = EFalse;
    CUpnpDlnaProtocolInfo* tmpInfo = NULL;
    TRAPD( err, tmpInfo = CUpnpDlnaProtocolInfo::NewL( aInfo ) );
    if ( err == KErrNone )
        {      
        // Match the first parameter and PN parameter
        TInt count = iSourceProtocolInfo.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( iSourceProtocolInfo[ i ]->PnParameter() ==
                tmpInfo->PnParameter() )
                {
                // PN parameter matches, try matching the first
                // parameter
                if( iSourceProtocolInfo[ i ]->FirstField() ==
                    tmpInfo->FirstField() ||
                    iSourceProtocolInfo[ i ]->FirstField() ==
                    KAsterisk )
                    {
                    __LOG( "MatchSourceProfileId - a match" );
                                        
                    // We have a match!
                    i = count;
                    match = ETrue;
                    }
                }     
            }
        delete tmpInfo;
        }            
    return match;
    }
   
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchSinkMime
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::MatchSinkMime( const TDesC8& aInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchSinkMime" );

    TBool match = EFalse;
    CUpnpDlnaProtocolInfo* tmpInfo = NULL;
    TRAPD( err, tmpInfo = CUpnpDlnaProtocolInfo::NewL( aInfo ) );
    if ( err == KErrNone )
        {      
        // Match the first parameter and mime-type
        TInt count = iSinkProtocolInfo.Count();
        for( TInt i = 0; i < count; i++ )
            {
            if( iSinkProtocolInfo[ i ]->ThirdField() ==
                tmpInfo->ThirdField() )
                {
                // Mime-parameter matches, try matching the first
                // parameter
                if( iSinkProtocolInfo[ i ]->FirstField() ==
                    tmpInfo->FirstField() )
                    {
                    __LOG( "MatchSinkMime - a match" );
                    
                    // We have a match!
                    i = count;
                    match = ETrue;
                    }
                }     
            }
        delete tmpInfo;
        }            
    return match;    
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::FindFirstMatchingInSinkL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
const TDesC8& CUpnpAVDeviceExtended::FindFirstMatchingInSinkL(
    const CUpnpItem& aItem ) const
    {
    __LOG( "CUpnpAVDeviceExtended::FindFirstMatchingInSinkL" );
    
    RUPnPElementsArray array;
    CleanupClosePushL( array );
    
    UPnPItemUtility::GetResElements( aItem, array );
    TBool match = EFalse;
    TInt i;
    
    TInt count = array.Count();
    for( i = 0; i < count; i ++ )
        {
        const CUpnpAttribute& protocolInfo =
            UPnPItemUtility::FindAttributeByNameL(
            *array[ i ], KProtocolInfo );
            
        if( MatchType( aItem.ObjectClass(), protocolInfo.Value() ) )
            {
            if( MatchSinkProtocolInfo( protocolInfo.Value() ) )
                {
                // We have a match!
                __LOG( "FindFirstMatchingInSinkL - a match" );
                
                match = ETrue;
                break;
                }
            else
                {
                __LOG( "FindFirstMatchingInSinkL - not a match" );
                }                
            }
        else
            {
            // Res-elements mime-type does not match to object-class
            // Ignore
            __LOG( "FindFirstMatchingInSinkL - Res doesn't match \
to objectclass" );
            }              
        }
    
    if( !match )
        {
        __LOG( "FindFirstMatchingInSinkL - No match" );
        
        User::Leave( KErrNotSupported );
        }
    
    const TDesC8& uri = array[ i ]->Value();
    
    CleanupStack::PopAndDestroy( &array );
    
    return uri;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::MatchType
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::MatchType( const TDesC8& aObjectClass,
    const TDesC8& aProtocolInfo ) const
    {
    __LOG( "CUpnpAVDeviceExtended::MatchType" );
    
    TBool retVal = EFalse;
    if( aObjectClass.Find( KClassAudio ) == 0 )
        {
        if( aProtocolInfo.Find( KAudioSupport ) >= 0 )
            {
            retVal = ETrue;
            }
        }
    else if( aObjectClass.Find( KClassImage ) == 0 )
        {
        if( aProtocolInfo.Find( KImageSupport ) >= 0 )
            {
            retVal = ETrue;
            }        
        }
    else if( aObjectClass.Find( KClassVideo ) == 0 )
        {
        if( aProtocolInfo.Find( KVideoSupport ) >= 0 )
            {
            retVal = ETrue;
            }        
        }
    else
        {
        __PANICD( __FILE__, __LINE__ );
        }
    return retVal;        
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetCapabilitiesBySupportedMimeTypesL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetCapabilitiesBySupportedMimeTypesL( 
    const TDesC8& aListOfMimeTypes )
    {
    __LOG( "CUpnpAVDeviceExtended::SetCapabilitiesBySupportedMimeTypesL" );
    
    if( aListOfMimeTypes != KNullDesC8 )
        {
        // Update the audio media capability
        if( UPnPCommonUtils::IsAudioSupported( aListOfMimeTypes ) )
            {
            iAudioMediaCapability = ETrue;
            }
        else
            {
            iAudioMediaCapability = EFalse;
            }

        // Update the audio media capability
        if( UPnPCommonUtils::IsImageSupported( aListOfMimeTypes ) )
            {
            iImageMediaCapability = ETrue;
            }
        else
            {
            iImageMediaCapability = EFalse;
            }
  
        // Update the video media capability
        if( UPnPCommonUtils::IsVideoSupported( aListOfMimeTypes ) )
            {
            iVideoMediaCapability = ETrue;
            }
        else
            {
            iVideoMediaCapability = EFalse;
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetSourceProtocolInfoL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetSourceProtocolInfoL(
        const RPointerArray<CUpnpDlnaProtocolInfo>& aProtocolInfo )
    {
    __LOG( "CUpnpAVDeviceExtended::SetSourceProtocolInfoL" );
    
    for( TInt i = 0; i < aProtocolInfo.Count(); i++ )
        {
        CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
            aProtocolInfo[ i ]->ProtocolInfoL() );
        iSourceProtocolInfo.Append( tmpInfo ); // Ownership is transferred
        }    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetSinkProtocolInfoL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetSinkProtocolInfoL(
        const RPointerArray<CUpnpDlnaProtocolInfo>& aProtocolInfo )
    {
    __LOG( "CUpnpAVDeviceExtended::SetSinkProtocolInfoL" );
    
    for( TInt i = 0; i < aProtocolInfo.Count(); i++ )
        {
        CUpnpDlnaProtocolInfo* tmpInfo = CUpnpDlnaProtocolInfo::NewL(
            aProtocolInfo[ i ]->ProtocolInfoL() );
        iSinkProtocolInfo.Append( tmpInfo ); // Ownership is transferred
        }
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::ParseToDelimeter
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::ParseToDelimeter( TLex8& aLex, TChar aDelimeter )
    {
    aLex.Mark();

    TChar chr = 0;
    TChar edchr = 0;

    while( !aLex.Eos() )
        {
        edchr = chr;
        chr = aLex.Peek();
        if( chr == aDelimeter && edchr != TChar( KSlash ) )
            {
            break;
            }
        aLex.Inc();
        }
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::RemoveIllegalCharactersL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
 HBufC8* CUpnpAVDeviceExtended::RemoveIllegalCharactersL(
    const TDesC8& aPtr ) const
    {
    HBufC8* ptrResult = NULL;
    TInt i = KErrNotFound;
    if ( aPtr.Length() != 0 )
        {
        ptrResult = aPtr.AllocL();
        CleanupStack::PushL( ptrResult );    
        TPtr8 ptr = ptrResult->Des();
        while( ++i < ptr.Length() )
            {       
            if( IsIllegalCharacter( ptr[i] ) )
                {     
                ptr.Delete( i, 1 );
                i--;
                }
                               
            }       
        CleanupStack::Pop( ptrResult );       
        } 
    return ptrResult;
   
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::IsIllegalCharacter
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::IsIllegalCharacter( TChar aCharacter ) const
    {
    
    TBool retVal = EFalse;
    if ( ( ( aCharacter >= TChar( KUnicodeC0RangeStart ) 
        && aCharacter <= TChar( KUnicodeC0RangeEnd ) ) 
        || ( aCharacter >= TChar( KUnicodeC1RangeStart ) 
        && aCharacter <= TChar( KUnicodeC1RangeEnd ) ) ) )
        {
        retVal = ETrue;
        }
    return retVal;
    
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetAudioUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetAudioUpload( TBool aAudioUpload )
    {
    iAudioUpload = aAudioUpload;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::AudioUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::AudioUpload() const
    {
    return iAudioUpload;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetImageUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetImageUpload( TBool aImageUpload )
    {
    iImageUpload = aImageUpload;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::ImageUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::ImageUpload() const
    {
    return iImageUpload;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetVideoUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetVideoUpload( TBool aVideoUpload )
    {
    iVideoUpload = aVideoUpload;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::VideoUpload
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::VideoUpload() const
    {
    return iVideoUpload;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetCreateChildContainer
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetCreateChildContainer(
    TBool aCreateChildContainer )
    {
    iCreateChildContainer = aCreateChildContainer;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::CreateChildContainer
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::CreateChildContainer() const
    {
    return iCreateChildContainer;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetDestroyObject
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetDestroyObject( TBool aDestroyObject )
    {
    iDestroyObject = aDestroyObject;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::DestroyObject
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::DestroyObject() const
    {
    return iDestroyObject;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetPInfoReceived
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetPInfoReceived( TBool aPInfoReceived )
    {
    iPInfoReceived = aPInfoReceived;
    }
    
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::PInfoReceived
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::PInfoReceived() const
    {
    return iPInfoReceived;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::PrepareForConnection
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TBool CUpnpAVDeviceExtended::PrepareForConnection() const
    {
    return iPrepareForConnection;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetPrepareForConnection
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetPrepareForConnection(
    TBool aPrepareForConnection )
    {
    iPrepareForConnection = aPrepareForConnection;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetDLNADeviceType
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetDLNADeviceType( TDLNADeviceType aDeviceType )
    {
    iDLNADeviceType = aDeviceType;
    }
   
// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::DLNADeviceType
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
CUpnpAVDeviceExtended::TDLNADeviceType
    CUpnpAVDeviceExtended::DLNADeviceType() const
    {
    return iDLNADeviceType;
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::SetIconUrlL
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
void CUpnpAVDeviceExtended::SetIconUrlL( const TInetAddr& aAddress,
    const TDesC8& aUrlBase, const TDesC8& aIconUrl )
    {
    __LOG8_1( "CUpnpAVDeviceExtended::SetIconUrlL - Base: %S", &aUrlBase );
    __LOG8_1( "CUpnpAVDeviceExtended::SetIconUrlL - Icon: %S", &aIconUrl );
    TBuf< KUrlIpBufLen > ip;
    aAddress.Output( ip );
    HBufC8* iconUrl = HBufC8::NewL( KUrlHttp().Length() + ip.Length() + 1 + // 1 for colon
                                    KUrlPortLen + 1 + // 1 for slash
                                    aUrlBase.Length() + 1 + // 1 for slash
                                    aIconUrl.Length() );
    TPtr8 ptr( iconUrl->Des() );
    if ( aUrlBase.Find( KUrlHttp ) == KErrNotFound )
        {
        ptr.Copy( KUrlHttp );
        ptr.Append( ip );
        ptr.Append( KUrlColon );
        ptr.AppendNumUC( aAddress.Port() );
        }
    EnsureFinalSlash( ptr );

    TPtrC8 urlBase( RemoveInitial( aUrlBase, KUrlSlash ) ); // Url base without initial slash
    ptr.Append( urlBase );
    EnsureFinalSlash( ptr );

    TPtrC8 icon( RemoveInitial( aIconUrl, KUrlSlash ) ); // Icon url without initial slash
    ptr.Append( RemoveInitial( icon, urlBase ) ); // Remove base from icon url if it already contains base

    delete iIconUrl;
    iIconUrl = iconUrl;

    __LOG8_1( "CUpnpAVDeviceExtended::SetIconUrlL - Device: %S", &( FriendlyName() ) );
    __LOG8_1( "CUpnpAVDeviceExtended::SetIconUrlL - Url: %S", iIconUrl );
    }

// --------------------------------------------------------------------------
// CUpnpAVDeviceExtended::IconUrl
// See upnpavdeviceextended.h
// --------------------------------------------------------------------------
TPtrC8 CUpnpAVDeviceExtended::IconUrl() const
    {
    return ( iIconUrl ? *iIconUrl : KNullDesC8() );
    }

// end of file
