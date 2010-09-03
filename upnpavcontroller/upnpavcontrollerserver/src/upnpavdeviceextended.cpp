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
// upnp stack api
#include <upnpdlnaprotocolinfo.h>
#include <upnpitem.h>

// upnpframework / avcontroller helper api
#include "upnpitemutility.h"
#include "upnpconstantdefs.h" // for upnp-specific stuff

// upnpframework / internal api's
#include "upnpcommonutils.h"

// avcontroller internal
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

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"

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
                iSinkProtocolInfo.AppendL( tmpInfo );
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
                iSourceProtocolInfo.AppendL( tmpInfo );
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

    if( DlnaCompatible() )
        {
        // The device is DLNA compatible
        
        // Try try find PN parameter to determine if it's dlna content
        if( aInfo.Find( KDlnaPn ) != KErrNotFound )
            {
            __LOG( "MatchSinkProtocolInfo - DLNA content and the renderer \
is DLNA compatible, start matching..." );

            match = MatchSinkProfileId( aInfo );
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
                tmpInfo->PnParameter() ||
                iSinkProtocolInfo[ i ]->FourthField() == KAsterisk )
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
        iSourceProtocolInfo.AppendL( tmpInfo ); // Ownership is transferred
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
        iSinkProtocolInfo.AppendL( tmpInfo ); // Ownership is transferred
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

void CUpnpAVDeviceExtended::SetDLNADeviceType( TDLNADeviceType aDeviceType )
    {
    iDLNADeviceType = aDeviceType;
    }
   
CUpnpAVDeviceExtended::TDLNADeviceType
    CUpnpAVDeviceExtended::DLNADeviceType() const
    {
    return iDLNADeviceType;
    }


    
// end of file
