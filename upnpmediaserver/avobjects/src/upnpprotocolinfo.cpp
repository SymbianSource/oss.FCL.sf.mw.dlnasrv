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
* Description:  ProtocolInfo base functionality class 
*
*/


#include "upnpprotocolinfo.h"

#include <e32base.h>
#include <e32cons.h>


const char KProtocolInfoDelimeter = ':'; 


//-----------------------------------------------------------------
//  CProtocolInfo::CProtocolInfo()
//  Constructor of the class
//-----------------------------------------------------------------
CUpnpProtocolInfo::CUpnpProtocolInfo()
    {
    }

//-----------------------------------------------------------------
//  CProtocolInfo::~CProtocolInfo()
//  Destructor of the class
//-----------------------------------------------------------------
CUpnpProtocolInfo::~CUpnpProtocolInfo()
    {
    delete iFirstParameter; 
    delete iSecondParameter; 
    delete iThirdParameter; 
    delete iFourthParameter;    
    delete iProtocolInfo;
    }


//-----------------------------------------------------------------
// CProtocolInfo::NewL
// Factory method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
EXPORT_C CUpnpProtocolInfo* CUpnpProtocolInfo::NewL()
    {
    CUpnpProtocolInfo* newElement = new (ELeave) CUpnpProtocolInfo();
    CleanupStack::PushL( newElement );
    CleanupStack::Pop( newElement );
    return newElement; 
    }
    
//-----------------------------------------------------------------
// CProtocolInfo::NewL
// Factory method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
EXPORT_C CUpnpProtocolInfo* CUpnpProtocolInfo::NewL(const TDesC8& aInputString)
    {
    CUpnpProtocolInfo* newElement = new (ELeave) CUpnpProtocolInfo();
    CleanupStack::PushL( newElement );
    newElement->BaseConstructL( aInputString );     
    CleanupStack::Pop( newElement );
    return newElement; 
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::BaseConstructL(TDesC8& aInputString)
//  Method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
void CUpnpProtocolInfo::BaseConstructL(const TDesC8& aInputString)
    {
    TInt result = ParseL( aInputString );
    if ( result != KErrNone ) 
        {
        User::Leave( result );            
        }
    }


//-----------------------------------------------------------------
//  CProtocolInfo::ParseL(TDesC8& aInputString)
//  Parses aInputString into protocolInfo object.
//-----------------------------------------------------------------
TInt CUpnpProtocolInfo::ParseL(const TDesC8& aInputString)
    {
    TLex8 input(aInputString);
    TInt i = 0;
    TInt result = KErrNone;
    while( (!input.Eos()) )
        {
        ParseAtomToDelimeter( input, KProtocolInfoDelimeter);
        SetParameterL(i++, input.MarkedToken());
        SkipAndMark( input, 1 );
        }
            
    return result;        
    }

//-----------------------------------------------------------------
//  CProtocolInfo::GetFourthParameterParameterLength
//  This function returns length of the string containing all relevant information from fourth ProtocolInfo field. 
//-----------------------------------------------------------------
TInt CUpnpProtocolInfo::GetFourthParameterLength()
    {
    if ( FourthField() != KNullDesC8)
        {
        return iFourthParameter->Length();
        }
    else
        {
        return 0;        
        }    
    }

//-----------------------------------------------------------------
//  CProtocolInfo::ParseAtomToDelimeter(TLex8& aLexeme, TChar aDelimeter)
//  It marks a string placed between start position and aDelimeter
//-----------------------------------------------------------------
void CUpnpProtocolInfo::ParseAtomToDelimeter(TLex8& aLexeme, TChar aDelimeter)
    {
    aLexeme.Mark(); 
    while( ( aLexeme.Peek() != aDelimeter ) && ( !aLexeme.Eos() ) )
        {
        aLexeme.Inc();        
        }
    }

//-----------------------------------------------------------------
//  CProtocolInfo::SetParameterL(TInt aWhich, TDesC8& aValue)    
//  Sets protcolInfo parameters in object.
//-----------------------------------------------------------------
void CUpnpProtocolInfo::SetParameterL(TInt aWhich, const TDesC8& aValue)    
    {
    switch ( aWhich )
        {
        case 0: // FIrst parameter.
            SetFirstFieldL(aValue);
            break;
        case 1: //Second parameter
            SetSecondFieldL(aValue);
            break; 
        case 2: // third parameter. 
            SetThirdFieldL(aValue);
            break;
        case 3: // fourth parameter. 
            SetFourthFieldL(aValue);
            break;
        default: 
            User::Leave(KErrTooBig);
        }
    }
        
//-----------------------------------------------------------------
//  CProtocolInfo::SkipAndMark(TLex8& aLexer, TInt aValue)
//  Skips aValue characters and marks after end of skipping.
//-----------------------------------------------------------------
void CUpnpProtocolInfo::SkipAndMark(TLex8& aLexer, TInt aValue)
    {
    if (!aLexer.Eos())
        {
        aLexer.SkipAndMark(aValue);
        }
    }

//-----------------------------------------------------------------
//  CProtocolInfo::Skip(TLex8& aLexer, TInt aValue)
//  Skips aValue characters. Do not mark
//-----------------------------------------------------------------
void CUpnpProtocolInfo::Skip(TLex8& aLexer, TInt aValue)
    {
    if (!aLexer.Eos())
        {
        aLexer.Inc(aValue);
        }
    }    
        
//-----------------------------------------------------------------
//  CProtocolInfo::CheckBooleanValue()
//  Check if character contains correct value for boolean type (1 or 0)
//-----------------------------------------------------------------     
TInt CUpnpProtocolInfo::CheckBooleanValue(TChar aCharacter)
    {
    if (aCharacter == '0' || 
        aCharacter == '1')
        return KErrNone;
    else
        return KErrArgument;
    }

//-----------------------------------------------------------------
// CProtocolInfo::GetProtocolInfoLength()
// Returns the length of the string currently stored in the class.
//-----------------------------------------------------------------
TInt CUpnpProtocolInfo::GetProtocolInfoLength()
    {
    TInt result = 0; 
    result += FirstField().Length();
    result++;
    result += SecondField().Length();
    result++; 
    result += ThirdField().Length(); 
    result++; 
    result += GetFourthParameterLength();
    return result; 
    }

//// PUBLIC ACCESS METHODS EXPORTED INTERFACE    
//-----------------------------------------------------------------
//  CProtocolInfo::GetProtocolInfoL()
//  Getter for pointer to ProtocolInfo
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpProtocolInfo::ProtocolInfoL()
    {
    const TInt size = GetProtocolInfoLength();
    HBufC8* buffer = HBufC8::NewL(size);
    CleanupStack::PushL( buffer );
    TPtr8 bufPtr(buffer->Des());
    bufPtr.Append(FirstField());
    bufPtr.Append(KProtocolInfoDelimeter);
    bufPtr.Append(SecondField());
    bufPtr.Append(KProtocolInfoDelimeter);
    bufPtr.Append(ThirdField());
    bufPtr.Append(KProtocolInfoDelimeter);
    GetFourthParameterInternalL(bufPtr);
    if ( iProtocolInfo ) 
        {
        delete iProtocolInfo;
        iProtocolInfo = NULL;            
        }
    iProtocolInfo = bufPtr.Alloc();
    CleanupStack::PopAndDestroy( buffer );
    return iProtocolInfo->Des();    
    }

//-----------------------------------------------------------------
//  CProtocolInfo::GetFirstField()
//  Getter for pointer to first parameter.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpProtocolInfo::FirstField()
    {
    if ( iFirstParameter )
        {
        return iFirstParameter->Des(); 
        }
    else
        {
        return KNullDesC8();
        }
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::SetFirstFieldL(TDesC8& aValue)
//  Setter for first parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpProtocolInfo::SetFirstFieldL(const TDesC8& aValue)
    {
    if ( aValue == KNullDesC8 )
        {
        return; 
        }
    if ( iFirstParameter )
        {
        delete iFirstParameter; 
        iFirstParameter = NULL;
        }
    iFirstParameter = aValue.AllocL();
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::GetSecondField()
//  Getter for second parameter.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpProtocolInfo::SecondField()
    {
    if ( iSecondParameter )
        {
        return iSecondParameter->Des(); 
        }
    else
        {
        return KNullDesC8();
        }
    }
    
//-----------------------------------------------------------------
// CProtocolInfo::SetSecondFieldL(TDesC8& aValue)
//  Setter for second parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpProtocolInfo::SetSecondFieldL(const TDesC8& aValue)
    {
    if ( aValue == KNullDesC8 )
        {
        return; 
        }
    if ( iSecondParameter )
        {
        delete iSecondParameter;
        iSecondParameter = NULL; 
        }
    iSecondParameter = aValue.AllocL();
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::GetThirdField()
//  Getter for third parameter.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpProtocolInfo::ThirdField()
    {
    if ( iThirdParameter )
        {
        return iThirdParameter->Des(); 
        }
    else 
        {
        return KNullDesC8();
        }
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::SetThirdFieldL(TDesC8& aValue)
//  Setter for third parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpProtocolInfo::SetThirdFieldL(const TDesC8& aValue)
    {
    if ( iThirdParameter )
        {
        delete iThirdParameter; 
        iThirdParameter = NULL;
        }
    iThirdParameter = aValue.AllocL();

    }

//-----------------------------------------------------------------
//  CProtocolInfo::GetFourthField()
//  Getter for fourth parameter.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpProtocolInfo::FourthField()
    {
    if ( iFourthParameter )
        {
        return iFourthParameter->Des(); 
        }
    else 
        {
        return KNullDesC8(); 
        }
    }
    
//-----------------------------------------------------------------
//  CProtocolInfo::SetFourthFieldL(TDesC8& aValue)
//  Setter for fourth parameter value.
//-----------------------------------------------------------------
EXPORT_C void CUpnpProtocolInfo::SetFourthFieldL(const TDesC8& aValue)
    {
    if ( iFourthParameter )
        {
        delete iFourthParameter; 
        iFourthParameter = NULL;
        }
    iFourthParameter = aValue.AllocL();
    }
        
//-----------------------------------------------------------------
//  CProtocolInfo::GetFourthParameterInternalL
//-----------------------------------------------------------------
void CUpnpProtocolInfo::GetFourthParameterInternalL(TDes8& aBuffer)
    {
    aBuffer.Append(FourthField());        
    }

//end of file
