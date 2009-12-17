/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      AO for file sharing operations
*
*/






// INCLUDE FILES
// upnp stack api
#include <upnpfilesharing.h>
#include <upnpitem.h>

// upnpframework / avcontroller helper api
#include "upnpconstantdefs.h" // for upnp-specific stuff
#include "upnpitemutility.h" // for GetResElements

// upnpframework / internal api's
#include "upnpsecaccesscontroller.h"

// avcontroller internal
#include "upnpfilesharingactive.h"

_LIT( KComponentLogfile, "upnpavcontrollerserver.txt");
#include "upnplog.h"


// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::NewL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
CUPnPFileSharingActive* CUPnPFileSharingActive::NewL()
    {
    CUPnPFileSharingActive* self = new (ELeave) CUPnPFileSharingActive();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::CUPnPFileSharingActive
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
CUPnPFileSharingActive::CUPnPFileSharingActive():
    CActive( EPriorityStandard )//,
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::~CUPnPFileSharingActive
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
CUPnPFileSharingActive::~CUPnPFileSharingActive()
    {
    Cancel();
    delete iFileSharing;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::ConstructL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::ConstructL()
    {
    __LOG( "CUPnPFileSharingActive::ConstructL" );
    
    iFileSharing = CUpnpFileSharing::NewL();    
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::RunL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::RunL()
    {
    CActiveScheduler::Stop(); 
    }
    
// --------------------------------------------------------------------------
// CUPnPFileSharingActive::DoCancel
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::DoCancel()
    {
    __LOG( "CUPnPFileSharingActive::DoCancel" );   
    }
    
// --------------------------------------------------------------------------
// CUPnPFileSharingActive::RunError
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
TInt CUPnPFileSharingActive::RunError( TInt /*aError*/ )
    {
    
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::ShareItemL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::ShareItemL( CUpnpItem& aItem )
    {
    __LOG( "CUPnPFileSharingActive::ShareItemL" );

    TInt status = KErrNone;

    // Allow access to the files listed in res-elements
    SetAccesstoItemResourcesL( aItem, ETrue );

    // Share the item
    TRAP( status,
          iFileSharing->ShareItemL( KContainerIdRoot, aItem, iStatus ) );
    if( status == KErrNone )
        {
        SetActive();
        CActiveScheduler::Start();
        status = iStatus.Int();
        }

    // If the sharing failed
    if( status != KErrNone )
        {
        // Deny access to the files listed in res-elements
        SetAccesstoItemResourcesL( aItem, EFalse );
        }

    // Leave if the was an error
    if( status != KErrNone )
        {
        User::Leave( status );
        }

    __LOG( "CUPnPFileSharingActive::ShareItemL - end" );
    }
    
// --------------------------------------------------------------------------
// CUPnPFileSharingActive::UnShareItemL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::UnShareItemL( const TDesC8& aId )
    {
    __LOG( "CUPnPFileSharingActive::ConstructL" );

    TLex8 lex( aId );
    TInt id;
    User::LeaveIfError( lex.Val( id ) );
    TInt status = KErrNone;

    // Get the shared items data via Metadata API
    CUpnpItem* item = CUpnpItem::NewL();
    CleanupStack::PushL( item );
    iFileSharing->GetSharedItemL( id, *item, iStatus );
    SetActive();
    CActiveScheduler::Start();
    status = iStatus.Int();

    // If the item's metadata was resolved succesfully
    if( status == KErrNone )
        {
        // Unshare the item
        TRAP( status, iFileSharing->UnshareItemL( id, iStatus ) );
        if( status == KErrNone )
            {
            SetActive();
            CActiveScheduler::Start();
            status = iStatus.Int();
            }
        }

    // If the unsharing succeeded
    if( status == KErrNone )
        {
        // Deny access to the files listed in res-elements
        SetAccesstoItemResourcesL( *item, EFalse );
        }

    // Clean up
    CleanupStack::PopAndDestroy( item );
    item = NULL;

    // Leave if error
    if( status != KErrNone )
        {
        User::LeaveIfError( status );
        }

    __LOG( "CUPnPFileSharingActive::UnShareItemL - end" );
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingActive::SetAccesstoItemResourcesL
// See upnpfilesharingactive.h
// --------------------------------------------------------------------------
void CUPnPFileSharingActive::SetAccesstoItemResourcesL(
                                                CUpnpItem& aItem,
                                                TBool aAccessAllowed )
    {
    __LOG( "CUPnPFileSharingActive::SetAccesstoItemResourcesL" );

    // Create security access controller
    CUpnpSecAccessController* accessController = 
                                    CUpnpSecAccessController::NewL();
    CleanupStack::PushL( accessController );

    // Get all filenames and set the access for those.
    RUPnPElementsArray resElements;
    UPnPItemUtility::GetResElements( aItem, resElements );
    for( TInt i=0; i<resElements.Count(); i++ )
        {
        if( aAccessAllowed )
            {
            accessController->AddAllowedFile( resElements[i]->FilePath() );
            }
        else
            {
            accessController->RemoveAllowedFile(
                                        resElements[i]->FilePath() );
            }
        }

    // Clean up
    resElements.Close();
    CleanupStack::PopAndDestroy( accessController );
    accessController = NULL;
    }

// End of file
