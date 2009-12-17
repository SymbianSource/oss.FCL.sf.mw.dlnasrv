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
* Description:      a device data class used internally in AVC server
*
*/






#ifndef C_UPNPAVDEVICEEXTENDED_H_
#define C_UPNPAVDEVICEEXTENDED_H_

// Include files
#include <e32base.h>
#include "upnpavdevice.h"

// FORWARD DECLARATIONS
class CUpnpDlnaProtocolInfo;
class CUpnpItem;

/**
 * Extended UPnP device. Contains protocolinfo and a reference count for
 * subscibe actions.
 *
 */
class CUpnpAVDeviceExtended: public CUpnpAVDevice
    {
 
public:

    enum TDLNADeviceType
        {
        EUPnP = 0,
        EDMR,
        EDMP,
        EDMS
        }; 
 
public:

    /**
     * Static 1st phase constructor
     * 
     * @param aDevice base device
     * @return new instance
     */    
    static CUpnpAVDeviceExtended* NewL( const CUpnpAVDevice& aDevice );

    /**
     * Static 1st phase constructor
     * 
     * @param aDevice base device
     * @return new instance
     */    
    static CUpnpAVDeviceExtended* NewL(
        const CUpnpAVDeviceExtended& aDevice );

    /**
     * Static 1st phase constructor
     *
     * @return new timer instance
     */    
    static CUpnpAVDeviceExtended* NewL();

    /**
     * Destructor
     */    
    virtual ~CUpnpAVDeviceExtended();
    
private:

    /**
     * Private constructor
     */
    CUpnpAVDeviceExtended();

    /**
     * 2nd phase constructor
     */    
    void ConstructL();
    

public: // New functions

    /**
     * Sets sink protocolinfo
     * 
     * @param aProtocolInfo sink protocolinfo
     */    
    void SetSinkProtocolInfoL( const TDesC8& aProtocolInfo );
    
    /**
     * Returns devices sink protocolinfos as an array
     * 
     * @return array of protocolinfos
     */    
    const RPointerArray<CUpnpDlnaProtocolInfo>&
        SinkProtocolInfo() const;
    
    /**
     * Sets source protocolinfo
     * 
     * @param aProtocolInfo source protocolinfo
     */    
    void SetSourceProtocolInfoL( const TDesC8& aProtocolInfo );
    
    /**
     * Returns devices source protocolinfos as an array
     * 
     * @return array of protocolinfos
     */    
    const RPointerArray<CUpnpDlnaProtocolInfo>&
        SourceProtocolInfo() const;
    
    /**
     * Increases devices subscription count
     * 
     * @return subscription count
     */    
    TInt IncreaseSubscriptionCount();
    
    /**
     * Decreases devices subscription count
     * 
     * @return subscription count
     */    
    TInt DecreaseSubscriptionCount();
    
    /**
     * Returns subscription count
     * 
     * @return subscription count
     */    
    TInt SubscriptionCount() const;
    
    /**
     * Sets device capabilities
     * 
     * @param aListOfMimeTypes
     */    
    void SetCapabilitiesBySupportedMimeTypesL(
        const TDesC8& aListOfMimeTypes );
        
    /**
     * Sets if the device is a local device (S60 Media Server)
     * 
     * @param aLocal
     */    
    void SetLocal( TBool aLocal );
    
    /**
     * Return if the device is local
     * 
     * @return ETrue if the device is local
     */    
    TBool Local() const;
    
    /**
     * Matches protocolinfo and return ETrue if it matches
     * 
     * @param aProtocolInfo objects protocolinfo
     * @return ETrue if it matches
     */    
    TBool MatchSinkProtocolInfo( const TDesC8& aInfo ) const;
    
    /**
     * Validates transfer
     * 
     * @param aProtocolInfo objects protocolinfo
     * @return ETrue if it matches
     */    
    TBool ValidateTransfer( const TDesC8& aInfo ) const;
        
    /**
     * Sets sink protocolinfo
     * 
     * @param aProtocolInfo
     */    
    const TDesC8& FindFirstMatchingInSinkL(
        const CUpnpItem& aItem ) const;

    /**
     * Matches objectclass to protocolInfo
     * 
     * @param aObjectClass
     * @param aProtocolInfo
     */    
    TBool MatchType( const TDesC8& aObjectClass, const TDesC8&
        aProtocolInfo ) const;
    
    /**
     * Sets if the device supports audio upload
     * 
     * @param aAudioUpload
     */    
    void SetAudioUpload( TBool aAudioUpload );
    
    /**
     * Return if the device supports audio upload
     * 
     * @return ETrue if the device supports audio upload
     */    
    TBool AudioUpload() const;
    
    /**
     * Sets if the device supports image upload
     * 
     * @param aImageUpload
     */    
    void SetImageUpload( TBool aImageUpload );
    
    /**
     * Return if the device supports image upload
     * 
     * @return ETrue if the device supports image upload
     */    
    TBool ImageUpload() const;
    
    /**
     * Sets if the device supports video upload
     * 
     * @param aVideoUpload
     */    
    void SetVideoUpload( TBool aVideoUpload );
    
    /**
     * Return if the device supports video upload
     * 
     * @return ETrue if the device supports video upload
     */    
    TBool VideoUpload() const;    

    /**
     * Sets if the device supports CreateChildContainer
     * 
     * @param aCreateChildContainer
     */    
    void SetCreateChildContainer( TBool aCreateChildContainer );
    
    /**
     * Return if the device supports video upload
     * 
     * @return ETrue if the device supports video upload
     */    
    TBool CreateChildContainer() const;
    
    /**
     * Sets if the device supports DestroyObject
     * 
     * @param aDestroyObject
     */    
    void SetDestroyObject( TBool aDestroyObject );
    
    /**
     * Return if the device supports DestroyObject
     * 
     * @return ETrue if the device is local
     */    
    TBool DestroyObject() const;
    
    /**
     * Sets if the protocolInfo has been received
     * 
     * @param aPInfoReceived
     */    
    void SetPInfoReceived( TBool aPInfoReceived );
    
     /**
     * Return if the device has received protocolInfo
     * 
     * @return ETrue if the device has received protocolInfo
     */    
   TBool PInfoReceived() const;
   
   void SetDLNADeviceType( TDLNADeviceType aDeviceType );
   
   TDLNADeviceType DLNADeviceType() const;
    
private:
    
    /**
     * Sets source protocolinfo
     * 
     * @param aProtocolInfo source protocolinfo
     */    
    void SetSourceProtocolInfoL(
        const RPointerArray<CUpnpDlnaProtocolInfo>& aProtocolInfo );
    
    /**
     * Sets sink protocolinfo
     * 
     * @param aProtocolInfo sink protocolinfo
     */    
    void SetSinkProtocolInfoL(
        const RPointerArray<CUpnpDlnaProtocolInfo>& aProtocolInfo );
        
    /**
     * Parses descriptor to a delimeter
     * 
     * @param aLexer lexer representing a descriptor
     * @param aDelimeter delimeter character
     */    
    void ParseToDelimeter( TLex8& aLex, TChar aDelimeter );
    
    /**
     * Judge character to be able remove 
     * 
     * @param aCharacter is a character
     * @return ETrue if the character can be removed
     */    
    TBool IsIllegalCharacter( TChar aCharacter ) const;   
    
    /**
     * Parses descriptor perhaps have illegal characters
     * 
     * @param aPtr representing a descriptor
     * @return a new hbufc8 buffer 
     */    
    HBufC8* RemoveIllegalCharactersL( const TDesC8& aPtr ) const;
    
    TBool MatchSourceProfileId( const TDesC8& aInfo ) const;

    TBool MatchSinkProfileId( const TDesC8& aInfo ) const;
    
    TBool MatchSinkMime( const TDesC8& aInfo ) const;
       
private:
    
    RPointerArray<CUpnpDlnaProtocolInfo> iSourceProtocolInfo;
    
    RPointerArray<CUpnpDlnaProtocolInfo> iSinkProtocolInfo;
    
    TInt iSubscriptionCount;
    
    TBool iLocal;
    
    TBool iAudioUpload;
    
    TBool iImageUpload;
    
    TBool iVideoUpload;
    
    TBool iCreateChildContainer;
    
    TBool iDestroyObject;
    
    TBool iPInfoReceived;
    
    TDLNADeviceType iDLNADeviceType;
    };


#endif // C_UPNPAVDEVICEEXTENDED_H_
