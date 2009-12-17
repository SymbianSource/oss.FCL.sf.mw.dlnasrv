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
* Description:  CMSEngineAO class implementation
*
*/



//  Include Files
#include "msengineobserver.h"
#include "msengineao.h"
#include "msengine.h"
#include "msdebug.h"


// ---------------------------------------------------------------------------
// CMSEngineAO::CMSEngineAO
// ---------------------------------------------------------------------------
//
CMSEngineAO::CMSEngineAO( CMSEngine* aEngine )
            : CActive( CActive::EPriorityStandard ),
              iEngine( aEngine )
    {
    LOG( _L( "[MediaServant ENG]\t CMSEngineAO::CMSEngineAO" ) );
    }


// ---------------------------------------------------------------------------
// CMSEngineAO::NewL
// ---------------------------------------------------------------------------
//
CMSEngineAO* CMSEngineAO::NewL( CMSEngine* aEngine )
    {
    LOG(_L("[MediaServant ENG]\t CMSEngineAO::NewL"));

    CMSEngineAO* self = new ( ELeave ) CMSEngineAO( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMSEngineAO::ConstructL
// ---------------------------------------------------------------------------
//
void CMSEngineAO::ConstructL()
    {
    LOG( _L("[MediaServant]\t CMSEngineAO::ConstructL") );

    // Add this active object to the active scheduler.
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::~CMSEngineAO
// ---------------------------------------------------------------------------
//
CMSEngineAO::~CMSEngineAO()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::~CMSEngineAO") );

    // cancel any ongoing operation
    Cancel();
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::RunL
// ---------------------------------------------------------------------------
//
void CMSEngineAO::RunL()
    {
    TRACE( Print( _L("[MediaServant ENG]\t CMSEngineAO::RunL \
    iStatus = %d" ), iStatus.Int() ) );

    switch ( iState )
        {
        case EMSEngineStore:
            {
            // stop if error occurs
            if ( iStatus.Int() == KErrNone  )
                {
                LOG(_L("[MediaServant ENG]\t CMSEngineAO::RunL start store"));
                // Store ok, continue by executing fill operation.
                ExecuteFillL();
                }
            else
                {
                // Close server session due to error.
                CloseServerSession();
                // Inform the observer about error.
                iObserver->ReadyL( iCurrentService, iStatus.Int() );
                }
            break;
            }
        case EMSEngineFill:
        // fall through
        case EMSEngineHarvest:
        // fall through
        case EMSEngineDeleteFiles:
        // fall through
        case EMSEnginePreprocess:
        // fall through
        case EMSEngineGetMetadata:
        // fall through
        case EMSEnginePreprocessList:
        // fall through
        case EMSEngineDeleteMetadata:
        // fall through
        case EMSUpdateFillLists:
            {
            CloseServerSession();
            // report for application
            iObserver->ReadyL( iCurrentService, iStatus.Int() );
            LOG( _L("[MediaServant ENG]\t CMSEngineAO::RunL close session") );
            break;
            }
        default:
            {
            LOG( _L("[MediaServant ENG]\t CMSEngineAO::RunL \
            not valid state") );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::DoCancel
// ---------------------------------------------------------------------------
//
void CMSEngineAO::DoCancel()
    {
    LOG( _L( "[MediaServant ENG]\t CMSEngineAO::DoCancel" ) );

    iContentManager.Cancel();
    }


// ---------------------------------------------------------------------------
// CMSEngineAO::ScanMediaServersL
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::ScanMediaServersL()
    {
    LOG( _L( "[MediaServant ENG]\t CMSEngineAO::ScanMediaServersL" ) );

    // Open content manager session.
    TInt error = OpenServerSession();

    if ( !error )
        {
        // Session created. Update state variables and execute service.
        iCurrentService = ECmServiceHarvest;
        iState = EMSEngineHarvest;
        iContentManager.ExecuteService( ECmServiceHarvest, iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::ExecuteFillL
// ---------------------------------------------------------------------------
//
void CMSEngineAO::ExecuteFillL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::ExecuteFillL") );

    // Update state variables and execute fill service.
    iCurrentService = ECmServiceFill;
    iState = EMSEngineFill;
    iContentManager.ExecuteService( ECmServiceFill, iStatus );

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        // Enable active object
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::ExecuteStoreL
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::ExecuteStoreL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::ExecuteStoreL") );

    iCurrentService = ECmServiceStore;
    iState = EMSEngineStore;

    // Open content manager session.
    TInt error = OpenServerSession();

    if ( !error )
        {
        iContentManager.ExecuteService( ECmServiceStore, iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::DeleteFilledFilesL()
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::DeleteFilledFilesL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::DeleteFilledFilesL") );

    // Open content manager session.
    TInt error = OpenServerSession();
    iState = EMSEngineDeleteFiles;
    if ( !error )
        {
        // Session created. Update state variables and execute service.
        iCurrentService = ECmServiceDeleteFilledFiles;
        iState = EMSEngineDeleteFiles;
        error = iContentManager.ExecuteService( ECmServiceDeleteFilledFiles,
                                                iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::ExecuteStorePreProcessingL()
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::ExecuteStorePreProcessingL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::ExecuteStorePreProcessingL") );

    // Open content manager session.
    TInt error = OpenServerSession();

    if ( !error )
        {
        iCurrentService = ECmServicePreProcessingStore;
        iState = EMSEnginePreprocess;
        error = iContentManager.ExecuteService( ECmServicePreProcessingStore,
                                                iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }
    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::ExecuteFillPreProcessingL()
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::ExecuteFillPreProcessingL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::ExecuteFillPreProcessingL") );

    // Open content manager session.
    TInt error = OpenServerSession();

    if ( !error )
        {
        iCurrentService = ECmServicePreProcessingFill;
        iState = EMSEnginePreprocess;
        error = iContentManager.ExecuteService( ECmServicePreProcessingFill,
                                                iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }
    return error;
    }

// --------------------------------------------------------------------------
// CMSEngine::GetFilteredMetadataL
// --------------------------------------------------------------------------
//
void CMSEngineAO::GetFilteredMetadataL(
                            CCmSqlPropertyCollector*& aCollector )
    {
    LOG(_L("[MediaServant ENG]\t CMSEngine::GetFilteredMetadataL"));

    // Open content manager session.
    User::LeaveIfError( OpenServerSession() );

    iCurrentService = ECmServiceFill;
    iState = EMSEngineGetMetadata;
    iContentManager.GetFilteredMetadataL( iCurrentService,
                                          aCollector,
                                          iStatus );

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        // Enable active object
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CMSEngineAO::PreProcessFillList
// --------------------------------------------------------------------------
//
TInt CMSEngineAO::PreProcessFillListL( const TDesC8& aListName )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::PreProcessFillListL") );

    // Open content manager session.
    TInt error = OpenServerSession();

    if ( !error )
        {
        iCurrentService = ECmServicePreProcessingFilllist;
        iState = EMSEnginePreprocessList;
        error = iContentManager.PreProcessFillList( aListName, iStatus );

        if ( IsActive() )
            {
            User::Leave( KErrInUse );
            }
        else
            {
            // Enable active object
            SetActive();
            }
        }
    return error;
    }

// --------------------------------------------------------------------------
// CMSEngineAO::UpdateFillListsL
// --------------------------------------------------------------------------
//
void CMSEngineAO::UpdateFillListsL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::UpdateFillListsL") );

    // Open content manager session.
    User::LeaveIfError( OpenServerSession() );

    iCurrentService = ECmServiceUpdateFillLists;
    iState = EMSUpdateFillLists;
    iContentManager.UpdateFillListsL( iStatus );

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        // Enable active object
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CMSEngineAO::DeleteMetadataL
// --------------------------------------------------------------------------
//
void CMSEngineAO::DeleteMetadataL()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::DeleteMetadataL") );

    // Open content manager session.
    User::LeaveIfError( OpenServerSession() );

    iCurrentService = ECmServiceDeleteMetadata;
    iState = EMSEngineDeleteMetadata;
    iContentManager.DeleteMetadataL( iStatus );

    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    else
        {
        // Enable active object
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::OpenServerSession
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::OpenServerSession()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::OpenServerSession") );

    TInt error( KErrInUse );
    // Enable active object
    if ( !IsActive() )
        {
        error = iContentManager.Connect();
        }

    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::CloseServerSession
// ---------------------------------------------------------------------------
//
TInt CMSEngineAO::CloseServerSession()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::CloseServerSession") );

    TInt error = KErrNone;
    iContentManager.Close();

    return error;
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::ApplicationExit
// ---------------------------------------------------------------------------
//
void CMSEngineAO::ApplicationExit()
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::ApplicationExit") );

    if ( !IsActive() )
        {
        OpenServerSession();
        }
    iContentManager.ApplicationExit();
    if ( IsActive() || iContentManager.Handle() )
        {
        CloseServerSession();
        }
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::StopOperation
// ---------------------------------------------------------------------------
//
void CMSEngineAO::StopOperation()
    {
    LOG(_L("[MediaServant ENG]\t CMSEngineAO::StopOperation"));

    if ( IsActive() )
        {
        LOG(_L("[MediaServant ENG]\t CMSEngineAO::StopOperation active \
        cancelled"));
        Cancel();
        }
    else
        {
        LOG( _L("[MediaServant ENG]\t CMSEngineAO::StopOperation \
        contentmanager cancelled") );
        // This is used to cancel scheduler initiated operation
        // Open server session first
        OpenServerSession();
        iContentManager.Cancel();
        }

    LOG( _L("[MediaServant ENG]\t CMSEngineAO::StopOperation Server \
    cancelled") );
    CloseServerSession();
    }

// ---------------------------------------------------------------------------
// CMSEngineAO::SetObserver
// ---------------------------------------------------------------------------
//
void CMSEngineAO::SetObserver( MMSEngineObserver* aObserver )
    {
    LOG( _L("[MediaServant ENG]\t CMSEngineAO::SetObserver") );

    iObserver = aObserver;
    }
    
// End of file
