/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ProtocolInfo DLNA functionality class 
*
*/


#include "upnpdlnaprotocolinfo.h"
#include "upnpdlnaprotocolinfocons.h"
#include "upnpprotocolinfolocal.h"

#include <e32base.h>
#include <e32cons.h>

using namespace UpnpDlnaProtocolInfo;


//-----------------------------------------------------------------
//  CProtocolInfoDlna::CProtocolInfoDlna()
//  Constructor of the class
//-----------------------------------------------------------------
CUpnpDlnaProtocolInfo::CUpnpDlnaProtocolInfo(): CUpnpProtocolInfo(),
                                            iDlnaCi( KErrNotFound ) 
    {    
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::~CProtocolInfoDlna()
//  Destructor of the class
//-----------------------------------------------------------------
CUpnpDlnaProtocolInfo::~CUpnpDlnaProtocolInfo()
    {    
    delete iDlnaPn; 
    delete iDlnaPs; 
    delete iOtherParams;
    delete iFlagsParam;
    delete iOpParam;
    }


//-----------------------------------------------------------------
// CProtocolInfoDlna::NewL
// Factory method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
EXPORT_C CUpnpDlnaProtocolInfo* CUpnpDlnaProtocolInfo::NewL()
    {
    CUpnpDlnaProtocolInfo* newElement = new (ELeave) CUpnpDlnaProtocolInfo();
    CleanupStack::PushL( newElement );
    CleanupStack::Pop( newElement );
    return newElement; 
    }

    
//-----------------------------------------------------------------
// CProtocolInfoDlna::NewL
// Factory method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
EXPORT_C CUpnpDlnaProtocolInfo* CUpnpDlnaProtocolInfo::NewL(const TDesC8& aInputString)
    {
    CUpnpDlnaProtocolInfo* newElement = new (ELeave) CUpnpDlnaProtocolInfo();
    CleanupStack::PushL( newElement );
    newElement->BaseConstructL( aInputString );     
    CleanupStack::Pop( newElement );
    return newElement; 
    }
    
//-----------------------------------------------------------------
//  CProtocolInfoDlna::ConstructL(TDesC8& aInputString)
//  Method for creating instance of class. aInputString must contain protocolInfo string.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ConstructL(const TDesC8& aInputString) 
    {
    BaseConstructL(aInputString);
    
    TInt result = ParseL( aInputString );
    if ( result != KErrNone ) 
        {
        User::Leave( result );            
        }
    }


//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOpTimeParam(TBool aValue)
//  Setter for DLNA.ORG_OP time seek parameter.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SetOpTimeParam(TBool aValue)
    {
    SetDlnaParam( aValue, A_VAL );
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOpRangeParam(TBool aValue)
//  Setter for DLNA.ORG_OP range seek parameter.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SetOpRangeParam(TBool aValue)
    {
    return SetDlnaParam( aValue, B_VAL );
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetDlnaFlag(TBool aValue, TInt aFlagIndex)
//  Setter for DLNA_FLAGS
//-----------------------------------------------------------------
void  CUpnpDlnaProtocolInfo::SetDlnaFlag(TBool aValue, TUint8 aFlagIndex)
    {
    if ( aFlagIndex >= KDLNA_FLAGS_QUANTITY) return;
    TInt index = aFlagIndex / K32BIT;
    SetIntBit32((TUint32&) iDlnaFlags[ index ], aValue, aFlagIndex);
    }    
    

//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaFlag(TInt aFlagIndex)
//  Getter for DLNA_FLAGS
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::GetDlnaFlag(TInt aFlagIndex)
    {
    if ( aFlagIndex >= KDLNA_FLAGS_QUANTITY ) return EFalse;
    TUint8 index = aFlagIndex / K32BIT;
    TUint8 bit_index = aFlagIndex % K32BIT;
    TUint32 bit = 1<<bit_index;    
    if ( iDlnaFlags[ index ]  & bit )
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
    
    }        

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseL(TDesC8& aInputString)
//  Parses aInputString into protocolInfo object.
//-----------------------------------------------------------------
TInt CUpnpDlnaProtocolInfo::ParseL(const TDesC8& aInputString)
    {
        TInt result = KErrNone;        
        CUpnpProtocolInfo::ParseL(aInputString);
        
        result = ParseForthParameterL();
        if ( iDlnaPn != NULL)
            {
            delete iFourthParameter; 
            iFourthParameter = NULL;
            }
        return result;        
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseForthParameterL()
//  Parses the fourth field.
//-----------------------------------------------------------------
TInt CUpnpDlnaProtocolInfo::ParseForthParameterL()
    {
    HBufC8* forth = iFourthParameter;
    if ( !forth )
        {
        return KErrArgument;
        }

    TLex8 input(*forth);
    TBool end = EFalse;
    while( (!input.Eos()) && (!end))
        {
        ParseAtomToDelimeter( input, KDlnaTokenizer);
        TPtrC8 name(input.MarkedToken());
        Skip(input,1);
        end = ParseForthParameterInternalL( name, input);
        }
    return KErrNone;
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseDlnaPsParamL(TLex8& aLexer)
//  Parses DLNA.ORG_PS params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseDlnaPsParamL(TLex8& aLexer)
    {
    ParseAtomToDelimeter(aLexer, KDlnaDelimeter);
    TPtrC8 value (aLexer.MarkedToken());
    if ( aLexer.MarkedToken().Length() == 0 ) User::Leave( KErrArgument );
    SetPsParameterL(value);
    SkipAndMark(aLexer, 1);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseDlnaPnParamL(TLex8& aLexer)
//  Parses DLNA.ORG_PN params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseDlnaPnParamL(TLex8& aLexer)
    {
    ParseAtomToDelimeter(aLexer, KDlnaDelimeter);
    TPtrC8 value (aLexer.MarkedToken());
    if ( aLexer.MarkedToken().Length() == 0 ) User::Leave( KErrArgument );
    SetPnParameterL(value);
    SkipAndMark(aLexer, 1);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseDlnaOpParamL(TLex8& aLexer)
//  Parses DLNA.ORG_OP params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseDlnaOpParamL(TLex8& aLexer)
    {
    ParseAtomToDelimeter(aLexer, KDlnaDelimeter);
    TPtrC8 value (aLexer.MarkedToken());
    if (value.Length() != 2) User::Leave( KErrArgument );
    TChar character = value[0]; 
    User::LeaveIfError(CheckBooleanValue(character));
    SetOpTimeParam(character == KDLNA_BOOLEAN_TRUE);    
    character = value[1]; 
    User::LeaveIfError(CheckBooleanValue(character));
    SetOpRangeParam(character == KDLNA_BOOLEAN_TRUE);
    //SetOpParameter
    SkipAndMark(aLexer, 1);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseDlnaCiParamL(TLex8& aLexer)
//  Parses DLNA.ORG_CI params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseDlnaCiParamL(TLex8& aLexer)
    {
    ParseAtomToDelimeter(aLexer, KDlnaDelimeter);
    TPtrC8 value (aLexer.MarkedToken());
    if (value.Length() != 1) User::Leave( KErrArgument );
    TChar character = value[0]; 
    User::LeaveIfError(CheckBooleanValue(character));    
    SetCiParameter(character == KDLNA_BOOLEAN_TRUE);
    //SetOpParameter
    SkipAndMark(aLexer, 1);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseDlnaFlagsParamL(TLex8& aLexer)
//  Parses DLNA.ORG_FLAGS params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseDlnaFlagsParamL(TLex8& aLexer)
    {
    ParseAtomToDelimeter(aLexer, KDlnaDelimeter);
    
    if ( aLexer.MarkedToken().Length() != 
        KDLNA_FLAGS_NUMBER * KMAX_INT_LENGTH_STRING) 
        User::Leave( KErrArgument );
    
    TLex8 plagLex(aLexer.MarkedToken());    
    
    for(int i = 0; i < KDLNA_FLAGS_NUMBER; i++)
        {
        plagLex.Mark();
        plagLex.Inc( KMAX_INT_LENGTH_STRING );
        TPtrC8 value (plagLex.MarkedToken());
        // check if values are from HEX range
        for (TInt j = 0; j < KMAX_INT_LENGTH_STRING;j++)
            if (!((value[j] >='0' &&  value[j] <='9') ||
                (value[j] >='a' &&  value[j] <='f') || 
                (value[j] >='A' &&  value[j] <='F')))
                User::Leave( KErrArgument );
        
        TLex8* ptr = new TLex8(value);
        CleanupStack::PushL(ptr);
        User::LeaveIfError(ptr->Val(iDlnaFlags[i], EHex)); 
        CleanupStack::PopAndDestroy(ptr);
        }
    SkipAndMark(aLexer,1);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseOtherParamsL(TLex8& aLexer)
//  Parses other params.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::ParseOtherParamsL(TLex8& /*aLexer*/)
    {
    if ( iDlnaPn != NULL )
        {
        
        //    SetOtherParamL(aLexer.RemainderFromMark());
        }
    }
    
//-----------------------------------------------------------------
//  CProtocolInfoDlna::ParseForthParameterInternalL( TDesC8& aName, TLex8& aLexer)
//  Parses the fourth param.
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::ParseForthParameterInternalL( const TDesC8& aName, TLex8& aLexer)
    {
    TBool result = EFalse;
    if ( aName.Compare(KDLNA_PN) == 0)
        {
        aLexer.Mark();
        ParseDlnaPnParamL(aLexer);
        }
    else if ( aName.Compare(KDLNA_OP) == 0 )
        {
        aLexer.Mark();
        ParseDlnaOpParamL(aLexer);
        }
    else if ( aName.Compare(KDLNA_PS) == 0 )
        {
        aLexer.Mark();
        ParseDlnaPsParamL(aLexer);
        }
    else if ( aName.Compare(KDLNA_CI) == 0 )        
        {
        aLexer.Mark();
        ParseDlnaCiParamL(aLexer);
        }
    else if ( aName.Compare(KDLNA_FLAGS) == 0 )                
        {
        aLexer.Mark();
        ParseDlnaFlagsParamL(aLexer);
        }
    else 
        {
        ParseOtherParamsL(aLexer);
        result = ETrue;
        }
    return result;
    }
        
//-----------------------------------------------------------------
//  CUpnpDlnaProtocolInfo::FourthField()
//  
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::FourthField()
    {
     TRAPD( error, FourthParameterInternalL() )
     if ( error )
         { 
         return KNullDesC8();
         }    
    return iFourthParameter->Des();
    }


//-----------------------------------------------------------------
//  CUpnpDlnaProtocolInfo::FourthParameterInternalL()
//  
//-----------------------------------------------------------------    
void CUpnpDlnaProtocolInfo::FourthParameterInternalL()
    {
    TInt len = GetFourthParameterLength();
    
    HBufC8* buf = HBufC8::NewLC(len);
    TPtr8 bufPtr(buf->Des());
    GetFourthParameterInternalL(bufPtr);
    
    delete iFourthParameter;
    iFourthParameter = buf;
    
    CleanupStack::Pop(buf);        
    }
    
    
        
//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetIntBit(TInt& aParameter, TBool aValue, TInt aBitNumber)
//  Setter for DLNA_FLAGS
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SetIntBit8(TUint8& aParameter, TBool aValue, TUint8 aBitNumber)
    {
    if ( aBitNumber >= KMAX_INT_LENGTH_STRING ) return;
    if ( ! (aValue == EFalse) )
        {
        aParameter |= (TUint8)1<<aBitNumber;
        }
    else
        {
        aParameter  &= ~((TUint8)1<<aBitNumber);
        }
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetIntBit(TInt& aParameter, TBool aValue, TInt aBitNumber)
//  Setter for DLNA_FLAGS
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SetIntBit32(TUint32& aParameter, TBool aValue, TUint8 aBitNumber)
    {
    if ( aBitNumber >= K32BIT ) return;
    if ( ! (aValue == EFalse) )
        {
        aParameter |= (TUint32)1<<aBitNumber;
        }
    else
        {
        aParameter  &= ~((TUint32)1<<aBitNumber);
        }
    }    
    
//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetDlnaParam(TBool aValue, TInt aParamIndex)
//  Setter for a DLNA parameter.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SetDlnaParam(TBool aValue, TUint8 aParamIndex)
    {
    SetIntBit8((TUint8&) iParams, aValue, aParamIndex);
    }    
            
//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaParam(TInt aParamIndex)
//  Getter for DLNA params.
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::GetDlnaParam(TInt aParamIndex)
    {
     return ( iParams  & ((TUint8)1<<aParamIndex));
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::IsDlnaFlagsSet()
//  This function returns ETrue if at least one dlna flag is set
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::IsDlnaFlagsSet()
    {
    for( int i = 0; i < KDLNA_FLAGS_NUMBER  ; i++)
        {
        if (iDlnaFlags[i] != 0 )
            return ETrue;     
        }
    //if not DLNA flags not set
    return EFalse;        
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetForthParameterInternalL(TPtr8& aBuffer)
//  This function produces a string containing all info taken from fourth ProtocolInfo field
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::GetFourthParameterInternalL(TDes8& aBuffer)
    {
    
    //if ( FourthField() != KNullDesC8)
    //    {
    //    aBuffer.Append( *iFourthParameter );
    //    return;
    //    }    
    
    
    if ( PnParameter() == KNullDesC8 )        
        {
        aBuffer.Append(KDlnaAllStar);
        return;
        }
    aBuffer.Append( KDLNA_PN );
    aBuffer.Append( KDlnaTokenizer ); 
    aBuffer.Append( *iDlnaPn );
    if ( GetDlnaOpRangeParam() || GetDlnaOpTimeParam() )
        {
        aBuffer.Append( KDlnaDelimeter ); 
        aBuffer.Append( KDLNA_OP );
        aBuffer.Append( KDlnaTokenizer ); 
        aBuffer.Append( GetDlnaOpTimeParam() ? KDLNA_BOOLEAN_TRUE:KDLNA_BOOLEAN_FALSE); //Time value 0,1
        aBuffer.Append( GetDlnaOpRangeParam() ? KDLNA_BOOLEAN_TRUE:KDLNA_BOOLEAN_FALSE); //Range value 0,1
        }
    if ( PsParameter() != KNullDesC8 )
        {
        aBuffer.Append( KDlnaDelimeter ); 
        aBuffer.Append( KDLNA_PS );
        aBuffer.Append( KDlnaTokenizer ); 
        aBuffer.Append( *iDlnaPs );
        }
    if ( iDlnaCi != KErrNotFound )
        {
        aBuffer.Append( KDlnaDelimeter ); 
        aBuffer.Append( KDLNA_CI );
        aBuffer.Append( KDlnaTokenizer ); 
        aBuffer.Append( iDlnaCi ? KOneChar() : KZeroChar() ); 
        }
    
    TBool saveDlnaFlags = IsDlnaFlagsSet();
    
    if ( saveDlnaFlags )
        {
        aBuffer.Append( KDlnaDelimeter ); 
        aBuffer.Append( KDLNA_FLAGS );
        aBuffer.Append( KDlnaTokenizer ); 
        SerializeDlnaFlagsL( aBuffer );
        }
    if ( GetOtherParams() != KNullDesC8) 
        {
        aBuffer.Append( KDlnaDelimeter ); 
        aBuffer.Append( *iOtherParams );
        }
    }
//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetForthParameterParameterLength
//  This function returns length of the string containing all relevant information from fourth ProtocolInfo field. 
//-----------------------------------------------------------------
TInt CUpnpDlnaProtocolInfo::GetFourthParameterLength()
    {
    
    //if ( FourthField() != KNullDesC8)
    //    {
    //    return iFourthParameter->Length();
    //    }
    
    
    if ( PnParameter() == KNullDesC8 )        
        {
        return 1; 
        }
    TInt result = 0; 
    result += KDLNA_PN.operator()().Length();
    result += 1 ; 
    result += iDlnaPn->Length();
    if ( GetDlnaOpRangeParam() || GetDlnaOpTimeParam() )
        {
        result += 1; // ; 
        result += KDLNA_OP.operator()().Length();
        result += 1; //= 
        result += 1; //Time value 0,1
        result += 1; //Range value 0,1
        }
    if ( PsParameter() != KNullDesC8 )
        {
        result += 1; // ; separator
        result += KDLNA_PS.operator()().Length();
        result += 1; // = 
        result += iDlnaPs->Length();
        }
    if ( iDlnaCi != KErrNotFound )
        {
        result += 1; //; 
        result += KDLNA_CI.operator()().Length();
        result += 1; //= 
        result += 1; // value 
        }
    
    TBool saveDlnaFlags = IsDlnaFlagsSet();
    
    if ( saveDlnaFlags )
        {
        result += 1; //; 
        result += KDLNA_FLAGS.operator()().Length();
        result += 1; // = 
        result += KDLNA_FLAGS_NUMBER * KMAX_INT_LENGTH_STRING; // Values length
        }
    if ( GetOtherParams() != KNullDesC8 ) 
        {
        result += 1; //; 
        result += iOtherParams->Length();
        }
    return result;
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SerializeDlnaFlagsL(TPtr8& aBuffer)
//  Serializes iDlnaFlags member variable into a string.
//-----------------------------------------------------------------
void CUpnpDlnaProtocolInfo::SerializeDlnaFlagsL(TDes8& aBuffer)
    {
    HBufC8* number = HBufC8::NewL(KMAX_INT_LENGTH_STRING);
    TPtr8 ptrNumber( number->Des() );
    for(int i = 0; i < KDLNA_FLAGS_NUMBER ; i++)    
        {
        ptrNumber.NumFixedWidth(iDlnaFlags[i], EHex, KMAX_INT_LENGTH_STRING);
        ptrNumber.UpperCase();
        aBuffer.Append( *number);
        }
    delete number;
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaOpTimeParam()
//  Getter for DLNA.ORG_OP time seek parameter.
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::GetDlnaOpTimeParam()
    {
    return GetDlnaParam(A_VAL);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaOpRangeParam()
//  Getter for DLNA.ORG_OP range seek parameter.
//-----------------------------------------------------------------
TBool CUpnpDlnaProtocolInfo::GetDlnaOpRangeParam()
    {
    return GetDlnaParam(B_VAL);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOpParameterL(TPtrC8 aValue)
//  Setter for DLNA.ORG_OP parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetOpParameterL(const TDesC8& aValue)
    {                   
    TPtrC8 opParamName;
    TPtrC8 opParamValue;
        
    //---- getting a proper value of DLNA.ORG_OP param --
    if ( aValue.Locate(KDlnaTokenizer) > KErrNotFound )
    {
        TLex8 lexer(aValue);    
        ParseAtomToDelimeter( lexer, KDlnaTokenizer);
        opParamName.Set( lexer.MarkedToken() );
        
        if ( opParamName != KDLNA_OP)
            {
            User::Leave(KErrBadDescriptor);    
            }
        
        Skip(lexer, 1);            
        opParamValue.Set( lexer.Remainder() );         
        }
    else
        {
        opParamValue.Set(aValue);            
        }
                
    //---- parsing DLNA.ORG_OP param value ---------------
    if ( opParamValue.Length() == 2 && 
        !CheckBooleanValue(((TChar)opParamValue[0])) && 
        !CheckBooleanValue(((TChar)opParamValue[1])) 
        )
        {                        
        TBool opTimeParam = ((TChar)opParamValue[0]).GetNumericValue();
        TBool opRangeParam = ((TChar)opParamValue[1]).GetNumericValue();
        
        SetOpTimeParam(opTimeParam);
        SetOpRangeParam(opRangeParam);        
        }
    else 
        {        
        User::Leave(KErrBadDescriptor);
        }                                                                                        
    } 
    
//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOpParameterL(UpnpDlnaProtocolInfo::TDlnaParams param,TBool aValue)
//  Setter for DLNA.ORG_OP parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetOpParameterL(UpnpDlnaProtocolInfo::TDlnaParams param,TBool aValue)
    {
    SetDlnaParam( aValue, param );
    }
    
//-----------------------------------------------------------------
//  CUpnpDlnaProtocolInfo::OpParameterL()
//  Getter for DLNA.ORG_OP parameter.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::OpParameterL()
    {
    TBool opTimeParam = GetDlnaOpTimeParam();
    TBool opRangeParam = GetDlnaOpRangeParam();
    HBufC8* opParameter = HBufC8::NewLC(2);    
    opParameter->Des().AppendNum( opTimeParam ? 1:0 );
    opParameter->Des().AppendNum( opRangeParam ? 1:0 );           
       
    CleanupStack::Pop(opParameter);
       
    delete iOpParam;
    iOpParam = opParameter;
    return iOpParam->Des();
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOpParameterL(UpnpDlnaProtocolInfo::TDlnaParams param,TBool aValue)
//  Setter for DLNA.ORG_OP parameter.
//-----------------------------------------------------------------
EXPORT_C TBool CUpnpDlnaProtocolInfo::OpParameter(UpnpDlnaProtocolInfo::TDlnaParams param)
    {
    return GetDlnaParam( param );
    }
    
//-----------------------------------------------------------------
//  CUpnpDlnaProtocolInfo::SetFlagsParameterL(const TDesC8& flags)
//  Setter for DLNA.ORG_CI parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetFlagsParameterL(const TDesC8& flags)
    {                
    TPtrC8 flagsParamName(KDLNA_FLAGS);        
    TLex8 lexer(flags);
    TInt len = flags.Length();
                
    //---- getting a proper value of DLNA.ORG_FLAGS ----
    if ( flags.Locate(KDlnaTokenizer) > KErrNotFound )
        {                    
        ParseAtomToDelimeter( lexer, KDlnaTokenizer);
        flagsParamName.Set( lexer.MarkedToken() );
            
        if ( flagsParamName != KDLNA_FLAGS )
            {
            User::Leave(KErrBadDescriptor);    
            }
            
        Skip(lexer, 1);
        TPtrC8 tmp = lexer.Remainder();
                        
        len = tmp.Length();
        lexer = tmp;
        }                                        
        
    //---- checking if DLNA.ORG_FLAGS has proper value --                
    if (len != 32)
        {
        User::Leave( KErrBadDescriptor );
        }
        
    while( !lexer.Eos() )
        {
        if ( !lexer.Get().IsHexDigit() )
            {
            User::Leave( KErrBadDescriptor );
            }
        }
        
    lexer.UnGetToMark();                                                
    ParseForthParameterInternalL( flagsParamName, lexer );                       
    }
    
//-----------------------------------------------------------------
//  CUpnpDlnaProtocolInfo::FlagsParameterL()
//  Getter for DLNA.ORG_CI parameter.
//----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::FlagsParameterL()
    {
    TInt len = KDLNA_FLAGS_NUMBER * KMAX_INT_LENGTH_STRING; // Values length
    HBufC8* flagsParameter = HBufC8::NewLC(len);
    TPtr8 ptr = flagsParameter->Des();
    SerializeDlnaFlagsL(ptr);
    CleanupStack::Pop(flagsParameter);    
    delete iFlagsParam;
    iFlagsParam = flagsParameter;            
    return iFlagsParam->Des();
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetCiParameter(TBool aValue)
//  Setter for DLNA.ORG_CI parameter.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetCiParameter(TBool aValue)
    {
    iDlnaCi = aValue;
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::CiParameter()
//  Getter for DLNA.ORG_CI parameter.
//-----------------------------------------------------------------
EXPORT_C TBool CUpnpDlnaProtocolInfo::CiParameter()
    {
    return !(iDlnaCi <= 0);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetOtherParamsL(TDesC8& aValue)
//  Setter for other parameter value.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetOtherParamL(const TDesC8& aValue)
     {
     if ( iOtherParams)
         {
         delete iOtherParams;
         iOtherParams = NULL;
         }
     iOtherParams = aValue.AllocL();
     }    

//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetOtherParams()
//  Getter for other parameter value.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::GetOtherParams()
     {
     if ( iOtherParams )
         {
         return iOtherParams->Des();
         }
     else
         {
         return KNullDesC8();
         }
     }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetDlnaFlag(TBool aValue,TInt flag)
//  Setter for a single flag.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetDlnaFlag(UpnpDlnaProtocolInfo::TDlnaFlags flag, TBool aValue)
    {
    SetDlnaFlag( aValue, flag);
    }

//-----------------------------------------------------------------
//  ProtocolInfo::DlnaFlag()
//  Getter for a single flag.
//-----------------------------------------------------------------
EXPORT_C TBool CUpnpDlnaProtocolInfo::DlnaFlag(UpnpDlnaProtocolInfo::TDlnaFlags flag)
    {
    return GetDlnaFlag(flag);
    }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetDlnaPnParameterL(TDesC8& aValue)
//  Setter for DLNA.ORG_PN.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetPnParameterL(const TDesC8& aValue)
     {
    if ( iDlnaPn )
        {
        delete iDlnaPn ; 
        iDlnaPn = NULL;
        }
    iDlnaPn = aValue.AllocL();
    }
 
//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaPnParameter()
//  Getter for DLNA.ORG_PN.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::PnParameter()
    {
     if ( iDlnaPn )
         {
         return iDlnaPn->Des();
         }
     else 
         {
         return KNullDesC8();
         }
     }

//-----------------------------------------------------------------
//  CProtocolInfoDlna::SetDlnaPsParameterL(TDesC8& aValue)
//  Setter for DLNA.ORG_PS.
//-----------------------------------------------------------------
EXPORT_C void CUpnpDlnaProtocolInfo::SetPsParameterL(const TDesC8& aValue)
    {
    if ( iDlnaPs )
        {
        delete iDlnaPs ; 
        iDlnaPs = NULL;
        }
    iDlnaPs = aValue.AllocL();    
    }
 
//-----------------------------------------------------------------
//  CProtocolInfoDlna::GetDlnaPsParameter()
//  Getter for DLNA.ORG_PS.
//-----------------------------------------------------------------
EXPORT_C TPtrC8 CUpnpDlnaProtocolInfo::PsParameter()
     {
     if ( iDlnaPs )
         {
         return iDlnaPs->Des();
         }
     else
        {
        return KNullDesC8();
        }
     }

     
//end of file
