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
* Description:  Declares ProtocolInfo field wrapper. 
*
*/


#include "upnpprotocolinfolocal.h"
#include "upnpdlnaprotocolinfocons.h"

#include <e32base.h>
#include <e32cons.h>

using namespace UpnpDlnaProtocolInfo;

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::CUpnpProtocolInfoLocal()
//  Constructor of the class
//-----------------------------------------------------------------
CUpnpProtocolInfoLocal::CUpnpProtocolInfoLocal(): CUpnpDlnaProtocolInfo()
    {
    }

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::~CUpnpProtocolInfoLocal()
//  Destructor of the class
//-----------------------------------------------------------------
CUpnpProtocolInfoLocal::~CUpnpProtocolInfoLocal()
    {
    }

//-----------------------------------------------------------------
// CUpnpProtocolInfoLocal::NewL
// Method for creating instance of class. aInputString has to contain protocolInfo string.
//-----------------------------------------------------------------
EXPORT_C CUpnpProtocolInfoLocal* CUpnpProtocolInfoLocal::NewL(const TDesC8& aInputString)
    {
    CUpnpProtocolInfoLocal* newElement = new (ELeave) CUpnpProtocolInfoLocal();
    CleanupStack::PushL( newElement );
    newElement->ConstructL( aInputString ); 
    newElement->SetDefaultFourthField();      
    CleanupStack::Pop( newElement );
    return newElement; 
    }
    
//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::NewL
//  Method for creating instance of class. 
//  aMimeType - mime type for file.
//  aAdditionalInfo - additional information about what DLNA.ORG_PN type should be used
//-----------------------------------------------------------------
EXPORT_C CUpnpProtocolInfoLocal* CUpnpProtocolInfoLocal::NewL(const TDesC8& aMimeType, TInt aAdditionalInfo)
    {
    CUpnpProtocolInfoLocal* newElement = new (ELeave) CUpnpProtocolInfoLocal();
    CleanupStack::PushL( newElement );
    newElement->ConstructL( aMimeType, aAdditionalInfo ); 
    CleanupStack::Pop( newElement );
    return newElement;   
    }
//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::NewL
//  Method for creating instance of class.
//  aMimeType - mime type for file.
//-----------------------------------------------------------------
EXPORT_C CUpnpProtocolInfoLocal* CUpnpProtocolInfoLocal::NewL(const TDesC8& aMimeType, const TDesC8& aDlnaPn)
    {
    CUpnpProtocolInfoLocal* self = new (ELeave) CUpnpProtocolInfoLocal();
    CleanupStack::PushL( self );
    self->ConstructFromDlnaPnL( aMimeType, aDlnaPn ); 
    CleanupStack::Pop( self );
    return self;   
    }

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::ConstructFromDlnaPnL(const TDesC8& aMimeType, const TDesC8& aDlnaPn)
//  Method for creating instance of class. aInputString has to contain protocolInfo string.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::ConstructFromDlnaPnL(const TDesC8& aMimeType, const TDesC8& aDlnaPn)
    {
    SetFirstFieldL(KDEFAULT_1ST_PARAM);
    SetSecondFieldL(KDEFAULT_STAR_PARAM);
    SetThirdFieldL(aMimeType);
    
    /*
    * Requirements:
    * 1)  7.3.28.3 MM CMS:GetProtocolInfo Rules: Formatting options for Connection Manager Service 
    * 2)  7.3.134.3 MM/CM General Rules for CDS:CreateObject Errors: Bad Class or ProtoInfo 
    *
    * This concerns the following protocolInfo's:
    *
    *       http-get:*:image/jpeg:*
    *       http-get:*:image/gif:*
    *       http-get:*:audio/mpeg:*
    *       http-get:*:audio/mp4:*
    *       http-get:*:*:*
    */
    if ( aDlnaPn == KDEFAULT_STAR_PARAM )
        {
        SetFourthFieldL(aDlnaPn);
        }
    else
        {
        SetPnParameterL(aDlnaPn);
        SetDefaultFourthField();
        }            
    }

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::ConstructL(TDesC8& aInputString)
//  Method for creating instance of class. aInputString has to contain protocolInfo string.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::ConstructL(const TDesC8& aInputString)
    {
    CUpnpDlnaProtocolInfo::ConstructL(aInputString);
    }

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::ConstructL(TDesC8& aInputString)
//  Method for creating instance of class.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::ConstructL(const TDesC8& aMimeType, TInt aAdditionalInfo)
    {
    DefaultValuesL();
    ChooseDlnaL(aMimeType, aAdditionalInfo);
    }

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::ChooseDlnaL(TDesC8& aMimeType, TInt aAdditionalInfo)
//  Main function for creating from mime type.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::ChooseDlnaL(const TDesC8& aMimeType, TInt aAdditionalInfo)
    {
    switch(aAdditionalInfo)
        {
        case NO_DLNA_DATA: 
            break; 
        case NO_ADDITIONAL_INFO: 
            FillAccordingToMimeL(aMimeType);
            break;
            
        default: 
            FillAccordingToAddInfoL(aAdditionalInfo);
            break; 
        }
    }
    
//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::DefaultValuesL()
//  Fills up default values to create protocolInfo in form: "http-get:*:*:*"
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::DefaultValuesL()
    {
    SetFirstFieldL(KDEFAULT_1ST_PARAM());
    SetSecondFieldL(KDEFAULT_STAR_PARAM());
    SetThirdFieldL(KDEFAULT_STAR_PARAM());
    SetFourthFieldL( KDEFAULT_STAR_PARAM() );
    }    

//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::FillAccordingToAddInfoL(TInt aAdditionalInfo)
//  Fills dlna parameters according only to additionalInfo which is TInt value.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::FillAccordingToAddInfoL(TInt aAdditionalInfo)
    {
    switch(aAdditionalInfo)
        {
        case JPEG_SM: 
            SetPnParameterL( KDLNA_PN_JPEG_SM() );
            break; 
        case JPEG_MED:
            SetPnParameterL( KDLNA_PN_JPEG_MED() );
            break; 
        case JPEG_LRG:
            SetPnParameterL( KDLNA_PN_JPEG_LRG() );
            break; 
        case JPEG_TN:
            SetPnParameterL( KDLNA_PN_JPEG_TN() );
            break;
        case JPEG_SM_ICO:
            SetPnParameterL( KDLNA_PN_JPEG_SM_ICO() );
            break;
        case JPEG_LRG_ICO:
            SetPnParameterL( KDLNA_PN_JPEG_LRG_ICO() );
            break;
        case PNG_TN:
            SetPnParameterL( KDLNA_PN_PNG_TN() );
            break;
        case PNG_SM_ICO:
            SetPnParameterL( KDLNA_PN_PNG_SM_ICO() );
            break;
        case PNG_LRG_ICO:
            SetPnParameterL( KDLNA_PN_PNG_LRG_ICO() );
            break;
        case PNG_LRG:
            SetPnParameterL(  KDLNA_PN_PNG_LRG() );
            break;
        case MP3:
            SetPnParameterL( KDLNA_PN_MP3() );
            break;
        default:
            break;
        }
        SetFourthFieldL( KNullDesC8() );        
    }
    
//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::FillAccordingToMimeL(TDesC8& aMimeType)
//  Fills dlna parameters according only to info in mime type.
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::FillAccordingToMimeL(const TDesC8& aMimeType)
    {
    HBufC8* mimeTypeH = aMimeType.AllocLC();
    TPtr8 mimeType(mimeTypeH->Des());
    mimeType.LowerCase();
    if ( mimeType.Compare( KMT_IMAGE_JPEG ) == 0 )
        {
        SetPnParameterL(KDLNA_PN_JPEG_LRG() );
        }
    else if (mimeType.Compare( KMT_IMAGE_PNG ) == 0 )
        {
        SetPnParameterL( KDLNA_PN_PNG_LRG() );    
        }
    else if ( mimeType.Compare( KMT_AUDIO_MP3 ) == 0 )
        {
        SetPnParameterL( KDLNA_PN_MP3() );
        }
    else
        {
        CleanupStack::PopAndDestroy(mimeTypeH);
        return;
        }
    CleanupStack::PopAndDestroy(mimeTypeH);
    SetFourthFieldL( KNullDesC8() );
    }        
    
//-----------------------------------------------------------------
//  CUpnpProtocolInfoLocal::SetDefaultForthParameter()
//  This member function sets defult values for optional parameters of 4th field
//-----------------------------------------------------------------
void CUpnpProtocolInfoLocal::SetDefaultFourthField()
    {
    
    if ( iThirdParameter -> FindC( KMimeImage() ) != 0 && 
        iThirdParameter -> FindC( KMimeAudio() ) != 0 &&
        iThirdParameter -> FindC( KMimeVideo() ) != 0 )
        {
        return;
        }
        
    SetOpTimeParam(EFalse);
    SetOpRangeParam(ETrue);
    
    iDlnaFlags[0] = 0;
    iDlnaFlags[1] = 0;
    iDlnaFlags[2] = 0;
    iDlnaFlags[3] = 0;
    
    if ( iThirdParameter -> FindC( KMimeImage() ) == 0 ) 
        {
        SetDlnaFlag( ETrue, TM_I_FLAG);
        SetDlnaFlag( ETrue, TM_B_FLAG);
        }

    if ( iThirdParameter -> FindC( KMimeAudio() ) == 0 ) 
        {
        SetDlnaFlag( ETrue, TM_S_FLAG);
        SetDlnaFlag( ETrue, TM_B_FLAG);
        }
    
    if ( iThirdParameter -> FindC( KMimeVideo() ) == 0 ) 
        {
        SetDlnaFlag( ETrue, TM_S_FLAG);
        SetDlnaFlag( ETrue, TM_B_FLAG);
        }    
    SetDlnaFlag( ETrue, DLNA_V1_5_FLAG );
    }        
    
void CUpnpProtocolInfoLocal::GetFourthParameterInternalL(TDes8& aBuffer)    
    {
    _LIT8( KTempAsterix, "*");
    if (iFourthParameter)
        {
        if ( iFourthParameter->Des() == KTempAsterix())
            {
            aBuffer.Append(iFourthParameter->Des());                
            }        
            else
            {
            CUpnpDlnaProtocolInfo::GetFourthParameterInternalL(aBuffer);                
            }
        }
     else
        {  
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
        //    aBuffer.Append( KDlnaDelimeter ); 
        //    aBuffer.Append( KDLNA_PS );
        //    aBuffer.Append( KDlnaTokenizer ); 
        //    aBuffer.Append( *iDlnaPs );
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
    }
    
    
//end of file
