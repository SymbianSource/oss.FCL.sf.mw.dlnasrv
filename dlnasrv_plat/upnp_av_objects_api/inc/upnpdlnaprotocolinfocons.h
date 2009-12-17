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
* Description:  Declares protocolInfo namespace
*
*/


#ifndef C_CUPNPDLNAPROTOCOLINFOCONS_H
#define C_CUPNPDLNAPROTOCOLINFOCONS_H

#include <e32base.h>

namespace UpnpDlnaProtocolInfo
	{


	enum TDlnaFlags
		{
		SP_FLAG = 31,
		LSOP_TSR_FLAG = 30, 
		LSOP_RANGE_FLAG = 29, 
		PLAY_CONTAINER = 28, 
		S0_INC_FLAG = 27, 
		SN_INC_FLAG = 26,
		RSTP_PAUSE_FLAG = 25,
		TM_S_FLAG = 24,	// Streaming transfer
		TM_I_FLAG = 23,	// Interactiv transfer
		TM_B_FLAG = 22,	// Background transfer
		DLNA_V1_5_FLAG = 20  
		};

	enum TDlnaParams
		{
		A_VAL = 0,
		B_VAL = 1,			
		};


	_LIT8(KDLNA_PN_JPEG_SM, "JPEG_SM");
	_LIT8(KDLNA_PN_JPEG_MED, "JPEG_MED");
	_LIT8(KDLNA_PN_JPEG_LRG, "JPEG_LRG");
	_LIT8(KDLNA_PN_JPEG_TN, "JPEG_TN");
	_LIT8(KDLNA_PN_JPEG_SM_ICO, "JPEG_SM_ICO");
	_LIT8(KDLNA_PN_JPEG_LRG_ICO, "JPEG_LRG_ICO");
	_LIT8(KDLNA_PN_PNG_TN, "PNG_TN");
	_LIT8(KDLNA_PN_PNG_SM_ICO, "PNG_SM_ICO");
	_LIT8(KDLNA_PN_PNG_LRG_ICO, "PNG_LRG_ICO");
	_LIT8(KDLNA_PN_PNG_LRG, "PNG_LRG");
	_LIT8(KDLNA_PN_MP3, "MP3");
	
	_LIT8(KDLNA_PN_AAC_ISO_320, "AAC_ISO_320");
	_LIT8(KDLNA_PN_AVC_MP4_BL_CIF15_AAC_520, "AVC_MP4_BL_CIF15_AAC_520");

	_LIT8(KDEFAULT_DLNA_FLAGS_IMAGE, "DLNA.ORG_FLAGS=00D00000000000000000000000000000");
	_LIT8(KDEFAULT_DLNA_FLAGS_AV, "DLNA.ORG_FLAGS=01500000000000000000000000000000");
	_LIT8(KDEFAULT_DLNA_FLAGS_AUDIO, "DLNA.ORG_FLAGS=01500000000000000000000000000000");

	_LIT8(KDEFAULT_DLNA_OP, "DLNA.ORG_OP=01");						
	}

#endif // C_CUPNPDLNAPROTOCOLINFOCONS_H
// End of file