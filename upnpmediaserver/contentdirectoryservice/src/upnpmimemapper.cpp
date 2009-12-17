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
* Description:  Element table data handler
*
*/


// INCLUDE FILES
#include "upnpmimemapper.h"
#include "upnpstring.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::CUpnpMimeMapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMimeMapper::CUpnpMimeMapper()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMimeMapper::ConstructL()
    {
    InitMimeToExtensionMapL();
    }
// -----------------------------------------------------------------------------
// CUpnpMimeMapper::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpMimeMapper* CUpnpMimeMapper::NewL()
    {
    CUpnpMimeMapper* self = new( ELeave ) CUpnpMimeMapper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::~CUpnpMimeMapper
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpMimeMapper::~CUpnpMimeMapper()
    {
    delete iMimeToExtMap;    
    }

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::InitMimeToExtensionMapL
// -----------------------------------------------------------------------------
//
void CUpnpMimeMapper::InitMimeToExtensionMapL()
    {
    iMimeToExtMap = new (ELeave) CUpnpMap<HBufC>();
    
    AddMimeMapElL(KXHtmlMime(),KXHtmlExt());
    AddMimeMapElL(KHtmlMime, KHtmlExt );
    AddMimeMapElL(KXmlMime1, KXmlExt  );
    AddMimeMapElL(KXmlMime2, KXmlExt  );
    AddMimeMapElL(KXmlMime3, KXmlExt  );  // 2)
    AddMimeMapElL(KTxtMime,  KTxtExt  );
    AddMimeMapElL(KTxtMime2, KTxtExt );  //  3)
    AddMimeMapElL(KTxtMime3, KTxtExt );  //  4)
    AddMimeMapElL(KTxtMime4, KTxtExt );  //  5)
    AddMimeMapElL(KTxtMime5, KTxtExt );  //  6)
    AddMimeMapElL(KTxtMime6, KTxtExt );  //  7)
    AddMimeMapElL(KJpegMime, KJpegExt );
    AddMimeMapElL(KJpegMime2, KJpegExt ); 
    AddMimeMapElL(KJpegMime3, KJpegExt );  
    AddMimeMapElL(KJpegMime4, KJpegExt );  
    AddMimeMapElL(KJpegMime5, KJpegExt ); 
    
    AddMimeMapElL(KJpgMime,  KJpgExt  );
    AddMimeMapElL(KJpgMime2, KJpgExt ); 
    AddMimeMapElL(KJpgMime3, KJpgExt );  
    AddMimeMapElL(KJpgMime4, KJpgExt );  
    AddMimeMapElL(KJpgMime5, KJpgExt ); 
    
    AddMimeMapElL(KGifMime,  KGifExt  );
    AddMimeMapElL(KGifMime2, KGifExt );   
     AddMimeMapElL(KGifMime3, KGifExt );   
    
    AddMimeMapElL(KPngMime,  KPngExt  );
    AddMimeMapElL(KPngMime2, KPngExt );  
    AddMimeMapElL(KPngMime3, KPngExt ); 
    
    AddMimeMapElL(KMpgMime1, KMpgExt1 );
    AddMimeMapElL(KMpgMime2, KMpgExt1 ); 
    AddMimeMapElL(KMpgMime3, KMpgExt1 );  
    AddMimeMapElL(KMpgMime4, KMpgExt1 );  
    AddMimeMapElL(KMpgMime5, KMpgExt1 );  
    AddMimeMapElL(KMpgMime6, KMpgExt1 );  
    AddMimeMapElL(KMpgMime7, KMpgExt1 );  
    AddMimeMapElL(KMpgMime8, KMpgExt1 );  
    AddMimeMapElL(KMpgMime9, KMpgExt1 );  
    AddMimeMapElL(KMpgMime10, KMpgExt1); 
    
    AddMimeMapElL(KMpeg4Mime,KMpeg4Ext);
    
    AddMimeMapElL(KMp4Mime,  KMp4Ext  );
    AddMimeMapElL(KMp4Mime2, KMp4Ext );  
    AddMimeMapElL(KMp4Mime3, KMp4Ext );  
    
    AddMimeMapElL(KAviMime1, KAviExt  );
    AddMimeMapElL(KAviMime2, KAviExt  );
    AddMimeMapElL(KAviMime3, KAviExt  );
    AddMimeMapElL(KAviMime4, KAviExt  );    
    AddMimeMapElL(KAviMime5, KAviExt  );    
    AddMimeMapElL(KAviMime6, KAviExt  );    
    AddMimeMapElL(KAviMime7, KAviExt  );    
    AddMimeMapElL(KAviMime8, KAviExt  );    
    
    AddMimeMapElL(KMp3Mime1, KMp3Ext  );
    AddMimeMapElL(KMp3Mime2, KMp3Ext  );
    AddMimeMapElL(KMp3Mime3, KMp3Ext );   // 1) AddMimeMapElL(KMpgMime2, KMpgExt2 );
    AddMimeMapElL(KMp3Mime4, KMp3Ext );   
    AddMimeMapElL(KMp3Mime5, KMp3Ext );   
    AddMimeMapElL(KMp3Mime6, KMp3Ext );   
    AddMimeMapElL(KMp3Mime7, KMp3Ext );   
    AddMimeMapElL(KMp3Mime8, KMp3Ext );    
    AddMimeMapElL(KMp3Mime9, KMp3Ext );    
    AddMimeMapElL(KMp3Mime10, KMp3Ext );   
    
    AddMimeMapElL(KAacMime,  KAacExt  );
    
    AddMimeMapElL(KWmaMime,  KWmaExt  );
    AddMimeMapElL(KWmaMime2, KWmaExt );   
    
    AddMimeMapElL(KBmpMime,  KBmpExt  );
    AddMimeMapElL(KBmpMime2, KBmpExt );   
    AddMimeMapElL(KBmpMime3, KBmpExt );   
    AddMimeMapElL(KBmpMime4, KBmpExt );   
    AddMimeMapElL(KBmpMime5, KBmpExt );   
    AddMimeMapElL(KBmpMime6, KBmpExt );    
    AddMimeMapElL(KBmpMime7, KBmpExt );    
    AddMimeMapElL(KBmpMime8, KBmpExt );    
    AddMimeMapElL(KBmpMime9, KBmpExt );    
    
    AddMimeMapElL(KAudioL16, KExtensionWav );    // .wav for audio/L16
    AddMimeMapElL(KAudioWav, KExtensionWav );    // .wav for audio/wav
    AddMimeMapElL(KAudioVnd, KExtensionAac );    // .aac for audio/vnd.dlna.adts
    AddMimeMapElL(KAudio3gpp, K3gpExt );   //audio/3gpp -> .3gp
    AddMimeMapElL(KVideo3gpp, K3gpExt );   //video/3gpp -> .3gp
    AddMimeMapElL(KAudioAMR, KAmrExt );   //audio/AMR -> .amr
    
    AddMimeMapElL(KVideoWmv, KWmvExt); //video/x-ms-wmv -> .wmv
    AddMimeMapElL(KVideoAvi, KAviExt); //video/x-ms-wmv -> .avi

    AddMimeMapElL(KRealMedia, KRmExt); //application/vnd.rn-realmedia -> .rm
    AddMimeMapElL(KRealAudio, KRealAudioExt); //audio/vnd.rn-realaudio -> .ra
    AddMimeMapElL(KPmRealAudioPlugin, KPmRealAudioPluginExt); //audio/x-pn-realaudio-plugin -> .rpm
    AddMimeMapElL(KPmRealAudio, KPmRealAudioExt); //audio/x-pn-realaudio -> .ra
    AddMimeMapElL(KRealVideo, KRealVideoExt); //video/vnd.rn-realvideo -> .rv
    
    }    

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::AddMimeMapElL
// -----------------------------------------------------------------------------
//
void CUpnpMimeMapper::AddMimeMapElL( const TDesC8& aKey, const TDesC& aVal )
    {
    HBufC* val = aVal.AllocLC();
    iMimeToExtMap->PutL(aKey, val);
    CleanupStack::Pop(val);        
    }   
    
// -----------------------------------------------------------------------------
// CUpnpMimeMapper::GetExtensionL
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpMimeMapper::GetExtensionL( const TDesC8& aMimeType )
    {
    HBufC8* result = NULL;
    const HBufC16* result16 = NULL;
    HBufC8* mimeTypeH = aMimeType.AllocLC();
    TPtr8 mimeType( mimeTypeH->Des() );
    mimeType.LowerCase();
    result16 = iMimeToExtMap->Get( mimeType );
    if(result16)
        {
    	result = UpnpString::FromUnicodeL( *result16);	
        }
    CleanupStack::PopAndDestroy( mimeTypeH );
    return result; 
    }

// -----------------------------------------------------------------------------
// CUpnpMimeMapper::Get
// -----------------------------------------------------------------------------
//
const HBufC* CUpnpMimeMapper::Get( const TDesC8& aMimeType )
    {
    return iMimeToExtMap->Get( aMimeType );
    }

//  End of File  
