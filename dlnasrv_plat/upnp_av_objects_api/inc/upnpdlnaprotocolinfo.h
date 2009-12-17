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


#ifndef C_CUPNPDLNAPROTOCOLINFO_H
#define C_CUPNPDLNAPROTOCOLINFO_H

#include <e32base.h>
#include "upnpdlnaprotocolinfocons.h"
#include "upnpprotocolinfo.h"

/**
* UpnpDlnaProtocolInfo class.
*  
*  
*
*  @lib upnpavobjects.lib
*  @since S60 3.1
*/
class CUpnpDlnaProtocolInfo : public CUpnpProtocolInfo 
{                          
//PUBLIC MEMBERS:
public: 


    /**
    * Factory method for creating instance of the class. 
    * aInputString must contain protocolInfo string. 
    */
    IMPORT_C static CUpnpDlnaProtocolInfo* NewL();
    
    /**
    * Factory method for creating instance of the class. 
    * aInputString must contain protocolInfo string. 
    */
    IMPORT_C static CUpnpDlnaProtocolInfo* NewL(const TDesC8& aInputString);

    /**
    * Destructor of the class. 
    */
    virtual ~CUpnpDlnaProtocolInfo();    
                        
// PUBLIC ACCESSED METHODS - EXPORTED INTERFACE
public: // 4th_field methods

   
     /*
     * 4th_field = pn-param [op-param] [ps-param] [ci-param] [flags-param] [ *(other-param)]
     */        
     IMPORT_C virtual TPtrC8 FourthField();

     
public: //pn-param methods

     /**
     * Setter for DLNA.ORG_PN
     */
     IMPORT_C void SetPnParameterL( const TDesC8& aValue ); 
     
     /**
     * Getter for DLNA.ORG_PN
     * pn-param= "DLNA.ORG_PN=" pn-value, where pn-value= *<'a'-'z', 'A'-'Z', '0'-'9', '_'>
     */
     IMPORT_C TPtrC8 PnParameter(); 


public: //op-param methods

     /**
     * Setter for DLNA.ORG_OP
     */
     IMPORT_C void SetOpParameterL( const TDesC8& aValue );
     
     /**
     * Setter for DLNA.ORG_OP
     */
     IMPORT_C void SetOpParameterL( UpnpDlnaProtocolInfo::TDlnaParams param, TBool aValue );
     
     /**
     * Getter for DLNA.ORG_OP
     * op-param = [op-param-delim] "DLNA.ORG_OP=" op-value
     * op-param-delim = ';'
     *           op-value = a-val b-val
     *           a-val = Boolean b-val = Boolean, Boolean = '1' | '0'
     */
     IMPORT_C TPtrC8 OpParameterL();
     
     
     IMPORT_C TBool OpParameter( UpnpDlnaProtocolInfo::TDlnaParams param );


public: //ps-param methods

     /**
     * Setter for DLNA.ORG_PS 
     */
     IMPORT_C void SetPsParameterL(const TDesC8& aValue);

     /**
     * Getter for DLNA.ORG_PS 
     * [ps-param-delim] "DLNA.ORG_PS=" ps-value
     * ps-param-delim = ';', ps-value = [server-speed *(',' server-speed)]
     * server-speed = 
     * <conforms to the TransportPlaySpeed string, as specified in the AVTransport specification>
     */
     IMPORT_C TPtrC8 PsParameter(); 

public: //ci-param methods

     /**
     * Setter for DLNA.ORG_CI
     */
     IMPORT_C void SetCiParameter( TBool aValue );

     /**
     * Getter for DLNA.ORG_CI
     * [ci-param-delim] "DLNA.ORG_CI=" ci-value
     *  ci-param-delim = ';' ci-value = Boolean Boolean = '0' | '1'
     */
     IMPORT_C TBool CiParameter();

                                                  
public: //flags-param methods
     

     //ACCESS METHODS FOR ALL FLAGS AND PARAMETRES                
     /**
     * Setter for a single flag.
     */
     IMPORT_C void SetDlnaFlag( UpnpDlnaProtocolInfo::TDlnaFlags flag, TBool aValue );
     /**
     * Getter for a single flag.
     */
     IMPORT_C TBool DlnaFlag( UpnpDlnaProtocolInfo::TDlnaFlags flag );
                                                                             
     /**
     * setter for DLNA.ORG_FLAGS 
     */
     IMPORT_C void SetFlagsParameterL( const TDesC8& flags );                  
     
     /**
     * getter for DLNA.ORG_FLAGS 
     * "DLNA.ORG_FLAGS=" flags-value
     *  flags-value = primary-flags reserved-data 
     *     primary-flags = 8 hexdigit
     *     reserved-data = 24 reserved-hexdigit
     *     hexdigit = <hexadecimal digit: '0' - '9', 'A' - 'F', 'a' - 'f'>, 
     *     reserved-hexdigit = '0'
     */
     IMPORT_C TPtrC8 FlagsParameterL();                  
     
     
public: //other-param methods
     
     /**
     * Setter for other parameter.
     * Refer
     */
     IMPORT_C void SetOtherParamL(const TDesC8& aValue);

     /**
     * Getter for other parameter value.
     * other-param = [other-param-delim] IANA-name '_' other-param-name '=' other-param-value
     * other-param-delim = ';'
     * IANA-name = <IANA-registered name, with top level domain (e.g. .net, .org, .com)>
     * other-param-name = *<'a'-'z', 'A'-'Z', '0'-'9'>
     */
     IMPORT_C TPtrC8 GetOtherParams();
     
                                
protected: //Base construct

     /** 
     * Constructor of the class          
     */
     CUpnpDlnaProtocolInfo();

      /**
     * Method for creating instance of class. 
     * aInputString must contain protocolInfo string. 
     */
     void ConstructL(const TDesC8& aInputString);
    
protected: 

     /**
     * Setter for DLNA.ORG_OP time seek parameter.
     */
     void SetOpTimeParam(TBool aValue);
     
     /**
     * Setter for DLNA.ORG_OP range seek parameter.
     */
     void SetOpRangeParam(TBool aValue);
     
     /**
     * Setter for DLNA_FLAGS 
     */
     void SetDlnaFlag(TBool aValue, TUint8 aFlagIndex); 
     
     /**
     * This function produces a string containing all info taken from fourth ProtocolInfo field         
     * aBuffer - on this variable values are appended. 
     */
     virtual void GetFourthParameterInternalL(TDes8& aBuffer); 

public:          
     /**
     * Getter for DLNA.ORG_OP time seek parameter.
     */
     TBool GetDlnaOpTimeParam(); 
                            
     /**
     * Getter for DLNA.ORG_OP range seek parameter
     */
     TBool GetDlnaOpRangeParam(); 
     

//MEMBER METHODS - USED INTERNALLY
private:     
              
     /**
     * Getter for DLNA_FLAGS 
     */
     TBool GetDlnaFlag(TInt aFlagIndex); 
                     
     /**
     * Parses aInputString into protocolInfo object.  
     * aInputString- descriptor with protocolInfo
     * Returns     KErrNone when everything ok.
     *             KErrArgument when there was additionalInfo string empty.
     */
     virtual TInt ParseL(const TDesC8& aInputString);
                               
     /**
     * Parses the fourth parameter.
     * Returns     KErrNone when everything is ok.
     *             KErrArgument when additionalInfo string is empty.
     */
     TInt ParseForthParameterL();

     /**
     * Parses DLNA.ORG_PN params.
     */        
     void ParseDlnaPnParamL(TLex8& aLexer);     
     
     /**
     * Parses DLNA.ORG_OP params.
     */         
     void ParseDlnaOpParamL(TLex8& aLexer);
     
     /**         
     * Parses DLNA.ORG_PS params.
     */
     void ParseDlnaPsParamL(TLex8& aLexer);                  
              
     /**
     * Parses DLNA.ORG_CI params.
     */
     void ParseDlnaCiParamL(TLex8& aLexer);
     
     /**
     * Parses DLNA.ORG_FLAGS params.
     */
     void ParseDlnaFlagsParamL(TLex8& aLexer);
     
     /**
     * Parses other params.
     */
     void ParseOtherParamsL(TLex8& aLexer);
     
     /**
     * Parses the fourth parameter
     * Returns: ETrue - When the last parameter has been parsed. 
     *             EFalse - In other situations.
     */
     TBool ParseForthParameterInternalL( const TDesC8& aName, TLex8& aLexer);
     
     /**
     * Setter for DLNA_FLAGS 
     * Sets in aParameter, aValue at aBitNumber counting from right.
     */
     void SetIntBit8(TUint8& aParameter, TBool aValue, TUint8 aBitNumber);

     /**
     * Setter for DLNA_FLAGS 
     * Sets aValue on position specified by aBitNumber in aParameter byte.         
     */
     void SetIntBit32(TUint32& aParameter, TBool aValue, TUint8 aBitNumber);

     /**
     * Setter for Internal variable. 
     * Sets aValue on position specified by aBitNumber in iParams byte.         
     */
     void SetDlnaParam(TBool aValue, TUint8 aParamIndex);
     
     /**
     * Getter for Internal variable. 
     * Gets a bit from iParams byte, from aParamIndex position.         
     */
     TBool GetDlnaParam(TInt aParamIndex);  
protected:
     /**
     * This function returns ETrue if at least one dlna flag is set.         
     */ 
     TBool IsDlnaFlagsSet(); 
public:                                                  
   
     /**
     * This function returns length of the string containing all relevant information from fourth ProtocolInfo field. 
     * Returns - length of the string for 4th field.
     */
     virtual TInt GetFourthParameterLength(); 
     
     /**
     * This member function serializes iDlnaFlags member variable into a string. 
     * aBuffer - variable to which iDlnaFlags will be appended.
     */
     void SerializeDlnaFlagsL(TDes8& aBuffer); 
     
     void FourthParameterInternalL();
     
                                                                                  
//MEMBER VARIABLES
protected: //data 

     /**
     * If not null, it contains values of DLNA.ORG_PN parameter. 
     */
     HBufC8* iDlnaPn;
                   
     /**
     * If not null, it contains values of DLNA.ORG_PS parameter. 
     * There is no additional support for this parameter.
     */
     HBufC8* iDlnaPs;
     
     /**
     *  It contains values of DLNA.ORG_CI parameter. 
     */
     TInt iDlnaCi;
     
     /**
     * There are stored binary parameters from DLNA specification in this member. 
     * 7-bit 0-bit 
     * 7-bit this is LSOP Time parameter from DLNA.ORG_OP
     * 6-bit this is LSOP Range parameter from DLNA.ORG_OP 
     * 5-bit this is CI parameter DLNA.ORG_CI
     * There is a enum containing this information: DLNA_PARAMS
     *
     */
     TUint8 iParams; 
     
     /**
     * This contains flags, which are DLNA.ORG_FLAGS parameter value
     * TUnit32[0] This main flags field contains: 
     * 31-bit ... 0-bit where: Values stored in enum: DLNA_FLAGS. 
     * 31-bit SP_FLAG Sender Paced flag.
     * 30-bit LSOP_TSR_FLAG limited time seek flag
     * 29-bit LSOP_RANGE_FLAG limited range seek flag 
     * 28-bit PLAY_CONTAINER  play container flag
     * 27-bit S0_INC_FLAG S0 increasing flag. 
     * 26-bit SN_INC_FLAG SN increasing flag
     * ... Unused
     * TUint32[1], TUint32[2], TUint32[3], there is no method to access this members.
     * this might be used in inherited classes.
     */ 
     TUint32 iDlnaFlags[4];        
              
     /**
     * String containing information about other parameters from DLNA specification. 
     * In case of this is not DLNA protocolInfo, it contains information about whole 4th field.
     */
     HBufC8* iOtherParams;       
     
     //Buffer for flags parameter
     HBufC8* iFlagsParam;     
       
     //Buffer for op paramater
     HBufC8* iOpParam;       
};

#endif // C_CUPNPDLNAPROTOCOLINFO_H
// End of file