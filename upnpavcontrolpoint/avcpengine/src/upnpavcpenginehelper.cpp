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


#include "upnpavcpenginehelper.h"

namespace UpnpAVCPEngine 
{

// -----------------------------------------------------------------------------
// UpnpAVCPEngine::StrToIntL
// -----------------------------------------------------------------------------
//    
TInt StrToIntL(const TDesC8& aValue)  
	{
	TLex8 returnedLex( aValue );
	TInt value;
	User::LeaveIfError( returnedLex.Val( value ));
	return value;    
	}

// -----------------------------------------------------------------------------
// UpnpAVCPEngine::GetLastPathElementL
// -----------------------------------------------------------------------------
//   
HBufC8* GetLastPathElementL(TDes8& aPath) 
	{        
	RemoveSlashes(aPath);
	TInt index = aPath.LocateReverse('/');
	if (index == KErrNotFound) 
	    index = 0;
	                        
	HBufC8* result = aPath.Right(aPath.Length() - index).AllocL();
	TPtr8 ptr = result->Des();
	RemoveSlashes(ptr);
	aPath.Delete( index, aPath.Length() - index);
	return result;                          
	}

// -----------------------------------------------------------------------------
// UpnpAVCPEngine::ParsePathToElementsL
// -----------------------------------------------------------------------------
//   
void ParsePathToElementsL(TPtr8 aPath, RPointerArray<TPtrC8>& aArray ) 
	{
	TChar delimiter('/');
	RemoveSlashes(aPath);

	TPtrC8 ptr(aPath);
    TInt i = 0;

	while(KErrNotFound != ptr.Locate(delimiter))
	    {	
	    aArray.Append( new (ELeave)TPtrC8() );
		aArray[i]->Set(ptr.Left((ptr.Locate(delimiter))));
    	ptr.Set(ptr.Right(ptr.Length() - (ptr.Locate(delimiter) + 1)) ); 
	    i++;
	    }
	    
	aArray.Append( new (ELeave) TPtrC8() );
   	aArray[i]->Set(ptr);    		
	}

// -----------------------------------------------------------------------------
// UpnpAVCPEngine::RemoveSlashes
// -----------------------------------------------------------------------------
//   
void RemoveSlashes(TDes8& aPath) 
	{
	if (aPath.Length() > 0 && aPath[0] == '/' ) 
		{        
		aPath.Delete(0,1);
		}

	// remove ending '/' 
	TInt length = aPath.Length();                
	if (length > 0 && aPath[length-1] == '/' ) 
		{
		aPath.Delete(length-1,1);   
		}	   
	}

}

// End of File
