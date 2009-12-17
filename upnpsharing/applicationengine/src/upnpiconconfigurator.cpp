/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Configurator class for mediaserver icons
 *
*/






// INCLUDE FILES
// System
#include <centralrepository.h>

// upnp stack
#include <upnpmediaserverclient.h>

// upnp framework internal API's
#include "upnpfileutilitytypes.h" // utilities / mime type definitions

// app.engine internal
#include "upnpapplicationcrkeys.h" //KUPnPAppMediaServerIcon
#include "upnpfilesharingengineao.h"
#include "upnpiconconfigurator.h"


// constants
_LIT( KIcon1Path, "\\system\\apps\\upnp\\upnp_ms_jpeg_sm.jpg");
const TInt KIcon1Width( 48 );
const TInt KIcon1Height( 48 );
const TInt KIcon1Depth( 24 );

_LIT( KIcon2Path, "\\system\\apps\\upnp\\upnp_ms_jpeg_lrg.jpg");
const TInt KIcon2Width( 120 );
const TInt KIcon2Height( 120 );
const TInt KIcon2Depth( 24 );

_LIT( KIcon3Path, "\\system\\apps\\upnp\\upnp_ms_png_sm.png");
const TInt KIcon3Width( 48 );
const TInt KIcon3Height( 48 );
const TInt KIcon3Depth( 24 );

_LIT( KIcon4Path, "\\system\\apps\\upnp\\upnp_ms_png_lrg.png");
const TInt KIcon4Width( 120 );
const TInt KIcon4Height( 120 );
const TInt KIcon4Depth( 24 );

_LIT(KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::CUPnPIconConfigurator
// --------------------------------------------------------------------------
//
CUPnPIconConfigurator::CUPnPIconConfigurator(
    RUpnpMediaServerClient& aMediaServer, 
    CUPnPFileSharingEngineAO& aFileShareingAO )
    : CActive(CActive::EPriorityStandard),
      iMediaServer( aMediaServer ),
      iFileShareEngineAO( aFileShareingAO )
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::CUPnPIconConfigurator" );
    CActiveScheduler::Add( this );
    }


// --------------------------------------------------------------------------
// CUPnPIconConfigurator::~CUPnPIconConfigurator()
// --------------------------------------------------------------------------
//
CUPnPIconConfigurator::~CUPnPIconConfigurator()
    {

    if ( IsActive() )
        {
        iState = EStateIdle;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }
    Cancel();
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::NeedToConfigureIcons()
// --------------------------------------------------------------------------
//
TBool CUPnPIconConfigurator::NeedToConfigureIcons()
    {
#ifdef _DEBUG
    // in debug mode ALWAYS add new icons
    return ETrue;
#else
    // in normal mode only add icons if they DO NOT exist
    TBool iconsExist = EFalse;
    TRAP_IGNORE( iconsExist = CheckIconsAddedL() );
    __LOG1( "[UPNP_ENGINE] CUPnPIconConfigurator: iconsExist %d",
        iconsExist );
    return !iconsExist;
#endif
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::ConfigureIconsL()
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::ConfigureIconsL()
    {
    RemoveIcons();
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::RunL()
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::RunL" );

    TInt err( iStatus.Int() );

    if ( ( err != KErrNone ) && ( err != KErrNotFound ) )
        {       
        iFileShareEngineAO.SetIconComplete( err );
        }
    else
        {
        switch ( iState )
            {
            case EStateRemoving:
                {
                AddIcon1L();
                iState = EStateAdding1;
                }
                break;
            case EStateAdding1:
                {
                delete iIcon1;
                iIcon1 = NULL;
                AddIcon2L();
                iState = EStateAdding2;
                }
                break;
            case EStateAdding2:
                {
                delete iIcon2;
                iIcon2 = NULL;
                AddIcon3L();
                iState = EStateAdding3;
                }
                break;
            case EStateAdding3:
                {
                delete iIcon3;
                iIcon3 = NULL;
                AddIcon4L();
                iState = EStateAdding4;
                }
                break;
            case EStateAdding4:
                {
                delete iIcon4;
                iIcon4 = NULL;
                // operation complete.
                MarkIconsAddedL( ETrue );
                Cancel();
                iFileShareEngineAO.SetIconComplete( KErrNone );
                }
                break;
            default:
                {
                __PANICD( __FILE__, __LINE__ );
                }
                break;
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::RunError
// RunL error handler
// --------------------------------------------------------------------------
//
TInt CUPnPIconConfigurator::RunError( TInt aError )
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::RunError begin" );

    iFileShareEngineAO.SetIconComplete( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::DoCancel()
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::DoCancel" );
    }
    
// --------------------------------------------------------------------------
// CUPnPIconConfigurator::RemoveIconsL
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::RemoveIcons()
    {
    __LOG( "[UPNP_ENGINE]\t CUPnPIconConfigurator::RemoveIconsL" );
    iState = EStateRemoving;
    iMediaServer.RemoveIcons( iStatus );
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        __LOG("[UPNP_ENGINE]\t CUPnPIconConfigurator::RemoveIconsL Error :\
                Already active");
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::AddIcon1
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::AddIcon1L()
    {
    __LOG( "[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon1" );
    TFileName iconPath;
    ResolveIconPath( KIcon1Path, iconPath );
    iIcon1 = CUpnpIcon::NewL( iconPath,
            KIcon1Width, KIcon1Height, KIcon1Depth, KJpegMime );
    iMediaServer.AddIcon( iIcon1, iStatus );
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        __LOG("[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon1L Error :\
                Already active");
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::AddIcon2
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::AddIcon2L()
    {
    __LOG( "[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon2" );
    TFileName iconPath;
    ResolveIconPath( KIcon2Path, iconPath );
    iIcon2 = CUpnpIcon::NewL( iconPath,
        KIcon2Width, KIcon2Height, KIcon2Depth, KJpegMime );
    iMediaServer.AddIcon( iIcon2, iStatus );
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        __LOG("[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon2L Error :\
                Already active");
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::AddIcon3
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::AddIcon3L()
    {
    __LOG( "[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon3" );
    TFileName iconPath;
    ResolveIconPath( KIcon3Path, iconPath );
    iIcon3 = CUpnpIcon::NewL( iconPath,
        KIcon3Width, KIcon3Height, KIcon3Depth, KPngMime );
    iMediaServer.AddIcon( iIcon3, iStatus );
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        __LOG("[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon3L Error :\
                Already active");
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::AddIcon4
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::AddIcon4L()
    {
    __LOG( "[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon4" );
    TFileName iconPath;
    ResolveIconPath( KIcon4Path, iconPath );
    iIcon4 = CUpnpIcon::NewL( iconPath,
        KIcon4Width, KIcon4Height, KIcon4Depth, KPngMime );
    iMediaServer.AddIcon( iIcon4, iStatus );
    if ( !IsActive() )
        {
        SetActive();
        }
    else
        {
        __LOG("[UPNP_ENGINE]\t CUPnPIconConfigurator::AddIcon4L Error :\
                Already active");
        }
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::ResolveIconPath
// Resolves absolute icon path
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::ResolveIconPath(
    const TDesC& relativePath, TFileName& iconPath )
    {
    iconPath = relativePath;
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left(2); // Drive letter followed by ':' 
    iconPath.Insert( 0, drive );
    }
        
// --------------------------------------------------------------------------
// CUPnPIconConfigurator::CheckIconsAddedL
// Set the mediaserver icon if it is needed
// --------------------------------------------------------------------------
//
TBool CUPnPIconConfigurator::CheckIconsAddedL()
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::CheckIconsAddedL" );

    TInt iconSet ( EFalse );
    CRepository *repository = CRepository::NewL( KCRUidUPnPApplication );
    repository->Get( KUPnPAppMediaServerIcon, iconSet );
    delete repository;
    repository = NULL;
    return iconSet;
    }

// --------------------------------------------------------------------------
// CUPnPIconConfigurator::MarkIconsAddedL
// store the cenrep value
// --------------------------------------------------------------------------
//
void CUPnPIconConfigurator::MarkIconsAddedL( TBool aMark )
    {
    __LOG( "[UPNP_ENGINE] CUPnPIconConfigurator::MarkIconsAddedL" );
    
    CRepository *repository = CRepository::NewL( KCRUidUPnPApplication );
    repository->Set( KUPnPAppMediaServerIcon, aMark );
    delete repository;
    repository = NULL;
    }


// End of file
