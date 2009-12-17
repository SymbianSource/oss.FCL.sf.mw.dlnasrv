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
* Description:  Content Directory factory class
 *
*/


// INCLUDE FILES
#include "upnpcontentdirectoryservice.h"
#include "upnpcontentdirectory.h"   
#include "upnpcddbfactory.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnplocalstorage.h"
#include "upnpmetadatastorage.h"
#include "upnpitem.h"
#include "upnpelement.h"
#include "upnpattribute.h"
#include "upnpcontentdirectoryeventobserver.h"
#include "upnpsender.h"
#include "upnpfiletransfer.h"
#include "upnptransferhandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContentDirectoryService::CContentDirectoryService
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryService::CUpnpContentDirectoryService()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryService::ConstructL( TUpnpCdSettings aSettings )
    {
    // create offline part
    ConstructL( );

    if ( !aSettings.iTransfer ) //create default transfer handler
        {
        iTransferHandler = CUpnpTransferHandler::NewL( );

        // create content directory 
        iCd = CUpnpContentDirectory::NewL( aSettings, iMetadataStorage,
            iTransferHandler );
        iTransferHandler->SetObserver( iCd );
        }
    else //use external transfer handler  
        {
        iCd = CUpnpContentDirectory::NewL( aSettings, iMetadataStorage,
            aSettings.iTransfer );
        }

    iLocalStorage->SetStateHandler( iCd->StateHandler( ) );
    // sender    
    iSender = CUpnpCdsSender::NewL( iCd );
    iCd->SetSender( iSender );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryService::ConstructL()
    {
    CUpnpCdDbFactory* cdf = CUpnpCdDbFactory::NewLC( );

    // database file name
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect( ) );

    TFileName path;
    User::LeaveIfError( fs.PrivatePath( path ) );
    TParse fp;
    fp.Set( KDatabaseFileName( ), &path, 0 );
    path = fp.FullName( );

    // create metastorage
    iMetadataStorage = cdf->CreateMetadataStorageL( path );

    // create localstorage
    iLocalStorage = CUpnpLocalStorage::NewL( iMetadataStorage );

    CleanupStack::PopAndDestroy( &fs );
    CleanupStack::PopAndDestroy( cdf );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentDirectoryService*
CUpnpContentDirectoryService::NewL( TUpnpCdSettings aSettings )
    {
    CUpnpContentDirectoryService* self =
    CUpnpContentDirectoryService::NewLC( aSettings );

    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentDirectoryService*
CUpnpContentDirectoryService::NewLC( TUpnpCdSettings aSettings )
    {
    CUpnpContentDirectoryService* self = new( ELeave ) CUpnpContentDirectoryService();

    CleanupStack::PushL( self );
    self->ConstructL(aSettings);

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentDirectoryService* CUpnpContentDirectoryService::NewL()
    {
    CUpnpContentDirectoryService* self = CUpnpContentDirectoryService::NewLC();
    CleanupStack::Pop(self);
    return self;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentDirectoryService* CUpnpContentDirectoryService::NewLC()
    {
    CUpnpContentDirectoryService* self =
    new( ELeave ) CUpnpContentDirectoryService();
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::~CUpnpContentDirectoryService
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryService::~CUpnpContentDirectoryService()
    {
    delete iTransferHandler;
    delete iLocalStorage;
    delete iCd;
    delete iMetadataStorage;
    delete iSender;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetDownloadDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::SetDownloadDirectoryL(
    const TDesC& aDownloadDir )
    {
    if ( IsOnlineMode() )
        {
        iCd->SetDownloadDirectoryL( aDownloadDir );
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::SetThumbnailDirectoryL(
    const TDesC& aDownloadDir )
    {
    if ( IsOnlineMode() )
        {
        iCd->SetThumbnailDirectoryL( aDownloadDir );
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::ShareObjectL( CUpnpObject* aItem )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->ShareObjectL( aItem ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->ShareObjectL( aItem ) );
        RecreateDbL( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::ShareReferenceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::ShareReferenceL( CUpnpItem* aItem )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->ShareReferenceL( aItem ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->ShareReferenceL( aItem ) );
        RecreateDbL( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::ShareReferenceListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::ShareReferenceListL(
    CUpnpItemList* aItemList, TInt* aExecutionStatus )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->ShareReferenceListL( aItemList,
        aExecutionStatus ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->ShareReferenceListL( aItemList,
            aExecutionStatus ) );
        RecreateDbL( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::UnshareItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::UnshareItemL( TInt aItemId )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->UnshareItemL( aItemId ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->UnshareItemL( aItemId ) );
        RecreateDbL( error );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::UnshareContainerL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::UnshareContainerL(
    TInt aContainerId )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->UnshareContainerL( aContainerId ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->UnshareContainerL( aContainerId ) );
        RecreateDbL( error );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpItem* CUpnpContentDirectoryService::GetSharedItemL( TInt aItemId )
    {
    CheckDbL();
    CUpnpItem* item = NULL;
    TRAPD( error, item = iLocalStorage->GetSharedItemL( aItemId ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, item = iLocalStorage->GetSharedItemL( aItemId ) );
        RecreateDbL( error );
        }
    return item;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::ShareItemListL(
    CUpnpItemList* aItemList, TInt* aExecutionStatus )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->ShareItemListL( aItemList, aExecutionStatus ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->ShareItemListL( aItemList,
            aExecutionStatus ) );
        RecreateDbL( error );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::UnshareItemListL(
    RArray<TInt>& aItemList, TInt* aExecutionStatus )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->UnshareItemListL( aItemList,
        aExecutionStatus ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->UnshareItemListL( aItemList,
            aExecutionStatus ) );
        RecreateDbL( error );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::GetItemListL( TInt aContainerId,
    CUpnpBrowseCriteria* aBrowseCriteria, TInt* aTotalMatches,
    CUpnpItemList* aItemList )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->GetItemListL( aContainerId, aBrowseCriteria,
        aTotalMatches, aItemList ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->GetItemListL( aContainerId,
            aBrowseCriteria, aTotalMatches, aItemList ) );
        RecreateDbL( error );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::GetContainerListL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::GetContainerListL(
    TInt aContainerId, CUpnpBrowseCriteria* aBrowseCriteria,
    TInt* aTotalMatches, CUpnpContainerList* aContainerList )
    {
    CheckDbL( );
    TRAPD( error, iLocalStorage->GetContainerListL( aContainerId,
        aBrowseCriteria, aTotalMatches, aContainerList ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, iLocalStorage->GetContainerListL( aContainerId,
            aBrowseCriteria, aTotalMatches, aContainerList ) );
        RecreateDbL( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContainer* CUpnpContentDirectoryService::GetSingleContainerL(
        TInt aContainerId)
    {
    CheckDbL();
    CUpnpContainer* container = NULL;
    TRAPD( error, container = iLocalStorage->GetSingleContainerL( aContainerId ) );
    HandleDbErrorL( error );
    if ( error )
        {
        TRAP( error, container = iLocalStorage->GetSingleContainerL( aContainerId ) );
        RecreateDbL( error );
        }
    return container;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::GetAddress
// -----------------------------------------------------------------------------
//
EXPORT_C TInetAddr CUpnpContentDirectoryService::GetAddress()
    {
    if ( IsOnlineMode() )
        {
        return iCd->GetAddress();
        }
    else
        {
        // no address available in offline mode
        return TInetAddr( INET_ADDR(0,0,0,0) );
        }
    }    
    
// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::GetContentDirectory
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpContentDirectory* CUpnpContentDirectoryService::GetContentDirectory()
    {
    return iCd;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::HandleDbErrorL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryService::HandleDbErrorL( TInt aError )
    {
    TInt error = iMetadataStorage->HandleDbError( aError );
    if ( error != KErrNone )
        {
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::RecreateDbL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectoryService::RecreateDbL( TInt aError )
    {
    if ( aError == KErrCorrupt )
        {
        iMetadataStorage->RecreateDatabaseFileL( );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::CheckDbL
// -----------------------------------------------------------------------------
//            
void CUpnpContentDirectoryService::CheckDbL()
    {
    if ( !iMetadataStorage->IsDbCreated( ) )
        {
        HandleDbErrorL( KErrCorrupt );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::IsOnlineMode
// -----------------------------------------------------------------------------
//            
TBool CUpnpContentDirectoryService::IsOnlineMode()
    {
    return reinterpret_cast<TBool>( iCd );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectoryService::AddressChangeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectoryService::AddressChangeL()
    {
    if ( IsOnlineMode() )
        {
        iCd->AddressChangeL();
        }
    }
//  End of File
