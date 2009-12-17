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
* Description:  helper class 
*
*/


#ifndef C_UPNPAVCPENGINEHELPER_H
#define C_UPNPAVCPENGINEHELPER_H

#include <e32base.h>
#include "upnpmdebug.h"

namespace UpnpAVCPEngine 
	{

	static const TInt KHttpGetStarted = 800;   // HTTP get started  ( port number )
	static const TInt KHttpPostStarted = 801;  // HTTP post started ( port number )

	static const TInt KRequestedCountLimit = 20; 
    
	_LIT8( KContentDirectory, "ContentDirectory:1" );
        
	_LIT8(KDefaultBrowseFilter, "res,res@protocolInfo,res@size,res@importUri,dc:date");
        
	_LIT8(KRootId,"0");
	_LIT8(KRootName,"Root");
	_LIT8(KBrowseDirectChildren, "BrowseDirectChildren");
	_LIT8(KBrowseMetadata,     "BrowseMetadata" );
	_LIT8(KResult, "Result");
	_LIT8(KNumberReturned, "NumberReturned" );
	_LIT8(KTotalMatches, "TotalMatches" );
	_LIT8(KStartingIndex, "StartingIndex");
	_LIT8(KRequestedCount, "RequestedCount");
	_LIT8(KSystemUpdateID, "SystemUpdateID");

	_LIT8(KContainer,"container");
	_LIT8(KItem,"item");
	_LIT8(KRes,"res");    
	_LIT8(KRestricted,"restricted");    
	_LIT8(KDate,"date");        
	_LIT8(KTitle,"title");
	_LIT8(KId,"id");    
	_LIT8(KSize,"size");    
	_LIT8(KParentId,"parentID");  
	_LIT8(KImportUri,"importUri");    
	_LIT8(KProtocolInfo,"protocolInfo"); 
	_LIT8(KProtocolInfoBegin,"http-get"); 

	_LIT8(KOne, "1");
	_LIT8(KTrue, "true");
	
                              
	/**
	* Convert descriptor into integer, if some problem leaves
	*
	* @param aValue value to be converted
	* @return integer value
	*/
	TInt StrToIntL(const TDesC8& aValue);
	/**
	* Get last path element from pathname.
	* From aPath last path element will be removed
	*
	* @param aPath original pathname 
	* @return last path element
	*/  
	HBufC8* GetLastPathElementL(TDes8& aPath);
	/**
	* Parse path into elements
	*/  
	void ParsePathToElementsL(TPtr8 aPath, RPointerArray<TPtrC8>& aArray );
	/**
	* Removes slashes from path
	*
	* @param aPath original pathname , path will be updated  
	*/  
	void RemoveSlashes(TDes8& aPath);
                       
	}

#endif // C_UPNPAVCPENGINEHELPER_H

// End Of File