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


#ifndef C_CUPNPPROTOCOLINFO_H
#define C_CUPNPPROTOCOLINFO_H

#include <e32base.h>


/**
*  CUpnpProtocolInfo class.
*  Class siplifies mainipulation of UPnP res@protocolInfo property.
*  Syntax: 
*      protocolInfo =  
*            <protocol>’:’ <network>’:’<contentFormat>’:’<additionalInfo>
*
*      (See ConnectionManager:1 Service Template Version 1.01 for details)
*
*  Use case 1: Building protocol info: 
*
*     _LIT8(KFirstParam, "http-get");
*     _LIT8(KSecondParam, "*");
*     _LIT8(KThirdParam, "image/jpeg");
*     _LIT8(KFourthParam, "*");          
*     
*     CUpnpProtocolInfo * protocolInfo = CUpnpProtocolInfo::NewL();
*     protocolInfo->SetFirstFieldL(KFirstParam());
*     protocolInfo->SetSecondFieldL(KSecondParam());
*     protocolInfo->SetThirdFieldL(KThirdParam());
*     protocolInfo->SetFourthFieldL(KFourthParam());
*
*  Use case 2: Parsing existing protocol info: 
*
*     _LIT8(KInfo, "http-get:*:audio/mpeg:*");
*     CUpnpProtocolInfo * protocolInfo = CUpnpProtocolInfo::NewL(KInfo());
*     TPtrC8 param = protocolInfo->SetFirstField();
*     param = protocolInfo->SecondParameterL();
*     param = protocolInfo->ThirdParameterL();
*     param = protocolInfo->FourthParameterL();
*
*  @lib upnpavobjects.lib
*  @since S60 3.1
*/
class CUpnpProtocolInfo: public CBase 
{    	    	  	    

//PUBLIC MEMBERS:
public: 
	
    /**
    * Factory method. Creates CProtocolInfo with empty parameters
    *  
    * @param aProtocolInfo protocolInfo string to be parsed
    * @return instance CProtocolInfo
    */
    IMPORT_C static CUpnpProtocolInfo* NewL();
    
    	 
    /**
    * Factory method. Parses input descriptor.
    * Example:
    * _LIT8(KProtocolInfo,""http-get:*:audio/mpeg:*");
    * CProtocolInfo* protocolInfo = CProtocolInfo::NewL( KProtocolInfo() ); 
    *  
    * @param aProtocolInfo protocolInfo string to be parsed
    * @return instance CProtocolInfo
    */
    IMPORT_C static CUpnpProtocolInfo* NewL(const TDesC8& aProtocolInfo);

    
    /**
    * Destructor of the class. 
    */
    virtual ~CUpnpProtocolInfo();	
    	    	    	    
// PUBLIC ACCESSED METHODS - EXPORTED INTERFACE
public:
        	 
     /**
     * Gets whole ProtocolInfo as a descriptor.
     * @return descriptor
     */
     IMPORT_C TPtrC8 ProtocolInfoL();
         	 		  		 
     /**
     * Getter for ProtocolInfo.
     * @return parameter value
     */
     IMPORT_C TPtrC8 FirstField(); 
     
     /**
     * Setter for first parameter value.
     * @param aValue parameter value
     */
     IMPORT_C void SetFirstFieldL(const TDesC8& aValue); 
    
     /**
     * Getter for second parameter.
     * @return parameter value
     */
     IMPORT_C TPtrC8 SecondField(); 
     
     /**
     * Setter for second parameter value.
     * @param aValue parameter value
     */
     IMPORT_C void SetSecondFieldL(const TDesC8& aValue);
    
     /**
     * Getter for third parameter.
     * @return parameter value
     */
     IMPORT_C TPtrC8 ThirdField(); 
    
     /**
     * Setter for third parameter value.
     * @param aValue parameter value
     */
     IMPORT_C void SetThirdFieldL(const TDesC8& aValue);
    
     /**
     * Getter for fourth parameter 
     * @return parameter value
     */
     IMPORT_C virtual TPtrC8 FourthField();
    
     /**
     * Setter for fourth parameter value.
     * @param aValue parameter value
     */
     IMPORT_C void SetFourthFieldL(const TDesC8& aValue);
    
    
    	     	     	     	     	 	 	 	
protected: //Base construct

     /** 
     * Constructor of the class     	 
     */
     CUpnpProtocolInfo();
    
      /**
     * Method for creating instance of class. 
     * aInputString must contain protocolInfo string. 
     */
     void BaseConstructL(const TDesC8& aInputString);
     
     /**
     * This function produces a string containing all info taken from fourth ProtocolInfo field    	 
     * aBuffer - on this variable values are appended. 
     */
     virtual void GetFourthParameterInternalL(TDes8& aBuffer);
    	                  	  		
//MEMBER METHODS - USED INTERNALLY
protected: 	
		 		 		 	 	 	 	 
     /**
     * Parses aInputString into protocolInfo object.  
     * aInputString- descriptor with protocolInfo
     * Returns 	KErrNone when everything ok.
     * 			KErrArgument when there was additionalInfo string empty.
     */
     virtual TInt ParseL(const TDesC8& aInputString);
     
     /**
     * This function returns length of the string containing all relevant information from fourth ProtocolInfo field. 
     * Returns - length of the string for 4th field.
     */
     virtual TInt GetFourthParameterLength(); 
     
     /**
     * It marks a string placed between start position and aDelimeter    	     	 
     * aLexeme - TLex8 object on which marking is occuring.
     * aDelimeter - character on which marking will end. 
     */
     void ParseAtomToDelimeter(TLex8& aLexeme, TChar aDelimeter);
     
     /**
     * Sets one of the four parameters of protcolInfo object:
     * <protocol>':'<network>':'<contentType>':'<additionalInfo>
     * protocol - iFirstField. 
     * network- iSecondField
     * contentType - iThirdField
     * additionalInfo iFourthField.
     */
     void SetParameterL(TInt aWhich, const TDesC8& aValue);
    	     	     	     	     	 
     /**
     * Skips aValue characters and marks after end of skipping.
     */
     void SkipAndMark(TLex8& aLexer, TInt aValue);
    
     /**
     * Skips aValue characters. Do not mark.
     */
     void Skip(TLex8& aLexer, TInt aValue);
         	 
     /**
     * Check if character contains correct value for boolean type (1 or 0)
     * @param character to check
     * @return KErrNone if value is correct, else KErrArgument
     */ 	 	 	 
     TInt CheckBooleanValue(TChar aCharacter);
    	 	 	 
     /**
     * Returns - integer value, which contains iformation about the length of the string currently stored in the class. 
     */
     TInt GetProtocolInfoLength(); 
    	     	     	     	 	 	     	    	     	     	              	     	    	     	     	 
//MEMBER VARIABLES
protected: //data

    /**
    * ProtoclInfo. 
    */	
    HBufC8* iProtocolInfo;
    /**
    * Third field of protoclInfo. 
    */	
    HBufC8* iThirdParameter;    
    
    /**
    * Fourth field of protocolInfo. 
    * If this field equals NULL this means that information in rest DLNA parameters is valid. 
    * else, the field contains information from parsed string.
    */	
    HBufC8* iFourthParameter;    
    	 
private: //data
	 
     /**
     * First field of protoclInfo. 
     */	
     HBufC8* iFirstParameter; 
    
     /**
     * Second field of protoclInfo. 
     */	
     HBufC8* iSecondParameter;     	  	 	     
};

#endif // C_CUPNPPROTOCOLINFO_H
// End of file