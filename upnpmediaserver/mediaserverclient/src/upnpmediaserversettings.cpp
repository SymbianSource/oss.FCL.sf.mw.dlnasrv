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
* Description:  Media Server Settings
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <centralrepository.h>
#include "upnpstring.h"
#include <platform_paths.hrh>
#include <bldvariant.hrh>

#include "upnpmediaserversettings.h"

#ifdef _DEBUG
#define KLogFile _L("UPnPMediaServer.txt")
#endif
#include "upnpcustomlog.h"

const TInt KBufferGranularity = 256;

// =========================== MEMBER FUNCTIONS ===============================
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CUpnpMediaServerSettings::ConstructL(TUid aRepositoryUid)
    {
    iRepository = CRepository::NewL(aRepositoryUid);
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerSettings::CUpnpMediaServerSettings
// ---------------------------------------------------------------------------
//
CUpnpMediaServerSettings::CUpnpMediaServerSettings() :CActive(EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }
	
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CUpnpMediaServerSettings* CUpnpMediaServerSettings::NewL()
    {
    CUpnpMediaServerSettings* self = new ( ELeave ) CUpnpMediaServerSettings();
    CleanupStack::PushL( self );
    self->ConstructL( KCRUidMediaServer );
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUpnpMediaServerSettings::~CUpnpMediaServerSettings()
    {
    Cancel();
    delete iRepository;
    }
    
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Set
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::SetL(TUint aId, const TDesC8& aValue)
    {
    HBufC* data = UpnpString::ToUnicodeL(aValue);
    TPtr dataPtr(data->Des());
    TInt err = iRepository->Set(aId, dataPtr);
    delete data;
    return err;
    }    

// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Set
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::Set(TUint aId, const TDesC& aValue)
    {
    return iRepository->Set(aId, aValue);
    }

// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Set
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::Set(TUint aId, const TInt& aValue)
    {
    return iRepository->Set(aId, aValue);
    }
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Get
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::Get(TUint aId, TInt& aValue)
    {
    return iRepository->Get(aId, aValue);
    }    
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Get
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::Get(TUint aId, TDes& aValue)
    {
    return iRepository->Get(aId, aValue);
    }    
// ----------------------------------------------------------------------------
// CUpnpMediaServerSettings::Get
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CUpnpMediaServerSettings::GetL(TUint aId)
    {
    TInt len(0);
    HBufC* buf(0);
    TInt err(KErrNotFound);

    do //until the buffer is large enough
        {
        len += KBufferGranularity;
        delete buf; // first time it is Null
        buf = HBufC::NewL(len);
        TPtr bufPtr(buf->Des());
        err = iRepository->Get(aId, bufPtr);
        }
    while(err == KErrOverflow );
	
    // handle error
    if(err)
        {
        delete buf;
        User::Leave(err);
        }

    // convert to UTF-8
    CleanupStack::PushL(buf);	
    HBufC8* buf8 = UpnpString::FromUnicodeL(*buf);
    CleanupStack::PopAndDestroy(buf);
    return buf8;
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerSettings::DoCancel
// ---------------------------------------------------------------------------
//	
void CUpnpMediaServerSettings::DoCancel()
    {
    iRepository->NotifyCancelAll();
    }
	
// ---------------------------------------------------------------------------
// CUpnpMediaServerSettings::Subscribe
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpMediaServerSettings::Subscribe(TUint aId, MUpnpSettingObserver* aObserver)
    {
    iObserver = aObserver;

    Cancel();

    TInt err = iRepository->NotifyRequest(aId, iStatus);
    if( !err )
        {
        SetActive();    
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CUpnpMediaServerSettings::RunL
// ---------------------------------------------------------------------------
//	
void CUpnpMediaServerSettings::RunL()
    {
    iObserver->SettingChangedL();
    }	

// ---------------------------------------------------------------------------
// CUpnpMediaServerSettings::RunError
// ---------------------------------------------------------------------------
//  
TInt CUpnpMediaServerSettings::RunError( TInt aErr )
    {
    LOGS1("CUpnpMediaServerSettings::RunError(%d)", aErr);
    return KErrNone;
    }

// End of File
