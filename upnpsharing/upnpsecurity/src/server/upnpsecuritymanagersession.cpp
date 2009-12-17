/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Defines the CUpnpSecurityManagerSession class
 *
 */

// INCLUDES
#include "upnpsecuritymanagersession.h"
#include "upnpsecuritymanagerengine.h"
#include "upnpsecuritymanagerserver.h"
#include "upnpsymbianserverbase.h"
#include "upnpsecuritymanagerclientsession.h"

// ============================= MEMBER FUNCTIONS ==============================

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerSession* CUpnpSecurityManagerSession::NewL(
    CUpnpSecurityManagerEngine& aEngine )
    {
    CUpnpSecurityManagerSession* self =
            CUpnpSecurityManagerSession::NewLC( aEngine );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerSession* CUpnpSecurityManagerSession::NewLC(
    CUpnpSecurityManagerEngine& aEngine )
    {
    CUpnpSecurityManagerSession* self =
            new (ELeave) CUpnpSecurityManagerSession( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::CUpnpSecurityManagerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerSession::CUpnpSecurityManagerSession(
    CUpnpSecurityManagerEngine& aEngine ) :
    CSession2(), iEngine( aEngine )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::~CUpnpSecurityManagerSession
// Destructor.
// -----------------------------------------------------------------------------
//
CUpnpSecurityManagerSession::~CUpnpSecurityManagerSession()
    {
    delete iAuthorizationNote;
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::ConstructL()
    {
    // Create object responsible for authorisation dialog 
    iAuthorizationNote = CUpnpAuthorizationNote::NewL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EAddFile:
            {
            EventAddFileL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case ERemoveFile:
            {
            EventRemoveFileL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case EAddAddress:
            {
            EventAddAddressL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case ERemoveAddress:
            {
            EventRemoveAddressL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case EResetFileList:
            {
            EventResetFileListL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case EResetAddressList:
            {
            EventResetAddressListL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case ECheckAuthorization:
            {
            EventCheckAuthorizationL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case EQueryAuthorisation:
            {
            EventQueryAuthorisationL( aMessage );
            //won't complete message as we have to wait for user response
            break;
            }
        default:
            PanicClient( aMessage, CUpnpSymbianServerBase::EBadRequest );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::PanicClient( const RMessage2& aMessage,
    TInt aPanic ) const
    {
    static_cast<const CUpnpSymbianServerBase*> ( Server() )->PanicClient(
        aMessage, aPanic );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventAddFileL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventAddFileL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLength( 0 );
    RBuf fileName;
    fileName.CreateL( length );
    CleanupClosePushL( fileName );
    aMessage.ReadL( 0, fileName );

    iEngine.AddNewFileL( fileName );

    CleanupStack::PopAndDestroy( &fileName );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventRemoveFileL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventRemoveFileL( const RMessage2& aMessage )
    {
    TInt length = aMessage.GetDesLength( 0 );
    RBuf fileName;
    fileName.CreateL( length );
    CleanupClosePushL( fileName );
    aMessage.ReadL( 0, fileName );

    iEngine.RemoveFileL( fileName );

    CleanupStack::PopAndDestroy( &fileName );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventAddAddressL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventAddAddressL( const RMessage2& aMessage )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*> ( &addr ), sizeof(addr),
        sizeof(addr) );
    aMessage.ReadL( 0, ptr );

    iEngine.AddNewAddressL( addr );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventRemoveAddressL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventRemoveAddressL(
    const RMessage2& aMessage )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*> ( &addr ), sizeof(addr),
        sizeof(addr) );
    aMessage.ReadL( 0, ptr );

    iEngine.RemoveAddressL( addr );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventResetFileListL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventResetFileListL( const RMessage2& /*aMessage*/)
    {
    iEngine.ResetFileListL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventResetAddressListL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventResetAddressListL( const RMessage2& /*aMessage*/)
    {
    iEngine.ResetAddressListL();
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventCheckAuthorizationL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventCheckAuthorizationL(
    const RMessage2& aMessage )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*> ( &addr ), sizeof(addr),
        sizeof(addr) );
    aMessage.ReadL( 0, ptr );

    TInt length = aMessage.GetDesLength( 1 );
    RBuf fileName;
    fileName.CreateL( length );
    CleanupClosePushL( fileName );
    aMessage.ReadL( 1, fileName );

    TAccessType at = iEngine.CheckAuthorization( addr, fileName );

    TPckg<TAccessType> atPtr( at );
    aMessage.WriteL( 2, atPtr );

    CleanupStack::PopAndDestroy( &fileName );
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerSession::EventQueryAuthorisationL
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::EventQueryAuthorisationL(
    const RMessage2& aMessage )
    {
    iAuthorizationNote->ShowNoteL( this );
    iQueryAuthMessage = aMessage; //copy message as we can't complete it now
    }

// -----------------------------------------------------------------------------
// CUpnpSecurityManagerEngine::NoteResponseL
// Callback from CUpnpAuthorizationNote.
// -----------------------------------------------------------------------------
//
void CUpnpSecurityManagerSession::NoteResponseL( TBool aAuthorized )
    {
    TInetAddr addr;
    TPtr8 ptr( reinterpret_cast<TUint8*> ( &addr ), sizeof(addr),
        sizeof(addr) );
    iQueryAuthMessage.ReadL( 0, ptr );

    TAccessType noteResult;
    if ( aAuthorized )
        {
        noteResult = EAddressAllowed;
        iEngine.AddNewAddressL( addr );
        }
    else
        {
        noteResult = ENoneAllowed;
        }
    TPckg<TAccessType> resPtr( noteResult );
    iQueryAuthMessage.WriteL( 1, resPtr );
    iQueryAuthMessage.Complete( KErrNone );
    }

// End of File
