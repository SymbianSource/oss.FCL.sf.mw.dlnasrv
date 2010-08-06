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
* Description:  ContentDirectory utils,
*
*/



#ifndef CDUTILS_H
#define CDUTILS_H

//  INCLUDES
#include <xml/dom/xmlengdom.h>
#include <e32base.h>


// CLASS DECLARATION

/**
*  Set of utils functions.
*
*  @lib AVContentDirectory.lib
*  @since Series60 3.1
*/
class UpnpCdUtils
{
 public:
 
    static void ValidateFilePath(TDes8& aPath);
    static void ValidateFilePath(TDes& aPath);
    static void ValidateSrcUriL(TDesC8& aPath,TDesC8& aIp);
    /**
    * Removes white spaces (and '\r \n \t and space') from the given descriptor 
    * @since Series S60 3.0
    * @param aString descriptor to be processed
    */
    static void RemoveWhiteSpacesL(TDes8& aString);

    /**
    * Checking if there are white spaces(and '\r \n \t and space' ) in the given descriptor
    * @since Series S60 3.1
    * @param aString 
    * @return boolean
    */
    static TBool IsWhiteString(const TDesC8& aString);
    
    /**
    * Gets element
    * @since Series S60 3.1
    * @param aFragment 
    * @return TXmlEngElement
    */      
    static TXmlEngElement GetObjectElementL(RXmlEngDocument& aFragment);
    
    /**
    * Checks if Ref id value exists
    * @since Series S60 5.0 HN
    * @param aFragment 
    * @return TBool
    */     
    static TBool HasRefIdL( RXmlEngDocument& aFragment );
    
    /**
    * Encodes Xml 
    * @since Series S60 3.1
    * @param aString
    * @return descriptor pointer
    */
    static HBufC8* EncodeXmlStringL(const TDesC8& aString);
    
     
   	static HBufC* Des8ToDesL(const TDesC8& aString);
    
    /**
    * Conversion from TDes8 to TDes16
    * @since Series S60 3.1
    * @param aString
    * @return descriptor pointer
    */
    static HBufC* Des8ToDesLC(const TDesC8& aString);
    
    /**
    * Conversion from TDes16 to TDes8
    * @since Series S60 3.1
        * @param aString
    * @return descrptor pointer
    */
    static HBufC8* DesToDes8LC(const TDesC& aString);
       
    /*
    * Getter
    * @since Series S60 3.1
        * @param aUri
    * @return TInt
    */
    static TInt64 ResIdFromUriL(const TDesC8& aUri);
    
    /*
    * Getter
    * @since Series S60 3.1
    * @param aUri
    * @return TPtrC8
    */
    static TPtrC8 ResIdFromUriDesL(const TDesC8& aUri);
    
    /*
    * Sets Object Id
    * @since Series S60 3.1
    * @param aElement TXmlEngElement of the element
    * @param aId Id
    */

    static void SetObjectIdL(TXmlEngElement aElement, TInt aId);
    /*
    * Sets refID
    * @since Series S60 5.0 HN
    * @param aElement TXmlEngElement of the element
    * @param aRefId
    */
    static void SetObjectRefIdL(TXmlEngElement aElement, TInt aRefId);
    
    /*
    * Sets parentID
    * @since Series S60 5.0 HN
    * @param aElement TXmlEngElement of the element
    * @param aRefId
    */
    static void SetObjectParentIdL(TXmlEngElement aElement, TInt aParentId);
        
    /*
     * Sets restricted field
     * @since Series S60 5.0 HN
     * @param aElement TXmlEngElement of the element
     * @param aRestrictedFlag
     */
    static void SetRestrictedFieldL( TXmlEngElement aElement, TBool aRestrictedFlag );

    /*
    * Sets Container Id
    * @since Series S60 3.1
    * @param aElement TXmlEngElement of the element
    * @param aParentId ParentId
    */
    static void SetContainerIdL(TXmlEngElement aElement, TInt aParentId);
   	static HBufC8* BuildImportUriLC(TInt aResId);
   	static HBufC8* BuildImportUriShorterLC(TInt64 aResId);
	static HBufC8* BuildContentUriL(TInt aResId, const TDesC& aFileExten, TDesC8& aObjectId);
    static HBufC8* UpnpCdUtils::RandomizeL(TInt aRange);
    /**
    * Checks whether the element is required and removes the attribute notifying it
    * @since Series S60 3.0
    * @param aElement TXmlEngElement of the element
    * @param aObjId aCd pointer to the ContentDirectory object - it provides a function which deletes
    *               a file from the private directory
    * @param ETrue if the element is required, otherwise EFalse
    */
    static TBool IsElementRequiredL(TXmlEngElement aElement);
        /**
    * Replaces the the first occurrence of true/false sequence with 1/0 within given descriptor 
    * @since Series S60 3.0
    * @param aBuf descriptor pointer
    */
    static void ReplaceTrueFalse(HBufC8* aBuf);

    /**
    * Gets the full name with namespace of the given element.
    * @since Series S60 3.0
    * @param aElement an element to get the name of
    * @return descriptor with the namespace:name of the element, 
    *              the caller should delete it when it is no longer needed
    */
    static HBufC8* GetElmNameWithNsL(TXmlEngElement aElement);
     
    static HBufC* EscapeAposL(const TDesC&  aValue);
    static TBool ValidateDateL(  TPtrC8 aValue );
    
    /**
    * Converts TDesC8 to TUint
    */
    static TInt StringToTUint(const TDesC8& aStr, TUint* aInt);
    
    /**
    * Checks whether input string contains a specific number of digits and whether whole number fit into specific range
    * @since Series S60 3.0
    * @param aInput input descriptor [a number]
    * @param aNumber number of digits the descriptor should contain
    * @param aExactDigitsNumber should the number have exact number of digits or not
    * @param aRange the number mustn't exceed the specified range
    * @return ETrue if condition is fulfilled
    */
    static TBool AreDigitsInSpecificRange(const TDesC8& aInput, TInt aNumber, TBool aExactDigitsNumber, TUint32 aRange); 
    
    /**
    * Validates res@duration attribute
    * @since Series S60 3.0
    * @param aInput descriptor containing duration value to validate
    * @return ETrue if res@duration has proper format
    */
    static TBool ValidateDurationValue(const TDesC8& aInput);  
    
    /**
    * Split string into pieces with delimeter
    * @since Series S60 3.0
    * @param aLexeme a lexer to parse
    * @param aDelimeter delimeter    
    */
    static void SplitStringByDelimeter(TLex8& aLexeme, TChar aDelimeter);
    
    /**
    * Skip characters in lexer
    * @since Series S60 3.0
    * @param aLexer a lexer to parse
    * @param aValue number of characters to skip
    */
    static void Skip(TLex8& aLexer, TInt aValue);
        
         /**
    * Extracts integer value from descriptor
    * @param aStr descriptor to be extracted
    * @param aInt integer to be fill in
    * @return error status
    */
    static TInt StringToInteger(const TDesC8& aStr, TInt* aInt);
            
 private:
     enum TPlaces
        {
        EYear = 4,
        EMonth = 7,
        EDay = 10,
        EHour = 13,
        EMinute = 16,
        ESecond = 19,
        EZone1 = 20,
        EHourOffset1 = 22,
        EMiliSecond = 23,
        EZone2 = 24,
        ETimeOffset1 = 25,
        EHourOffset2 = 26,
        ETimeOffset2 = 29
        };
    /*
    * C++ default constructor.
    */
    UpnpCdUtils();
};

#endif      // CDUTILS_H   
            
// End of File
