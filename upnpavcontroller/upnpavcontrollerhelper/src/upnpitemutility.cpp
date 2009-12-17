/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Utility for parsing upnp items
*
*/






// INCLUDE FILES
// System
#include <e32std.h>

// upnp stack api
#include <upnpitem.h>
#include <upnpdlnaprotocolinfo.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for KValueNotImplemented
#include "upnpitemutility.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KDateStringLength        = 10;
const TInt KDateTimeStringLength    = 19;
const TInt KMaxDateStringLength     = 30;
const TInt KCodeSemicolon           = 58;
_LIT( KSeparator,                   ":" );
_LIT( KNullTime,                    "000000" );
_LIT8( KHttp,                       "http://" );
_LIT8( KCiParam,                    "DLNA.ORG_CI" );

/* removed due to M-DMC CTT test case 7.3.26.2
_LIT8( KAudioSupport,               "audio/" );
_LIT8( KImageSupport,               "image/" );
_LIT8( KVideoSupport,               "video/" );
*/

// ============================ LOCAL FUNCTIONS =============================

// --------------------------------------------------------------------------
// UPnPItemUtility::BelongsToClass
//---------------------------------------------------------------------------
EXPORT_C TBool UPnPItemUtility::BelongsToClass(
    const CUpnpObject& aObject,
    const TDesC8& aClass )
    {
    TBool beginsWith = 
        ( aObject.ObjectClass().Find( aClass ) == 0 );
    return beginsWith;
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::GetResElements
//---------------------------------------------------------------------------
EXPORT_C void UPnPItemUtility::GetResElements(
    const CUpnpObject& aObject,
    RUPnPElementsArray& aResElementsArray )
    {
    const RUPnPElementsArray& array =
        const_cast<CUpnpObject&>(aObject).GetElements();
    
    for( TInt i = 0; i < array.Count(); i++ )
        {
        if( array[ i ]->Name() == KElementRes() )
            {
            aResElementsArray.Append( array[ i ] );
            }
        }    
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::ResourceFromItemL
//---------------------------------------------------------------------------
EXPORT_C const CUpnpElement& UPnPItemUtility::ResourceFromItemL(
    const CUpnpItem& aItem )
    {
    __LOG( "UpnpItemUtility:ResourceFromItemL" );

    // Obtain list of item's res elements   
    RUPnPElementsArray elms;
    GetResElements( aItem, elms );
    TInt count = elms.Count();

    CUpnpDlnaProtocolInfo* pInfo = NULL;
    TBool found = EFalse;
    TInt i(0);
    // bestCandidate is an index of some res element in the list. This res
    // will be considered as the best candidate for desired res and that
    // candidate will be returned if no res element contains false CI-flag.

    const CUpnpElement* bestCandidate = 0;

    // determine which resource is the original one
    // 1. In DLNA 1.5 case, parse protocolInfo attribute and see if some res 
    //    element has CI-flag false (=not converted so that is what we want)
    // 2. In non-DLNA 1.5 case and in DLNA 1.5 case where CI-flag does not 
    //    exist, do the following:
    //      o filter out other than HTTP GET resources (internal uri's, RTP)
    //      o filter out resources that do not match itemtype (mime type of 
    //        audio file resources should start with "audio/" etc.)
    for( i = 0 ; i < count; i++ )
        {
        // Make sure that it is a HTTP GET resource. Otherwise continue.
        if( elms[ i ]->Value().Left( KHttp.iTypeLength ).Compare( KHttp() ) 
            != 0 )
            {
            continue;
            }

        // Obtain protocolInfo of the res element.
        const CUpnpAttribute* attr = FindAttributeByName(
            *elms[i], KAttributeProtocolInfo() );
        if ( attr ) 
            {           
            TRAP_IGNORE( pInfo = CUpnpDlnaProtocolInfo::NewL( attr->Value() ) );
            if( !pInfo )
                {
                //if pInfo, start next one!
                continue;
                }
            // check if CI parameter is false or it doesn't have CI parameters at all.
            //for upnp item, always the first res element is the best, resolution
            // should be checked in the future
            if ( ( attr->Value().Find( KCiParam() ) != KErrNotFound && 
                 pInfo->CiParameter() == EFalse ) || 
                 attr->Value().Find( KCiParam() ) == KErrNotFound )
                {
                // end loop, we found what we were looking for. 
                found = ETrue;
                delete pInfo; pInfo = NULL;
                break;                
                }

/* removed due to M-DMC CTT test case 7.3.26.2            
            // check that mimetype corresponds to objectType
            TPtrC8 mime = pInfo->ThirdField();
            
            TPtrC8 objectClass = aItem.ObjectClass();
            if ( objectClass.Compare( KClassAudio ) == 0 ) 
                {
                if ( mime.Left( KAudioSupport().Length() ).CompareF( 
                                                    KAudioSupport() ) != 0 ) 
                    {
                    // if mime type does not match to object type, this is 
                    // not the correct resource.
                    delete pInfo; pInfo = NULL;
                    continue;
                    }
                }
            else if ( objectClass.Compare( KClassVideo ) == 0 ) 
                {
                if ( mime.Left( KVideoSupport().Length() ).CompareF( 
                                                    KVideoSupport() ) != 0 ) 
                    {
                    // if mime type does not match to object type, this is 
                    // not the correct resource.
                    delete pInfo; pInfo = NULL;
                    continue;
                    }
                }
            else if ( objectClass.Compare( KClassImage ) == 0 ) 
                {
                if ( mime.Left( KImageSupport().Length() ).CompareF( 
                                                    KImageSupport() ) != 0 ) 
                    {
                    // if mime type does not match to object type, this is 
                    // not the correct resource.
                    delete pInfo; pInfo = NULL;
                    continue;
                    }
                }
 */
            // use the first suitable res field as candidate which will be
            // returned if better is not found.
            // More sophisticated solution would be to compare resolution
            // etc. attributes to determine the best candidate,
            if ( 0 == bestCandidate ) 
                {
                bestCandidate = elms[i];
                }
            delete pInfo; pInfo = NULL;
            }
        else 
            {
            // No mandatory protocolinfo attribute. This is not what we want.
            }
        }
    if ( found ) 
        {
        bestCandidate = elms[i];
        }

    // close the elements array
    elms.Close();

    if( bestCandidate == 0 )
        {
        User::Leave( KErrNotFound );
        }
    return *bestCandidate;
    }


// --------------------------------------------------------------------------
// UPnPItemUtility::FindElementByName
//---------------------------------------------------------------------------
EXPORT_C const CUpnpElement* UPnPItemUtility::FindElementByName(
    const CUpnpObject& aObject, const TDesC8& aName )
    {
    __LOG( "UpnpItemUtility:FindElementByName" );

    CUpnpElement* element = NULL;
    const RUPnPElementsArray& array =
        const_cast<CUpnpObject&>(aObject).GetElements();
    for( TInt i = 0; i < array.Count(); i++ )
        {
        if( array[ i ]->Name() == aName )
            {
            element = array[ i ];
            i = array.Count();
            }
        }
    return element;
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::FindElementByNameL
//---------------------------------------------------------------------------
EXPORT_C const CUpnpElement& UPnPItemUtility::FindElementByNameL(
    const CUpnpObject& aObject, const TDesC8& aName )
    {
    __LOG( "UpnpItemUtility:FindElementByNameL" );

    const CUpnpElement* element = FindElementByName(
        aObject, aName );
    if( !element )
        {
        User::Leave( KErrNotFound );
        }
    return *element;    
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::FindAttributeByName
//---------------------------------------------------------------------------
EXPORT_C const CUpnpAttribute* UPnPItemUtility::FindAttributeByName(
    const CUpnpElement& aElement, const TDesC8& aName )
    {
    __LOG( "UpnpItemUtility:FindAttributeByName" );

    CUpnpAttribute* attribute = NULL;
    const RUPnPAttributesArray& array =
        const_cast<CUpnpElement&>(aElement).GetAttributes();
    
    for( TInt i = 0; i < array.Count(); i++ )
        {
        
        TBufC8<255> buf(array[ i ]->Name());
        if( array[ i ]->Name() == aName )
            {
            attribute = array[ i ];
            i = array.Count();
            }
        }
    return attribute;
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::FindAttributeByNameL
//---------------------------------------------------------------------------
EXPORT_C const CUpnpAttribute& UPnPItemUtility::FindAttributeByNameL(
    const CUpnpElement& aElement, const TDesC8& aName )
    {
    __LOG( "UpnpItemUtility:FindAttributeByNameL" );

    const CUpnpAttribute* attribute = FindAttributeByName(
        aElement, aName );
    if( !attribute )
        {
        User::Leave( KErrNotFound );
        }
    return *attribute;        
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::UpnpDateAsTTime
//---------------------------------------------------------------------------
EXPORT_C TInt UPnPItemUtility::UPnPDateAsTTime(
    const TDesC8& aUpnpDate, TTime& aTime )
    {
    __LOG( "UpnpItemUtility:UpnpDateAsTTime" );

    TRAPD( err, UPnPItemUtility::UPnPDateAsTTimeL( aUpnpDate, aTime ) );
    return err;
    }

// --------------------------------------------------------------------------
// UPnPItemUtility::UpnpDurationAsMilliseconds
//---------------------------------------------------------------------------
EXPORT_C TInt UPnPItemUtility::UPnPDurationAsMilliseconds(
    const TDesC8& aDuration, TInt& aMilliseconds )
    {
    __LOG( "UpnpItemUtility:UpnpDurationAsMilliseconds" );

    TInt retVal = KErrNone;    
    if( aDuration.Length() > 0 )
        {
        // Check if information is actually returned by the device
        if( aDuration.Compare( KValueNotImplemented ) != 0 )
            {        
            TInt time = 0;
            TChar separator( KCodeSemicolon );
            TInt lposit = aDuration.Locate( separator );
            
            if ( lposit != KErrNotFound )
                {
                TInt rposit = aDuration.LocateReverse( separator );
                if( rposit != lposit )
                    {
                    // Hours
                    TLex8 lex( aDuration.Left( lposit ) );
                    retVal = lex.Val( time ); 
                    if( retVal == KErrNone  )
                        {
                        // Convert to ms and add
                        aMilliseconds += time * 3600 * 1000;
                        // Minutes
                        lex.Assign( aDuration.Mid(
                            lposit + 1, rposit - lposit - 1 ) );
                        retVal = lex.Val( time ); 
                        if( retVal == KErrNone )
                            {
                            // Convert to ms and add
                            aMilliseconds += time * 60* 1000;
                            // Seconds
                            lex.Assign( aDuration.Mid(
                                rposit + 1, 2 ) );
                            retVal = lex.Val( time ); 
                            if( retVal  == KErrNone )
                                {
                                // Convert to ms and add
                                aMilliseconds += time * 1000;
                                }
                            }
                        }
                    }
                else
                    {
                    retVal = KErrNotSupported;
                    }        
                }
            else
                {
                retVal = KErrNotSupported;
                }   
            }  
        else
            {
            retVal = KErrNotSupported;
            }
        }
    else
        {
        retVal = KErrNotSupported;
        }
        
    return retVal;
    }

void UPnPItemUtility::UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
    TTime& aTime )
    {
    // This method is capable of handling the most common dc:date formats:
    // CCYY-MM-DD and CCYY-MM-DDThh:mm:ss
    // Rest of the dc:date formats are handled as well, but they might not
    // be converted precisely
    
    TBuf<KMaxDateStringLength> formatDateString;
    HBufC* dateString = HBufC::NewL( aUpnpDate.Length() );
    dateString->Des().Copy( aUpnpDate );

    if( aUpnpDate.Length() >= KDateStringLength )
        {
        // CCYY-MM-DD --> CCYYMMDD
        formatDateString.Copy( dateString->Des().Left( 4 ) ); // Year
        formatDateString.Append( dateString->Des().Mid( 5,2 ) ); // Month
        formatDateString.Append( dateString->Des().Mid( 8,2 ) ); // Day        

        if( aUpnpDate.Length() >= KDateTimeStringLength )
            {
            // hh:mm:ss --> hhmmss
            formatDateString.Append( KSeparator );
            // Hours
            formatDateString.Append( dateString->Des().Mid( 11, 2 ) ); 
            // Minutes
            formatDateString.Append( dateString->Des().Mid( 14, 2 ) );
            // Seconds 
            formatDateString.Append( dateString->Des().Mid( 17, 2 ) ); 
            }
        else
            {
            // hh:mm:ss --> 000000
            formatDateString.Append( KSeparator );
            formatDateString.Append( KNullTime );
            }
        }
    delete dateString;
    
    User::LeaveIfError( aTime.Set( formatDateString ) );
    }

