/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for meta data gathering for audio files
*
*/


// INCLUDE FILES
// System
#include <e32std.h>
#include <e32base.h>

// upnp stack api
#include <upnpitem.h> // CUpnpItem
#include <upnpstring.h>
#include <upnpelement.h>
#include <upnpdlnaprotocolinfo.h>
#include <upnpprotocolinfo.h>

// upnpframework / avcontroller helper api
#include <upnpconstantdefs.h> // for upnp class defs

// upnpframework internal api's
#include "upnpresresolver.h" // CUpnpResResolver
#include "upnpresparameters.h"

// utilities internal
#include "upnpcommonutils.h"
#include "upnpcdsreselementutility.h"

_LIT( KHttp, "http://" );
const TInt KColon = 58;
const TInt KIpAddLen = 25;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpCdsResElementUtility::AddResElementL
// 
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void UpnpCdsResElementUtility::AddResElementL( 
                                                CUpnpItem& aItem, 
                                                const TDesC& aFilename )
    {

    // Create CUpnpElement for res
    CUpnpElement* elRes = CUpnpElement::NewL( KElementRes() );
    CleanupStack::PushL( elRes );
    elRes->SetFilePathL( aFilename );
 
    // Create res resolver component and put it to CleanupStack
    CUpnpResResolver* resolver = CUpnpResResolver::NewLC();
    
    // Obtain res element parameters using res resolver component.
    CUpnpResParameters* resParameters = NULL;
	TRAPD(err, resParameters = resolver->GetResParametersL( aFilename ) );

   if (err == KErrGeneral)	
    {
	// File not recognized, handling it manually by adding
	// following res-item:  'http-get:*:MIMETYPE:*'

   		_LIT8( KProtInfo1, "http-get:");
    	_LIT8( KStar8, "*");
    	_LIT8( KColon8, ":");
    	HBufC8* mimetype = UPnPCommonUtils::ResolveMimeTypeL(aFilename);
    	mimetype->Des().Trim();		// needs to be trimmed

        TInt resultLen = KProtInfo1().Length() + KStar8().Length() + 
    	KColon8().Length() + mimetype->Des().Length() + KColon8().Length() +
        KStar8().Length();
    	HBufC8* resValue = HBufC8::NewLC( resultLen );

    // construct final value
	    resValue->Des().Append( KProtInfo1 );
	    resValue->Des().Append( KStar8 );
	    resValue->Des().Append( KColon8 );
	    resValue->Des().Append( *mimetype );
	    resValue->Des().Append( KColon8 );
	    resValue->Des().Append( KStar8 );

		CUpnpProtocolInfo* pInfo = CUpnpProtocolInfo::NewL(*resValue);
		CUpnpAttribute* attribute = CUpnpAttribute::NewLC( KAttributeProtocolInfo() );
		attribute->SetValueL( pInfo->ProtocolInfoL() );
		elRes->AddAttributeL( attribute );    
    	aItem.AddElementL(elRes);

		CleanupStack::Pop( attribute );
		CleanupStack::PopAndDestroy( resValue );
		CleanupStack::PopAndDestroy( resolver );
    	CleanupStack::Pop( elRes );
  		resValue = NULL;
    	mimetype = NULL;
    	
    	return;
    }
    // Fixes ESLX-7B7HBL
    else if( err )
        {
        User::Leave( err );
        }

    CleanupStack::PushL( resParameters );

    // Add protocolInfo parameter to res element.
    if ( resParameters->ProtocolInfo().Length() > 0 ) 
        {
        CUpnpAttribute* prInfo = CUpnpAttribute::NewLC( KAttributeProtocolInfo() );
        prInfo->SetValueL( resParameters->ProtocolInfo() );   
        elRes->AddAttributeL( prInfo ); //transfer ownership
        CleanupStack::Pop( prInfo );       
        }

    // Add size parameter to res element.

    HBufC8* sizeString = resParameters->FileSizeL();
    if ( sizeString ) 
        {
        CleanupStack::PushL( sizeString );
        CUpnpAttribute* size =
            CUpnpAttribute::NewLC( KAttributeSize() );
        size->SetValueL( *sizeString );

        elRes->AddAttributeL( size ); //transfer ownership
        CleanupStack::Pop( size );        
        CleanupStack::PopAndDestroy( sizeString );
        sizeString = NULL;
        }

    if ( resParameters->IsImageFile() ) 
        {
        // In image case, add resolution parameter to res element.
        HBufC8* resolutionString = resParameters->ResolutionL();
        if ( resolutionString ) 
            {
            CleanupStack::PushL( resolutionString );
            CUpnpAttribute* resolution = 
                CUpnpAttribute::NewLC( KAttributeResolution() );
            resolution->SetValueL( *resolutionString );
            elRes->AddAttributeL( resolution ); //transfer ownership
            CleanupStack::Pop( resolution );            
            CleanupStack::PopAndDestroy( resolutionString );
            resolutionString = NULL;
            }
        }
    else if ( resParameters->IsAudioOrVideoFile() )
        {
        // In music or video case, add duration parameter to res element.
        HBufC8* durationString = resParameters->DurationL();
        if ( durationString ) 
            {
            CleanupStack::PushL( durationString );
            CUpnpAttribute* duration =
                CUpnpAttribute::NewLC( KAttributeDuration() );
            duration->SetValueL( *durationString );
            elRes->AddAttributeL( duration ); //transfer ownership
            CleanupStack::Pop( duration );
            CleanupStack::PopAndDestroy( durationString );
            durationString = NULL;
            }
        }

    CleanupStack::PopAndDestroy( resParameters );
    CleanupStack::PopAndDestroy( resolver );

    // Attach res element to aItem.
    aItem.AddElementL( elRes ); //transfer ownership
    CleanupStack::Pop( elRes );
    }

// --------------------------------------------------------------------------
// CUpnpCdsResElementUtility::IsUriAbsolute
// 
// (other items were commented in a header).
// --------------------------------------------------------------------------
EXPORT_C TBool UpnpCdsResElementUtility::IsUriAbsolute( const TDesC8& aUri )
    {
    TInetAddr addr;
    TBool retVal = EFalse;
    TBuf<KIpAddLen> ipAddrBuf;
    // if aUri.Length is less than KHttp().Length(),aUri is invalid
    if ( aUri.Length() >= KHttp().Length() )
		{
		TPtrC8 right = aUri.Right( aUri.Length() - KHttp().Length() );

		TInt posit = right.Locate( TChar( KColon ) );
		if( posit != KErrNotFound )
			{
			TPtrC8 ipAddr = right.Left( posit );
			if( ipAddr.Length() <= KIpAddLen )
				{
				ipAddrBuf.Copy( ipAddr );
				}
			}

		if( addr.Input( ipAddrBuf ) == KErrNone )
			{
			retVal = ETrue;
			}
		}

	return retVal;    
    }

//  End of File  
