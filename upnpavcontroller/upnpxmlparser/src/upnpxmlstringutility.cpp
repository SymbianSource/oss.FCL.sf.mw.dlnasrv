/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Utility for string operations in UpnpXmlParser component
*
*/






// INCLUDE FILES
#include <e32base.h>
#include "upnpxmlstringutility.h"

_LIT( KComponentLogfile, "upnpxmlstringutility.txt" );
#include "upnplog.h"


// CONSTANTS
/**
 * Range of control characters taken from http:
 * //www.w3.org/International/questions/qa-controls. 
 * Remove characters that belong to "C0" range U+0000-U+001F 
 * (0-31) and also DEL (Delete) U+007F (127). C1 range U+0080-U+009F 
 * (128-159) is ignored for now because it seems to contain ok 
 * characters like € (euro sign).
 * */

const TInt KC0RangeStart = 0;
const TInt KC0RangeEnd = 31;
const TInt KDeleteChar = 127;

// All control characters start with &#
_LIT8( KXmlControlCharStart, "&#" );
// All xml entities end with ; because they are of for &entityname;
_LIT8( KXmlEntityEnd, ";" );

// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UpnpXmlStringUtility::RemoveXmlControlCharactersL
//---------------------------------------------------------------------------
EXPORT_C HBufC8* UpnpXmlStringUtility::RemoveXmlControlCharactersL(
    const TDesC8& aXmlData )
    {
    __LOG( "UpnpXmlStringUtility::RemoveXmlControlCharactersL Begin" );
    HBufC8* filtrateBuf = NULL;
    //if no data
    if ( aXmlData.Length() != 0 )
        {
        //The offset of the KXmlControlCharStart from the beginning of \
          this descriptor's data
        
        TInt controlCharStartOffset = aXmlData.Find( KXmlControlCharStart );
        // to make sure the data contain control character.
        if ( controlCharStartOffset != KErrNotFound )
            {
            filtrateBuf = aXmlData.AllocL();
            CleanupStack::PushL( filtrateBuf );    
            //The offset of the KXmlEntityEnd from the beginning of this \
              descriptor's data
            TInt controlCharEndOffset = 0;
            //The offset of the buffer who not be checked
            TInt partofBufferOffset = 0;
   
            //circle find control character.
            while( controlCharStartOffset != KErrNotFound )
                {
                controlCharStartOffset += partofBufferOffset;
                partofBufferOffset = controlCharStartOffset;
                controlCharEndOffset = ( filtrateBuf->Des().
                    Mid( controlCharStartOffset ).Find( KXmlEntityEnd ) );
                if ( controlCharEndOffset != KErrNotFound )
                    {
                    TLex8 tempLex( filtrateBuf->Des().Mid( partofBufferOffset
                      + KXmlControlCharStart().Length() ,controlCharEndOffset
                      - KXmlControlCharStart().Length() ) );
                    //Parses the string to extract a signed 8-bit integer.
                    TInt tempNum;
                    User::LeaveIfError( tempLex.Val( tempNum ) );
                    if( ( tempNum >= KC0RangeStart && tempNum <= KC0RangeEnd )
                         || ( tempNum == KDeleteChar ) )
                        {
                        __LOG( "UpnpXmlStringUtility::\
RemoveXmlControlCharactersL delete control character" );
                        TPtrC8 tmpBuf = filtrateBuf->Des().Mid( 
                            partofBufferOffset, controlCharEndOffset + 
                            KXmlEntityEnd().Length() );
                        __LOG8( tmpBuf);
                        
                        filtrateBuf->Des().Delete( partofBufferOffset, 
                        controlCharEndOffset + KXmlEntityEnd().Length() );
                        }
                    else 
                        {
                        __LOG1( "UpnpXmlStringUtility::\
RemoveXmlControlCharactersL between &# and; the Number is %d",tempNum );
                        }
                    controlCharStartOffset = partofBufferOffset;
                    controlCharStartOffset = ( filtrateBuf->Des().
                                        Mid( controlCharStartOffset ).
                                        Find( KXmlControlCharStart ) );
                    }
                else 
                    {
                    __LOG( "UpnpXmlStringUtility::RemoveXmlControlCharactersL\
 Can not find EntityEnd" );
                    User::Leave( KErrArgument );
                    }
                }
            CleanupStack::Pop( filtrateBuf );
            }
        else
            {
            // not contains control character.
            __LOG( "UpnpXmlStringUtility::RemoveXmlControlCharactersL Can \
                   not find control character!" );
            }
        }
    else
        {
        //xml data is NULL
        __LOG( "UpnpXmlStringUtility::RemoveXmlControlCharactersL the \
                            xml data is NULL" );
        }
    __LOG( "UpnpXmlStringUtility::RemoveXmlControlCharactersL End" );
    return filtrateBuf;
    }
