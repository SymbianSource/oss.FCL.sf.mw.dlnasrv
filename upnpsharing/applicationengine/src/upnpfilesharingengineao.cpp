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
* Description:      CUPnPFileSharingEngineAO class implementation
 *
*/






// INCLUDE FILES
// System
#include <s32file.h>
#include <f32file.h>
#include <pathinfo.h>
#include <e32property.h>
#include <centralrepository.h>

// upnp stack api
#include <upnpstring.h>
#include <upnpmediaserverclient.h>

// upnpframework / avcontroller api
#include "upnpavcontrollerfactory.h"

// upnpframework / internal api's
#include "upnpsecaccesscontroller.h"

// homemedia internal
#include "upnpfilesharingengineao.h"
#include "upnpfilesharingengine.h"
#include "upnpapplicationcrkeys.h" //KUPnPAppMediaServerIcon
#include "upnpfileutilitytypes.h" //KJpegMime2
#include "upnpiconconfigurator.h"

_LIT( KComponentLogfile, "applicationengine.txt");
#include "upnplog.h"


// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::CUPnPFileSharingEngineAO
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUPnPFileSharingEngineAO::CUPnPFileSharingEngineAO(
    CUPnPFileSharingEngine& aEngine,
    RUpnpContentServerClient& aContentServer )
    : CActive(CActive::EPriorityStandard),
      iEngine( aEngine ),
      iContentServer( aContentServer )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO:: \
            CUPnPFileSharingEngineAO");
    CActiveScheduler::Add( this );
    }


// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::~CUPnPFileSharingEngineAO()
// Destructor
// --------------------------------------------------------------------------
//
CUPnPFileSharingEngineAO::~CUPnPFileSharingEngineAO()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO:: \
            ~CUPnPFileSharingEngineAO");

    if ( IsActive() )
        {
        __LOG( "CUPnPFileSharingEngineAO destructor: IsActive==TRUE!" );
        iState = ENotActive;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }

    Cancel();
    iMediaServer.Close();

    delete iIconConfigurator;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::RunL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::RunL begin");

    TInt err( iStatus.Int() );
    if ( err )
        {
        __LOG1( "iStatus err =%d",err );
        }
    switch ( iState )
        {
        case ENotActive :
            {
            if ( err )
                {
                // Inform Observer to handle the error.
                if ( iEngine.Observer() )
                    {
                    iEngine.Observer()->
                        HandleSharingStatus( iEngine, err, EFalse );
                    }
                }
            break;
            }
        // Show files outside
    case ECheckIcon :
        {
        if( !err )
            {
            __LOG1( "[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::ECheckIcon\
err=%d", err );
            iState = EStartSharing;
            // callback SetIconComplete
            iIconConfigurator->ConfigureIconsL();
            }
        else
            {
            iState = ENotActive;
            if ( iEngine.Observer() )
                {
                iEngine.Observer()->
                    HandleSharingStatus( iEngine, err, ETrue );
                }
            }
        }
        break;
    case EStartSharing :
        {
        iMediaServer.Close();

        if ( !err )
            {
            iVisibility = EOnline;
            iEngine.SetConMonStateL( ETrue, iStatus );
            SetActive();
            iState = EWaitSetConMonStateTrue;
            }
        else
            {
            iState = ENotActive;
            if ( iEngine.Observer() )
                {
                iEngine.Observer()->
                    HandleSharingStatus( iEngine, err, ETrue );
                }
            }
        break;
        }
    case EWaitSetConMonStateTrue:
        {
        iState = ENotActive;
        if ( iEngine.Observer() )
            {
            iEngine.Observer()->
                HandleSharingStatus( iEngine, err, ETrue );
            }
        break;
        }
    case EStopSharing :
        {
        if ( !err )
            {
            iEngine.SetConMonStateL( EFalse, iStatus );
            SetActive();
            iState = EWaitSetConMonStateFalse;
            }
        else
            {
            iState = ENotActive;
            if ( iEngine.Observer() )
                {
                iEngine.Observer()->
                    HandleSharingStatus( iEngine, err, EFalse );
                }
            }
        }
        break;
    case EWaitSetConMonStateFalse:
        {
        iState = ENotActive;
        if ( iEngine.Observer() )
            {
            iEngine.Observer()->
                HandleSharingStatus( iEngine, err, EFalse );
            }
        break;
        }
    default :
        {
        __PANICD( __FILE__, __LINE__ );
        break;
        }
        }
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::RunL end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::RunError
// Handles a leave occurring in the request completion event handler
// --------------------------------------------------------------------------
//
TInt CUPnPFileSharingEngineAO::RunError( TInt aError )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::RunL begin");
    TState state ( iState );
    iState = ENotActive;
    if ( iEngine.Observer() )
        {
        TBool phase( EFalse );
        if( state == EStartSharing ||
            state == ECheckIcon ||
            state == EWaitSetConMonStateTrue )
            {
            phase = ETrue;
            }

        iEngine.Observer()->
            HandleSharingStatus( iEngine, aError, phase );
        }
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::RunL end");
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::DoCancel()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::DoCancel begin");
    if ( iIconConfigurator )
        {
        iIconConfigurator->Cancel();
        }

    if ( iEngine.Observer() &&
         iState != ENotActive )
        {
        TBool phase( EFalse );
        if( iState == EStartSharing ||
            iState == ECheckIcon ||
            iState == EWaitSetConMonStateTrue )
            {
            phase = ETrue;
            }

        iEngine.Observer()->
            HandleSharingStatus( iEngine, KErrCancel, phase );
        }
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::DoCancel end");
    }

// --------------------------------------------------------------------------
// CUPnPSettingsEngineAO::SetSharingState
// Starts or stops file sharing
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::SetSharingStateL( TBool aState )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::SetSharingState \
 begin");

    if ( IsActive() )
        {
        User::Leave ( KErrInUse );
        }

    // Open file sharing session from mediaserver
    TBool started = SharingStateL();
    TInt err = OpenMediaServer();

    if ( aState && !started )
        {
        __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
SetSharingState: StartSharing");

        // check media server availibility
        if ( !err )
            {
            EnableUpnpSecurityL();

            CheckMediaserverIconsL();

            if ( !UPnPAVControllerFactory::MSServicesInUse() )
                {
                if ( !HasDiskSpace() )
                    {
                    err = KErrDiskFull;
                    }
                }
            else
                {
                err = KErrInUse;
                }
            }
        if ( !err )
            {
            iMediaServer.Start( iStatus );
            SetActive();
            }
        else
            {
            SetActive();
            TRequestStatus* stat = &iStatus;
            User::RequestComplete( stat, err );
            }
        }
    else if ( !aState && started )
        {
        __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
SetSharingState: StopSharing");
        iState = EStopSharing;
        // Ask current state of content server
        TUpnpProgress totalProgress;
        TPckgBuf<TUpnpProgress> progressBuf( totalProgress );
        TInt pubErr = RProperty::Get( KUpnpContentServerCat,
                                      ESharingProgress, progressBuf );
        TBool sharingOngoing( EFalse );
        if ( !pubErr )
            {
            Mem::Copy( &totalProgress,
                       progressBuf.Ptr(), sizeof( TUpnpProgress ) );
            if( totalProgress.iImageVideoProgress.iProgressType !=
                TUpnpProgressInfo::EVisualStatus ||
                totalProgress.iMusicProgress.iProgressType !=
                TUpnpProgressInfo::EVisualStatus )
                {
                sharingOngoing = ETrue;
                }
            }

        if( !err )
            {
            if ( !sharingOngoing )
                {
                // no sharing operations ongoing, stop possible
                err = iMediaServer.Stop();
                }
            else
                {
                __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
->Offline");
                err = iMediaServer.StartOffline();
                }

            if ( err != KErrNone )
                {
                __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO:: \
Error: iMediaServer");
                err = KErrGeneral;
                // if the MediaServer lost, setting iVisibility = EUnknown 
                // will insure the function SharingState() to start the
                // MediaServer session again.
                iVisibility = EUnknown;
                }
            else
                {
                iVisibility = EOffline;
                }

            iMediaServer.Close();
            }
        SetActive();
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, err );
        }
    else
        {
        //do nothing, it's already started
        iState = EStopSharing;
        if ( aState )
            {
            iState = EStartSharing;
            }
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::SetSharingState \
 end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::SharingState
// Gets file sharing status.
// --------------------------------------------------------------------------
//
TBool CUPnPFileSharingEngineAO::SharingStateL()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::SharingState begin");

    TBool status( EFalse );
    if ( iVisibility == EUnknown )
        {
        // Start media server session
        TInt err( OpenMediaServer() );
        if ( err == KErrNoMemory )
            {
            User::Leave( KErrNoMemory );
            }
        
        if ( !err )
            {
            TInt s( 0 );
            err = iMediaServer.Status( s );
            if ( s == RUpnpMediaServerClient::EStartedOnline )
                {
                status = ETrue;
                }
            iMediaServer.Close();
            }
        if ( status && !err )
            {
            iVisibility = EOnline;
            }
        else
            {
            iVisibility = EOffline;
            }
        }
     else if( iVisibility == EOffline ) 
        {
        TInt err( OpenMediaServer() );
        if ( err == KErrNoMemory )
            User::Leave( KErrNoMemory );  
        if ( !err )
            {
            TInt s( 0 );
            err = iMediaServer.Status( s );
            if ( s == RUpnpMediaServerClient::EStartedOnline )
                {
                status = ETrue;
                }
            iMediaServer.Close();
            }
        }     
    else if ( iVisibility == EOnline )
        {
        status = ETrue;
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::SharingState end");
    return status;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::ConnectionLost
// Gets file sharing status.
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::ConnectionLost()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
ConnectionLost begin");
    iVisibility = EOffline;
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
ConnectionLost end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::HasDiskSpace
// Check that there is enough disk space in c drive
// --------------------------------------------------------------------------
//
TBool CUPnPFileSharingEngineAO::HasDiskSpace()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
HasDiskSpace begin");
    const TInt KDiskSpaceThreshold( 4096 ); //4kB
    RFs fileServer;
    TInt error = fileServer.Connect();
    TBool retVal( EFalse );
    TVolumeInfo freeVol;
    if( !error )
        {
        error = fileServer.SetSessionPath( PathInfo::PhoneMemoryRootPath() );
        if( !error )
            {
            error = fileServer.Volume( freeVol );
            }
        }
    fileServer.Close();

    if( !error )
        {
        if( freeVol.iFree > KDiskSpaceThreshold )
            {
            __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO:\
disk space:OK!");
            retVal = ETrue;
            }
        }

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
HasDiskSpace end");
    return retVal;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::SetIconComplete
// callback from icon configurator
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::SetIconComplete( TInt aError )
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
SetIconComplete begin");
    
    if ( aError != KErrNone )
        {
        iMediaServer.Stop();  
        }
    
    // delete iconConfigurator as not needed anymore
    delete iIconConfigurator;
    iIconConfigurator = NULL;

    // continue sharing
    iState = EStartSharing;
    __ASSERTD( !IsActive(), __FILE__, __LINE__ );
    SetActive();
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aError );

    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
SetIconComplete end");
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::OpenMediaServer
// Creates connection to mediaserver
// --------------------------------------------------------------------------
//
TInt CUPnPFileSharingEngineAO::OpenMediaServer()
    {
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
OpenMediaServer start");

    TInt error = iMediaServer.Connect();
    if ( error == KErrNoMemory )
        {
        return error;
        }
    
    if ( error != KErrNone )
        {
        __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
OpenMediaServer: Connect FAILed");

        // try to reconnect
        iMediaServer.Close();
        error = iMediaServer.Connect();
        if( error )
            {
            __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
OpenMediaServer: ReConnect FAILed");
            }

        }
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
OpenMediaServer end");
    return error;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::EnableUpnpSecurityL
// store the cenrep value
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::EnableUpnpSecurityL()
    {
    // Check if the stack's security is enabled
    __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
EnableUpnpSecurityL: Checking the state of stack's security manager");
    TBool upnpSecurityEnabled = EFalse;
    TRAPD( secCheckError,
           upnpSecurityEnabled =
           CUpnpSecAccessController::IsMediaServerSecurityEnabledL());

    // If the security is not enabled, enable it now
    if( !secCheckError &&
        !upnpSecurityEnabled )
        {
        __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
EnableUpnpSecurityL: Activating stack's security manager");
        TRAPD( secEnableError,
               CUpnpSecAccessController::EnableMediaServerSecurityL());
        if( secEnableError )
            {
            __LOG("[UPNP_ENGINE]\t CUPnPFileSharingEngineAO::\
EnableUpnpSecurityL: Failed to activate stack's security manager");
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngineAO::CheckMediaserverIconsL
// Checks whether icons should be configured
// --------------------------------------------------------------------------
//
void CUPnPFileSharingEngineAO::CheckMediaserverIconsL()
    {
    // Create the icon configurator and see if icons must be configured.
    // If they need to, start asynchronous icon configuration process.
    delete iIconConfigurator;
    iIconConfigurator = NULL;
    iIconConfigurator = new (ELeave )
        CUPnPIconConfigurator( iMediaServer, *this );
    if ( iIconConfigurator->NeedToConfigureIcons() )
        {
        iState = ECheckIcon;
        }
    else
        {
        // icons already configured, go directly to sharing state
        delete iIconConfigurator;
        iIconConfigurator = NULL;
        iState = EStartSharing;
        }
    }

// End of file
