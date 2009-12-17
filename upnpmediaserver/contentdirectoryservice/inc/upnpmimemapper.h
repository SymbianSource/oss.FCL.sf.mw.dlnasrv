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



#ifndef UPNPMIMEMAPPER_H
#define UPNPMIMEMAPPER_H

//  INCLUDES
#include "upnptablebean.h"
#include "upnpmap.h"
#include <e32base.h>
#include <d32dbms.h>


// Mapping file extensions 
_LIT8( KExtensionJpg,                   "jpg"                               );
_LIT8( KExtensionPng,                   "png"                               );
_LIT8( KExtensionTxt,                   "txt"                               );
_LIT8( KExtensionMp3,                   "mp3"                               );
_LIT8( KExtensionGif,                   "gif"                               );
_LIT8( KExtensionBmp,                   "bmp"                               );
_LIT( KExtensionWav,                   ".wav"                               );
_LIT( KExtensionAac,                   ".aac"                               );



// mime types <-> files ext
_LIT8( KXHtmlMime,   "application/xhtml+xml" ); _LIT( KXHtmlExt,".xhtml" ); 
_LIT8( KHtmlMime,    "text/html" );             _LIT( KHtmlExt, ".html" );
_LIT8( KXmlMime1,    "text/xml" );       
_LIT8( KXmlMime2,    "application/xml" );
_LIT8( KXmlMime3,	"application/x-xml");       _LIT( KXmlExt,  ".xml" );
_LIT8( KTxtMime,     "text/plain" );
_LIT8( KTxtMime2,	"application/txt" );
_LIT8( KTxtMime3,	"browser/internal" );
_LIT8( KTxtMime4,	"text/anytext" );
_LIT8( KTxtMime5,	"widetext/plain" );
_LIT8( KTxtMime6,	"widetext/paragraph" );            _LIT( KTxtExt,  ".txt" );
_LIT8( KJpegMime,    "image/jpeg" );            _LIT( KJpegExt, ".jpeg" );
_LIT8( KJpegMime2,  "image/jpg" );
_LIT8( KJpegMime3,   "image/jpe_" );
_LIT8( KJpegMime4,   "image/pjpeg" );  
_LIT8( KJpegMime5,    "image/vnd.swiftview-jpeg" );  

_LIT8( KJpgMime,    "image/jpeg" );            _LIT( KJpgExt, ".jpg" );
_LIT8( KJpgMime2,  "image/jpg" );  
_LIT8( KJpgMime3,   "image/jpe_" );  
_LIT8( KJpgMime4,   "image/pjpeg" );  
_LIT8( KJpgMime5,    "image/vnd.swiftview-jpeg" );  

_LIT8( KGifMime,     "image/gif" );             _LIT( KGifExt,  ".gif" );
_LIT8( KGifMime2,	"image/x-xbitmap" );   
_LIT8( KGifMime3,	"image/gi_" );   

_LIT8( KPngMime,     "image/png" );             _LIT( KPngExt,  ".png" );
_LIT8( KPngMime2,	 "application/png" );  
_LIT8( KPngMime3,	 "application/x-png" );  

_LIT8( KMpgMime1,    "video/mpeg");             _LIT( KMpgExt1,  ".mpg" );
_LIT8( KMpgMime2,    "video/mpg");   
_LIT8( KMpgMime3,     "video/x-mpg");  
_LIT8( KMpgMime4,     "video/mpeg2" );  
_LIT8( KMpgMime5,     "application/x-pn-mpg" );  
_LIT8( KMpgMime6,	    "video/x-mpeg" );  
_LIT8( KMpgMime7,	    "video/x-mpeg2a");  
_LIT8( KMpgMime8,     "audio/mpeg" );   
_LIT8( KMpgMime9,	    "audio/x-mpeg" );  
_LIT8( KMpgMime10,   "image/mpg" );   

_LIT8( KMpeg4Mime,   "video/mpeg4" );           _LIT( KMpeg4Ext,".mpeg4" );

_LIT8( KMp4Mime,     "video/mp4" );             _LIT( KMp4Ext,  ".mp4" );
_LIT8( KMp4Mime2,   "video/mp4v-es" );      
_LIT8( KMp4Mime3,   "audio/mp4" );		

_LIT8( KAviMime1,    "video/avi" );
_LIT8( KAviMime2,    "video/msvideo" );
_LIT8( KAviMime3,    "video/x-msvideo" );       _LIT( KAviExt,  ".avi" );
_LIT8( KAviMime4,    "image/avi" );        
_LIT8( KAviMime5,    "video/xmpg2" );   
_LIT8( KAviMime6,	 "application/x-troff-msvideo" );   
_LIT8( KAviMime7, 	 "audio/aiff" );                       
_LIT8( KAviMime8, 	 "audio/avi" );			

_LIT8( KMp3Mime1,    "audio/x-mpeg-3");
_LIT8( KMp3Mime2,    "audio/mpeg3" );           _LIT( KMp3Ext,  ".mp3" );
_LIT8( KMp3Mime3,    "audio/mpeg" );        // 1)   _LIT( KMpgExt2,  ".mp3" );
_LIT8( KMp3Mime4,    "audio/x-mpeg" );   
_LIT8( KMp3Mime5,    "audio/mp3" );        
_LIT8( KMp3Mime6,    "audio/x-mp3" );      
_LIT8( KMp3Mime7,    "audio/x-mpeg3" );  
_LIT8( KMp3Mime8,     "audio/mpg" );        
_LIT8( KMp3Mime9,     "audio/x-mpg" );       
_LIT8( KMp3Mime10,   "audio/x-mpegaudio" );  

_LIT8( KAacMime,     "audio/aac" );             _LIT( KAacExt,  ".aac" );
_LIT8( KWmaMime,     "audio/x-ms-wma" );        _LIT( KWmaExt,  ".wma" );
_LIT8( KWmaMime2,   "video/x-ms-asf" );     

_LIT8( KBmpMime,     "image/bmp" );             _LIT( KBmpExt,  ".bmp" );
_LIT8( KBmpMime2,    "image/x-bmp" );    
_LIT8( KBmpMime3,    "image/x-bitmap" );  
_LIT8( KBmpMime4,    "image/x-xbitmap" );  
_LIT8( KBmpMime5,    "image/x-ms-bmp" );  
_LIT8( KBmpMime6,    "application/bmp" );    
_LIT8( KBmpMime7,    "application/x-bmp" );    
_LIT8( KBmpMime8,    "application/x-win-bitmap" );     
_LIT8( KBmpMime9,     "application/preview" );    

_LIT8( KAudioL16,     "audio/L16" );
_LIT8( KAudioVnd,     "audio/vnd.dlna.adts" );

_LIT8( KAudio3gpp,     "audio/3gpp" ); 
_LIT8( KVideo3gpp,     "video/3gpp" ); 
_LIT( K3gpExt,  ".3gp" );
_LIT8( KAudioAMR,     "audio/AMR" );     _LIT( KAmrExt,  ".amr" );

_LIT8(KAudioWav, "audio/wav");
_LIT8(KVideoWmv, "video/x-ms-wmv");     _LIT( KWmvExt,    ".wmv" );
_LIT8(KVideoAvi, "video/x-ms-avi");

_LIT8(KRealMedia, "application/vnd.rn-realmedia");  _LIT( KRmExt,    ".rm" );
_LIT8(KRealAudio, "audio/vnd.rn-realaudio");        _LIT( KRealAudioExt,    ".ra" );
_LIT8(KPmRealAudioPlugin, "audio/x-pn-realaudio-plugin"); _LIT( KPmRealAudioPluginExt,    ".rpm" );
_LIT8(KPmRealAudio, "audio/x-pn-realaudio");        _LIT( KPmRealAudioExt,    ".ra" );
_LIT8(KRealVideo, "video/vnd.rn-realvideo");        _LIT( KRealVideoExt,    ".rv" );

// CLASS DECLARATION

/**
*  Provides mime type to file extention mapping.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class CUpnpMimeMapper: public CBase 
    {
public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
    static CUpnpMimeMapper* NewL();

     /**
        * Destructor.
        */
    virtual ~CUpnpMimeMapper();

public: // New functions
    

    void AddMimeMapElL( const TDesC8& aKey, const TDesC& aVal );
    void InitMimeToExtensionMapL();
    
    /**
    * This function gives extension to a file.  
    * @since Series 60 3.1
    * @param aMimeType - mime type from which additional information will be retrieved. 
    * @return pointer to HBufC8 object with extension value for this file. 
    */
    HBufC8* GetExtensionL( const TDesC8& aMimeType );
    
    const HBufC* Get( const TDesC8& aKey );

private:

  /**
    * C++ default constructor.
    */
    CUpnpMimeMapper();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
    void ConstructL();
   
private:    // Data

    CUpnpMap<HBufC>* iMimeToExtMap;
  
    };

#endif      // UPNPMIMEMAPPER_H   
        
// End of File
