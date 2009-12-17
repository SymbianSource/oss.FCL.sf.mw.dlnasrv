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
* Description:  Declares ProtocolInfo field wrapper. 
*
*/


#ifndef C_CUPNPPROTOCOLINFOLOCAL_H
#define C_CUPNPPROTOCOLINFOLOCAL_H

#include <e32base.h>
#include "upnpdlnaprotocolinfo.h"

	_LIT8( KOneChar, "1" );
	_LIT8( KZeroChar, "0" );

	_LIT8( KDEFAULT_1ST_PARAM, "http-get");
	_LIT8( KDEFAULT_STAR_PARAM, "*");

	const char KDlnaDelimeter = ';';
	const char KDlnaTokenizer = '=';
	const char KDlnaAllStar = '*';

	//Length of string from which should be serialized TInt into iDlnaFlags
	const TInt KMAX_INT_LENGTH_STRING = 8 ;

	// There are only 4 TInt32 or TUint32 flags serialized in DLNA.ORG_FLAGS parameter.
	const TInt KDLNA_FLAGS_NUMBER = 4; 
	//to represent the 32-byte-size of TUIn32
	const TInt K32BIT = 32;
	//number of all flags
	const TInt KDLNA_FLAGS_QUANTITY = KDLNA_FLAGS_NUMBER * K32BIT;
	
	_LIT8(KAllValues, "*");

	_LIT8( KDLNA_PN, "DLNA.ORG_PN");
	_LIT8( KDLNA_OP, "DLNA.ORG_OP");
	_LIT8( KDLNA_PS, "DLNA.ORG_PS");
	_LIT8( KDLNA_CI, "DLNA.ORG_CI");
	_LIT8( KDLNA_FLAGS, "DLNA.ORG_FLAGS");

	const char KDLNA_BOOLEAN_FALSE = '0';
	const char KDLNA_BOOLEAN_TRUE = '1';

	enum ADDITIONAL_INFO
		{
		NO_DLNA_DATA = -1,
		NO_ADDITIONAL_INFO = 0,
		JPEG_SM = 1, 
		JPEG_MED, 
		JPEG_LRG,
		JPEG_TN,	
		JPEG_SM_ICO,
		JPEG_LRG_ICO,
		PNG_TN, 
		PNG_SM_ICO,
		PNG_LRG_ICO,
		PNG_LRG, 
		MP3, 
		};

	_LIT8(KMT_IMAGE_JPEG, "image/jpeg");
	_LIT8(KMT_IMAGE_PNG, "image/png");
	_LIT8(KMT_IMAGE_GIF, "image/gif");
	_LIT8(KMT_IMAGE_BMP, "image/bmp");
	_LIT8(KMT_AUDIO_MP3, "audio/mpeg");
	_LIT8(KMT_TEXT_PLAIN, "text/plain");

	_LIT8( KMimeImage,      "image" );		
	_LIT8( KMimeAudio,      "audio" );
	_LIT8( KMimeVideo,      "video" );

	_LIT8(KDEFAULT_FOURTH_FIELD_OP_PATTERN, "*DLNA.ORG_OP=??*");
	_LIT8(KDEFAULT_FOURTH_FIELD_AVIMAGE_PATTERN, "*DLNA.ORG_FLAGS=????????????????????????????????*");
	
class CUpnpProtocolInfoLocal: public CUpnpDlnaProtocolInfo
{
	
	private:
    	 /** 
    	 * Constructor of the class     	 
    	 */
    	 CUpnpProtocolInfoLocal();

	
	public: 
	
    	 /**
    	 * Destructor of the class. 
    	 */
    	 virtual ~CUpnpProtocolInfoLocal();	
    	 
    	 /**
    	 * Method for creating instance of class. 
    	 * aInputString has to contain protocolInfo string. 
    	 */
    	IMPORT_C static CUpnpProtocolInfoLocal* NewL(const TDesC8& aInputString);
    	 
    	 /**
    	 * Creates Default value of ProtocolInfo containing DLNA.ORG_PN specific data. 
    	 * aMimeType - name of mime type for file. 
    	 * aAdditionalInfo - additional information about what DLNA.ORG_PN type should be used. 
    	 * Default behaviour is choosen according to aMimeType, when aAdditionalInfo is equal to NO_ADDITIONAL_INFO.
    	 * image/jpeg 	---- JPEG_LRG
    	 * image/png  	---- PNG_LRG
    	 * audio/mp3	---- MP3
    	 * rest not defined. 
    	 */
    	IMPORT_C static CUpnpProtocolInfoLocal* NewL(const TDesC8& aMimeType, TInt aAdditionalInfo);
    	IMPORT_C static CUpnpProtocolInfoLocal* NewL(const TDesC8& aMimeType, const TDesC8& aDlnaPn);
	
    //MEMBER METHODS used internally 	
	private: 
	
    	 /**
    	 *  Method for creating instance of class. 
    	 * aInputString has to contain protocolInfo string. 
    	 */
    	 void ConstructL(const TDesC8& aInputString);	 	 
    	 void ConstructFromDlnaPnL(const TDesC8& aMimeType, const TDesC8& aDlnaPn);	 
    	 
    	 /**
    	 * Creates object for further use. 
    	 * uses DefaultValue. 
    	 * aMimeType - mime type will be compared case insensitive. 
    	 * aAdditionalInfo - flag saying which specific DLNA.ORG_PN should be used. 
    	 *   values taken from ADDITIONAL_INFO
    	 */  
    	 void ConstructL(const TDesC8& aMimeType, TInt aAdditionalInfo);
	 
	 private:
	 
    	 /**
    	 * Main function for creating from mime type. 
    	 * See ConstructL(TDesC8&, TInt)
    	 */
    	 void ChooseDlnaL(const TDesC8& aMimeType, TInt aAdditionalInfo);
    	 		 
    	 /**
    	 * Fills up default values to create protocolInfo in form: "http-get:*:*:*"
    	 */
    	 void DefaultValuesL();
    	 
    	 /**
    	 * Fills dlna parameters according only to additionalInfo which is int value. 
    	 * aAdditionalInfo - int value denoting value which should be taken into consideration.
    	 */
    	 void FillAccordingToAddInfoL(TInt aAdditionalInfo);

    	 /**
    	 * Fills dlna parameters according only to info in mime type. 
    	 * aMimeType - name of a mime type.
    	 */
    	 void FillAccordingToMimeL(const TDesC8& aMimeType);
    	 
    	 /**
    	 * Does nothing.
    	 */
    	 void ZeroParameters();    
    	 
    	 /**
    	 * This member function sets defult values for optional parameters 
    	 * of 4-th field of protocolInfo 
    	 * Value for Audio/Video:
    	 * DLNA.ORG_OP=01;DLNA.ORG_FLAGS=01500000000000000000000000000000
    	 * Value for Image :
    	 * DLNA.ORG_OP=01;DLNA.ORG_FLAGS=00D00000000000000000000000000000
    	 *
    	 */
    	 void SetDefaultFourthField();     	     	 	 	 	 	 
    	 
    	 /**
         * This function produces a string containing all info taken from fourth ProtocolInfo field    	 
         * aBuffer - on this variable values are appended. 
         */
         virtual void GetFourthParameterInternalL(TDes8& aBuffer);     	 	 	 	 	 
};

#endif // C_CUPNPPROTOCOLINFOLOCAL_H
// End of file
