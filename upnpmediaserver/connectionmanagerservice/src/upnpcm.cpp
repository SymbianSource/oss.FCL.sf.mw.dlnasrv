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
* Description:  Declares ConnectionManager class
*
*/



// INCLUDE FILES
#include <upnpdlnaprotocolinfo.h>
#include "upnpprotocolinfolocal.h"

#include <e32std.h>
#include "upnpcm.h"
#include "upnperrors.h"
#include "upnpargument.h"
#include "upnpcommonupnplits.h"
#include "upnpdeviceimplementationbase.h"
#include "upnpsecuritymanager.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCM::CUpnpCM
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpCM::CUpnpCM( CUpnpDevice& aDevice ) : CUpnpServiceImplementation(aDevice)
	{
	}

// -----------------------------------------------------------------------------
// CUpnpCM::~CUpnpCM
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpCM::~CUpnpCM()
	{
	delete iSecurityManager;
	}

// -----------------------------------------------------------------------------
// CUpnpCM::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpCM* CUpnpCM::NewL( CUpnpDevice& aDevice, 
                                 const TDesC8& aMediaTypes,
                                 const TDesC& aDescriptionPath )
	{
	CUpnpCM* self = new (ELeave) CUpnpCM(aDevice);
	CleanupStack::PushL( self );
	self->ConstructL( aMediaTypes, aDescriptionPath );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CUpnpCM::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCM::ConstructL( const TDesC8& aMediaTypes, const TDesC& aDescriptionPath )
	{
	BaseConstructL( aDescriptionPath, KConnectionManagerType() );	
	
	// create ptocolInfos string
	HBufC8* protInfs = CreateProtocolInfosL(aMediaTypes);
	CleanupStack::PushL(protInfs);
	
	// set variable	
	SetStateVariableL( KProtInf, KEmptyString);
	SetStateVariableL( KSourceInf, *protInfs );
	SetStateVariableL( KSinkInf, KEmptyString );
		
	// clean up
	CleanupStack::PopAndDestroy(protInfs);
	
	SetStateVariableL( KCurConId,KZero );
	SetStateVariableL( KRcsId, UpnpCD::KMinusOne );
	SetStateVariableL( KAvtransId, UpnpCD::KMinusOne );
	SetStateVariableL( KConMan, KEmptyString );
    SetStateVariableL( KConId, UpnpCD::KMinusOne );
	SetStateVariableL( KDirection, KOutput );
	SetStateVariableL( KConStat, KOk );
	
	iSecurityManager = CUpnpSecurityManager::NewInstanceFromUpnpSettingsL();
	}
// -----------------------------------------------------------------------------
// CUpnpCM::CreateProtocolInfosL
// Handle action received from ServiceFramework
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpCM::CreateProtocolInfosL(const TDesC8& aMediaTypes )
    {
	// parse comma separated media types
	CPtrC8Array* pairs = new(ELeave) CPtrC8Array(KGranularity);
	CleanupStack::PushL(pairs);
	GetMediaListFromStringL(pairs, aMediaTypes);

	// buffer
	CBufFlat* buf = CBufFlat::NewL(KBufExpandSize);
	CleanupStack::PushL(buf);

	// for each media element
	for (TInt i = 0; i < pairs->Count(); i++ )
	    {
		// create protocolInfo
		TInt pos(KErrNotFound);
		User::LeaveIfError(pos = (*pairs)[i].Find(KColon));
		TPtrC8 a((*pairs)[i].Left(pos));
		CUpnpProtocolInfoLocal* protInfo = 
				CUpnpProtocolInfoLocal::NewL( (*pairs)[i].Left(pos), (*pairs)[i].Mid(pos+1) );
		CleanupStack::PushL(protInfo);
		
		// get protocolInfo string
		TPtrC8 protocolInfo = protInfo->ProtocolInfoL();
		HBufC8* protStr = protocolInfo.Alloc();
		CleanupStack::PushL(protStr);
		
		// add to the buffer
		buf->InsertL(buf->Size(), *protStr);
		
		// add separator
		buf->InsertL(buf->Size(), KComma);
		CleanupStack::PopAndDestroy(protStr);
		CleanupStack::PopAndDestroy(protInfo);
	    }
	// delete last separator
	buf->Delete(buf->Size() - 1,KComma().Length());
	
	// alloc heap descriptor
	HBufC8* ret = buf->Ptr(0).AllocL();	
	
	// clean up
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(pairs);
	
	// return descritpor
	return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpCM::GetMediaListFromStringL
// Handle action received from ServiceFramework
// -----------------------------------------------------------------------------
//
void CUpnpCM::GetMediaListFromStringL(CPtrC8Array* aList, const TDesC8& aMediaTypes)
    {
	TPtrC8 ml(aMediaTypes);
	TInt pos = KErrNotFound;
	while ((pos = ml.Find(KComma)) != KErrNotFound)
	    {
		aList->AppendL(ml.Left(pos));
		ml.Set(ml.Mid(pos+1));
	    }
	aList->AppendL(ml);
    }

// -----------------------------------------------------------------------------
// CUpnpCM::ActionReceivedLD
// Handle action received from ServiceFramework
// -----------------------------------------------------------------------------
//
void CUpnpCM::ActionReceivedLD( CUpnpAction* aAction )
	{
	CleanupStack::PushL( aAction );
        //check security
    if ( iSecurityManager &&
            KErrNone != iSecurityManager->AuthorizeAction( aAction ) )
        {
        //server returns upnp error action failed when not authorized
        User::Leave(EActionFailed);
        }
	TUpnpErrorCode eCode = EHttpOk;

    if ( !aAction )
        {
        User::Leave( KErrNotFound );
        }

	if ( aAction->Name().Compare( KGetProtInfo ) == 0 )
		{
		//Get supported Protocols
		eCode = GetProtocolInfoL( aAction );
		}
	else if ( aAction->Name().Compare( KGetCurConIds ) == 0 ) 
		{
		//Get Current Connection IDs 
		eCode = GetCurrentConnectionIDsL( aAction );
		}
	else if ( aAction->Name().Compare( KGetCurConInfo ) == 0 )
		{
		//Get Current Connection Info
		eCode = GetCurrentConnectionInfoL( aAction );
		}
	else if ( aAction->Name().Compare( KPrepareForConnection ) == 0 ) 
		{
		eCode = PrepareForConnection( aAction );
		}
	else if ( aAction->Name().Compare( KConnectionComplete ) == 0 )
		{
		eCode = ConnectionComplete( aAction );
		}
    else
        {
        // Action not defined
		eCode = EInvalidAction;
        }
	SendL( aAction, eCode );
	CleanupStack::PopAndDestroy( aAction );
	}

// -----------------------------------------------------------------------------
// CUpnpCM::GetProtocolInfoL
// Checks current stateVariables from stateVariableList returns Protocol-related
// info
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpCM::GetProtocolInfoL( CUpnpAction* aAction )
	{			
	if ( aAction->SetArgumentL( KSource, 
				StateVariableValue( KSourceInf )) == KErrNone && 
				
 		aAction->SetArgumentL( KSink, 
				StateVariableValue( KSinkInf )) == KErrNone) 
		{
		return EHttpOk;
		}
	else 	
		{
		return EInvalidArgs;
		}					
	}

// -----------------------------------------------------------------------------
// CUpnpCM::PrepareForConnection
// Gets source and sink protocol info
// source = iSourceProtocolInfo
// sink = iSinkProtocolInfo
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpCM::PrepareForConnection( CUpnpAction* /*aAction*/ )
	{
    // Not implemented in Media Server side.
	return ENotImplemented;
	}

// -----------------------------------------------------------------------------
// CUpnpCM::ConnectionComplete
// Fetch the AVTransport ID by giving ConnectionID
// iAVTransport->GetAVTransportID(ConnectionID)
// remove this ID from ConnectionIDs
// release network resources
// Cleanup
// Update source & sink devices information in stateVariableList
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpCM::ConnectionComplete( CUpnpAction* /*aAction*/ )
	{
    // Not implemented in Media Server side.
	return ENotImplemented;
	}

// -----------------------------------------------------------------------------
// CUpnpCM::GetCurrentConnectionIDsL
// Check the current connections and insert a CSV list of them into the aAction
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpCM::GetCurrentConnectionIDsL( CUpnpAction* aAction )
	{
	if ( aAction->SetArgumentL( KConnectionIDs, 
				StateVariableValue( KCurConId )) == KErrNone) 
		{
		return EHttpOk;
		}
	else 	
		{
		return EInvalidArgs;
		}	
	}

// -----------------------------------------------------------------------------
// CUpnpCM::GetCurrentConnectionInfoL
// Checks the ConnectionInfo of the connection specified in the aAction and sets
// the data to aAction
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpCM::GetCurrentConnectionInfoL( CUpnpAction* aAction )
	{
	/**
	*Check the given connection is set up by PrepareForConnection()
	*if(connectionID == 0){
	*iRcsID=-1;
	*iAVTransportID = -1;
	*iProtocolInfo = NULL;
	*iPeerConnectionManager = NULL;
	*iPeerConnectionID = -1;
	*iDirection = input (or output);
	*iStatus = OK (or Unknown);
	* Gets the connection information by given connection ID.. 
	*
	*Acceptable Error Codes
	*	EInvalidArgs	=	402
	*	EParameterMismatch = 706
	**/	

	const TDesC8& connectionID = aAction->ArgumentValue( KArgument() ); 
	TLex8 string( connectionID );
	TInt testInt;
	TInt error = string.Val( testInt );
	if ( error != KErrNone )
		{
		return EInvalidArgs;
		}

	if ( connectionID.CompareC( KZero() ) == 0 )
		{
		if ( aAction->SetArgumentL( KTypeRcsID, 
				StateVariableValue( KRcsId )) == KErrNone && 
				
 			aAction->SetArgumentL( KTypeAVTransportID, 
				StateVariableValue( KAvtransId )) == KErrNone && 
				
 			aAction->SetArgumentL( KTypeProtocolInfo, 
				StateVariableValue( KArgTypeProtocolInfo )) == KErrNone && 
				
 			aAction->SetArgumentL( KTypePeerConnectionManager, 
				StateVariableValue( KConMan )) == KErrNone && 
				
 			aAction->SetArgumentL( KTypePeerConnectionID, 
				StateVariableValue( KConId )) == KErrNone && 
				
 			aAction->SetArgumentL( KTypeDirection, 
				StateVariableValue( KDirection )) == KErrNone && 	
				
 			aAction->SetArgumentL( KTypeStatus, 
				StateVariableValue( KConStat )) == KErrNone ) 
			{
			return EHttpOk;
			}
		else 	
			{
			return EInvalidArgs;
			}
		} 
	
	return EParameterMismatch;
	}

//  End of File
