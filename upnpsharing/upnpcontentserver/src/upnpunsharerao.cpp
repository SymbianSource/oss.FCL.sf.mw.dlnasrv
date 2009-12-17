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
* Description:      CUpnpUnsharerAo class implementation
 *
*/







// INCLUDE FILES
#include "upnpdlnaprofiler.h"
#include "upnpcdsreselementutility.h"
#include "upnpcommonutils.h"
#include <upnpelement.h>

#include "upnpunsharerao.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

// CONSTANTS
const TInt KDefaultBrowseReqCount( 20 );
const TInt KDefaultBrowseStartingInd( 0 );

using namespace UpnpContentServer;

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::CUpnpUnsharerAo
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpUnsharerAo::CUpnpUnsharerAo( MUpnpSharingCallback* aCallback,
                                  TInt aPriority )
    : CActive( aPriority ),
      iCallback( aCallback )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iTotalItems = KErrNotFound;
    iAllTotalItems = 0;
    CActiveScheduler::Add( this );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::~CUpnpUnsharerAo()
// C++ destructor.
// --------------------------------------------------------------------------
//
CUpnpUnsharerAo::~CUpnpUnsharerAo()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iFileSharing;
    iFileSharing = NULL;
    iMediaServer.Close();

    if ( IsActive() )
        {
        __LOG1("IsActive: true  iCurrentOperation: %d", iCurrentOperation );
        iCurrentOperation = ENotActive;
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrNone );
        }

    Cancel();

    Cleanup();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::RunL
// Called when asyncronous request is ready
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::RunL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( iStatus.Int() == KErrCorrupt ||
        iStatus.Int() == KErrNoMemory ||
        iStatus.Int() == KErrDiskFull )
        {
        iCurrentOperation = ENotActive;
        }

    if ( iStopRequested == MUpnpSharingCallback::ESharingPause )
        {
        delete iFileSharing;
        iFileSharing = NULL;
        iFileSharing = CUpnpFileSharing::NewL();
        iStopRequested = MUpnpSharingCallback::ESharingNoStop;
        }

    switch ( iCurrentOperation )
        {
    case EInitialize :
        {
        TInt err( InitializeL( iUpContainer ) );
        if ( !err )
            {
            QueryContainersFromMediaServerL( iUpContainer );
            }
        else
            {
            iCurrentOperation = ENotActive;
            SelfComplete( KErrNone );
            }
        }
        break;
    case EQueryContainersFromMediaServer :
        {
        QueryContainersFromMediaServerL( iCurrentContainerId );
        }
        break;
    case EQueryContainersFromMediaServerResult :
        {
        QueryContainersFromMediaServerResultL();
        }
        break;
    case EQueryItemsFromMediaServer :
        {
        QueryContainerItemsL( iCurrentContainerId );
        }
        break;
    case EUnshareItemList:
        {
        UnshareItemListL();
        }
        break;
    case EQueryItemsFromMediaServerResult :
        {
        QueryContainerItemsResultL();
        }
        break;
    case EUnshareContainer :
        {
        UnshareContainerL();
        }
        break;
    case EUnshareContainerResult :
        {
        UnshareContainerResultL();
        }
        break;
    case ENotActive :
        {
        Cleanup();
        if ( iCallback )
            {
            iCallback->CompleteSharingOperationL( iStatus.Int(), 0 );
            }
        }
        break;
    default :
        break;
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::DoCancel
// Cancels active object
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::DoCancel()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // NOTE: The cancelling of mediaserver request must be made in destructor
    // because there is no cancel in CUpnpFileSharing
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::RunError
// Handle leaves
// --------------------------------------------------------------------------
//
TInt CUpnpUnsharerAo::RunError( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    TInt ret( KErrNone );
    if ( aError == KErrNoMemory ||
         aError == KErrDiskFull ||
         aError == KErrCorrupt )
        {
        iCurrentOperation = ENotActive;
        SelfComplete( aError );
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return ret;
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::QueryContainersFromMediaServerL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::QueryContainersFromMediaServerL( TInt aId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpBrowseCriteria* browseCriteria = CreateBrowseCriteriaLC();
    browseCriteria->SetStartingIndex( iContainerBrowsePosition );

    if ( iContainerList )
        {
        delete iContainerList;
        iContainerList = NULL;
        }
    iContainerList = CUpnpContainerList::NewL();

    // Get shared containers to iContainerList
    iFileSharing->GetSharedContainerListL( aId,
                                           *browseCriteria,
                                           *iContainerList,
                                           iTotalContainers,
                                           iStatus );
    iCurrentOperation = EQueryContainersFromMediaServerResult;
    CleanupStack::PopAndDestroy( browseCriteria );
    SetActive();

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::QueryContainersFromMediaServerResultL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::QueryContainersFromMediaServerResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpContainer* nextContainer( NULL );
    TInt containerCount( iContainerList->ObjectCount() );
    if ( containerCount )
        {
        // there are subcontainers -> store them and query if they
        // have subcontainers
        iContainerLists.AppendL( iContainerList ); //Transfer ownership
        // check if we neet to get containers in segments

        if ( iContainerBrowsePosition + containerCount 
             >= iTotalContainers )
            {
            // all segments queried
            // next container under processing is always last in the list
            nextContainer =
                static_cast< CUpnpContainer* >(
                    iContainerList->operator[](
                        containerCount - 1 ) );
            iCurrentContainerId = UPnPCommonUtils::DesC8ToInt(
                nextContainer->Id());
            iContainerList = NULL;
            iContainerBrowsePosition = 0;
            }
        else
            {
            __LOG("CUpnpUnsharerAo: retrieving next segment");
            // set the starting position for retrieving next segment
            iContainerBrowsePosition += KDefaultBrowseReqCount;
            iContainerList = NULL;
            }

        iCurrentOperation = EQueryContainersFromMediaServer;
        }
    else
        {
        // no subcontainers -> query the items
        if ( iContainerLists.Count() )
            {
            CUpnpContainerList* contList =
                iContainerLists[ iContainerLists.Count()-1 ];
            nextContainer =
                static_cast< CUpnpContainer* >(
                    contList->operator[](
                        contList->ObjectCount() - 1 ) );
            iCurrentContainerId = UPnPCommonUtils::DesC8ToInt(
                nextContainer->Id() );
            // there should be at least the initial container
            iCurrentOperation = EQueryItemsFromMediaServer;

            delete iContainerList;
            iContainerList = NULL;

            iCurrentOperation  = EQueryItemsFromMediaServer;

            }
        else
            {
            // error: no initial container
            iCurrentOperation  = ENotActive;

            }
        }
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::QueryContainerItemsL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::QueryContainerItemsL( TInt aId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    delete iItemList;
    iItemList = NULL;
    iItemList = CUpnpItemList::NewL();
    if ( !iFileSharing )
        {
        User::Leave( KErrGeneral );
        }
    CUpnpBrowseCriteria* browseCriteria = CreateBrowseCriteriaLC();
    if ( iTotalItems == KErrNotFound )
        {
        iFileSharing->GetSharedItemListL (
            aId, *browseCriteria,
            *iItemList, iTotalItems, iStatus );

        if ( iTotalItems > 0 )
            {
            iAllTotalItems += iTotalItems;
            }
        }
    else
        {
        TInt tmp( 0 );
        iFileSharing->GetSharedItemListL (
            aId, *browseCriteria,
            *iItemList, tmp, iStatus );

        if ( tmp > 0 )
            {
            iAllTotalItems += tmp;
            }
        }
    iCurrentOperation = EQueryItemsFromMediaServerResult;
    CleanupStack::PopAndDestroy( browseCriteria );
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::QueryContainerItemsResultL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::QueryContainerItemsResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    if ( !iItemList )
        {
        User::Leave( KErrGeneral );
        }
    TInt objCount( iItemList->ObjectCount() );
    if ( objCount && ( iExecStatus < iTotalItems ) )
        {
        // unshare the items in parts
        iCurrentOperation = EUnshareItemList;
        
        }
    else
        {
        // the container iCurrentContainerId is empty -> unshare it unless it
        // is the container received at the start

        if ( iContainerLists.Count() > 1 )
            {
            iCurrentOperation = EUnshareContainer;
            }
        else
            {
            // all done
            iCurrentOperation = ENotActive;
            }
        }

    if ( iExecStatus > 0  )
        {
        iCallback->SetProgressL( iExecStatus );
        }

    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::UnshareItemListL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::UnshareItemListL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    iExecStatus += iItemList->ObjectCount();
    iFileSharing->UnshareItemListL( *iItemList, iUnsharePosition, iStatus );

    delete iItemList;
    iItemList = NULL;
    iCurrentOperation = EQueryItemsFromMediaServer;
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::UnshareContainerL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::UnshareContainerL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iFileSharing->UnshareContainerL( iCurrentContainerId, iStatus );
    iCurrentOperation = EUnshareContainerResult;
    iExecStatus = 0;
    iTotalItems = KErrNotFound;
    SetActive();
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::UnshareContainerResultL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::UnshareContainerResultL()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    if ( iContainerLists.Count() )
        {
        // one container is now fully handled -> remove from list of
        // containers
        CUpnpContainerList* contList =
            iContainerLists[ iContainerLists.Count()-1 ];
        // note: iCurrentContainerId is always last in the list
        contList->RemoveAndDestroy( *UPnPCommonUtils::IntToDesC8LC(
            iCurrentContainerId ) );
        CleanupStack::PopAndDestroy(); // UPnPCommonUtils::IntToDesC8LC
        if ( contList->ObjectCount() )
            {
            // get the next container ( last of the list ) to process
            TInt nextPos = contList->ObjectCount() - 1;
            CUpnpContainer* nextContainer = static_cast< CUpnpContainer* >(
                contList->operator[]( nextPos ));
            iCurrentContainerId = UPnPCommonUtils::DesC8ToInt(
                nextContainer->Id());
            // Restart cycle: query if it has subcontainers
            iCurrentOperation = EQueryContainersFromMediaServer;

            }
        else
            {
            // one directory hierarchy is empty
            CUpnpContainerList* removeList = iContainerLists[
                iContainerLists.Count()-1 ];
            iContainerLists.Remove( iContainerLists.Count()-1 );
            delete removeList;
            if ( iContainerLists.Count() )
                {
                // go back in containerlists
                contList =
                    iContainerLists[ iContainerLists.Count()-1 ];
                TInt backPos( contList->ObjectCount() - 1 );
                CUpnpContainer* nextContainer =
                    static_cast< CUpnpContainer* >(
                        contList->operator[]( backPos ));
                iCurrentContainerId = UPnPCommonUtils::DesC8ToInt(
                    nextContainer->Id() );
                // Restart cycle: query if it has items
                // no need to query containers
                iCurrentOperation = EQueryItemsFromMediaServer;

                }
            else
                {
                // all done
                iCurrentOperation = ENotActive;
                }
            }
        }
    else
        {
        // all done
        iCurrentOperation = ENotActive;
        }
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::EmptyContainer
// (other items are commented in header)
// --------------------------------------------------------------------------
//
TInt CUpnpUnsharerAo::EmptyContainer( TInt& aContainerId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt err( KErrNotFound );

    iUpContainer = aContainerId;
    iCurrentContainerId = aContainerId;
    iCurrentOperation = EInitialize;
    SelfComplete( KErrNone );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return err;
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::RequestStop
// ( other items are commented in header )
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::RequestStop(
    MUpnpSharingCallback::TSharingStopType aStopType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iStopRequested = aStopType;
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::InitializeL
// (other items are commented in header)
// --------------------------------------------------------------------------
//
TInt CUpnpUnsharerAo::InitializeL( TInt& aContainerId )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    // First initialize resources
    iFileSharing = CUpnpFileSharing::NewL();
    TInt error( iMediaServer.Connect() );
    if ( !error )
        {
        // start offline if not started
        TInt status( RUpnpMediaServerClient::EStopped );
        iMediaServer.Status( status );
        if ( status == RUpnpMediaServerClient::EStopped )
            {
            error = iMediaServer.StartOffline();
            }
        if ( !error )
            {
            CUpnpContainerList* beginList = CUpnpContainerList::NewLC();
            CUpnpContainer* beginContainer = CUpnpContainer::NewL();
            CleanupStack::PushL( beginContainer );
            beginContainer->SetIdL(
                *UPnPCommonUtils::IntToDesC8LC( aContainerId ) );
            CleanupStack::PopAndDestroy(); // IntToDesC8LC
            beginList->AppendObjectL( *beginContainer ); //Transfer ownership
            CleanupStack::Pop( beginContainer );
            iContainerLists.AppendL( beginList ); //Transfer ownership
            CleanupStack::Pop( beginList );
            }
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return error;
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::CreateBrowseCriteriaLC
// (other items are commented in header)
// --------------------------------------------------------------------------
//
CUpnpBrowseCriteria* CUpnpUnsharerAo::CreateBrowseCriteriaLC() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    CUpnpBrowseCriteria* browseCriteria = CUpnpBrowseCriteria::NewLC();
    browseCriteria->SetRequestedCount( KDefaultBrowseReqCount );
    browseCriteria->SetStartingIndex( KDefaultBrowseStartingInd );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return browseCriteria;
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::SelfComplete
// Complete own request
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::SelfComplete( TInt aError )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    SetActive();
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aError );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::Cleanup
// (other items are commented in header)
// --------------------------------------------------------------------------
//
void CUpnpUnsharerAo::Cleanup()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iExecStatus = 0;
    iTotalItems = KErrNotFound;
    iTotalContainers = KErrNotFound;
    iContainerBrowsePosition = 0;

    iCurrentContainerId = KErrNotFound;
    iUpContainer = KErrNotFound;
    iStopRequested = MUpnpSharingCallback::ESharingNoStop;

    delete iItemList;
    iItemList = NULL;
    delete iContainerList;
    iContainerList = NULL;

    iContainerLists.ResetAndDestroy();

    iCurrentOperation = ENotActive;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::UnSharingProgress
// (the usage in header)
// --------------------------------------------------------------------------
//
TInt CUpnpUnsharerAo::UnSharingProgress( ) 
    { 
    TInt progress( 0 );
    if ( iExecStatus >= 0 )
        {
        progress = iExecStatus;
        }
    return progress;
    }

// --------------------------------------------------------------------------
// CUpnpUnsharerAo::TotalItemsForUnShare
// (the usage in header)
// --------------------------------------------------------------------------
//
TInt CUpnpUnsharerAo::TotalItemsForUnShare( ) 
    {
    TInt items( 0 );
    if ( iTotalItems >= 0 )
        {
        items = iTotalItems;
        }
    return items;
    }

// End of file
