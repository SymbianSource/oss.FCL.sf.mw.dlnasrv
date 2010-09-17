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
* Description:  ContentDirectory utils
*
*/



// INCLUDE FILES
#include <uri8.h>
#include "upnpcdutils.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpdominterface.h"
#include "upnperror.h"
#include "upnpcommonupnplits.h"
#include "upnpstring.h"
#include "upnpcons.h"
#include <utf.h>
#include <xml/dom/xmlengdocument.h>
#include <e32math.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryDb::RemoveWhiteSpacesL
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::RemoveWhiteSpacesL(TDes8& aString)
{
    for(TInt i=0; i<aString.Length(); i++)
    {
        if(    aString[i] == ' '
           ||  aString[i] == '\r'
           ||  aString[i] == '\n'
           ||  aString[i] == '\t')
        {
            aString.Replace(i,1,_L8(""));
            i--;
        }
    }
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::IsWhiteString
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool UpnpCdUtils::IsWhiteString(const TDesC8& aString)
{
    TBool ret = ETrue;
    for(TInt i = 0; i < aString.Length(); i++)
    {
        if( !(  aString[i] == '\n' ||
                aString[i] == '\r' ||
                aString[i] == ' '  ||
                aString[i] == '\t' )  ) 
        {
            ret = EFalse;
            break;
        }
    }
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::GetObjectElementL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TXmlEngElement UpnpCdUtils::GetObjectElementL(RXmlEngDocument& aFragment)
{
    TXmlEngElement element;
    // try to get container
    UpnpDomInterface::GetElementL( aFragment.DocumentElement(), element, KContainer );
    if(element.IsNull())
    { // this is not a container
        // try to get an item
        UpnpDomInterface::GetElementL( aFragment.DocumentElement(), element, KItem );
        if(element.IsNull())
        { // this is neither an item - error
            User::Leave(EInvalidArgs);
        }
    }
    return element;
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::HasRefIdL
// -----------------------------------------------------------------------------
//
TBool UpnpCdUtils::HasRefIdL( RXmlEngDocument& aFragment )
    {
    TXmlEngElement object = UpnpCdUtils::GetObjectElementL( aFragment );
    //reference id value
    TPtrC8 refID = UpnpDomInterface::GetAttrValueL( object, KRefID );
    return !refID.CompareF( KNullDesC8 ) ? EFalse : ETrue;
    }

// -----------------------------------------------------------------------------
// UpnpCdUtils::EncodeXmlStringL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* UpnpCdUtils::EncodeXmlStringL(const TDesC8& aString)
{
    // new buffer size 
    TInt extensionSize = 0;
    for(TInt i = 0; i < aString.Length(); i++)
    {
        if(aString[i] == '&')       extensionSize += KEtAmp().Length() - 1;
        else if(aString[i] == '<')  extensionSize += KEtlt().Length() - 1;
        else if(aString[i] == '>')  extensionSize += KEtgt().Length() - 1;
        else if(aString[i] == '\"') extensionSize += KEtQuot().Length() - 1;
        else if(aString[i] == '\'') extensionSize += KEtApos().Length() - 1;
    }
    
    // alloc new buffer
    HBufC8* ret = HBufC8::NewLC(aString.Length() + extensionSize);
    TPtr8 retPtr(ret->Des());
    retPtr.Copy(aString);
    
    // replace
    for(TInt i = 0; i < retPtr.Length(); i++)
    {
        if(retPtr[i] == '&')        retPtr.Replace(i, 1, KEtAmp);
        else if(retPtr[i] == '<')   retPtr.Replace(i, 1, KEtlt);
        else if(retPtr[i] == '>')   retPtr.Replace(i, 1, KEtgt);
        else if(retPtr[i] == '\"')  retPtr.Replace(i, 1, KEtQuot);
        else if(retPtr[i] == '\'')  retPtr.Replace(i, 1, KEtApos);
    }
    
    // clean up
    CleanupStack::Pop(ret);
    
    return ret; 
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::ReplaceTrueFalse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::ReplaceTrueFalse(HBufC8* aBuf)
{
    TPtr8 ptr(aBuf->Des());
    TInt pos;
    // false -> 0
    pos = ptr.FindC(KFalseString8);
    if (pos != KErrNotFound)
    {
        ptr.Replace(pos, KFalseString8().Length(), KFalseValue8);
    }
    // true -> 1
    pos = ptr.FindC(KTrueString8);
    if (pos != KErrNotFound)
    {
        ptr.Replace(pos, KTrueString8().Length(), KTrueValue8);
    }
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::GetElmNameWithNsL
// -----------------------------------------------------------------------------
//
HBufC8* UpnpCdUtils::GetElmNameWithNsL(TXmlEngElement aElement)
{
    TPtrC8 elmName( aElement.Name() );
    HBufC8* ret = NULL;
    if( aElement.NamespaceDeclaration().Prefix().Length() )
    {
        TPtrC8 prefix( aElement.NamespaceDeclaration().Prefix() );
        ret = HBufC8::NewL(elmName.Length() + prefix.Length() + 1);
        TPtr8 ptr(ret->Des());
        
        // concatenate prefix and name
        ptr.Copy(prefix);
        ptr.Append(KCol);
        ptr.Append(elmName);
    }
    else
    {
        ret = elmName.AllocL();
    }
    
    return ret; // the caller should delete it
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::Des8ToDesL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* UpnpCdUtils::Des8ToDesL(const TDesC8& aString)
{
    HBufC* ret = Des8ToDesLC(aString);
    CleanupStack::Pop(ret);
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::Des8ToDesLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* UpnpCdUtils::Des8ToDesLC(const TDesC8& aString)
{    
    HBufC* ret = UpnpString::ToUnicodeL(aString);
    CleanupStack::PushL(ret);
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::DesToDes8LC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* UpnpCdUtils::DesToDes8LC(const TDesC& aString)
{
    HBufC8* ret = UpnpString::FromUnicodeL(aString);
    CleanupStack::PushL(ret);
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::ResIdFromUriL
// -----------------------------------------------------------------------------
//
TInt64 UpnpCdUtils::ResIdFromUriL(const TDesC8& aUri)
{
    TUriParser8 up;
    User::LeaveIfError(up.Parse(aUri));
    TPtrC8 path(up.Extract(EUriPath));
    if (path.Length() == 0)
    {
    User::Leave(KErrNotFound);
    }
    
    if(path[0] == '/')
    {
        path.Set(path.Mid(1));
    }
    TInt index = path.Locate(TChar('/'));
    
    if ((index>0)&&(index != KMaxIDResLength))
    {
    	User::Leave(KErrNotFound);    
    }
    TLex8 lexer(path);
    TInt64 ret;
    User::LeaveIfError(lexer.Val(ret));
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::ResIdFromUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC8 UpnpCdUtils::ResIdFromUriDesL(const TDesC8& aUri)
{
    TUriParser8 up;
    User::LeaveIfError(up.Parse(aUri));
    TPtrC8 path(up.Extract(EUriPath));
    if (path.Length() == 0)
    {
    User::Leave(KErrNotFound);
    }
    
    if(path[0] == '/')
    {
        path.Set(path.Mid(1));
    }
    
    TInt index = path.Locate(TChar('/'));
    
    if (index > 0)       
    {
        path.Set(path.Left(index));        
    }
    
    return path;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::SetObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::SetObjectIdL(TXmlEngElement aElement, TInt aId)
{
    TXmlEngAttr idAttr = aElement.AttributeNodeL(KIdAttrName());
    TBuf8<KMaxIntegerLen> idVal;
    idVal.Num(aId);
    if(idAttr.NotNull())
    {
        idAttr.SetValueL(idVal);
    }
    else
    {
        aElement.AddNewAttributeL(KIdAttrName(), idVal);
    }
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::SetObjectRefIdL
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::SetObjectRefIdL(TXmlEngElement aElement, TInt aRefId)
{
    TXmlEngAttr refIdAttr = aElement.AttributeNodeL( KRefID() );
    TBuf8<KMaxIntegerLen> refIdVal;
    refIdVal.Num( aRefId );
    if( refIdAttr.NotNull() )
        {
        refIdAttr.SetValueL( refIdVal );
        }
    else
        {
        aElement.AddNewAttributeL( KRefID(), refIdVal );
        }
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::SetObjectParentIdL
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::SetObjectParentIdL(TXmlEngElement aElement, TInt aParentId)
{
    TXmlEngAttr parentIdAttr = aElement.AttributeNodeL( KParentID() );
    TBuf8<KMaxIntegerLen> parentIdVal;
    parentIdVal.Num( aParentId );
    if( parentIdAttr.NotNull() )
        {
        parentIdAttr.SetValueL( parentIdVal );
        }
    else
        {
        aElement.AddNewAttributeL( KParentID(), parentIdVal );
        }
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::SetRestrictedFieldL
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::SetRestrictedFieldL(TXmlEngElement aElement, TBool aRestrictedFlag)
{
    TXmlEngAttr restrictedField = aElement.AttributeNodeL( KRestricted() );    
    TBuf8<KMaxIntegerLen> restrictedVal;
    
    if( aRestrictedFlag )
        {
        restrictedVal.Copy( KTrueValue8 );
        }    
    else
        {
        restrictedVal.Copy( KFalseValue8 );
        }    

    if( restrictedField.NotNull() )
        {
        restrictedField.SetValueL( restrictedVal );
        }
    else
        {
        aElement.AddNewAttributeL( KRestricted(), restrictedVal );
        }
}

// UpnpCdUtils::SetObjectIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void UpnpCdUtils::SetContainerIdL(TXmlEngElement aElement, TInt aParentId)
{
    TXmlEngAttr idAttr = aElement.AttributeNodeL(KParentID());
    TBuf8<KMaxIntegerLen> idVal;
    idVal.Num(aParentId);
    if(idAttr.NotNull())
    {
        idAttr.SetValueL(idVal);
    }
    else
    {
        aElement.AddNewAttributeL(KParentID(), idVal);
    }
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::IsElementRequiredL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool UpnpCdUtils::IsElementRequiredL(TXmlEngElement aElement)
{
    TBool ret = EFalse;
    TXmlEngAttr reqAttr = aElement.AttributeNodeL( KRequiredAtrName() );
    if(reqAttr.NotNull())
    { // element required
        ret = ETrue;
        reqAttr.Remove();
    }   
    return ret;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::BuildImportUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
HBufC8* UpnpCdUtils::BuildImportUriLC(TInt aResId)
{
	// get next resId
	TBuf8<KMaxIntegerLen + KRandomRangeLength> num;
	TBuf8<KMaxIntegerLen + KRandomRangeLength> uriId;
	num.Num(aResId);
	uriId.Justify(num, KMaxIntegerLen + KRandomRangeLength, ERight, '0');

	// build uri
	HBufC8* impUri = HBufC8::NewLC(
						  KHttpTag().Length()
						+ KIpPortPlaceholder8().Length()						
						+ KSlash8().Length()						
						+ KMaxLongIntegerLen
						+ KRandomRangeLength );
	TPtr8 impUriPtr(impUri->Des());
	impUriPtr.Append(KHttpTag);
	impUriPtr.Append(KIpPortPlaceholder8);
	impUriPtr.Append(KSlash8);		
	impUriPtr.Append(uriId);
	
	
	return impUri;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::BuildImportUriL
// -----------------------------------------------------------------------------
HBufC8* UpnpCdUtils::BuildImportUriShorterLC(TInt64 aResId)
{
	// get next resId
	TBuf8<KMaxIntegerLen > num;
	TBuf8<KMaxIntegerLen > uriId;
	num.Num(aResId);
	uriId.Justify(num, KMaxIntegerLen , ERight, '0');

	// build uri
	HBufC8* impUri = HBufC8::NewLC(
						  KHttpTag().Length()
						+ KIpPortPlaceholder8().Length()						
						+ KSlash8().Length()						
						+ KMaxLongIntegerLen
						 );
	TPtr8 impUriPtr(impUri->Des());
	impUriPtr.Append(KHttpTag);
	impUriPtr.Append(KIpPortPlaceholder8);
	impUriPtr.Append(KSlash8);		
	impUriPtr.Append(uriId);
	
	
	return impUri;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::BuildContentUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
HBufC8* UpnpCdUtils::BuildContentUriL(TInt aResId, const TDesC& aFileExten,
                                           TDesC8& aObjectId)
{
    HBufC8* number = UpnpCdUtils::RandomizeL(KRandomMax);
    CleanupStack::PushL(number);
    HBufC8* number2 = UpnpCdUtils::RandomizeL(KRandomMax);
    CleanupStack::PushL(number2);
    
	HBufC8* impUri = BuildImportUriShorterLC(aResId);	
	
    HBufC8* fileExten8 = UpnpCdUtils::DesToDes8LC(aFileExten);
	HBufC8* contUri = HBufC8::NewLC(
								impUri->Length() 
								+ number->Des().Length()
								+ number2->Des().Length()
								+ KSlash().Length()
                                + aObjectId.Length()                                
                                + fileExten8->Length());
	TPtr8 contUriPtr(contUri->Des());
	contUriPtr.Copy(*impUri);
	contUriPtr.Append(*number);	
	contUriPtr.Append(*number2);
	contUriPtr.Append(KSlash);
    contUriPtr.Append(aObjectId);    
    contUriPtr.Append(*fileExten8);

    // clean up
	CleanupStack::Pop(contUri);
    CleanupStack::PopAndDestroy(fileExten8);
	CleanupStack::PopAndDestroy(impUri);
	CleanupStack::PopAndDestroy(number2);
	CleanupStack::PopAndDestroy(number);
	return contUri;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::RandomizeL
// -----------------------------------------------------------------------------
HBufC8* UpnpCdUtils::RandomizeL(TInt aRange)
{
    TUint number = Math::Random();
    number = number % aRange;    
    HBufC8* numberDes = HBufC8::NewL(KRandomRangeLength);               
   	TBuf8<KRandomRangeLength> num;	
	num.Num(number);
    numberDes->Des().Justify(num, KRandomRangeLength, ERight, '0');    

    return numberDes;
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::ValidateFilePath
// -----------------------------------------------------------------------------
void UpnpCdUtils::ValidateFilePath(TDes8& aPath)
{
    // replace
    for(TInt i = 0; i < aPath.Length(); i++)
    {
        if( aPath[i] == ':' && i!=1 || 
            aPath[i] == '<'         ||
            aPath[i] == '>'         ||
            aPath[i] == '\"'        ||
            aPath[i] == '/'         ||    
            aPath[i] == '|'         ||
            aPath[i] == '*'       ||
            aPath[i] == '?'
          )
        {
            aPath[i] = '_';
        }
    }
}
// -----------------------------------------------------------------------------
// UpnpCdUtils::ValidateFilePath
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void UpnpCdUtils::ValidateFilePath(TDes& aPath)
{
    // replace
    for(TInt i = 0; i < aPath.Length(); i++)
    {
        if( aPath[i] == ':' && i!=1 || 
            aPath[i] == '<'         ||
            aPath[i] == '>'         ||
            aPath[i] == '\"'        ||
            aPath[i] == '/'         ||    
            aPath[i] == '|'         ||
            aPath[i] == '*'       ||
            aPath[i] == '?'
          )
        {
            aPath[i] = '_';
        }
    }
}


// -----------------------------------------------------------------------------
// UpnpCdUtils::ValidateSrcUriL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void UpnpCdUtils::ValidateSrcUriL(TDesC8& aPath,TDesC8& aIp )
{
    // Check if parsing is correct
    TUriParser8 srcParser;
    if (!(srcParser.Parse(aPath) == KErrNone))
    {
        User::Leave( ENoSourceResource );                        
    }
    
    // check if source uri is correct one
    TInt pos = aPath.Find(aIp);
    if(pos != UpnpHTTP::KHTTPUrl().Length())
        {
        User::Leave( ENoSourceResource ); 
        }

    if (!(aPath.Find( UpnpHTTP::KHTTPUrl ) == 0))
    {
        User::Leave( ENoSourceResource );                        
    }
    TInt len = UpnpHTTP::KHTTPUrl().Length() + aIp.Length();
    if ( aPath.Length() <= len || aPath[ len ] != '/' )
        {
        User::Leave( ENoSourceResource );
        }
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::EscapeAposL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
HBufC* UpnpCdUtils::EscapeAposL(const TDesC&  aValue)
{
    if (aValue.Length())
    {            
        HBufC8* value8 = DesToDes8LC(aValue);
        HBufC8* valueReplaced8 = UpnpString::StringReplaceL(*value8,_L8("'"),_L8("\'\'"));
        CleanupStack::Pop(value8);
        HBufC* valueReplaced16 = UpnpCdUtils::Des8ToDesL(*valueReplaced8);
        delete valueReplaced8;
        delete value8;        
        return valueReplaced16;
    }
    else
    {
        HBufC* valueReplaced16 = aValue.AllocL();
        return valueReplaced16;
    }   
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateDateL()
// Only Those date string is validate
// YYYY-MM-DD
// YYYY-MM-DDTHH:MM:SS
// YYYY-MM-DDTHH:MM:SSZ
// YYYY-MM-DDTHH:MM:SS.xxx
// YYYY-MM-DDTHH:MM:SS.xxxZ
// YYYY-MM-DDTHH:MM:SS+HH:MM
// YYYY-MM-DDTHH:MM:SS.xxx+HH:MM
// -----------------------------------------------------------------------------
//
TBool UpnpCdUtils::ValidateDateL( TPtrC8 aValue )
    {         
 /*   TBool wrong = 0;

    TInt dateLength = aValue.Length();                                         

    if ((dateLength!=EDay)&&(dateLength!=ESecond)&&
        (dateLength!=ETimeOffset1)&&(dateLength!=EMiliSecond)&&
        (dateLength!=ETimeOffset2)&&(dateLength!=EZone1)&&
        (dateLength!=EZone2))                                     
        {
        return EFalse;                
        }
    else
        {
        const TChar KColo = ':';
        const TChar KMinus = '-';
        const TChar KPlus = '+';
        const TChar KDot = '.';       
        const TChar KT = 'T';
        const TChar KZ = 'Z'; 

        TChar ch;
        TLex8 input(aValue);         
        for (TInt i =0; i<aValue.Length();i++)
            {
            ch = input.Get();         
            if ((i!=EYear)&&(i!=EMonth)&&(i!=EDay)&&(i!=EHour)&&(i!=EMinute)&&(i!=ESecond)&&(i!=EHourOffset1)&&(i!=EMiliSecond)&&(i!=EHourOffset2))
                {                
                if (!ch.IsDigit())          
                    {
                    wrong = ETrue;
                    break;  
                    }
                }
            else if (((i==EYear)||(i==EMonth))&&(ch!=KMinus))
                {
                wrong = ETrue;
                break;  
                }    
            else if ((i==EDay)&&(ch!= KT))
                {
                wrong = ETrue;
                break;  
                }     
            else if (((i==EHour)||(i==EMinute)||(i==EHourOffset2))&&(ch!=KColo))
                {
                wrong = ETrue;
                break;  
                }     
            else if (((i==ESecond)&&(dateLength==ETimeOffset1))&&((ch!=KPlus)&&(ch!=KMinus)))
                {
                wrong = ETrue;
                break;  
                }    
            else if (((i==ESecond)&&(dateLength==EZone1))&&(ch!=KZ))
                {
                wrong = ETrue;
                break;  
                }
            else if (((i==ESecond)&&((dateLength==ETimeOffset2)||(dateLength==EZone2)||(dateLength==EMiliSecond)))&&(ch!=KDot))
                {
                wrong = ETrue;
                break;  
                }              
            else if (((i==EHourOffset1)&&(dateLength==ETimeOffset1))&&(ch!=KColo))
                {
                wrong = ETrue;
                break;  
                }   
                                
            else if ((i==EHourOffset1)&&((dateLength==ETimeOffset2)||(dateLength==EZone2)||(dateLength==EMiliSecond)))
                {                    
                if (!ch.IsDigit())          
                    {
                    wrong = ETrue;
                    break;  
                    }
                }
            else if (((i==EMiliSecond)&&(dateLength==ETimeOffset2))&&((ch!=KPlus)&&(ch!=KMinus)))
                {
                wrong = ETrue;
                break;  
                }            
            else if (((i==EMiliSecond)&&(dateLength==EZone2))&&(ch!=KZ))
                {
                wrong = ETrue;
                break;  
                }      
            else if ((i==EMiliSecond)&&(dateLength==ETimeOffset1))
                {                
                if (!ch.IsDigit())          
                    {
                    wrong = ETrue;
                    break;  
                    }                               
                }
            }
        }
            
        TInt yearInt=0;
        TInt monthInt=0;
        TInt dayInt=0;
        TInt hourInt=0;
        TInt minuteInt=0;
        TInt secondInt=0;
        TInt milisecondInt=0;
        TInt hhInt=0;
        TInt mmInt=0;
        TDateTime a;

        TPtrC8 yearPtrC = aValue.Left(4);
        TPtrC8 tmp = aValue.Mid(5);
        TPtrC8 monthPtrC = tmp.Left(2);
        tmp.Set(aValue.Mid(8));
        TPtrC8 dayPtrC = tmp.Left(2);

        TLex8 lexer(yearPtrC);	
        lexer.Val(yearInt);
        lexer.Assign(monthPtrC);
        lexer.Val(monthInt);
        monthInt--;
        TMonth monthEnum = (TMonth)monthInt;
        lexer.Assign(dayPtrC);
        lexer.Val(dayInt);

        if (dateLength>EDay)                
            {
            tmp.Set(aValue.Mid(11));
            TPtrC8 hourPtrC = tmp.Left(2);
            tmp.Set(aValue.Mid(14));
            TPtrC8 minutePtrC = tmp.Left(2);
            tmp.Set(aValue.Mid(17));
            TPtrC8 secondPtrC = tmp.Left(2);  
            lexer.Assign(hourPtrC);
            lexer.Val(hourInt);
            lexer.Assign(minutePtrC);
            lexer.Val(minuteInt);
            lexer.Assign(secondPtrC);
            lexer.Val(secondInt);  
            }                
        if ((dateLength==ETimeOffset2)||(dateLength==EZone2)||(dateLength==EMiliSecond))
            {
            tmp.Set(aValue.Mid(20)); 
            TPtrC8 milisecondPtrC = tmp.Left(3);  
            lexer.Assign(milisecondPtrC);
            lexer.Val(milisecondInt);
            }
        if (dateLength==ETimeOffset1)
            {
            tmp.Set(aValue.Mid(20)); 
            TPtrC8 hhOffset = tmp.Left(2);
            tmp.Set(aValue.Mid(23)); 
            TPtrC8 mmOffset = tmp.Left(2);      
            lexer.Assign(hhOffset);
            lexer.Val(hhInt);
            
            if (hhInt>12) 
                {
                wrong = ETrue;                
                }
            
            lexer.Assign(mmOffset);
            lexer.Val(mmInt);     
            
            if (mmInt>59) 
                {
                wrong = ETrue;   
                }
            }
            
        if (dateLength==ETimeOffset2)
            {
            tmp.Set(aValue.Mid(24)); 
            TPtrC8 hhOffset = tmp.Left(2);
            tmp.Set(aValue.Mid(27)); 
            TPtrC8 mmOffset = tmp.Left(2);     
            lexer.Assign(hhOffset);
            lexer.Val(hhInt);
            if (hhInt>12) 
                {
                wrong = ETrue;                
                }
            lexer.Assign(mmOffset);
            lexer.Val(mmInt); 
                
            if (mmInt>59) 
                {
                wrong = ETrue;                
                }
            }
            
        TInt err = a.Set(yearInt,monthEnum,dayInt,hourInt,minuteInt,secondInt,milisecondInt);
        if (err<0) 
            {
            wrong = ETrue;    
            }

    if (wrong)
        {                        
        return EFalse;      
        }
*/
    return ETrue;
    }
    
// -----------------------------------------------------------------------------
// UpnpCdUtils::StringToUint
// -----------------------------------------------------------------------------
    TInt UpnpCdUtils::StringToTUint(const TDesC8& aStr, TUint* aInt)
	{
	if( 0 == aStr.Length() || aInt == NULL)
		{
		return KErrArgument;
		}
	TLex8 lex( aStr );
	return lex.Val( *aInt );
	}                   

// -----------------------------------------------------------------------------
// UpnpCdUtils::SplitStringByDelimeter
// -----------------------------------------------------------------------------
void UpnpCdUtils::SplitStringByDelimeter(TLex8& aLexeme, TChar aDelimeter)
{
	aLexeme.Mark(); 
	while( (aLexeme.Peek() != aDelimeter) && (!aLexeme.Eos()) )
	{
		aLexeme.Inc();		
	}
}

// -----------------------------------------------------------------------------
// UpnpCdUtils::Skip
// -----------------------------------------------------------------------------
void UpnpCdUtils::Skip(TLex8& aLexer, TInt aValue)
{
    if(!aLexer.Eos())
	{
	    aLexer.Inc(aValue);
	}
}	

// -----------------------------------------------------------------------------
// UpnpCdUtils::AreDigitsInSpecificRange
// -----------------------------------------------------------------------------    
TBool UpnpCdUtils::AreDigitsInSpecificRange(const TDesC8& aInput, TInt aNumber, TBool aExactDigitsNumber, TUint32 aRange)
{
    // checking length
    TInt length = aInput.Length();
    
    if(aExactDigitsNumber)
    {
        // if exact number of digits = real number of digits -> ok
        if(length != aNumber) 
            return EFalse;
    }
    else
    {
        // if real number of digits fit into specific range -> ok
        if(length > aNumber || length < 1)
            return EFalse;
    }
    
    // checking digits
    for(TInt i=0; i < length; i++)
    {
        // if input characters are all digits -> ok
        TChar tmp( aInput[i] );       
        if( !tmp.IsDigit() )
            return EFalse;            
    }
    
    // checking range
    TLex8 field(aInput);	        
    TUint32 value;
    
    // if e.g minutes fit into range: 00-59 -> ok
    TInt err = field.BoundedVal(value, EDecimal, aRange);
    
    if(err != KErrNone)
        return EFalse;
    
    return ETrue;
}    

// -----------------------------------------------------------------------------
// UpnpCdUtils::ValidateDurationValueL
// -----------------------------------------------------------------------------
TBool UpnpCdUtils::ValidateDurationValue(const TDesC8& aInput)
{
	/*
	* 7.3.22 MM DIDL-Lite res@duration Format
	* Requirement [7.3.22.1]: The syntax of the res@duration must be compliant to the following definition:
	*    - duration = hours ":" minutes ":" seconds
	*    - hours = 1*5 DIGIT; 0-99999
	*    - minutes = 2 DIGIT ; 00-59
	*    - seconds = 2 DIGIT ["." 3 DIGIT] ; 00-59 (.000-.999)
	*/
    TChar KColon(':');
    TChar KDot('.');

    TInt fieldNum = 1;        
    TLex8 input(aInput);
		
	while( !input.Eos() )
	{		
	    // extract subsequent field
		SplitStringByDelimeter( input, KColon );
		TPtrC8 field( input.MarkedToken() );
		
		// if its length != 0 ...
		if( field != KNullDesC8() )
		{		
		    if(fieldNum == 1)
		    {	
		        // if 1st field = 1-5 digits in range: 0-99999 -> ok
		        if( !AreDigitsInSpecificRange(field, 5, EFalse, 99999) )
		            return EFalse;
		            
		        ++fieldNum;
		    }
		    else if(fieldNum == 2)
		    {
		        // if 2nd field = exactly 2 digits in range: 0-59 -> ok
		        if( !AreDigitsInSpecificRange(field, 2, ETrue, 59) )
		            return EFalse;
		        
		        ++fieldNum;
		    }		    
		    else if(fieldNum == 3)
		    {
		        TInt length = field.Length();
		    
		        TLex8 fourth(field);		        		        
		        SplitStringByDelimeter( fourth, KDot );
		        
		        // if 3rd field = exactly 2 digits in range: 0-59 -> ok
		        TPtrC8 leftPart( fourth.MarkedToken() );
		        if( !AreDigitsInSpecificRange(leftPart, 2, ETrue, 59) )
		            return EFalse;
		        
		        Skip(fourth, 1);
		        TPtrC8 rightPart( fourth.Remainder() );
		        
		        // if 3rd field is divided by dot and right part is exactly 3 digits in range: 000-999 -> ok
		        if( rightPart != KNullDesC8() )
		        {		        
		            if( !AreDigitsInSpecificRange(rightPart, 3, ETrue, 999) )
		                return EFalse;
		        }
		        else if( field[length-1] == '.' )
		            return EFalse;
		        
		        ++fieldNum;
		    }
		    else
		        return EFalse;
		}
		else
		{
		    return EFalse;
		}
				
		Skip(input,1);	
	}
	
	// if while loop is completed successfully -> ok
	if(fieldNum == 4)
	    return ETrue;
	else
	    return EFalse;	    
}


// -----------------------------------------------------------------------------
// CUpnpContentDirectory::StringToInteger
// -----------------------------------------------------------------------------
//
TInt UpnpCdUtils::StringToInteger(const TDesC8& aStr, TInt* aInt)
    {
    if( aStr.Length() == 0 || aInt == NULL)
        {
        return KErrArgument;
        }
    
    TLex8 lex(aStr);    
    lex.Mark();
    TBool isFirst = ETrue;
    
    while( !lex.Eos() )
        {   
        TChar tmp = lex.Get();
        
        if( tmp.IsDigit() )
            {
            if(isFirst && tmp == '0' && aStr.Length() > 1)
                {
                return KErrArgument;
                }                        
            }
        else
            {
            return KErrArgument;
            }
        
        isFirst = EFalse;        
        }
               
    lex.UnGetToMark();                    
    return lex.Val( *aInt );
    }

//  End of File  
