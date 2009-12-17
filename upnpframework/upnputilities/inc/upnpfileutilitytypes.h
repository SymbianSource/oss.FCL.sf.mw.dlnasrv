/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP Object-related constant definitions
*
*/


// System includes
#include <e32base.h>

// mime types <-> files ext

// XHTML
_LIT8( KXHtmlMime,  "application/xhtml+xml" );
_LIT( KXHtmlExt,    ".xhtml" ); 

// HTML
_LIT8( KHtmlMime,   "text/html" );
_LIT( KHtmlExt,     ".html" );

// XML
_LIT8( KXmlMime1,   "text/xml" );       
_LIT8( KXmlMime2,   "application/xml" );
_LIT8( KXmlMime3,	"application/x-xml");
_LIT( KXmlExt,      ".xml" );

// TXT
_LIT8( KTxtMime,    "text/plain" );
_LIT8( KTxtMime2,	"application/txt" );
_LIT8( KTxtMime3,	"browser/internal" );
_LIT8( KTxtMime4,	"text/anytext" );
_LIT8( KTxtMime5,	"widetext/plain" );
_LIT8( KTxtMime6,	"widetext/paragraph" );
_LIT( KTxtExt,      ".txt" );

// JPEG
_LIT8( KJpegMime,   "image/jpeg" );
_LIT( KJpegExt,     ".jpeg" );

// JPG
_LIT8( KJpegMime2,  "image/jpg" );
_LIT8( KJpegMime3,  "image/jpe_" );
_LIT8( KJpegMime4,  "image/pjpeg" );
_LIT8( KJpegMime5,   "image/vnd.swiftview-jpeg" );
_LIT( KJpgExt,      ".jpg" );

// GIF
_LIT8( KGifMime,    "image/gif" );             
_LIT8( KGifMime2,	"image/x-xbitmap" );
_LIT8( KGifMime3,	"image/gi_" );
_LIT( KGifExt,      ".gif" );

// PNG
_LIT8( KPngMime,    "image/png" );             
_LIT8( KPngMime2,	"application/png" );
_LIT8( KPngMime3,	"application/x-png" );
_LIT( KPngExt,      ".png" );

// MPG
_LIT8( KMpgMime1,   "video/mpeg");
_LIT8( KMpgMime2,   "video/mpg");
_LIT8( KMpgMime3,   "video/x-mpg");
_LIT8( KMpgMime4,   "video/mpeg2" );
_LIT8( KMpgMime5,   "application/x-pn-mpg" );
_LIT8( KMpgMime6,	"video/x-mpeg" );
_LIT8( KMpgMime7,	"video/x-mpeg2a");
_LIT8( KMpgMime10,  "image/mpg" );
_LIT( KMpgExt1,     ".mpg" );

// MPEG4
_LIT8( KMpeg4Mime,  "video/mpeg4" );
_LIT( KMpeg4Ext,    ".mpeg4" );

// MP4
_LIT8( KMp4Mime,    "video/mp4" );
_LIT8( KMp4Mime2,   "video/mp4v-es" );
_LIT8( KMp4Mime3,   "audio/mp4" );
_LIT( KMp4Ext,      ".mp4" );

// AVI
_LIT8( KAviMime1,   "video/avi" );
_LIT8( KAviMime2,   "video/msvideo" );
_LIT8( KAviMime3,   "video/x-msvideo" );
_LIT8( KAviMime4,   "image/avi" );
_LIT8( KAviMime5,   "video/xmpg2" );
_LIT8( KAviMime6,	"application/x-troff-msvideo" );
_LIT8( KAviMime7, 	"audio/aiff" );
_LIT8( KAviMime8, 	"audio/avi" );
_LIT8( KAviMime9,   "video/x-ms-avi" );
_LIT( KAviExt,      ".avi" );

// MP3
_LIT8( KMp3Mime1,   "audio/x-mpeg-3");
_LIT8( KMp3Mime2,   "audio/mpeg3" );           
_LIT8( KMp3Mime3,   "audio/mpeg" );
_LIT8( KMp3Mime4,   "audio/x-mpeg" );
_LIT8( KMp3Mime5,   "audio/mp3" );
_LIT8( KMp3Mime6,   "audio/x-mp3" );
_LIT8( KMp3Mime7,   "audio/x-mpeg3" );
_LIT8( KMp3Mime8,   "audio/mpg" );
_LIT8( KMp3Mime9,   "audio/x-mpg" );
_LIT8( KMp3Mime10,  "audio/x-mpegaudio" );
_LIT( KMp3Ext,      ".mp3" );

//M4A
_LIT8( KM4aMime,    "audio/x-m4a" );
_LIT( KM4aExt,      ".m4a" );
// AAC
_LIT8( KAacMime,    "audio/aac" );
_LIT8( KAacMime2,   "audio/vnd.dlna.adts" );
_LIT8( KAacMime3,   "audio/adts" );
_LIT8( KAacMime4,   "audio/x-aac" );
_LIT( KAacExt,      ".aac" );

// WMA             
_LIT8( KWmaMime,    "audio/x-ms-wma" );        
_LIT8( KWmaMime2,   "video/x-ms-asf" );
_LIT( KWmaExt,      ".wma" );

// WMV
_LIT8( KWmvMime,    "video/x-ms-wmv" );
_LIT( KWmvExt,      ".wmv" );

// BMP
_LIT8( KBmpMime,    "image/bmp" );             
_LIT8( KBmpMime2,   "image/x-bmp" );
_LIT8( KBmpMime3,   "image/x-bitmap" );
_LIT8( KBmpMime4,   "image/x-xbitmap" );
_LIT8( KBmpMime5,   "image/x-ms-bmp" );
_LIT8( KBmpMime6,   "application/bmp" );
_LIT8( KBmpMime7,   "application/x-bmp" );
_LIT8( KBmpMime8,   "application/x-win-bitmap" );
_LIT8( KBmpMime9,   "application/preview" );
_LIT( KBmpExt,      ".bmp" );

// WAV
_LIT8( KAudioXWav,   "audio/x-wav" );
_LIT8( KAudioWav,   "audio/wav" );
_LIT8( KAudioL16,   "audio/L16" );
_LIT( KWavExt,      ".wav" );

// 3GP
_LIT8( KAudio3gpp,  "audio/3gpp" );
_LIT8( KVideo3gpp,  "video/3gpp" );
_LIT( K3gpExt,      ".3gp" );

// AMR
_LIT8( KAudioAMR,   "audio/AMR" );
_LIT( KAmrExt,      ".amr" );

// Real audio
_LIT8( KRAMime8,   "audio/x-pn-realaudio" );
_LIT( KRAExt,      ".ra" );

// DLNA
_LIT8( KVideoTts,   "video/vnd.dlna.mpeg-tts" );
_LIT( KTtsExt,       ".tts" );



