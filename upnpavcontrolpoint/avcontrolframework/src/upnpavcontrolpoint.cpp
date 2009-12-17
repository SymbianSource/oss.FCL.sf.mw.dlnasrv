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
* Description:  CUpnpAVControlPoint
*
*/


// INCLUDE FILES
#include "upnpavcontrolpoint.h"
#include "upnpargument.h"

#include "upnpstateupdatehandler.h"
#include "upnpactionresponsehandler.h"
#include "upnpavtactionfactory.h"
#include "upnpcdsactionfactory.h"
#include "upnpcmactionfactory.h"
#include "upnprcactionfactory.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"
#include "upnpcons.h"


// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAVControlPoint* CUpnpAVControlPoint::NewL(
            MUpnpAVControlPointObserver& aAVControlPointObserver)
    {
    CUpnpAVControlPoint* self = 
        new (ELeave) CUpnpAVControlPoint(aAVControlPointObserver);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CUpnpAVControlPoint
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAVControlPoint::CUpnpAVControlPoint(
     MUpnpAVControlPointObserver& aAVControlPointObserver)
                : CUpnpControlPoint(),
                  iAVControlPointObserver(aAVControlPointObserver)
    {
    }

// -----------------------------------------------------------------------------
// CUpnpStateUpdateHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::ConstructL( )
    {
    _LIT8( KMediaServer, "urn:schemas-upnp-org:device:MediaServer" );
    _LIT8( KMediaRenderer, "urn:schemas-upnp-org:device:MediaRenderer" );

    CDesC8ArrayFlat* targetDeviceTypes = new(ELeave) CDesC8ArrayFlat(1);
    CleanupStack::PushL( targetDeviceTypes );
    targetDeviceTypes->AppendL( KMediaServer() );
    targetDeviceTypes->AppendL( KMediaRenderer() );    
    CUpnpControlPoint::ConstructL( *targetDeviceTypes );
    CleanupStack::Pop( targetDeviceTypes );
    targetDeviceTypes->Reset();
    delete targetDeviceTypes; 
    targetDeviceTypes = NULL;

    TPtrC8 devicePtr;
    devicePtr.Set( UpnpSSDP::KUPnPRootDevice );
    SearchL( devicePtr );

    iStateUpdateHandler = CUpnpStateUpdateHandler::NewL( 
        iAVControlPointObserver, 
        *this );
    iActionResponseHandler = CUpnpActionResponseHandler::NewL(
        iAVControlPointObserver,
        *this );
    
    iAvtActionFactory = CUpnpAvtActionFactory::NewL(
        iAVControlPointObserver,
        *this );
    iCdsActionFactory = CUpnpCdsActionFactory::NewL(
        iAVControlPointObserver,
        *this );
    iCmActionFactory = CUpnpCmActionFactory::NewL(
        iAVControlPointObserver,
        *this );
    iRcActionFactory = CUpnpRcActionFactory::NewL(
        iAVControlPointObserver,
        *this );
  
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::~CUpnpAVControlPoint
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpAVControlPoint::~CUpnpAVControlPoint()
    {
    delete iStateUpdateHandler;
    delete iActionResponseHandler;
    delete iAvtActionFactory;
    delete iCdsActionFactory;
    delete iCmActionFactory;
    delete iRcActionFactory;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::InitializeCdsActionFactoryL
// Initialize CdsActionFactory if is NULL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::InitializeCdsActionFactoryL() 
    {
    
    if (!iCdsActionFactory)
        {
        iCdsActionFactory = CUpnpCdsActionFactory::NewL(
            iAVControlPointObserver,
            *this );
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::StateUpdatedL
// This function implements an inteface and forwards request 
// to stateupdate handler.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::StateUpdatedL(CUpnpService*  aService)
    {
    iStateUpdateHandler->StateUpdated( aService );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::DeviceDiscoveredL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::DeviceDiscoveredL(CUpnpDevice* aDevice)
    {
    iAVControlPointObserver.DeviceDiscoveredL( aDevice );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::DeviceDisappearedL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::DeviceDisappearedL(CUpnpDevice* aDevice)
    {    
    iAVControlPointObserver.DeviceDisappearedL(aDevice);
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::ActionResponseReceivedL
// This function ralizes an interface. Functionality is located in separate 
// handler class.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::ActionResponseReceivedL(CUpnpAction* aAction)
    {
    TInt err = iActionResponseHandler->ActionResponseReceived( aAction );
#ifdef _DEBUG
    if (err != KErrNone)
        {
        LOGS("Error in action response received.");
        }
#endif //_DEBUG
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::HttpResponseReceivedL
// This function implements an inteface and notifies an observer.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::HttpResponseReceivedL(CUpnpHttpMessage* aMessage)
    {    
    iAVControlPointObserver.HttpResponseL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::Service
// This function returns a pointer to appropriate service instance.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpService* CUpnpAVControlPoint::Service( CUpnpDevice* aMediaServer, 
                                    const TDesC8& aServiceType )
    {
    RPointerArray<CUpnpService>& services = aMediaServer->ServiceList();
    
    for( TInt i(0); i < services.Count(); i++ )
        {
        if( services[i]->ServiceType().Match( aServiceType ) != KErrNotFound )
            {
            return services[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtSetTransportUriActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtSetTransportUriActionL(const TDesC8& aUuid,
                                                 TInt aInstanceId,
                                                 const TDesC8& aCurrentUri,
                                                 const TDesC8& aCurrentMetaData)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtSetTransportUriActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aCurrentUri,
            aCurrentMetaData
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtSetNextTransportUriActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtSetNextTransportUriActionL(
                                                 const TDesC8& aUuid,
                                                 TInt aInstanceId,
                                                 const TDesC8& aNextUri,
                                                 const TDesC8& aNextMetaData)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtSetNextTransportUriActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aNextUri,
            aNextMetaData
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtSeekActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtSeekActionL(const TDesC8& aUuid,
                                      TInt aInstanceId,
                                      const TDesC8& aUnit,
                                      const TDesC8& aTarget)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtSeekActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aUnit,
            aTarget
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtMediaInfoActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtMediaInfoActionL(const TDesC8& aUuid,
                                           TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtMediaInfoActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtTransportInfoActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtTransportInfoActionL(const TDesC8& aUuid,
                                               TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtTransportInfoActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtPositionInfoActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtPositionInfoActionL(const TDesC8& aUuid,
                                              TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtPositionInfoActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtDeviceCapabilitiesActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtDeviceCapabilitiesActionL(const TDesC8& aUuid,
                                                    TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtDeviceCapabilitiesActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtTransportSettingsActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtTransportSettingsActionL(const TDesC8& aUuid,
                                                   TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtTransportSettingsActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtStopActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtStopActionL(const TDesC8& aUuid,
                                      TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtStopActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtPlayActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtPlayActionL(const TDesC8& aUuid,
                                      TInt aInstanceId,
                                      const TDesC8& aSpeed)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtPlayActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aSpeed
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtPauseActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtPauseActionL(const TDesC8& aUuid,
                                       TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtPauseActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtRecordActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtRecordActionL(const TDesC8& aUuid,
                                        TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtRecordActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtNextActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtNextActionL(const TDesC8& aUuid,
                                      TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtNextActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtPreviousActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtPreviousActionL(const TDesC8& aUuid,
                                          TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtPreviousActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtSetPlayModeActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtSetPlayModeActionL(const TDesC8& aUuid,
                                             TInt aInstanceId,
                                             const TDesC8& aPlayMode)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtSetPlayModeActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aPlayMode
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtSetRecordModeActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtSetRecordModeActionL(const TDesC8& aUuid,
                                               TInt aInstanceId,
                                               const TDesC8& aRecordMode)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtSetRecordModeActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aRecordMode
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::AvtTransportsActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::AvtTransportsActionL(const TDesC8& aUuid,
                                            TInt aInstanceId)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iAvtActionFactory->AvtTransportsActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsBrowseActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsBrowseActionL(const TDesC8& aUuid,
                                        const TDesC8& aObjectId,
                                        const TDesC8& aBrowseFlag,
                                        const TDesC8& aFilter, 
                                        TInt aStartingIndex,
                                        TInt aRequestedCount,
                                        const TDesC8& aSortCriteria)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsBrowseActionLC(
            action,
            (CUpnpDevice*)device,
            aObjectId,
            aBrowseFlag,
            aFilter,
            aStartingIndex,
            aRequestedCount,
            aSortCriteria
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsSearchActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsSearchActionL(const TDesC8& aUuid,
                                        const TDesC8& aObjectId,
                                        const TDesC8& aSearchCriteria,
                                        const TDesC8& aFilter, 
                                        TInt aStartingIndex,
                                        TInt aRequestedCount,
                                        const TDesC8& aSortCriteria
                                        )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsSearchActionLC(
            action,
            (CUpnpDevice*)device,
            aObjectId,
            aSearchCriteria,
            aFilter,
            aStartingIndex,
            aRequestedCount,
            aSortCriteria
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsSystemUpdateIdActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsSystemUpdateIdActionL(const TDesC8& aUuid)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsSystemUpdateIdActionLC(
            action,
            (CUpnpDevice*)device
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsSearchCapabilitiesActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsSearchCapabilitiesActionL(const TDesC8& aUuid)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsSearchCapabilitiesActionLC(
            action,
            (CUpnpDevice*)device
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsSortCapabilitiesActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsSortCapabilitiesActionL(const TDesC8& aUuid)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsSortCapabilitiesActionLC(
            action,
            (CUpnpDevice*)device
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsCreateObjectActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//                                                                                               
EXPORT_C TInt CUpnpAVControlPoint::CdsCreateObjectActionL(const TDesC8& aUuid,
                                              const TDesC8& aId, 
                                              const TDesC8& aElements)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsCreateObjectActionLC(
            action,
            (CUpnpDevice*)device,
            aId,
            aElements
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsImportResourceActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsImportResourceActionL(const TDesC8& aUuid,
                                                const TDesC8& aSourceUri, 
                                                const TDesC8& aDestinationUri
                                                )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsImportResourceActionLC(
            action,
            (CUpnpDevice*)device,
            aSourceUri,
            aDestinationUri
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsExportResourceActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsExportResourceActionL(const TDesC8& aUuid,
                                                const TDesC8& aSourceUri, 
                                                const TDesC8& aDestinationUri )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsExportResourceActionLC(
            action,
            (CUpnpDevice*)device,
            aSourceUri,
            aDestinationUri
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsCreateReferenceActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsCreateReferenceActionL(
                                        const TDesC8& aUuid,
                                        const TDesC8& aDestinationContainerId, 
                                        const TDesC8& aSourceObjectId )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsCreateReferenceActionLC(
            action,
            (CUpnpDevice*)device,
            aDestinationContainerId,
            aSourceObjectId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsDeleteResourceActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsDeleteResourceActionL(const TDesC8& aUuid,
                                                const TDesC8& aResourceUri )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsDeleteResourceActionLC(
            action,
            (CUpnpDevice*)device,
            aResourceUri
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsDestroyObjectActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsDestroyObjectActionL(const TDesC8& aUuid,
                                               const TDesC8& aObjectId )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsDestroyObjectActionLC(
            action,
            (CUpnpDevice*)device,
            aObjectId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsStopTransferActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsStopTransferActionL(const TDesC8& aUuid,
                                              TInt aTransferId )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsStopTransferActionLC(
            action,
            (CUpnpDevice*)device,
            aTransferId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsTransferProgressActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsTransferProgressActionL(const TDesC8& aUuid,
                                                  TInt aTransferId )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsTransferProgressActionLC(
            action,
            (CUpnpDevice*)device,
            aTransferId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CdsUpdateObjectActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CdsUpdateObjectActionL(const TDesC8& aUuid,
                                              const TDesC8& aObjectID,
                                              const TDesC8& aCurrentTagValue,
                                              const TDesC8& aNewTagValue )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCdsActionFactory->CdsUpdateObjectActionLC(
            action,
            (CUpnpDevice*)device,
            aObjectID,
            aCurrentTagValue,
            aNewTagValue
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CmProtocolInfoActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CmProtocolInfoActionL(
                                                        const TDesC8& aUuid)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCmActionFactory->CmProtocolInfoActionLC(
            action,
            (CUpnpDevice*)device
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CmCurrentConnectionsActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CmCurrentConnectionsActionL(const TDesC8& aUuid)
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCmActionFactory->CmCurrentConnectionsActionLC(
            action,
            (CUpnpDevice*)device
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CmPrepareConnectionActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CmPrepareConnectionActionL(
                                          const TDesC8& aUuid,
                                          const TDesC8& aRemoteProtocolInfo,
                                          const TDesC8& aPeerConnectionManager,
                                          TInt aPeerConnectionID,
                                          const TDesC8& aDirection
                                                  )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCmActionFactory->CmPrepareConnectionActionLC(
            action,
            (CUpnpDevice*)device,
            aRemoteProtocolInfo,
            aPeerConnectionManager,
            aPeerConnectionID,
            aDirection
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CmConnectionCompleteActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CmConnectionCompleteActionL(
                                                   const TDesC8& aUuid,
                                                   TInt aConnectionId
                                                   )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCmActionFactory->CmConnectionCompleteActionLC(
            action,
            (CUpnpDevice*)device,
            aConnectionId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::CmCurrentConnectionInfoActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::CmCurrentConnectionInfoActionL(
                                                      const TDesC8& aUuid,
                                                      TInt aConnectionId
                                                      )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iCmActionFactory->CmCurrentConnectionInfoActionLC(
            action,
            (CUpnpDevice*)device,
            aConnectionId
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::RcGetVolumetActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::RcGetVolumetActionL(
                                           const TDesC8& aUuid,
                                           TInt aInstanceID,
                                           const TDesC8& aChannel
                                           )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iRcActionFactory->RcGetVolumeActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceID,
            aChannel
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::RcSetVolumetActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::RcSetVolumetActionL(
                                           const TDesC8& aUuid,
                                           TInt aInstanceId,
                                           const TDesC8& aChannel,
                                           TInt aVolume
                                           )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iRcActionFactory->RcSetVolumeActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aChannel,
            aVolume
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::RcGetMuteActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::RcGetMuteActionL(
                                        const TDesC8& aUuid,
                                        TInt aInstanceId,
                                        const TDesC8& aChannel
                                        )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iRcActionFactory->RcGetMuteActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aChannel
            );

        if( err == KErrNone )
            {
            
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::RcSetMuteActionL
// This function creates and sends a action.
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CUpnpAVControlPoint::RcSetMuteActionL(
                                        const TDesC8& aUuid,
                                        TInt aInstanceId,
                                        const TDesC8& aChannel,
                                        const TDesC8& aMute
                                        )
    {
    const CUpnpDevice* device = Device( aUuid );
    if( !device ) 
        {
        return KErrBadHandle;
        }
    else
        {
        CUpnpAction* action = NULL;
        TInt err( KErrNone );
        err = iRcActionFactory->RcSetMuteActionLC(
            action,
            (CUpnpDevice*)device,
            aInstanceId,
            aChannel,
            aMute
            );

        if( err == KErrNone )
            {
            SendL( action );
            CleanupStack::Pop( action );

            return action->SessionId();
            }
        else
            {
            return err;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpAVControlPoint::NetworkEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpAVControlPoint::NetworkEvent( CUpnpNetworkEventBase* aEvent )
    {
    CUpnpControlPoint::NetworkEvent( aEvent );                
    TRAP_IGNORE( SearchL( UpnpSSDP::KUPnPRootDevice ) );
    }    

//end of file
