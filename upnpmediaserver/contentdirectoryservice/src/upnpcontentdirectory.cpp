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
* Description:  Content Directory
 *
*/


// INCLUDE FILES
#include <fbs.h>
#include <uri8.h>
#include <pathinfo.h>
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengserializationoptions.h>
#include <xml/dom/xmlengdomparser.h>
#include <sysutil.h>
#include <bautils.h>
#include <caf/caf.h>
#include <caf/manager.h>

#include "upnpsender.h"
#include "upnpcontentdirectory.h"
#include "upnperrors.h"
#include "upnpdominterface.h"
#include "upnpstring.h"
#include "upnpargument.h" 
#include "upnphttpmessagefactory.h" 
#include "upnpcons.h"
#include "upnpdevice.h"
#include "upnpfileutils.h"
#include "upnpcommonupnplits.h"
#include "upnpcontainerupdateid.h"
#include "upnpcontentdirectorydb.h"
#include "upnpfiletransfertimerobserver.h"
#include "upnperror.h"
#include "upnpcustomlog.h"
#include "upnpprotocolinfo.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpresourcesbean.h"
#include "upnpcdutils.h"
#include "upnpmetadatastorage.h"
#include "upnpautodestroyobjectlist.h"
#include "upnpautodestroyobject.h"
#include "upnpcontentdirectoryeventobserver.h"
#include "upnpfiletransferevent.h"
#include "upnpfiletransfer.h"
#include "upnpprotocolinfolocal.h"
#include "upnpxmlcontentfilter.h"
#include "upnpstatehandler.h"
#include "upnptransferuploader.h"
#include "upnpmimemapper.h"
#include "upnptransfercontroller.h"
#include "upnptransferinterface.h"
#include "upnpdlnafilter.h"
#include "upnphttpserversession.h"
#include "upnphttpserverruntime.h"
#include "upnpsettings.h"
#include "upnpdeviceimplementationbase.h"
#include "upnpsecuritymanager.h"
#include "upnpmediaserversettings.h"

// Constant definitions
using namespace UpnpPaths;
using namespace UpnpDlnaProtocolInfo;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DestroyRPointerArray
// Defined in upnpelementfactory.cpp
// -----------------------------------------------------------------------------
//
void DestroyRArray( TAny* aArray );
void RPointerArrayTPtrC8( TAny* aArray )
    {
    RPointerArray<TPtrC8>* array =
            reinterpret_cast<RPointerArray<TPtrC8>*>(aArray);
    array->ResetAndDestroy( );
    }

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CUpnpContentDirectory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectory::CUpnpContentDirectory( CUpnpDevice& aDevice,
    MUpnpThumbnailCreatorObserver* aThmbObs, MUpnpTransferInterface* aTransfer ) :

    CUpnpServiceImplementation(aDevice), iThmbObs(aThmbObs),
            iTransferHandler(aTransfer)

    {
    SetExecutedAction( EUndefinedAction );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::~CUpnpContentDirectory
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpContentDirectory::~CUpnpContentDirectory()
    {
    delete iDownloadDir;
    delete iThumbDir;
    delete iElementdb;

    delete iTransferController;
    delete iMimeToExtMap;

    delete iContentDirectoryDb;

    iThObjectIds.Close( );

    iThumbnailCreators.ResetAndDestroy( );

    iDOMImpl.Close( );
    if ( iBitmapServerConnection == KErrNone )
        RFbsSession::Disconnect( );
    iFs.Close( );
    if ( iEcomUsed )
        {
        REComSession::FinalClose( );
        }
    delete iAutoDestroyObjects;
    delete iStateHandler;
    if ( iHttpServerSession )
        {
        iHttpServerSession->Stop();
        }
    delete iHttpServerSession;
    delete iDlnaFilter;
    delete iSecurityManager;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ConstructL
// Two-phased constructor.
// Initialize variables, set parent service and create XML reader.
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::ConstructL( TUpnpCdSettings aSettings,
    CUpnpMetadataStorage* aMetadataStorage )
    {
    iDOMImpl.OpenL( );

    iTransferController = CUpnpTransferController::NewL( this );

    iMimeToExtMap = CUpnpMimeMapper::NewL( );

    BaseConstructL( aSettings.iDescriptionPath, KContentDirectoryType( ) );

    User::LeaveIfNull( aSettings.iDevice );
    
    CUpnpSecurityManager* secMan = CUpnpSecurityManager::NewInstanceFromUpnpSettingsL();
    
    iSecurityManager = secMan;
    iDlnaFilter = CUpnpDlnaFilter::NewL( this, secMan );
    
    iElementdb = CUpnpElementFactory::NewL( aSettings.iObjectsXmlPath );
    iElementdb->GetContentDirectoryReference( this );

    SetStateVariableL( KSystemUpdateID( ), KZero( ) );
    SetStateVariableL( KContainerUpdateIDs( ), UpnpCD::KEmptyString( ) );
    SetStateVariableL( KTransferIDs( ), UpnpCD::KEmptyString( ) , EFalse );
    SetStateVariableL( KSortCapsVariableName( ), KSortCapabilities( ) );
    SetStateVariableL( KSearchCapsVariableName( ), KSearchCapabilities( ) );

    // file session 
    User::LeaveIfError( iFs.Connect( ) );

    iBitmapServerConnection = RFbsSession::Connect( );
    iDownloadDir = aSettings.iDownloadDir.AllocL( );
    iThumbDir = aSettings.iThumbnailDir.AllocL( );
    iContentDirectoryDb = CUpnpContentDirectoryDb::NewL( aMetadataStorage );
    iAutoDestroyObjects = CUpnpAutoDestroyObjectList::NewL( );
    iCreateThumbnails = aSettings.iCreateThumbnail;
    iMaxRequestCount = aSettings.iMaxRequestCount;
    iEventObserver = aSettings.iEventObserver;
    iStateHandler = CUpnpStateHandler::NewL( this );
    
    ConstructHttpL();
    }

void CUpnpContentDirectory::ConstructHttpL()
    {
    TInt iapId = CUpnpSettings::GetIapL();
    iHttpServerSession = CUpnpHttpServerSession::NewL( iapId, *this );
    iHttpServerSession->DefaultRuntime().SetCreator( *this );
    iHttpServerSession->StartL();
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpContentDirectory* CUpnpContentDirectory::NewL(
    TUpnpCdSettings aSettings, CUpnpMetadataStorage* aMetadataStorage,
    MUpnpTransferInterface* aTransfer )
    {
    CUpnpContentDirectory* cd = new (ELeave) CUpnpContentDirectory( *aSettings.iDevice,
            aSettings.iThmbObs,
            aTransfer
    );
    CleanupStack::PushL( cd );
    cd->ConstructL( aSettings, aMetadataStorage );
    CleanupStack::Pop( cd );
    return cd;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ActionReceivedLD
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::ActionReceivedLD( CUpnpAction* aAction )
    {
    CleanupStack::PushL( aAction );
    //check security
    if ( iSecurityManager &&
            KErrNone != iSecurityManager->AuthorizeAction( aAction ) )
        {
        //server returns upnp error action failed when not authorized
        User::Leave(EActionFailed);
        }
    // in case of low disk space
    if ( !IsDbOrDiskProblemL( aAction ) )
        {
        // handle event
        TInt ret = EUpnpUndefined;
        TRAPD( err, ret = DoActionReceivedL( aAction ) );
        if ( DataBaseHasBeenRecoveredL( err ) )
            {
            TRAP( err, ret = DoActionReceivedL( aAction ) );
            if ( err == KErrCorrupt )
                {
                err = iContentDirectoryDb->RecreateDatabaseFile();
                }
            }
        else if ( err == KErrNone )
            {
            err = ret;
            }
        LOGS( "ContentDirectory ActionReceivedLD sending soon." );
        DoSendActionL( aAction, TUpnpError( err ) );
        }
    CleanupStack::PopAndDestroy( aAction );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoActionReceivedL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::DoActionReceivedL( CUpnpAction* aAction )
    {
    // sendAndDestroy defines that after processing action must be 
    // sent and destroyed
    TUpnpErrorCode err = EUndefined;

    // Get the name, saves calls in the if-else statements
    TPtrC8 name( aAction->Name() );
    // Verify the action and do the operation(s)
    if ( name.Compare( KBrowse ) == 0 )
        {
        err = BrowseL( aAction );
        }
    else if ( name.Compare( KCreateReference ) == 0 )
        {
        err = CreateReferenceL( aAction );
        }
    else if ( name.Compare( KCreateObject ) == 0 )
        {
        LOGS( "ContentDirectory: Creating a new object" );
        err = CreateObjectL( aAction );
        }
    else if ( name.Compare( KDeleteResource ) == 0 )
        {
        err = DeleteResourceL( aAction );
        }
    else if ( name.Compare( KDestroyObject ) == 0 )
        {
        err = DestroyObjectL( aAction );
        }
    else if ( name.Compare( KExportResource ) == 0 )
        {
        err = ExportResourceL( aAction );
        }
    else if ( name.Compare( KGetSearchCapabilities ) == 0 )
        {
        err = GetSearchCapabilitiesL( aAction );
        }
    else if ( name.Compare( KGetSortCapabilities ) == 0 )
        {
        err = GetSortCapabilitiesL( aAction );
        }
    else if ( name.Compare( KGetSystemUpdateID ) == 0 )
        {
        err = GetSystemUpdateIdL( aAction );
        }
    else if ( name.Compare( KGetTransferProgress ) == 0 )
        {
        err = GetTransferProgressL( aAction );
        }
    else if ( name.Compare( KImportResource ) == 0 )
        {
        err = ImportResourceL( aAction );
        }
    else if ( name.Compare( KStopTransferResource ) == 0 )
        {
        err = StopTransferResourceL( aAction );
        }
    else
        {
        err = EInvalidAction;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoActionReceivedCDL
// -----------------------------------------------------------------------------
//    
void CUpnpContentDirectory::HttpGetStartedSoapNotifyL( TInt /*aSessionId*/)
    {
    // not needed upnp response returned by ActionReceivedCDL
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoActionReceivedCDL
// -----------------------------------------------------------------------------
//    
void CUpnpContentDirectory::HttpPostStartedL( TInt /*aSessionId*/)
    {
    // not needed upnp response returned by ActionReceivedCDL
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::UnknownHttpEventReceivedL 
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::UnknownHttpEventReceivedL( CUpnpHttpMessage& aHttpMsg )
    {
    CUpnpFileTransferEvent* transferEvent =
            iTransferController->TransferL( aHttpMsg.SessionId( ) );
    if ( !transferEvent )
        {
        return;
        }

    TInt err = aHttpMsg.Error( );
    if ( err != EHttpRequestTimeout && err != EHttpInsufficientStorage )
        {
        err = EHttpOk;
        }

    HBufC* fileName16 = UpnpString::ToUnicodeL( aHttpMsg.InFilename( ) );
    CleanupStack::PushL( fileName16 );
    transferEvent->SetFilePathL( *fileName16 );
    CleanupStack::PopAndDestroy( fileName16 );

    iTransferController->HttpResponseReceivedL( aHttpMsg.SessionId( ), err );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsDrmFileL 
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsDrmFileL( const TDesC8& aFileName )
    {
    HBufC* tmp = UpnpCdUtils::Des8ToDesLC( aFileName );
    TBool ret = IsDrmFileL( *tmp );
    CleanupStack::PopAndDestroy( tmp );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsDrmFileL 
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsDrmFileL( const TDesC& aFileName )
    {
    if ( !BaflUtils::FileExists( iFs, aFileName ) )
        {
        return EFalse;
        }
    TBool ret = EFalse;
    TInt value=0, err;

    ContentAccess::CManager *manager = ContentAccess::CManager::NewL( );
    CleanupStack::PushL( manager );

    ContentAccess::CVirtualPath *path =
            ContentAccess::CVirtualPath::NewL( aFileName );
    CleanupStack::PushL( path );

    ContentAccess::TVirtualPathPtr virtualPath = *path;

    err = manager->GetAttribute( ContentAccess::EIsProtected, value,
        virtualPath );
    if ( err == KErrNone )
        {
        if ( value )
            {
            // delete file
            iFs.Delete( aFileName );
            ret = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( 2, manager );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateReferenceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::CreateReferenceL( CUpnpAction*& aAction )
    {
    TUpnpErrorCode ret = EUpnpUndefined;

    // Get IDs from action 
    TInt conId( KErrNotFound );
    TInt objId( KErrNotFound );

    TInt err = UpnpCdUtils::StringToInteger(
        aAction->ArgumentValue( KContainerID ), &conId );
    if ( err!=KErrNone )
        {
        err=ENoContainer;
        User::Leave( ENoContainer );
        }

    err = UpnpCdUtils::StringToInteger(
        aAction->ArgumentValue( UpnpCD::KObjectID ), &objId );
    if ( err!=KErrNone )
        {
        err=ENoContainer;
        User::Leave( ENoSuchObject );
        }

    objId = iContentDirectoryDb->ReferedObjectIdL( objId );

    RXmlEngDocument conDidl;
    RXmlEngDocument objDidl;
    CleanupClosePushL( conDidl );
    CleanupClosePushL( objDidl );

    ret = iContentDirectoryDb->GetObjectL( conId, conDidl, KAsterisk8 );
    if ( ret != EUpnpOk )
        User::Leave( ENoContainer );

    ret = iContentDirectoryDb->GetObjectL( objId, objDidl, KAsterisk8 );
    if ( ret != EUpnpOk )
        User::Leave( ENoSuchObject );

    TXmlEngElement con = conDidl.DocumentElement().FirstChild().AsElement( );
    TXmlEngElement obj = objDidl.DocumentElement().FirstChild().AsElement( );

    if ( obj.Name().Compare( KItem ) != 0 )
        {
        User::Leave( ENoSuchObject );
        }
    if ( con.Name().Compare( KContainer ) != 0 )
        {
        User::Leave( ENoContainer );
        }
    if ( iContentDirectoryDb->CheckObjectRestrictionL( conId ) != EUpnpOk )
        {
        User::Leave( ERestrictedParentObject );
        }

    TInt refId;
    RXmlEngDocument refDidl = iContentDirectoryDb->PrepareDidlDocumentL( );
    CleanupClosePushL( refDidl );

    TXmlEngElement refEl = refDidl.DocumentElement().AddNewElementSameNsL( KItem( ) );

    refEl.AddNewAttributeL( KRefID( ),
        aAction->ArgumentValue( UpnpCD::KObjectID ) );

    refEl.AddNewAttributeL( KParentID( ),
        aAction->ArgumentValue( KContainerID ) );

    refEl.AddNewAttributeL( KRestricted( ), KFalseValue8( ) );

    // title
    TXmlEngElement objTitle;
    UpnpDomInterface::GetElementL( obj, objTitle, KObjTiltleColName8( ) );
    refEl.AppendChildL( objTitle.Unlink().AsElement( ) );

    // class
    TXmlEngElement objClass;
    UpnpDomInterface::GetElementL( obj, objClass, KObjClassColName8( ) );
    refEl.AppendChildL( objClass.Unlink().AsElement( ) );

    iContentDirectoryDb->InsertObjectL( refDidl, conId, &refId );

    //increase system update ID
    iStateHandler->IncreaseSystemUpdateIdL( );
    iStateHandler->IncreaseContainerUpdateIdL( (TDesC8&)aAction->ArgumentValue( KContainerID ) );

    TBuf8<KMaxIntegerLen> newid;
    newid.Num( refId );
    aAction->SetArgumentL( (TDesC8&)KNewID, newid );

    CleanupStack::PopAndDestroy( &refDidl );
    CleanupStack::PopAndDestroy( &objDidl );
    CleanupStack::PopAndDestroy( &conDidl );

    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::BrowseL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::BrowseL( CUpnpAction*& aAction )
    {
    RDebug::Print( _L( "CUpnpContentDirectory::BrowseL start" ) );
    TInt err( KErrGeneral );

    //Check ObjectID
    TInt objectID( KErrNotFound );
    err = UpnpCdUtils::StringToInteger(
        (TDesC8&)aAction->ArgumentValue( UpnpCD::KObjectID( ) ), &objectID );
    if ( err != KErrNone )
        {
        User::Leave( ENoSuchObject );
        }
    //Check SortCriteria
    if ( aAction->ArgumentValue( (TDesC8&) KSortCriteria ).Compare( (TDesC8&)KSortCapabilities )
            != 0 )
        {
        User::Leave( EInvalidSort ); //wrong sort flag
        }

    //Check BrowseFlag
    TDesC8& browseFlag =
            (TDesC8&)aAction->ArgumentValue( (TDesC8&)KBrowseFlag );
    if ( browseFlag.Compare( KBrowseMeta ) != 0
            && browseFlag.Compare( KBrowseChilds ) != 0 )
        {
        User::Leave( EInvalidArgs ); //wrong browse flag    
        }

    //Check StartIndex
    TInt startIndex( KErrNotFound );
    err = UpnpString::StringToInt(
        (TDesC8&)aAction->ArgumentValue( (TDesC8&)KStartingIndex ),
        &startIndex );
    if ( KErrNone != err )
        {
        User::Leave( EInvalidArgs );
        }

    //Additional check of StartIndex, see documentation
    if ( browseFlag.Compare( KBrowseMeta ) == 0 && startIndex != 0 )
        {
        User::Leave( EInvalidArgs );
        }

    //Check RequestedCount
    TInt reqCount( KErrNotFound );
    err = UpnpString::StringToInt(
        (TDesC8&)aAction->ArgumentValue( (TDesC8&)KRequestedCount ),
        &reqCount );
    if ( KErrNone != err )
        {
        User::Leave( EInvalidArgs );
        }

    //Filter
    HBufC8* filter = aAction->ArgumentValue( KFilter() ).AllocLC( );
    filter->Des().TrimAll( );

    // Metadata browse
    if ( browseFlag.Compare( KBrowseMeta ) == 0 )
        {

        RXmlEngDocument objectDoc;
        CleanupClosePushL( objectDoc );
        // Try to get original object not a reference 
        TInt id = iContentDirectoryDb->ReferedObjectIdL( objectID );
        CleanupStack::Check( &objectDoc );
        TUpnpErrorCode err = EUndefined;
        err = iContentDirectoryDb->GetObjectL( id, objectDoc, *filter );
        if ( err != EUpnpOk )
            User::Leave( err );
        CleanupStack::Check( &objectDoc );
        //--------- new solution for references --------

        // if id != objectID it means that we have a reference, so now id = 'real object id' and objectID = 'reference object id' 
        if ( id != objectID )
            {
            TInt refParentID = iContentDirectoryDb->GetParentIdL( objectID );
            User::LeaveIfError( refParentID );

            UpnpCdUtils::SetObjectIdL(
                UpnpCdUtils::GetObjectElementL( objectDoc ), objectID );
            UpnpCdUtils::SetObjectRefIdL(
                UpnpCdUtils::GetObjectElementL( objectDoc ), id );
            UpnpCdUtils::SetObjectParentIdL(
                UpnpCdUtils::GetObjectElementL( objectDoc ), refParentID );
            UpnpCdUtils::SetRestrictedFieldL(
                UpnpCdUtils::GetObjectElementL( objectDoc ), EFalse );
            }
        //---------------------------------------------

        // Serialize object to descriptor - without DIDL-Lite element
        RBuf8 buf;
        TXmlEngSerializationOptions
                options(TXmlEngSerializationOptions::KOptionOmitXMLDeclaration
                        | TXmlEngSerializationOptions::KOptionIncludeNsPrefixes);
        objectDoc.SaveL( buf, iElementdb->ActiveElementL( objectDoc ),
            options );
        CleanupStack::Check( &objectDoc );
        CleanupClosePushL( buf );
        HBufC8* subResponse = buf.AllocLC( );

        CBufFlat* response = CBufFlat::NewL( KRespBufGranularity );
        CleanupStack::PushL( response );
        UpnpString::AppendStringL( *response, KDidlTag );
        UpnpString::AppendStringL( *response, *subResponse );
        UpnpString::AppendStringL( *response, (TDesC8&) KDidlEnd( ) );

        HBufC8* resultTmp =
                UpnpCdUtils::EncodeXmlStringL( response->Ptr( 0 ) );
        CleanupStack::PushL( resultTmp );
        TPtr8 resultTmpPtr(resultTmp->Des( ));
        InsertRealIpAndPortL( resultTmpPtr );

        // Set action's OUT arguments       
        TBuf8<KMaxIntegerLen> num;

        //Result
        aAction->SetArgumentL( KResult, *resultTmp );

        //NumberReturned
        num.Num( 1 );
        aAction->SetArgumentL( KNumberReturned, num );

        //TotalMatches
        //num.Num(1);
        aAction->SetArgumentL( KTotalMatches, num );

        //UpadateID 
        num.Format(
            UpnpCD::KOneNumber,
            ContainerUpdateId( (TDesC8&)aAction->ArgumentValue( (TDesC8&)UpnpCD::KObjectID ) ) );
        aAction->SetArgumentL( KUpdateID, num );

        // clean up
        CleanupStack::PopAndDestroy( resultTmp );
        CleanupStack::PopAndDestroy( response );
        CleanupStack::PopAndDestroy( subResponse );
        CleanupStack::PopAndDestroy( &buf );
        CleanupStack::PopAndDestroy( &objectDoc );

        }
    //Children browse
    else if ( browseFlag.Compare( KBrowseChilds ) == 0 )
        {
        //array contains pointers to direct childrens.
        RArray<TInt> listOfAllObjects;
        CleanupClosePushL( listOfAllObjects );

        TUpnpErrorCode err = iContentDirectoryDb->GetObjectListL( objectID,
            listOfAllObjects );

        if ( err == ENoSuchObject )
            {
            User::Leave( err );
            }

        // if requestedCount==0 then return all matched objects
        if ( !reqCount )
            {
            reqCount = listOfAllObjects.Count( );
            }

        // if requestedCount cannot be greater than iMaxRequestCount
        if ( iMaxRequestCount > 0 && reqCount > iMaxRequestCount )
            {
            reqCount = iMaxRequestCount;
            }

        // get upnp objects
        RArray<RXmlEngDocument> objs;
        CleanupStack::PushL(TCleanupItem( DestroyRArray, &objs ));

        if ( startIndex < listOfAllObjects.Count( ) )
            { // prepare list of objects which are to be returned
            if ( startIndex + reqCount > listOfAllObjects.Count( ) )
                reqCount = listOfAllObjects.Count( ) - startIndex;
            RArray<TInt> alistOfWantedObjs( &listOfAllObjects[startIndex],
                reqCount);

            iContentDirectoryDb->GetObjectListL( alistOfWantedObjs, *filter,
                objs );
            }

        // prepare response
        // Result
        HBufC8* resp = SerializeObjectsListL( objs );
        CleanupStack::PushL( resp );
        TPtr8 respPtr(resp->Des( ));
        InsertRealIpAndPortL( respPtr );
        aAction->SetArgumentL( KResult, *resp );
        // NumberReturned
        TBuf8<KMaxIntegerLen> num;
        num.Num( objs.Count( ) );
        aAction->SetArgumentL( KNumberReturned, num );
        // TotalMatches
        num.Num( listOfAllObjects.Count( ) );
        aAction->SetArgumentL( KTotalMatches, num );
        // UpdateID
        num.Format(
            UpnpCD::KOneNumber,
            ContainerUpdateId( (TDesC8&)aAction->ArgumentValue( (TDesC8&)UpnpCD::KObjectID ) ) );
        aAction->SetArgumentL( KUpdateID, num );

        // clean up
        CleanupStack::PopAndDestroy( resp );
        CleanupStack::PopAndDestroy( &objs );
        CleanupStack::PopAndDestroy( &listOfAllObjects );
        }
    else //no valid browseflag
        {
        User::Leave( EInvalidArgs );
        }

    RDebug::Print( _L( "CUpnpContentDirectory::BrowseL Result got OK" ) );

    CleanupStack::PopAndDestroy( filter );

    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SerializeObjectsListL
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::SerializeObjectsListL(
    const RArray<RXmlEngDocument>& aObjs )
    {
    CBufFlat* buff = CBufFlat::NewL( KRespBufGranularity );
    CleanupStack::PushL( buff );

    // Didl start
    UpnpString::AppendStringL( *buff, KDidlTag );

    // content
    for ( TInt i = 0; i < aObjs.Count( ); i++ )
        {
        RBuf8 tmp;
        TXmlEngSerializationOptions
                options(TXmlEngSerializationOptions::KOptionOmitXMLDeclaration
                        | TXmlEngSerializationOptions::KOptionIncludeNsPrefixes);
        aObjs[i].SaveL( tmp, iElementdb->ActiveElementL( aObjs[i] ), options );
        CleanupClosePushL( tmp );
        UpnpString::AppendStringL( *buff, tmp );
        UpnpString::AppendStringL( *buff, KLF8 );
        CleanupStack::PopAndDestroy( &tmp );
        }

    // Didl end
    UpnpString::AppendStringL( *buff, KDidlEnd );

    // prepare to return
    HBufC8* ret = UpnpCdUtils::EncodeXmlStringL( buff->Ptr( 0 ) );

    // clean up
    CleanupStack::PopAndDestroy( buff );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DestroyObjectL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::DestroyObjectL( CUpnpAction*& aAction )
    {
    TInt objId( KErrNotFound );

    TInt error(0);
    error = UpnpCdUtils::StringToInteger(
        (TDesC8&)aAction->ArgumentValue( UpnpCD::KObjectID( ) ), &objId );
    if ( (error == KErrArgument)||(error == KErrOverflow) )
        {
        User::Leave( ENoSuchObject );
        }
    else if ( error<0 )
        {
        User::Leave( error );
        }
    // check object restriction
    TUpnpErrorCode err = iContentDirectoryDb->CheckObjectRestrictionL( objId ); // if returned ENoContainer it is OK because first of all the restriction is checked
    if ( err == ERestrictedObject || err == ENoSuchObject )
        {
        User::Leave( err );
        }

    // check parent restriction
    err = iContentDirectoryDb->CheckObjectRestrictionL( objId );
    if ( err == ERestrictedObject )
        {
        User::Leave( ERestrictedParentObject );
        }

    // root and download containers must not be deleted
    if ( objId <= KDownloadContId )
        {
        /*
         Designing a UPnP AV MediaServer (3.2.4): 
         "If a CDS implementation exposes a CDS object that is not restricted but cannot fulfill a
         CDS:DestroyObject(), CDS:CreateObject(), or CDS:UpdateObject() request, then the MediaServer should
         return error code 720 to indicate the request could not be processed."
         */
        User::Leave( ECannotProcess );
        }

    RArray<TInt> refList;
    CleanupClosePushL( refList );

    TBuf8<KMaxIntegerLen> containerID;
    containerID.Num( iContentDirectoryDb->GetParentIdL( objId ) );

    iContentDirectoryDb->DeleteObjectL( objId, refList, ETrue );

    iStateHandler->IncreaseSystemUpdateIdL( );
    iStateHandler->IncreaseContainerUpdateIdL( containerID );

    for ( TInt i(0); i<refList.Count( ); i++ )
        {
        if ( objId!=refList[i] )
            {
            containerID.Num( refList[i] );
            iStateHandler->IncreaseSystemUpdateIdL( );
            iStateHandler->IncreaseContainerUpdateIdL( containerID );
            }
        }
    // clean up
    CleanupStack::PopAndDestroy( &refList );
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddToMetaDbL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::AddToMetaDbL(
    RXmlEngDocument& aFragment, TInt aContainer, TInt* aId, TBool aIsLocal )
    {
    // res list
    RArray<TXmlEngElement> resList;
    CleanupClosePushL( resList );
    UpnpDomInterface::GetElementListL( aFragment.DocumentElement( ), resList,
        KRes );

    // a source file for thumbnail
    HBufC* thumbSrcFile = NULL;

    if ( aIsLocal )
        { // local action
        // create thumbnail if there is only one res in the object
        if ( resList.Count( ) == 1 )
            {
            // get resource file name
            TPtrC8 val(resList[0].Text( ));
            // ignore if empty string
            if ( val.Length( ) && (!UpnpCdUtils::IsWhiteString( val )) )
                {
                TUriParser8 up;
                User::LeaveIfError( up.Parse( val ) );
                if ( up.Extract( EUriScheme ) == UpnpHTTP::KSchemeFile8 )
                    {
                    thumbSrcFile = up.GetFileNameL( ); // thumbnail will be created at the end of this function
                    CleanupStack::PushL( thumbSrcFile );// and the pointer will be poped 
                    }
                }
            }
        TInt nextId = iContentDirectoryDb->GetObjectIdL( );
        // handle resources
        for ( TInt i = 0; i < resList.Count( ); i++ )
            {
            HandleLocalResourcesL( resList[i], nextId );
            }
        }

    // if only one empty res add importUri
    if ( resList.Count( ) )
        {
        TPtrC8 resValue(resList[0].Text( ));
        if ( !resValue.Length( ) || UpnpCdUtils::IsWhiteString( resValue ) )
            {
            AddImportUriToElL( resList[0] );
            }
        }
    if ( !resList.Count( ) )
        {
        AddNewResTagL( aFragment.DocumentElement( ) );
        }
    // add to database
    TUpnpErrorCode ret = iContentDirectoryDb->InsertObjectL( aFragment,
        aContainer, aId );
    if ( ret == EUpnpOk )
        {
        PrepareAutoDestroyObjectL( aFragment.DocumentElement( ), *aId );
        ret = EHttpOk;
        }

    if ( thumbSrcFile )
        {
        CreateThumbnailL( *thumbSrcFile, *aId );
        CleanupStack::PopAndDestroy( thumbSrcFile );
        }
    CleanupStack::PopAndDestroy( &resList );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateThumbnailForResourceL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::PrepareAutoDestroyObjectL(
    TXmlEngElement aObject, TInt aObjId )
    {
    // check res
    RArray<TXmlEngElement> resList;
    CleanupClosePushL( resList );
    UpnpDomInterface::GetElementListL( aObject, resList, KRes );
    if ( resList.Count( ) > 0 )
        {
        // only first res is important
        TPtrC8 resValue(resList[0].Text( ));
        if ( !resValue.Length( ) || UpnpCdUtils::IsWhiteString( resValue ) )
            {
            TXmlEngAttr impUri = resList[0].AttributeNodeL( KImportUri( ) );
            // auto destroy
            CUpnpAutoDestroyObject* ado = CUpnpAutoDestroyObject::NewLC(
                this, aObjId, UpnpCdUtils::ResIdFromUriL( impUri.Value( ) ) );
            iAutoDestroyObjects->AppendL( ado );
            CleanupStack::Pop( ado );
            }
        }

    // clean up
    CleanupStack::PopAndDestroy( &resList );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateThumbnailForResourceL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::CreateThumbnailL( const TDesC& aSrcFile,
    TInt aObjId )
    {
    if ( iCreateThumbnails && IsThumbnailAvailableL( aSrcFile ) )
        {
        // thumbnail name
        HBufC* thumbName = CreateNameForThumbnailL( aSrcFile );
        CleanupStack::PushL( thumbName );

        // create thumbnail creator
        CUpnpThumbnailCreator* thCreator = CreateThumbnailCreatorL( aObjId ); // do not push it onto CleanupStack
        // ThumbnailCreator is created and added to iThumbnailCreators
        // but the creation process is not yet started,
        // therefore, not leaving code should be performed until the thCreator->CreateL method is called
        if ( thCreator )
            {
            // start creation process
            TRAPD( err, thCreator->CreateL( aSrcFile, *thumbName ) );
            if ( err )
                {
                RDebug::Print( _L( "Error while creating thumbnail, it won't be created. Removing from creator list." ) );
                RemoveThCreatorAndObjectIdLD( thCreator );
                }
            iEcomUsed = ETrue;
            }
        // clean up
        CleanupStack::PopAndDestroy( thumbName );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsThumbnailAvailableL
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsThumbnailAvailableL( const TDesC& aFile )
    {
    HBufC8* tmp = HBufC8::NewLC( aFile.Length( ) );
    tmp->Des().Copy( aFile );
    TBool ret = IsThumbnailAvailableL( *tmp );
    CleanupStack::PopAndDestroy( tmp );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsThumbnailAvailableL
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsThumbnailAvailableL( const TDesC8& aFile )
    {
    TBool result = EFalse;

    // get mime type for file
    HBufC8* mt = GetMimeTypeLC( aFile );
    TPtr8 mimeType(mt->Des( ));
    mimeType.LowerCase( );

    if ( !mimeType.Compare( KMT_IMAGE_JPEG( ) ) )
        {
        result = ETrue;
        }
    else if ( !mimeType.Compare( KMT_IMAGE_PNG( ) ) )
        {
        result = ETrue;
        }
    else if ( !mimeType.Compare( KMT_IMAGE_GIF( ) ) )
        {
        result = ETrue;
        }
    else if ( !mimeType.Compare( KMT_IMAGE_BMP( ) ) )
        {
        result = ETrue;
        }
    CleanupStack::PopAndDestroy( mt );

    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddImportUriToElL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::AddImportUriToElL( TXmlEngElement aElement )
    {
    HBufC8* impUriVal = BuildImportUriShorterLC( );
    HBufC8* impUriValRandomize = HBufC8::NewLC( impUriVal->Des().Length( )+ KRandomRangeLength );
    HBufC8* number = UpnpCdUtils::RandomizeL( KRandomMax );
    CleanupStack::PushL( number );
    impUriValRandomize->Des().Copy( *impUriVal );
    impUriValRandomize->Des().Append( *number );
    aElement.AddNewAttributeL( KImportUri( ), *impUriValRandomize );
    // clean up
    CleanupStack::PopAndDestroy( number );
    CleanupStack::PopAndDestroy( impUriValRandomize );
    CleanupStack::PopAndDestroy( impUriVal );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddNewResTagL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::AddNewResTagL( TXmlEngElement aObjEl )
    {
    TXmlEngElement item;
    UpnpDomInterface::GetElementL( aObjEl, item, KItem );

    // ignore if null
    if ( item.NotNull( ) )
        {
        TXmlEngElement newEl = item.AddNewElementL( KRes( ) );
        newEl.AddNewAttributeL( KprotocolInfo( ), KEmptyProtocolInfoVal8( ) );
        newEl.AddNewAttributeL( KProtInfoReqAtrSufPattern( ), KTrueValue8( ) );
        // add importUri
        AddImportUriToElL( newEl );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::BuildContentUriL
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::BuildContentUriL( const TDesC& aFileExt,
    TInt aObjectId )
    {
    TBuf8<KMaxIntegerLen> objectId;
    objectId.Num( aObjectId );
    return UpnpCdUtils::BuildContentUriL(
        iContentDirectoryDb->GetKeyForUriL( ), aFileExt, objectId );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::BuildImportUriL
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::BuildImportUriLC()
    {
    return UpnpCdUtils::BuildImportUriLC( iContentDirectoryDb->GetKeyForUriL( ) );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::BuildImportUriShorterLC
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::BuildImportUriShorterLC()
    {
    return UpnpCdUtils::BuildImportUriShorterLC( iContentDirectoryDb->GetKeyForUriL( ) );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::InsertRealIpAndPortL
// -----------------------------------------------------------------------------
void CUpnpContentDirectory::InsertRealIpAndPortL( TDes8& aString )
    {
    // prepare pettern
    HBufC8* pattern = HBufC8::NewLC( KHttpTag().Length( ) + KIpPortPlaceholder8().Length( ) );
    TPtr8 patternPtr(pattern->Des( ));
    patternPtr.Append( KHttpTag );
    patternPtr.Append( KIpPortPlaceholder8 );

    // repare address
    HBufC8* address = GetIpAndPortDes8L( );
    CleanupStack::PushL( address );

    TInt pos = aString.Find( patternPtr );

    while ( pos != KErrNotFound )
        {
        aString.Replace( pos + KHttpTag().Length( ), KIpPortPlaceholder8().Length( ), *address );
        pos = aString.Find( patternPtr );
        }

    // clean up
    CleanupStack::PopAndDestroy( address );
    CleanupStack::PopAndDestroy( pattern );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::InsertIpAndPortPlaceholderL
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::InsertIpAndPortPlaceholderL(
    const TDesC8& aString )
    {
    HBufC8* ipAndPort = GetIpAndPortDes8L( );
    CleanupStack::PushL( ipAndPort );

    HBufC8* ret = aString.AllocLC( );

    TInt pos = KErrNotFound;
    while ( (pos = ret->Find( *ipAndPort )) != KErrNotFound )
        {
        // realloc if too small
        if ( ipAndPort->Length( ) < KIpPortPlaceholder8().Length( ) )
            {
            HBufC8* tmp = ret->ReAllocL( ret->Length( )
                    + KIpPortPlaceholder8().Length( ) - ipAndPort->Length( ) );
            CleanupStack::Pop( ret );
            ret = tmp;
            CleanupStack::PushL( ret );
            }
        ret->Des().Replace( pos, ipAndPort->Length( ), KIpPortPlaceholder8 );
        }
    // clean up
    CleanupStack::Pop( ret );
    CleanupStack::PopAndDestroy( ipAndPort );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetAddress
// -----------------------------------------------------------------------------
TInetAddr CUpnpContentDirectory::GetAddress()
    {
    TInetAddr resourceServerAddress;    
    iHttpServerSession->GetAddress( resourceServerAddress );
    return resourceServerAddress;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AutoDestroyObjectList
// -----------------------------------------------------------------------------
CUpnpAutoDestroyObjectList* CUpnpContentDirectory::GetAutoDestroyObjectList()
    {
    return iAutoDestroyObjects;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetIpAndPortDes8L
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::GetIpAndPortDes8L()
    {
    TInetAddr resourceServerAddress = GetAddress();        
    
    TBuf<KMaxName> addrTmp;
    resourceServerAddress.Output( addrTmp );
    HBufC8* addr = UpnpString::FromUnicodeL( addrTmp );
    CleanupStack::PushL( addr );
    
    TInt portNo = resourceServerAddress.Port();
    TBuf8<KMaxName> port;
    port.Num( portNo );
    
    HBufC8* temp = HBufC8::NewLC( addr->Length( ) + KColon8().Length( ) + port.Length( ) );
    TPtr8 tempPtr( temp->Des() );
    tempPtr.Copy( *addr );
    tempPtr.Append( KColon8 );
    tempPtr.Append( port );

    CleanupStack::Pop( temp );
    CleanupStack::PopAndDestroy( addr );

    return temp;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetSearchCapabilitiesL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::GetSearchCapabilitiesL(
    CUpnpAction*& aAction )
    {
    aAction->SetArgumentL( KSearchCaps,
        StateVariableValue( KSearchCapsVariableName ) );
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetSortCapabilitiesL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::GetSortCapabilitiesL(
    CUpnpAction*& aAction )
    {
    aAction->SetArgumentL( KSortCaps,
        StateVariableValue( KSortCapsVariableName ) );
    return EHttpOk;
    } 
 
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetSystemUpdateIdL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::GetSystemUpdateIdL(
    CUpnpAction*& aAction )
    {
    const TPtrC8 idValue = StateVariableValue( (TDesC8&)KSystemUpdateID );
    aAction->SetArgumentL( KId, idValue );
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ImportResourceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::ImportResourceL( CUpnpAction*& aAction )
    {
    if ( iTransferController->IsMaxImportExportTransfers( ) )
        {
        return ETransferBusy;
        }

    TPtrC8 src = aAction->ArgumentValue( (TDesC8&) KSourceURI );
    TPtrC8 dest = aAction->ArgumentValue( (TDesC8&) KDestinationURI );
    TUriParser8 srcParser;
    TUriParser8 dstParser;

    if ( !(dstParser.Parse( dest ) == 
          KErrNone && dstParser.Extract( EUriScheme ).CompareF( UpnpHTTP::KSchemeHTTP8 ) == 0 ) )
        {
        User::Leave( ENoDestinationResource );
        }

    // there are placeholders instead the real values in the database
    HBufC8* destBuf = InsertIpAndPortPlaceholderL( dest );
    CleanupStack::PushL( destBuf );

    TInt objId;
    // object existence
    if ( (objId
            = iContentDirectoryDb->GetObjIdByAttrL( KImportUri8, *destBuf ) )
            == KErrNotFound )
        {
        User::Leave( ENoDestinationResource );
        }
    // object restrictions
    if ( iContentDirectoryDb->CheckObjectRestrictionL( objId )
            == ERestrictedObject )
        {
        User::Leave( ERestrictedObject );
        }
    // parent restricions
    if ( iContentDirectoryDb->CheckParentRestrictionL( objId ) != EUpnpOk )
        {
        User::Leave( ERestrictedParentObject );
        }

    TInt destinationCut = dest.LocateReverse( '/' );
    if ( destinationCut < KErrNone )
        {
        User::Leave( EInvalidArgs );
        }

    HBufC* name = GetFileNameL( objId );
    CleanupStack::PushL( name );
    if ( !name )
        {
        User::Leave( ENoDestinationResource );
        }

    for ( TInt i( 0); i < iTransferController->iFileTransfers.Count( ) ; i++ )
        {
        if ( iTransferController->iFileTransfers[i]->ObjectId( ) == objId
                && iTransferController->iFileTransfers[i]->TransferState( )
                        == ETransferInProgress )
            {
            User::Leave( EDestinationAccess );
            }
        }

    HBufC8* name8 = UpnpString::FromUnicodeL( *name );
    CleanupStack::PushL( name8 );

    TPtr8 fileNamePtr(name8->Des( ) );

    HBufC16* fileNamePtr16 = UpnpString::ToUnicodeL( fileNamePtr );
    CleanupStack::PushL( fileNamePtr16 );
    TParse parseFilename;
    parseFilename.Set( fileNamePtr16->Des( ), NULL, NULL );
    HBufC16* diskNameTemp=parseFilename.Drive().Alloc( );
    HBufC16* diskName = diskNameTemp->ReAlloc( diskNameTemp->Des().Length( ) +
                                               KDiskPathElSep().Length( ) );
    CleanupStack::PushL( diskName );
    TPtr16 diskNamePtr = diskName->Des( );
    diskNamePtr.Append( KDiskPathElSep );
    if ( !BaflUtils::PathExists( iFs, diskNamePtr ) )
        {
        User::Leave( ENoDestinationResource );
        }

    UpnpString::ReplaceHttpCharacters( fileNamePtr );

    // forbidden chars
    UpnpCdUtils::ValidateFilePath( fileNamePtr );

    HBufC* targetPath = HBufC::NewMaxLC( fileNamePtr.Length( ) );
    TPtr targetPtr = targetPath->Des( );
    targetPtr.Copy( fileNamePtr );

    TInt transferid = CUpnpHttpMessage::NewSessionIdL( );
    iTransferHandler->DownloadFileL( transferid, src, targetPtr );

    CleanupStack::PopAndDestroy( targetPath );

    TransferStartL( dest );

    TBuf8<KMaxIntegerLen> tid;
    tid.Num( transferid );
    aAction->SetArgumentL( (TDesC8&)KTransferID, tid );

    CUpnpFileTransfer* transfer = CUpnpFileTransfer::NewLC( transferid,
        objId, CUpnpFileTransfer::EImport );

    transfer->SetTransferState(:: ETransferInProgress );

    // starting to get the number if import
    TPtrC8 dest1 = aAction->ArgumentValue( (TDesC8&) KDestinationURI );
    TInt posOfLastSlash = dest1.LocateReverse( '/' );
    if( posOfLastSlash != KErrNotFound )
        {
        dest1.Set( dest1.Mid( posOfLastSlash + 1 ) );
        // converting value
        TLex8 string( dest1 );
        TInt value( 0 );
        TInt error = string.Val( value );
        // conversion ok, we can add this to 
        if( error == KErrNone )
            {
            transfer->SetImportNumber( value );
            }
        }
    //add transfer to transfers list
    CleanupStack::Pop( transfer );
    iTransferController->iFileTransfers.Append( transfer );

    HBufC* fileName16 = UpnpString::ToUnicodeL( *name8 );
    CleanupStack::PushL( fileName16 );

    CUpnpFileTransferEvent* event = CUpnpFileTransferEvent::NewLC();
    event->SetProgressState( CUpnpFileTransferEvent::ECompleted );
    event->SetDirection( CUpnpFileTransferEvent::EIncoming );
    event->SetFilePathL( *fileName16 );
    event->SetContentDirectoryObjectId( objId );
    event->SetTransferId( transferid );
    CleanupStack::Pop( event );
    transfer->SetEvent( event );

    AddFileTransferToTransferIDsStateVariableL( transferid );
    // clean up
    CleanupStack::PopAndDestroy( fileName16 );
    CleanupStack::PopAndDestroy( diskName );
    CleanupStack::PopAndDestroy( fileNamePtr16 );
    CleanupStack::PopAndDestroy( name8 );
    CleanupStack::PopAndDestroy( name );
    CleanupStack::PopAndDestroy( destBuf );

    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ExportResourceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::ExportResourceL( CUpnpAction*& aAction )
    {
 /*   if ( iTransferController->IsMaxImportExportTransfers( ) )
        {
        return ETransferBusy;
        }
    HBufC8* srcBuf = aAction->ArgumentValue( KSourceURI ).AllocLC( );
    TPtr8 src(srcBuf->Des( ));
    UpnpString::ReplaceHttpCharacters( src );

    HBufC8* ipAndPort = GetIpAndPortDes8L( );
    CleanupStack::PushL( ipAndPort );
    UpnpCdUtils::ValidateSrcUriL( src, *ipAndPort );
    CleanupStack::PopAndDestroy( ipAndPort );

    TPtrC8 dest = aAction->ArgumentValue( KDestinationURI );

    // check src existence -----------------------------
    CUpnpResourcesBean* rscBean = NULL;
    TRAPD(
        error,
        rscBean
                = iContentDirectoryDb->GetResourceL( UpnpCdUtils::ResIdFromUriL( src ) ) );
    if ( error < 0 || !rscBean )
        {
        User::Leave( ENoSourceResource );
        }
    CleanupStack::PushL( rscBean );
    // check whether resource and src uri file names matches
    TPtrC rscPath(rscBean->Path( ) );

    if ( !BaflUtils::FileExists( iFs, rscPath ) )
        {
        User::Leave( ENoSourceResource );
        }

    TParse rscParse;
    rscParse.Set( rscPath, NULL, NULL );
    TPtrC rscFileName = rscParse.NameAndExt( );

    TBuf8<KMaxIntegerLen> objId;
    objId.Num( rscBean->ObjectId( ) );

    TPtrC rscFileExt = rscParse.Ext( );
    HBufC8* srcNoEsc = src.AllocLC( );
    TPtr8 srcNoEscPtr(srcNoEsc->Des( ));

    HBufC* srcPath = UpnpCdUtils::Des8ToDesLC( srcNoEscPtr );
    TPtr srcPathPtr(srcPath->Des( ) );

    TInt position = srcPathPtr.LocateReverse( '/' );
    HBufC* newFileName = HBufC::NewLC( KMaxIntegerLen + rscFileExt.Length( ) );
    newFileName->Des().Copy( objId );
    newFileName->Des().Append( rscFileExt );
    if ( position )
        {
        position = srcPathPtr.Length( )-position-1;
        TPtrC srcFilePtr = srcPathPtr.Right( position );
        if ( srcFilePtr.Compare( *newFileName ) )
            {
            User::Leave( ENoSourceResource );
            }
        }
    if ( srcPathPtr.Find( *newFileName ) == KErrNotFound )
        {
        User::Leave( ENoSourceResource );
        }
    // get mimeType = Content-Type in the header
    HBufC8* contentType = UpnpFileUtil::GetMimeTypeForFileL( *newFileName );
    TPtrC8 contentTypePtr(contentType->Des( ) );

    CleanupStack::PopAndDestroy( newFileName );
    CleanupStack::PushL( contentType );

    //checking number of slash in sourceURI 
    TInt numberOfSlash(0);
    position = srcPathPtr.LocateReverse( '/' );
    while ( position>0 )
        {
        numberOfSlash++;
        if ( numberOfSlash>KMaxSlashAmount )
            {
            User::Leave( ENoSourceResource );
            }
        srcPathPtr = srcPathPtr.Left( position );
        position = srcPathPtr.LocateReverse( '/' );
        }

    TInt transferid = CUpnpHttpMessage::NewSessionIdL( );
    iTransferHandler->UploadFileL( transferid, dest, rscPath, contentTypePtr );

    TBuf8<KMaxIntegerLen> tid;
    tid.Num( transferid );
    aAction->SetArgumentL( (TDesC8&)KTransferID, tid );

    TPtrC8 pert = UpnpCdUtils::ResIdFromUriDesL( src );

    TInt objectId = rscBean->ObjectId( );
    CUpnpFileTransfer* transfer = CUpnpFileTransfer::NewL( transferid,
        objectId, CUpnpFileTransfer::EExport );
    transfer->SetTransferState(:: ETransferInProgress);

    iTransferController->iFileTransfers.Append( transfer );

    CUpnpFileTransferEvent* event = CUpnpFileTransferEvent::NewLC();
    event->SetProgressState( CUpnpFileTransferEvent::ECompleted );
    event->SetDirection( CUpnpFileTransferEvent::EOutgoing );
    event->SetFilePathL( rscPath);
    event->SetContentDirectoryObjectId( objectId );
    event->SetTransferId( transferid );
    CleanupStack::Pop( event );
    transfer->SetEvent(event);

    AddFileTransferToTransferIDsStateVariableL( transferid );
    CleanupStack::PopAndDestroy( contentType );
    CleanupStack::PopAndDestroy( srcPath );
    CleanupStack::PopAndDestroy( srcNoEsc );
    CleanupStack::PopAndDestroy( rscBean );
    CleanupStack::PopAndDestroy( srcBuf );
*/
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::StopTransferResourceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::StopTransferResourceL(
    CUpnpAction*& aAction )
    {
    TInt transferId;
    TInt err =
            UpnpString::StringToInt(
                (TDesC8&) aAction->ArgumentValue( (TDesC8&)KTransferID ),
                &transferId );
    if ( KErrNone != err )
        {
        User::Leave( ENoFileTransfer );
        }
    TUpnpErrorCode
            upnpErr(iTransferController->StopTransferResourceL( transferId ) );
    TransferCompletedL( transferId, KErrCancel );
    return upnpErr;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetTransferProgressL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::GetTransferProgressL(
    CUpnpAction*& aAction )
    {
    TUint transferId;
    TInt length( KErrNone );
    TInt totalLength( KErrNone );
    // TInt lenght is max 15 digits, KMaxInfoName is 16
    TBuf8<KMaxInfoName> res;

    TInt err = UpnpCdUtils::StringToTUint(
        (TDesC8&)aAction->ArgumentValue( (TDesC8&)KTransferID ), &transferId );

    if ( KErrNone != err )
        {
        User::Leave( EInvalidArgs );
        }
    if ( transferId > KMaxTInt )
        {
        User::Leave( ENoFileTransfer );
        }
    TInt transferFound = iTransferController->Transfer( transferId );
    if ( KErrNotFound == transferFound )
        {
        User::Leave( ENoFileTransfer );
        }

    switch ( (iTransferController->iFileTransfers[ transferFound ])->TransferState( ) )
        {
        case:: ETransferInProgress:
            {
            aAction->SetArgumentL( (TDesC8&)KTransferStatus, KInProgress );
            break;
            }
        case EStopped:
            {
            aAction->SetArgumentL( (TDesC8&)KTransferStatus, KStopped );
            break;
            }
        case EError:
            {
            aAction->SetArgumentL( (TDesC8&)KTransferStatus, KError );
            break;
            }
        case ECompleted:
            {
            aAction->SetArgumentL( (TDesC8&)KTransferStatus, KCompleted );
            break;
            }
        default:
            {
            break;
            }
        }
    length = (iTransferController->iFileTransfers[ transferFound ])->TransferLength();
    totalLength = (iTransferController->iFileTransfers[ transferFound ])->TransferTotal();

    res.Num( length );
    aAction->SetArgumentL( (TDesC8&)KTransferLength, res );
    res.Num( totalLength );
    aAction->SetArgumentL( (TDesC8&)KTransferTotal, res );
    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DeleteResourceL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::DeleteResourceL( CUpnpAction*& aAction )
    {
    TPtrC8 uri8(aAction->ArgumentValue( KResourceURI( ) ));

    if ( !uri8.Length( ) )
        User::Leave( ENoSourceResource );

    // replace http characters
    HBufC8* uri = uri8.AllocLC( );
    TPtr8 uriPtr(uri->Des( ));

    // ip placeholder
    HBufC8* internalUri = InsertIpAndPortPlaceholderL( uriPtr );
    CleanupStack::PushL( internalUri );
    // delete resource
    RArray<TInt> contIds;
    CleanupClosePushL( contIds );
    iContentDirectoryDb->DeleteResourceL( *internalUri, contIds );

    // updateIDs
    for ( TInt i = 0; i < contIds.Count( ); i++ )
        {
        TBuf8<KMaxIntegerLen> num;
        num.Num( contIds[i] );
        iStateHandler->IncreaseContainerUpdateIdL( num );
        }

    // systemUpdateId
    iStateHandler->IncreaseSystemUpdateIdL( );

    // clean up
    CleanupStack::PopAndDestroy( &contIds );
    CleanupStack::PopAndDestroy( internalUri );
    CleanupStack::PopAndDestroy( uri );

    return EHttpOk;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddFileTransferL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::AddFileTransferToTransferIDsStateVariableL(
    TInt aTransferID )
    {
    TBuf8<KMaxIntegerLen> tid;
    tid.Format( UpnpCD::KOneNumber, aTransferID );
    TPtrC8 tmp = StateVariableValue( KTransferIDs( ) );
    HBufC8* buf = UpnpString::AddValueToCsvL( tmp, tid );
    CleanupStack::PushL( buf );
    SetStateVariableL( KTransferIDs( ), *buf, EFalse );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::RemoveFileTransferL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::RemoveFileTransferFromTransferIDsStateVariableL(
    TInt aTransferID )
    {
    TBuf8<KMaxIntegerLen> tid;
    tid.Format( UpnpCD::KOneNumber, aTransferID );
    TPtrC8 tmp = StateVariableValue( KTransferIDs( ) );
    HBufC8* buf = UpnpString::RemoveFromCsvLC( tmp, tid );
    SetStateVariableL( KTransferIDs( ), *buf, EFalse );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ContainerUpdateId
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::ContainerUpdateId( const TDesC8& aContainerID )
    {
    return iStateHandler->ContainerUpdateId( aContainerID );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateObjectL
// -----------------------------------------------------------------------------
//
TUpnpErrorCode CUpnpContentDirectory::CreateObjectL( CUpnpAction*& aAction )
    {
    SetExecutedAction( ECreateObjectAction );

    HBufC8* objNoDes(aAction->ArgumentValue(KContainerID()).AllocLC( ) );
    TPtr8 objNoPtr(objNoDes->Des( ));
    objNoPtr.TrimAll( );

    TInt objNo = KErrNotFound; // container id
    TInt error( KErrGeneral );

    // any container support
    if ( !objNoPtr.Compare( KAnyContainerIndicator8 ) )
        {
        objNo = KAnyContainerId;
        }
    else
        {
        // checking whether containerID has proper decimal value
        error = UpnpCdUtils::StringToInteger( objNoPtr, &objNo );
        if ( error != KErrNone )
            {
            User::Leave( ENoContainer );
            }

        TUpnpErrorCode err =
                iContentDirectoryDb->CheckObjectRestrictionL( objNo );
        if ( err != EUpnpOk )
            {
            CleanupStack::PopAndDestroy( objNoDes );
            if ( err == ENoSuchObject )
                err = ENoContainer;
            if ( err == ERestrictedObject )
                err = ERestrictedParentObject;
            return err;
            }
        }
    CleanupStack::PopAndDestroy( objNoDes );

    // now we have found the unrestricted parent container
    // let's inspect the element that should be added
    // decode "elements" argument of the create action
    const TDesC8& elem = (TDesC8&)aAction->ArgumentValue( KElements( ) );

    // check that we've got correctly formed xml
    if ( elem.FindF( KXmlNsDc( ) ) == KErrNotFound )
        {
        User::Leave( EBadMetadata );
        }

    HBufC8* trimmed = UpnpString::TrimLC( elem, EFalse );

    HBufC8* trimmedlf8 = UpnpString::StringReplaceL( *trimmed, KLF8,
        KNullString8 );
    CleanupStack::PushL( trimmedlf8 );
    // parse decoded buffer
    RXmlEngDocument fragment;
    TRAPD( xmlerr, fragment = XmlFragmentL( *trimmedlf8 ) );
    if ( xmlerr )
        {
        RDebug::Print( _L( "CUpnpContentDirectory::CreateObjectL ERROR IN XML" ) );
        User::Leave( xmlerr );
        }
    CleanupClosePushL( fragment );

    // if container, check the number of nested levels
    TXmlEngElement container;
    UpnpDomInterface::GetElementL( fragment.DocumentElement( ), container,
        KContainer );
    if ( container.NotNull( ) && !iContentDirectoryDb->CanBeNestedL( objNo ) )
        {
        User::Leave( ECannotProcess );
        }

    RArray<TXmlEngAttr> importUris;
    CleanupClosePushL( importUris );
    RArray<TXmlEngElement> ress;
    CleanupClosePushL( ress );

    TBool isLocal = aAction->Local( );

    // to avoid errors in validation process
    UpnpCdUtils::SetObjectIdL( UpnpCdUtils::GetObjectElementL( fragment ),
        objNo ); // value does not matter - it will be changed later
    UpnpCdUtils::SetContainerIdL( UpnpCdUtils::GetObjectElementL( fragment ),
        objNo );

    //created new object shouldn't have refID value
    if ( UpnpCdUtils::HasRefIdL( fragment ) )
        {
        User::Leave( EParameterMismatch );
        }

    // validate the new element
    iElementdb->ValidateNewObjectL( fragment.DocumentElement( ), ETrue,
        isLocal );

    // now we have fragment, lets add this to CD metadata
    TInt id( KErrNotFound );
    TUpnpErrorCode err = AddToMetaDbL( fragment, objNo, &id, isLocal );

    CleanupStack::PopAndDestroy( &ress );
    CleanupStack::PopAndDestroy( &importUris );

    ConstructCreateResultsetL( aAction, fragment, id );

    CleanupStack::PopAndDestroy( &fragment );
    CleanupStack::PopAndDestroy( trimmedlf8 );
    CleanupStack::PopAndDestroy( trimmed );

    if ( err == EHttpOk )
        {
        iStateHandler->IncreaseSystemUpdateIdL( );
        iStateHandler->IncreaseContainerUpdateIdL( 
                             ( TDesC8& )aAction->ArgumentValue( KContainerID ) );

        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::HandleLocalResourcesL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::HandleLocalResourcesL( TXmlEngElement aRes,
    TInt aObjectId )
    {
    // contentUri, contentPath          
    TUriParser8 up;
    TPtrC8 resVal = aRes.Text( );
    if ( resVal.Length( ) )
        {
        TPtrC8 resValCandidatePtr(resVal);

        // There should be no slash at the end of URI, if so -> leave occurs
        if ( resValCandidatePtr.Length( ) > 0 )
            {
            if ( !resValCandidatePtr.Right(1).Compare( KSlash8( ) ) )
                {
                User::Leave( EBadMetadata );
                }
            }

        User::LeaveIfError( up.Parse( resValCandidatePtr ) );

        // get schema
        TPtrC8 scheme(up.Extract( EUriScheme ));

        // only local sharing
        if ( !scheme.Compare( UpnpHTTP::KSchemeFile8 ) )
            {
            HBufC* fn = UpnpString::ToUnicodeL( resValCandidatePtr );
            CleanupStack::PushL( fn );

            TInt extPosition = fn->Des().LocateReverse( '.' );
            TPtrC ext;
            if ( extPosition>0 )
                ext.Set( fn->Des().Mid( extPosition ) );
            else
                ext.Set( KNullString );

            HBufC8* contentUri = BuildContentUriL( ext, aObjectId );
            CleanupStack::PushL( contentUri );

            HBufC* contentPath = up.GetFileNameL( );
            CleanupStack::PushL( contentPath );

            // update res tag
            aRes.SetTextL( *contentUri );

            // add resource to the database
            CUpnpResourcesBean* resBn = CUpnpResourcesBean::NewLC( );
            resBn->SetId( UpnpCdUtils::ResIdFromUriL( *contentUri ) );
            resBn->SetPathL( *contentPath );
            resBn->SetIsReadonly( ETrue );
            resBn->SetIsThumbnail( EFalse );

            iContentDirectoryDb->AddResourceL( resBn, aObjectId );

            // clean up
            CleanupStack::PopAndDestroy( resBn );
            CleanupStack::PopAndDestroy( contentPath );
            CleanupStack::PopAndDestroy( contentUri );
            CleanupStack::PopAndDestroy( fn );
            }
        // else -> ignore
        }
    // else -> ignore
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ConstructCreateResultsetL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::ConstructCreateResultsetL( CUpnpAction*& aAction,
    RXmlEngDocument aDocument, TInt aId )
    {
    TBuf8<KMaxIntegerLen> num;
    num.Num( aId );

    aAction->SetArgumentL( KObjectID, num );
    TXmlEngElement elem = aDocument.DocumentElement( );

    HBufC8* buf = CUpnpXmlContentFilter::SerializeLC( aDocument, elem );

    TPtr8 bufPtr(buf->Des( ));
    InsertRealIpAndPortL( bufPtr );
    HBufC8* encoded = UpnpCdUtils::EncodeXmlStringL( bufPtr );
    CleanupStack::PushL( encoded );

    aAction->SetArgumentL( KResult, *encoded );

    CleanupStack::PopAndDestroy( encoded );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::XmlFragmentL
// -----------------------------------------------------------------------------
//
RXmlEngDocument CUpnpContentDirectory::XmlFragmentL( const TDesC8 &aBuffer )
    {
    //Create a parser
    RXmlEngDOMParser parser;
    User::LeaveIfError( parser.Open( iDOMImpl ) );
    CleanupClosePushL( parser );
    // DOM document
    RXmlEngDocument docTree;

    if ( aBuffer.FindF( KXmlNsDc( ) ) == KErrNotFound )
        {
        HBufC8* withDidl = HBufC8::NewLC( KDidlTag().Length( ) + aBuffer.Length( ) +
                                          KDidlEnd().Length( ) );
        withDidl->Des().Append( KDidlTag( ) );
        withDidl->Des().Append( aBuffer );
        withDidl->Des().Append( KDidlEnd( ) );

        docTree = parser.ParseL( *withDidl );
        CleanupStack::PopAndDestroy( withDidl );
        }
    else
        {
        docTree = parser.ParseL( aBuffer );
        }
    CleanupStack::PopAndDestroy( &parser );
    return docTree;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetMimeTypeLC
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::GetMimeTypeLC( const TDesC8& aFilename )
    {
    HBufC16* filename16 = UpnpString::ToUnicodeL( aFilename );
    CleanupStack::PushL( filename16 );
    HBufC8* mimetype = UpnpFileUtil::GetMimeTypeForFileL( *filename16 );
    if ( !mimetype )
        {
        mimetype = KNullString8().AllocL( );
        }
    CleanupStack::PopAndDestroy( filename16 );
    CleanupStack::PushL( mimetype );
    return mimetype;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ChangeFileNameWithExtensionL
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::ChangeFileNameWithExtensionL(
    TDesC8& aFilePath, const TDesC8& aMimeType )
    {

    HBufC8* temp = NULL;
    TInt position = aFilePath.LocateReverse( '.' );
    if ( position != KErrNotFound )
        {
        return temp;
        }
    if ( aMimeType == KNullDesC8 )
        {
        return temp;
        }

    HBufC8* ext = iMimeToExtMap->GetExtensionL( aMimeType );
    if ( ext == NULL )
        {
        return temp;
        }
    CleanupStack::PushL( ext );
    HBufC8* newFilename = HBufC8::NewLC( ext->Length( ) + 1
            + aFilePath.Length( ) );
    TPtr8 fileNamePtr(newFilename->Des( ) );
    fileNamePtr.Append( aFilePath );
    //fileNamePtr.Append( KDot8());
    fileNamePtr.Append( *ext );

    HBufC* oldFilename16 = HBufC::NewLC( aFilePath.Length( ) );
    TPtr oldFilename16Ptr(oldFilename16->Des( ));
    oldFilename16Ptr.Copy( aFilePath );

    HBufC* newFilename16 = HBufC::NewLC( newFilename->Length( ) );
    TPtr newFilename16Ptr(newFilename16->Des( ));
    newFilename16Ptr.Copy( *newFilename );

    TBool exists = BaflUtils::FileExists( iFs, *newFilename16 );
    if ( exists )
        {
        TInt error( KErrNone );
        TInt count( 0);
        _LIT( KUnderline, "_" );

        CleanupStack::PopAndDestroy( newFilename16 );
        newFilename16 = NULL;

        HBufC16* ext16 = HBufC16::NewLC( ext->Length( ) );
        ext16->Des().Copy( *ext );

        TBuf<KMaxIntegerLen> value;
        newFilename16 = HBufC16::NewLC( aFilePath.Length( ) + KMaxIntegerLen
                + ext16->Length( ) + KUnderline().Length( ) + KDot().Length( ) );
        for ( ;; )
            {
            // name_xxxxx.ext
            value.Num( count );

            newFilename16->Des().Copy( aFilePath );
            newFilename16->Des().Append( KUnderline );
            newFilename16->Des().Append( value );
            //adding dot is not not needed anymore
            newFilename16->Des().Append( *ext16 );

            RFile file;
            error = file.Open( iFs, *newFilename16, EFileShareExclusive );
            file.Close( );

            if ( error == KErrNotFound )
                {
                break;
                }
            else
                {
                count++;
                }
            }
        CleanupStack::Pop( newFilename16 );
        CleanupStack::PopAndDestroy( ext16 );
        CleanupStack::PushL( newFilename16 );
        }

    TInt result = iFs.Rename( *oldFilename16, *newFilename16 );
    if ( result == KErrNone )
        {
        temp = HBufC8::NewL( newFilename16->Length( ) );
        temp->Des().Copy( *newFilename16 );

        }
    CleanupStack::PopAndDestroy( newFilename16 );
    CleanupStack::PopAndDestroy( oldFilename16 );
    CleanupStack::PopAndDestroy( newFilename );
    CleanupStack::PopAndDestroy( ext );

    return temp;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateResTagWithDBRecoveyrL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::CreateResTagInternalWithDBRecoveryL(
    TInt aSessionId, TDesC8& aFilePath, HBufC8* aImportUri,
    TBool aOnlyThumbnail )
    {
    TInt result = 0;
    TRAPD( err, result = CreateResTagInternalL( aSessionId, aFilePath,
        aImportUri, aOnlyThumbnail ) );
    if ( err )
        {
        if ( DataBaseHasBeenRecoveredL( err ) )
            {
            TRAP( err, iContentDirectoryDb->CheckDatabaseL( ) );
            if ( err == KErrCorrupt )
                {
                err = iContentDirectoryDb->RecreateDatabaseFile( );
                User::LeaveIfError( err );
                }
            TRAP( err, result = CreateResTagInternalL( aSessionId, aFilePath,
                aImportUri, aOnlyThumbnail ) );
            if ( err == KErrCorrupt )
                {
                err = iContentDirectoryDb->RecreateDatabaseFile( );
                }
            }
        User::LeaveIfError( err );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateResTagInternalL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::CreateResTagInternalL( TInt aSessionId,
    TDesC8& aFilePath, HBufC8* aImportUri, TBool aOnlyThumbnail )
    {
    TInt ret = KErrGeneral;
    HBufC8* number = UpnpCdUtils::RandomizeL( KRandomMax );
    CleanupStack::PushL( number );
    HBufC8* number2 = UpnpCdUtils::RandomizeL( KRandomMax );
    CleanupStack::PushL( number2 );
    HBufC8* importRandomize = HBufC8::NewLC( aImportUri->Des().Length( )+number->Des().Length( ) );
    importRandomize->Des().Copy( *aImportUri );
    importRandomize->Des().Replace( importRandomize->Length( ) - 4, 4, *number2 );
    importRandomize->Des().Append( *number );

    RXmlEngDocument obj;
    TInt objId = KErrNotFound;
    iContentDirectoryDb->GetObjectByAttrL( obj, &objId, KImportUri8,
        *aImportUri );
    CleanupClosePushL( obj );

    if ( !obj.NotNull( ) )
        {
        CleanupStack::PopAndDestroy( &obj );
        CleanupStack::PopAndDestroy( importRandomize );
        CleanupStack::PopAndDestroy( number2 );
        CleanupStack::PopAndDestroy( number );
        return ret;
        }

    HBufC8* mimeType = GetMimeTypeLC( aFilePath );
    // CheckFileName 
    HBufC8* newName = ChangeFileNameWithExtensionL( aFilePath, *mimeType );
    if ( !newName )
        {
        // renaming file failed, set the old  name
        newName = HBufC8::NewL( aFilePath.Length( ) );
        newName->Des().Copy( aFilePath );
        }
    CleanupStack::PushL( newName );

    TSize objectSize( 0, 0);

    //GetFileName
    HBufC16* filename16 = UpnpString::ToUnicodeL( *newName );
    CleanupStack::PushL( filename16 );

    if ( obj.NotNull( ) && !aOnlyThumbnail )
        {
        // if there is already a file related to this importUri delete it
        DeleteOldResourcesL( obj, *aImportUri, *filename16 );

        // set new contentUri
        TXmlEngElement objEl = obj.DocumentElement( );

        TPtrC8 fileNamePtr =
                newName->Mid( newName->LocateReverse( '\\' ) + 1 );

        TInt extPosition = fileNamePtr.LocateReverse( '.' );
        TPtrC8 ext;
        if ( extPosition>0 )
            ext.Set( fileNamePtr.Mid( extPosition ) );
        else
            ext.Set( KNullString8 );
        HBufC8* contUri = HBufC8::NewLC( importRandomize->Length( ) + KSlash().Length( ) +
                                         KMaxLongIntegerLen + ext.Length( ) );
        TPtr8 contUriPtr(contUri->Des( ));
        contUriPtr.Copy( *importRandomize );
        contUriPtr.Append( KSlash );

        TBuf8<KMaxLongIntegerLen> num;
        num.Num( objId );
        contUriPtr.Append( num );
        contUriPtr.Append( ext );

        TXmlEngElement resEl;
        UpnpDomInterface::GetDirectoryElementL( objEl, resEl, KRes( ),
            KImportUri8( ), aImportUri->Des( ) );
        resEl.SetTextL( contUriPtr );

        CleanupStack::PopAndDestroy( contUri );

        // set res@size  
        TXmlEngAttr size = resEl.AttributeNodeL( KSize( ) );
        if ( size.NotNull( ) )
            {
            SetResSizeL( size, *filename16 );
            }

        // set res@protocolInfo
        HBufC8* protocolInfo =
                ProtocolInfoFromMimeL( mimeType->Des( ), resEl );
        CleanupStack::PushL( protocolInfo );

        TXmlEngAttr protInf = resEl.AttributeNodeL( KprotocolInfo( ) );
        protInf.SetValueL( *protocolInfo );

        CleanupStack::PopAndDestroy( protocolInfo );

        CheckItemTypeForObjectL( obj, *mimeType );

        RemoveFileTransferFromTransferIDsStateVariableL( aSessionId );

        // update database
        iElementdb->ValidateNewObjectL( obj.DocumentElement( ), EFalse );
        iContentDirectoryDb->UpdateObjectL( objId, obj );

        // add resource
        CUpnpResourcesBean* resBn = CUpnpResourcesBean::NewLC( );
        resBn->SetId( UpnpCdUtils::ResIdFromUriL( *importRandomize ) );
        resBn->SetPathL( *newName );
        resBn->SetIsReadonly( EFalse );
        resBn->SetIsThumbnail( EFalse );
        iContentDirectoryDb->AddResourceL( resBn, objId );
        // clean up
        CleanupStack::PopAndDestroy( resBn );
        ret = KErrNone;
        }

    //send event to Media server      


    CUpnpFileTransferEvent* event =
            iTransferController->TransferL( aSessionId );

    if ( event )
        {
        event->SetFilePathL( *filename16 );
        //CleanupStack::Pop( event);
        //iEventObserver->FileTransferEvent( event );
        }

    // create thumbnail
    CreateThumbnailL( *filename16, objId );

    // clean up
    CleanupStack::PopAndDestroy( filename16 );
    CleanupStack::PopAndDestroy( newName );
    CleanupStack::PopAndDestroy( mimeType );
    CleanupStack::PopAndDestroy( &obj );
    CleanupStack::PopAndDestroy( importRandomize );
    CleanupStack::PopAndDestroy( number2 );
    CleanupStack::PopAndDestroy( number );

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SetResSizeL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::SetResSizeL( TXmlEngAttr aSize,
    const TDesC& aFilePath )
    {
    // get file size
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Open( iFs, aFilePath, EFileRead|EFileShareAny ) );
    TInt size;
    User::LeaveIfError( file.Size( size ) );

    // set size
    TBuf8<KMaxLongIntegerLen> num;
    num.Num( size );
    aSize.SetValueL( num );

    CleanupStack::PopAndDestroy( &file );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DeleteOldResourcesL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::DeleteOldResourcesL( RXmlEngDocument& aObj,
    const TDesC8& aImportUri, const TDesC& aNewFile )
    {
    // for main res tag delete only corresponding record in the resources table
    TXmlEngElement el;
    UpnpDomInterface::GetDirectoryElementL( aObj.DocumentElement( ), el,
        KImportUri8( ), aImportUri );
    if ( el.NotNull( ) ) //  should always be true
        {
        TPtrC8 resVal(UpnpDomInterface::GetElementValueL( el ));
        if ( resVal.Length( ) )
            {
            TInt64 resId = UpnpCdUtils::ResIdFromUriL( resVal ) ;
            // check src existence -----------------------------
            CUpnpResourcesBean* rscBean = NULL;
            rscBean = iContentDirectoryDb->GetResourceL( resId );
            TBool deletefile = ETrue;
            //checking if a new file is the same as the old one
            //if so, it means the old one was by chance deleted from the file system
            if ( rscBean )
                {
                TPtrC rscPath(rscBean->Path( ) );
                if ( !rscPath.Compare( aNewFile ) )
                    deletefile = EFalse;
                delete rscBean;
                }
            iContentDirectoryDb->DeleteResourceByResIdL( resId, deletefile );
            }

        }

    // destroy all other res tags   
    RArray<TXmlEngElement> elms;
    CleanupClosePushL( elms );
    UpnpDomInterface::GetElementListL( aObj.DocumentElement( ), elms, KRes );
    // for each element
    for ( TInt i=0; i < elms.Count( ); i++ )
        {
        TXmlEngAttr impUri(elms[i].AttributeNodeL( KImportUri( ) ) );
        if ( impUri.IsNull( ) && !el.IsSameNode( elms[i] ) )
            {
            TPtrC8 resVal(UpnpDomInterface::GetElementValueL( elms[i] ));
            if ( resVal.Length( ) )
                {
                iContentDirectoryDb->DeleteResourceByResIdL( 
                                                        UpnpCdUtils::ResIdFromUriL( resVal ) );
                }
            elms[i].Remove( );
            }
        }
    CleanupStack::PopAndDestroy( &elms );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateThumbnailCreatorL
// -----------------------------------------------------------------------------
//
CUpnpThumbnailCreator* CUpnpContentDirectory::CreateThumbnailCreatorL(
    TInt aObjectId )
    {
    CUpnpThumbnailCreator* thumbCreator = NULL;

    if ( iBitmapServerConnection == KErrNone )
        {
        //check if from the file THUMB can be created. 
        //create thumbnail 
        // in thumbnail save the information. 
        thumbCreator = CUpnpThumbnailCreator::NewLC( this );
        iThumbnailCreators.AppendL( thumbCreator );
        TInt index = iThumbnailCreators.Find( thumbCreator );
        User::LeaveIfError( index );
        iThObjectIds.InsertL( aObjectId, index );
        CleanupStack::Pop( thumbCreator );
        }
    return thumbCreator;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::RemoveThCreatorAndObjectIdLD
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::RemoveThCreatorAndObjectIdLD(
    CUpnpThumbnailCreator* aThCreator )
    {
    TInt index = iThumbnailCreators.Find( aThCreator );
    if ( index == KErrNotFound )
        {
        RDebug::Print( _L( "There is no corresponding thCreator in list. Error situation." ) );
        delete aThCreator;
        return;
        }
    else
        {
        iThumbnailCreators.Remove( index );
        }
    if ( index < iThObjectIds.Count( ) )
        {
        iThObjectIds.Remove( index );
        }
    delete aThCreator;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetObjectIdFromThCreator
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::GetObjectIdFromThCreator(
    CUpnpThumbnailCreator* aThCreator )
    {
    TInt result = KErrNotFound;
    TInt index = iThumbnailCreators.Find( aThCreator );
    RDebug::Print( _L( "Whole number of objects. %d" ) ,
        iThObjectIds.Count( ) );
    if ( index < iThObjectIds.Count( ) )
        {
        result = iThObjectIds[index];
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NotifyThumbnailResultL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::NotifyThumbnailResultL(
    CUpnpThumbnailCreator* aThCreator, TInt aResult )
    {
    TInt result = aResult;
    RDebug::Print( _L( "Thumbnail has been created with result: %d" ),
        aResult );
    if ( aResult == KErrNone )
        {
        RXmlEngDocument obj;
        CleanupClosePushL( obj );
        TInt objectId = GetObjectIdFromThCreator( aThCreator );
        TInt err =
                iContentDirectoryDb->GetObjectL( objectId, obj, KAsterisk8 );
        if ( err == EUpnpOk )
            {
            // get item element
            TXmlEngElement objElement;
            UpnpDomInterface::GetElementL( obj.DocumentElement( ),
                objElement, KItem );

            // add new res tag
            TXmlEngElement thumbRes(objElement.AddNewElementL( KRes( ) ) );

            // add protocolInfo
            HBufC8* protocolInfo = ProtocolInfoFromMimeL( KThumbMimeType( ),
                JPEG_TN, ETrue, ETrue );
            CleanupStack::PushL( protocolInfo );
            thumbRes.AddNewAttributeL( KprotocolInfo( ), *protocolInfo );
            CleanupStack::PopAndDestroy( protocolInfo );

            // add contentUri
            HBufC8* contUri = BuildContentUriL( aThCreator->GetFileExt( ),
                objectId );
            CleanupStack::PushL( contUri );
            thumbRes.SetTextL( *contUri );

            // add new resource
            CUpnpResourcesBean* resBean = CUpnpResourcesBean::NewLC( );
            resBean->SetId( UpnpCdUtils::ResIdFromUriL( *contUri ) );
            resBean->SetPathL( aThCreator->GetPath( ) );
            resBean->SetIsReadonly( EFalse );
            resBean->SetIsThumbnail( ETrue );
            iContentDirectoryDb->AddResourceL( resBean, objectId );

            // update
            iElementdb->ValidateNewObjectL( obj.DocumentElement( ), EFalse );
            iContentDirectoryDb->UpdateObjectL( objectId, obj );
            RemoveThCreatorAndObjectIdLD( aThCreator );

            // clean up
            CleanupStack::PopAndDestroy( resBean );
            CleanupStack::PopAndDestroy( contUri );
            }
        else
            {
            // delete thumbnail file if exists
            iFs.Delete( aThCreator->GetPath( ) ); // ignore errors
            // remove thCreator and objectID from list 
            RemoveThCreatorAndObjectIdLD( aThCreator );
            result = err;
            }
        CleanupStack::PopAndDestroy( &obj );
        }
    else
        {
        // delete thumbnail file if exists
        iFs.Delete( aThCreator->GetPath( ) ); // ignore errors
        // remove thCreator and objectID from list 
        RemoveThCreatorAndObjectIdLD( aThCreator );
        }

    // notify
    if ( iThmbObs )
        {
        iThmbObs->NotifyThumbnailResultL( 0, result );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateResTagL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::CreateResTagL( TInt aSessionId,
    TDesC8& aFilePath, TInt aImportNumber, TBool aOnlyThumbnail )
    {
    HBufC8* importUri = UpnpCdUtils::BuildImportUriLC( aImportNumber );
    TInt ret = CreateResTagInternalWithDBRecoveryL( aSessionId, aFilePath,
        importUri, aOnlyThumbnail );
    CleanupStack::PopAndDestroy( importUri );
    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ProtocolInfoFromMimeLC
// -----------------------------------------------------------------------------
//
CUpnpDlnaProtocolInfo* CUpnpContentDirectory::CreateProtocolInfoFromMimeL(
    const TDesC8& aMimeType, TInt aAddInfo )
    {
    CUpnpProtocolInfoLocal* protInfo = CUpnpProtocolInfoLocal::NewL(
        aMimeType, aAddInfo );
    CleanupStack::PushL( protInfo );
    protInfo->SetThirdFieldL( aMimeType );
    CleanupStack::Pop( protInfo );
    return protInfo;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ProtocolInfoFromMimeL
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::ProtocolInfoFromMimeL(
    const TDesC8& aMimeType, TInt aAddInfo, TBool aOpRangeParam,
    TBool aCiParam /*= EFalse*/)
    {
    CUpnpDlnaProtocolInfo* protInfo = CreateProtocolInfoFromMimeL( aMimeType,
        aAddInfo );
    if ( aCiParam )
        {
        protInfo->SetCiParameter( ETrue );
        }
    if ( aOpRangeParam )
        {
        protInfo->SetOpParameterL( B_VAL, ETrue );
        }

    CleanupStack::PushL( protInfo );
    TPtrC8 protocolInfo = protInfo->ProtocolInfoL( );
    HBufC8* protocolInfoDes = protocolInfo.Alloc( );
    CleanupStack::PopAndDestroy( protInfo );
    return protocolInfoDes;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ProtocolInfoFromMimeL
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::ProtocolInfoFromMimeL(
    const TDesC8& aMimeType, TXmlEngElement& aResEl )
    {
    HBufC8* result = NULL;
    const TDesC8& value = UpnpDomInterface::GetAttrValueL( aResEl,
        KprotocolInfo );
    if ( aMimeType.Length( ) > 0 )
        {
        if ( value.Length( ) )
            {
            CUpnpDlnaProtocolInfo* protocolInfo =
                    CUpnpDlnaProtocolInfo::NewL( (TDesC8&) value );
            CleanupStack::PushL( protocolInfo );
            TPtrC8 third = protocolInfo->ThirdField( );
            if ( !(aMimeType == third ) )
                {
                protocolInfo->SetThirdFieldL( (TDesC8&)aMimeType );
                protocolInfo->SetFourthFieldL( KAsterisk8( ) );
                }
            TPtrC8 protInfo = protocolInfo->ProtocolInfoL( );
            result = protInfo.Alloc( );
            CleanupStack::PopAndDestroy( protocolInfo );
            }
        }
    else
        {
        result = value.AllocL( );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetFileNameL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpContentDirectory::GetFileNameL( TInt aObjId )
    {
    TInt objId = aObjId;
    HBufC* result = NULL;
    RXmlEngDocument objDidl;
    iContentDirectoryDb->GetObjectL( objId, objDidl, KAsterisk8( ) );
    CleanupClosePushL( objDidl );
    TXmlEngElement obj = objDidl.DocumentElement().FirstChild().AsElement( );
    if ( obj.IsNull( ) )
        {
        CleanupStack::PopAndDestroy( &objDidl );
        return NULL;
        }

    // file name
    TXmlEngElement objTitle;
    UpnpDomInterface::GetElementL( obj, objTitle, KObjTiltleColName8( ) );
    if ( objTitle.IsNull( ) )
        {
        CleanupStack::PopAndDestroy( &objDidl );
        return NULL;
        }

    // mime type
    RArray<TXmlEngElement> reses;
    CleanupClosePushL( reses );
    UpnpDomInterface::GetElementListL( obj, reses, KRes );
    HBufC8* mimeType = NULL;
    for ( TInt i = 0; i < reses.Count( ); i++ )
        { // should be only one importUri
        TXmlEngAttr impUri;
        impUri = reses[i].AttributeNodeL( KImportUri );
        if ( impUri.NotNull( ) )
            {
            TXmlEngAttr prInfoAttr = reses[i].AttributeNodeL( KprotocolInfo );
            CUpnpDlnaProtocolInfo* prInfo =
                    CUpnpDlnaProtocolInfo::NewL( prInfoAttr.Value( ) );
            CleanupStack::PushL( prInfo );
            mimeType = prInfo->ThirdField().AllocL( );
            CleanupStack::PopAndDestroy( prInfo );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &reses );

    TPtrC ext;
    if ( mimeType )
        {
        TPtrC8 mimeTypeCut( *mimeType);
        TInt position = mimeType->Des().Find( KSemicolon8 );
        if ( position != KErrNotFound )
            {
            mimeTypeCut.Set( mimeType->Des().Left( position ) );
            }

        const HBufC* tmp = iMimeToExtMap->Get( mimeTypeCut );
        if ( tmp )
            {
            ext.Set( tmp->Ptr( ), tmp->Length( ) ); // do not delete
            }
        delete mimeType;
        }

    result = HBufC::NewLC( objTitle.Value().Length( ) + ext.Length( ) );
    TPtr resPtr(result->Des( ) );
    HBufC* title = UpnpCdUtils::Des8ToDesLC( objTitle.Value( ) );

    resPtr.Copy( *title );

    TInt wholeNameLength = title->Des().LocateReverse( '.' );
    TInt extenLength = title->Length( )-wholeNameLength;
    TPtrC exten;
    if ( extenLength )
        {
        exten.Set( title->Right( extenLength ) );
        }

    if ( ext.CompareF( exten ) != 0 )
        {
        resPtr.Append( ext );
        }

    // forbidden chars
    UpnpCdUtils::ValidateFilePath( resPtr );

    HBufC* basePath = HBufC::NewL( iDownloadDir->Des().Length( ) + result->Des().Length( ) );
    basePath -> Des().Append( iDownloadDir->Des( ) );
    basePath -> Des().Append( result->Des( ) );

    CleanupStack::PopAndDestroy( title );
    CleanupStack::PopAndDestroy( result );
    CleanupStack::PopAndDestroy( &objDidl );

    if ( BaflUtils::FileExists( iFs, *basePath ) )
        {
        CleanupStack::PushL( basePath );
        HBufC* fileName = CreateFileNameL( basePath->Des( ) );
        CleanupStack::PopAndDestroy( basePath );
        return fileName;
        }

    return basePath;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetFileNameWithoutPathL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpContentDirectory::GetFileNameWithoutPathL( TInt aObjId )
    {
    TInt objId = aObjId;
    HBufC* result = NULL;
    RXmlEngDocument objDidl;
    iContentDirectoryDb->GetObjectL( objId, objDidl, KAsterisk8( ) );
    CleanupClosePushL( objDidl );
    TXmlEngElement obj = objDidl.DocumentElement().FirstChild().AsElement( );
    if ( obj.IsNull( ) )
        {
        CleanupStack::PopAndDestroy( &objDidl );
        return NULL;
        }

    // file name
    TXmlEngElement objTitle;
    UpnpDomInterface::GetElementL( obj, objTitle, KObjTiltleColName8( ) );
    if ( objTitle.IsNull( ) )
        {
        CleanupStack::PopAndDestroy( &objDidl );
        return NULL;
        }

    // mime type
    RArray<TXmlEngElement> reses;
    CleanupClosePushL( reses );
    UpnpDomInterface::GetElementListL( obj, reses, KRes );
    HBufC8* mimeType = NULL;
    for ( TInt i = 0; i < reses.Count( ); i++ )
        { // should be only one importUri
        TXmlEngAttr impUri;
        impUri = reses[i].AttributeNodeL( KImportUri );
        if ( impUri.NotNull( ) )
            {
            TXmlEngAttr prInfoAttr = reses[i].AttributeNodeL( KprotocolInfo );
            CUpnpDlnaProtocolInfo* prInfo =
                    CUpnpDlnaProtocolInfo::NewL( prInfoAttr.Value( ) );
            CleanupStack::PushL( prInfo );
            mimeType = prInfo->ThirdField().AllocL( );
            CleanupStack::PopAndDestroy( prInfo );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &reses );

    TPtrC ext;
    if ( mimeType )
        {
        TPtrC8 mimeTypeCut(*mimeType);
        TInt position = mimeType->Des().Find( KSemicolon8 );
        if ( position != KErrNotFound )
            {
            mimeTypeCut.Set( mimeType->Des().Left( position ) );
            }

        const HBufC* tmp = iMimeToExtMap->Get( mimeTypeCut );
        if ( tmp )
            {
            ext.Set( tmp->Ptr( ), tmp->Length( ) ); // do not delete
            }
        delete mimeType;
        }

    result = HBufC::NewLC( objTitle.Value().Length( ) + ext.Length( ) );
    TPtr resPtr(result->Des( ));
    HBufC* title = UpnpCdUtils::Des8ToDesLC( objTitle.Value( ) );

    resPtr.Copy( *title );

    TInt wholeNameLength = title->Des().LocateReverse( '.' );
    TInt extenLength = title->Length( )-wholeNameLength;
    TPtrC exten;
    if ( extenLength )
        {
        exten.Set( title->Right( extenLength ) );
        }

    if ( ext.CompareF( exten )!=0 )
        resPtr.Append( ext );

    // forbidden chars
    UpnpCdUtils::ValidateFilePath( resPtr );

    // clean up
    CleanupStack::PopAndDestroy( title );
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( &objDidl );
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateFileNameL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpContentDirectory::CreateFileNameL( const TDesC16& aFileName )
    {
    const TInt KMaxSameFileNames = 10000;
    TParse parseFilename;
    parseFilename.Set( aFileName, NULL, NULL );

    // orginal file + underscore + number 
    HBufC* name = HBufC::NewLC( aFileName.Length( ) + KMaxIntegerLen );
    TPtr namePtr(name->Des( ) );

    for ( TInt count = 1; count < KMaxSameFileNames; count++ )
        {
        TBuf<KMaxIntegerLen> value;
        value.Num( count );

        namePtr.Append( parseFilename.DriveAndPath( ) );
        namePtr.Append( parseFilename.Name( ) );
        namePtr.Append( KUndersc( ) );
        namePtr.Append( value );
        namePtr.Append( parseFilename.Ext( ) );

        if ( !BaflUtils::FileExists( iFs, namePtr ) )
            {
            break;
            }
        namePtr.Zero( );
        }

    CleanupStack::Pop( name );
    return name;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetTitleForUriL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::GetTitleForUriL( TInt aObjectId, TPtr& aValue )
    {
    HBufC* result = GetFileNameWithoutPathL( aObjectId );
    TPtr des = result->Des( );
    UpnpCdUtils::ValidateFilePath( des );
    if ( result == NULL )
        {
        return;
        }
    aValue.Copy( *result );
    delete result;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CheckImportUriL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::CheckImportUriL( const TDesC8& aImportUri )
    {
    TInt objId = KErrNotFound;

    // http characters  
    HBufC8* dest8 = HBufC8::NewLC( aImportUri.Length( ) );
    TPtr8 dest8Ptr(dest8->Des( ));
    dest8Ptr.Copy( aImportUri );

    UpnpString::ReplaceHttpCharacters( dest8Ptr );

    // ip and port placeholder
    HBufC8* dest = InsertIpAndPortPlaceholderL( dest8Ptr );
    CleanupStack::PushL( dest );

    objId = iContentDirectoryDb->GetObjIdByAttrLikeL( KImportUri8, *dest );

    // clean up
    CleanupStack::PopAndDestroy( dest );
    CleanupStack::PopAndDestroy( dest8 );

    return objId;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateAddressValueLC
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::CreateAddressValueLC()
    {

    HBufC8* result = NULL;

    CBufFlat* resBuf = CBufFlat::NewL( KMaxName );
    CleanupStack::PushL( resBuf );

    UpnpString::AppendStringL( *resBuf, KHttpTag( ) );
    UpnpString::AppendStringL( *resBuf, KIpPortPlaceholder8( ) );

    result = HBufC8::NewL( resBuf->Size( ) );
    TPtr8 resultPtr(result->Des( ));
    resultPtr.Copy( resBuf->Ptr( 0 ) );
    CleanupStack::PopAndDestroy( resBuf );
    CleanupStack::PushL( result );
    return result;
    // path 
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetImportUriLC
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpContentDirectory::GetImportUriLC( CUpnpHttpMessage* aMessage )
    {
    TPtrC8 path = aMessage->SenderPathFromHeader( );
    if ( path.Find( KHttpTag ) == KErrNotFound )
        {
        HBufC8* serverAddr = CreateAddressValueLC( );
        HBufC8* result =
                HBufC8::NewL( serverAddr->Length( ) + path.Length( ) );
        TPtr8 resultPtr(result->Des( ) );
        resultPtr.Append( *serverAddr );
        resultPtr.Append( path );
        CleanupStack::PopAndDestroy( serverAddr );
        CleanupStack::PushL( result );
        return result;
        }
    else
        {
        HBufC8* pathHBuf = InsertIpAndPortPlaceholderL( path );
        CleanupStack::PushL( pathHBuf );
        return pathHBuf;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetProtocolInfoByImportUriL
// -----------------------------------------------------------------------------
//
CUpnpDlnaProtocolInfo* CUpnpContentDirectory::GetProtocolInfoByImportUriL(
    const TDesC8& aImportUri )
    {
    TPtrC8 importUri;
    importUri.Set( UpdateImportUriL( aImportUri ) );

    CUpnpDlnaProtocolInfo* protocolInfo = NULL;
    // check restriction - cannot export to the restricted object
    TInt objId = iContentDirectoryDb->
    GetObjIdByAttrLikeL( KImportUri8, importUri );

    if ( objId < 0 )
        {
        User::Leave( ENoSuchObject );
        }

    if ( iContentDirectoryDb->
    CheckObjectRestrictionL( objId ) == ERestrictedObject )
        {
        User::Leave( ERestrictedObject );
        }
    // parent restricions
    if ( iContentDirectoryDb->CheckParentRestrictionL( objId ) != EUpnpOk )
        {
        User::Leave( ERestrictedParentObject );
        }
    else
        {
        HBufC8* protInfoBuf = iContentDirectoryDb->
        GetProtocolInfoByImportUriL( importUri );

        if ( protInfoBuf )
            {
            CleanupStack::PushL( protInfoBuf );
            protocolInfo = CUpnpDlnaProtocolInfo::NewL( protInfoBuf->Des( ) );
            CleanupStack::PopAndDestroy( protInfoBuf );
            }
        }
    return protocolInfo;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::UpdateImportUriL
// -----------------------------------------------------------------------------
//
TPtrC8 CUpnpContentDirectory::UpdateImportUriL( const TDesC8& aImportUri )
    {
    HBufC8* portAndIp =  GetIpAndPortDes8L();
    CleanupStack::PushL( portAndIp );

    TPtrC8 importUri;
    importUri.Set( aImportUri );
    
    TInt index = aImportUri.Find( *portAndIp );
    if ( index > 0 )
        {
        importUri.Set( aImportUri.Mid( index + portAndIp->Des().Length() ) );
        }
                
    CleanupStack::PopAndDestroy( portAndIp );
    
    //--- replaceing e.g '/0000000004' -> '0000000004'   
    index = importUri.Find( KSlash8() );
	// importUri must begins with '/'
    if ( !index )
        {
        importUri.Set( importUri.Mid( index + 1 ) );        
        }
     else
        {
        User::Leave( ENoSuchObject );         
        }   
    //----------------------------------------
    
    if ( importUri.Length() < KMaxIntegerLen )
        {
        User::Leave(ENoSuchObject);        
        }
    
    // Check if all signs in uri are digits
    TLex8 lex( importUri );
    for( TInt i( 0 ); i < importUri.Length(); i++ )
        {
        if ( !( lex.Get() ).IsDigit() ) 
            {
            User::Leave( ENoSuchObject ); 
            }
        }    
    return importUri; 
    }
    
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetProtocolInfoL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::GetProtocolInfoL( const TDesC8& aContentUri,
    CUpnpDlnaProtocolInfo*& aProtocolInfo )
    {
    HBufC8 *buf = InsertIpAndPortPlaceholderL( aContentUri );
    CleanupStack::PushL( buf );
    TInt err = iContentDirectoryDb->GetProtocolInfoL( buf->Des( ),
        aProtocolInfo );
    CleanupStack::PopAndDestroy( buf );
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::FindSharedFolderL
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::FindSharedFolderL( const TDesC& aUrlPath,
    const TDesC& aFileName, HBufC*& aSystemPath )
    {
    TInt ret = KErrNotFound;
    aSystemPath = NULL;
    if ( aUrlPath.Length( ) )
        {
        // validate the path string
        if ( IsValidUrlPath( aUrlPath ) )
            {
            // get resource id
            TLex lexer(aUrlPath);
            TInt64 resId;
            if ( !lexer.Val( resId ) )
                {
                // get resource
                CUpnpResourcesBean* resBn = iContentDirectoryDb->
                GetResourceL( resId );
                if ( resBn )
                    {
                    CleanupStack::PushL( resBn );
                    aSystemPath = resBn->Path().AllocL( );
                    TInt objectId = resBn->ObjectId( );
                    CleanupStack::PopAndDestroy( resBn );

                    // make sure that systam path points to the wanted file
                    TParse fp;
                    fp.Set( *aSystemPath, 0, 0 );
                    HBufC* tmp = aFileName.AllocLC( );
                    TPtr tmpPtr(tmp->Des( ));
                    UpnpString::ReplaceHttpCharactersL( tmpPtr );
                    TPtrC ext = fp.Ext( );
                    HBufC* file = HBufC::NewLC( KMaxIntegerLen
                            + ext.Length( ) );
                    TBuf<KMaxIntegerLen> num;
                    num.Num( objectId );
                    file->Des().Append( num );
                    file->Des().Append( ext );

                    if ( file->Des( ) == tmpPtr )
                        {
                        ret = KErrNone;
                        }
                    CleanupStack::PopAndDestroy( file );
                    CleanupStack::PopAndDestroy( tmp );
                    }
                }
            }
        }
    else
        { // default download dir
        aSystemPath = iDownloadDir->AllocL( );
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsValidUrlPathL
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsValidUrlPath( const TDesC& aUrlPath )
    {
    TInt ret = ETrue;
    if ( (aUrlPath.Length( ) != KMaxIntegerLen + KRandomRangeLength)
            && (aUrlPath.Length( ) != KMaxIntegerLen + KRandomRangeLength
                    + KRandomRangeLength) )
        {
        ret = EFalse;
        }
    else
        {
        for ( TInt i = 0; i < aUrlPath.Length( ); i++ )
            if ( aUrlPath[i] < '0' || aUrlPath[i] > '9' )
                {
                ret = EFalse;
                break;
                }
        }

    return ret;
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetItemTypeFromMimeTypeLC
// -----------------------------------------------------------------------------
HBufC8* CUpnpContentDirectory::GetItemTypeFromMimeTypeLC(
    const TDesC8& aMimeType )
    {
    HBufC8* result = NULL;
    if ( aMimeType.Compare( KNullDesC8( ) ) == 0 )
        {
        return result;
        }
    TInt indexOfSlash = aMimeType.Find( KSlash8( ) );
    if ( indexOfSlash < KErrNone )
        {
        return result;
        }
    TPtrC8 firstValue = aMimeType.Left( indexOfSlash );
    if ( firstValue.Compare( KImageMime( ) ) == 0 )
        {
        result = KObjectClassImageItem().AllocLC( );
        }
    else if ( firstValue.Compare( KTextMime( ) ) == 0 )
        {
        result = KObjectClassTextItem().AllocLC( );
        }
    else if ( firstValue.Compare( KVideoMime( ) ) == 0 )
        {
        result = KObjectClassVideoItem().AllocLC( );
        }
    else if ( firstValue.Compare( KAudioMime( ) ) == 0 )
        {
        result = KObjectClassAudioItem().AllocLC( );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::GetItemTypeFromMimeTypeLC
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::CheckItemTypeForObjectL( RXmlEngDocument& aObj,
    const TDesC8& aMimeType )
    {
    //  temporary disable because of mime-type recognition problem 
    HBufC8* itemType = GetItemTypeFromMimeTypeLC( aMimeType );
    if ( !itemType )
        {
        return;
        }
    TXmlEngElement obj = aObj.DocumentElement( );
    TXmlEngElement objClass;
    UpnpDomInterface::GetElementL( obj, objClass, KObjClassColName8 );
    TPtrC8 classValue(UpnpDomInterface::GetElementValueL( objClass ));
    if ( classValue.Compare( itemType->Des( ) ) < 0 )
        {
        objClass.SetTextL( *itemType );
        }
    CleanupStack::PopAndDestroy( itemType );

    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SetDownloadDirectoryL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::SetDownloadDirectoryL( const TDesC& aDownloadDir )
    {
    // perform necessary changes in database
    iContentDirectoryDb->NewMediaDirectoryL( *iDownloadDir );

    // alloc new string
    delete iDownloadDir;
    iDownloadDir = NULL;
    TPtrC last = aDownloadDir.Right( 1 );
    if ( last.Compare( KDiskPathElSep )!=0 )
        {
        iDownloadDir = HBufC::NewL( aDownloadDir.Length( )+ KDiskPathElSep().Length( ) );
        iDownloadDir->Des().Copy( aDownloadDir );
        iDownloadDir->Des().Append( KDiskPathElSep );
        }
    else
        {
        iDownloadDir = HBufC::NewL( aDownloadDir.Length( ) );
        iDownloadDir->Des().Copy( aDownloadDir );
        }
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SetThumbnailDirectoryL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::SetThumbnailDirectoryL( const TDesC& aDownloadDir )
    {
    // perform necessary changes in database
    iContentDirectoryDb->NewMediaDirectoryL( *iThumbDir );

    // alloc new string
    delete iThumbDir;
    iThumbDir = NULL;
    iThumbDir = aDownloadDir.AllocL( );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::HandleDbError
// -----------------------------------------------------------------------------
//
TInt CUpnpContentDirectory::HandleDbError( TInt aError )
    {
    return iContentDirectoryDb->HandleDbError( aError );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DataBaseHasBeenRecoveredL
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::DataBaseHasBeenRecoveredL( TInt aError )
    {
    TInt err = aError;
    if ( err == KErrCorrupt )
        {
        err = HandleDbError( err );
        }
    return (err == KErrNone && aError == KErrCorrupt ) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsDbOrDiskProblemL
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsDbOrDiskProblemL( CUpnpAction* aAction )
    {
    TBool noSpace = SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
        KMinDiskSpace, EDriveC );
    TInt error( KErrNone );
    if ( !noSpace )
        {
        if ( !IsDbCreated() )
            {
            error = HandleDbError( KErrCorrupt );
            }
        }
    if ( noSpace || error != KErrNone )
        {
        DoSendActionL( aAction, ECannotProcess );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsDbCreated
// -----------------------------------------------------------------------------
//
TBool CUpnpContentDirectory::IsDbCreated()
    {
    return iContentDirectoryDb->IsDbCreated( );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::CreateNameForThumbnailL
// -----------------------------------------------------------------------------
//
HBufC* CUpnpContentDirectory::CreateNameForThumbnailL( const TDesC& aFileName )
    {
    // only name
    TParse fp;
    User::LeaveIfError( fp.Set( aFileName, 0, 0 ) );
    TFileName bareName = fp.Name( );
    // full name
    User::LeaveIfError( fp.Set( bareName, iThumbDir, &KDefaultThumbExtension ) );
    TFileName fullName = fp.FullName( );

    //check if exists
    TUint ignored;
    TInt q = 0;
    while ( iFs.Att( fullName, ignored ) == KErrNone )
        {
        q++;
        TBuf<KMaxIntegerLen> num;
        num.Num( q );
        TFileName tmpName;
        tmpName.Copy( bareName );
        tmpName.Append( KUndersc );
        tmpName.Append( num );
        fp.Set( tmpName, iThumbDir, &KDefaultThumbExtension );
        fullName = fp.FullName( );
        }

    return fullName.AllocL( );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AutoDestroyEventL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::AutoDestroyEventL( TInt aObjId, TBool aDeleteObj )
    {
    if ( !ValidateObjectL( aObjId ) )
        {
        return;
        }

    if ( aDeleteObj )
        {
        TBuf8<KMaxIntegerLen> containerID;
        containerID.Num( iContentDirectoryDb->GetParentIdL( aObjId ) );
        RArray<TInt> refList;
        CleanupClosePushL( refList );
        iContentDirectoryDb->DeleteObjectL( aObjId, refList, ETrue );

        // supporting ContainerUpdateIDs behaviour
        iStateHandler->IncreaseSystemUpdateIdL( );
        iStateHandler->IncreaseContainerUpdateIdL( containerID );
        for ( TInt i(0); i<refList.Count( ); i++ )
            {
            iStateHandler->IncreaseSystemUpdateIdL( );
            containerID.Num( refList[i] );
            iStateHandler->IncreaseContainerUpdateIdL( containerID );
            }

        // clean up
        CleanupStack::PopAndDestroy( &refList );
        }
    iAutoDestroyObjects->RemoveAndDestroyL( iAutoDestroyObjects->FindObjIndexByObjId( aObjId ) );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::HttpTransferEventReceivedL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::HttpTransferEventReceivedL( CUpnpHttpMessage& aHttpMsg )
    {
    switch ( aHttpMsg.Type( ) )
        {
        case ETransferStart:
            {
            TransferStartL( aHttpMsg.SenderPathFromHeader( ) );

            CUpnpFileTransfer::TTransferType type;
            TInt objectid(0);

            if ( aHttpMsg.Method().Find( KHttpGet ) == 0 )
                {
                objectid = iContentDirectoryDb->GetObjIdByAttrL( KImportUri8,
                    aHttpMsg.SenderPathFromHeader( ) );
                type = CUpnpFileTransfer::EImport;
                }
            else
                {
                objectid = iContentDirectoryDb->GetObjIdByAttrLikeL(
                    KImportUri8, aHttpMsg.SenderPathFromHeader( ) );
                type = CUpnpFileTransfer::EUpload;

                }
            CUpnpFileTransfer* transfer = CUpnpFileTransfer::NewLC(
                aHttpMsg.SessionId( ), objectid, type );

            transfer->SetTransferState(:: ETransferInProgress);
            transfer->SetImportNumber(UpnpCdUtils::ResIdFromUriL(
                    aHttpMsg.SenderPathFromHeader()));

            iTransferController->iFileTransfers.Append(transfer);
            CleanupStack::Pop( transfer );

            iTransferController->AddIncomingTransferInternalL(&aHttpMsg);
            break;
            }
        case ETransferError:
            {
            iTransferController->TransferFailL( &aHttpMsg );
            break;
            }
        default:
        break;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ImportComplete
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::TransferProgress( TInt aId, TInt aBytes,
    TInt aTotalBytes )
    {
    TInt transferId = iTransferController->Transfer( aId );
    if ( transferId != KErrNotFound )
        {
        (iTransferController->iFileTransfers[ transferId ])->SetTransferLength( aBytes );
        (iTransferController->iFileTransfers[ transferId ])->SetTransferTotal( aTotalBytes );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferStartL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::TransferStartL( const TDesC8& aImpUri )
    {
    TInt index = iAutoDestroyObjects->FindObjIndexByResId( UpnpCdUtils::ResIdFromUriL( aImpUri ) );
    if ( index != KErrNotFound )
        {
        (*iAutoDestroyObjects)[index]->SetTransferStartedL( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferCompleteL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::TransferCompleteL( TInt aResId, TInt aObjId )
    {
    TInt index = iAutoDestroyObjects->FindObjIndexByResId( aResId );
    if ( index != KErrNotFound )
        {
        iAutoDestroyObjects->RemoveAndDestroyL( index );
        }

    if ( (aObjId>=0)&&(aResId>=0)
            &&(iContentDirectoryDb->GetParentIdL( aObjId )>=0) )
        {
        TBuf8<KMaxIntegerLen> containerID;
        containerID.Num( iContentDirectoryDb->GetParentIdL( aObjId ) );
        iStateHandler->IncreaseSystemUpdateIdL( );
        iStateHandler->IncreaseContainerUpdateIdL( containerID );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NotifyTransferFailL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::NotifyTransferFailL( TInt aSessionId,
    TInt aStatusCode )
    {
    CUpnpFileTransferEvent* event =
            iTransferController->RemoveTransferLC( aSessionId );
    if ( event )
        {
        event->SetProgressState( CUpnpFileTransferEvent::EFailed );
        event->SetErrorCode( aStatusCode );
        CleanupStack::Pop( event );
        iEventObserver->FileTransferEvent( event );
        }

    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NotifyTransferStoppedL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::NotifyTransferStoppedL( TInt aSessionId,
    CUpnpFileTransfer::TTransferType aType )
    {
    CUpnpFileTransferEvent* event =
            iTransferController->RemoveTransferLC( aSessionId );
    if ( event )
        {
        event->SetProgressState( CUpnpFileTransferEvent::EStopped );
        if ( aType == CUpnpFileTransfer::EImport )
            {
            event->SetDirection( CUpnpFileTransferEvent::EIncoming );
            }
        else
            {
            event->SetDirection( CUpnpFileTransferEvent::EOutgoing );
            }

        CleanupStack::Pop( event );
        iEventObserver->FileTransferEvent( event );
        }

    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NotifyOutgoingTransferL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::NotifyOutgoingTransferL( TInt aSessionId )
    {
    CUpnpFileTransferEvent* event =
            iTransferController->RemoveTransferLC( aSessionId );
    if ( event )
        {
        CleanupStack::Pop( event );
        //send event to Media Server        
        iEventObserver->FileTransferEvent( event );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SetSender
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::SetSender( MUpnpCdsSender* aSender )
    {
    iSender = aSender;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoSendActionL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::DoSendActionL( CUpnpAction* aAction,
    TUpnpErrorCode aError )
    {
    iSender->SendL( aAction, aError );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoSendActionL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::DoSendMessageL( CUpnpHttpMessage* aMessage )
    {
    iSender->SendL( aMessage );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SendActionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::SendActionL( CUpnpAction* aAction,
    TUpnpErrorCode aError )
    {
    SendL( aAction, aError );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::DoSendActionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::SendMessageL( CUpnpHttpMessage* aMessage )
    {
    SendL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::StateVariableEvented
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::StateVariableEvented( const TDesC8& aVariableName )
    {
    iStateHandler->StateVariableEvented( aVariableName );
    }
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::StateHandler
// -----------------------------------------------------------------------------
//
CUpnpStateHandler* CUpnpContentDirectory::StateHandler()
    {
    return iStateHandler;
    }

// helper functions
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ExecutedAction
// -----------------------------------------------------------------------------
//
TCurrentAction CUpnpContentDirectory::ExecutedAction()
    {
    return iCurrentAction;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::SetExecutedAction
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::SetExecutedAction( TCurrentAction aAction )
    {
    iCurrentAction = aAction;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::HttpResponseReceivedL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::HttpResponseReceivedL( TInt aSessionId,
    TInt aStatusCode )
    {
    iTransferController->HttpResponseReceivedL( aSessionId, aStatusCode );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::EventObserver
// -----------------------------------------------------------------------------
//
MUpnpContentDirectoryEventObserver* CUpnpContentDirectory::EventObserver()
    {
    return iEventObserver;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ContentDirectoryDb
// -----------------------------------------------------------------------------
//
CUpnpContentDirectoryDb* CUpnpContentDirectory::ContentDirectoryDb()
    {
    return iContentDirectoryDb;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::IsObjectExistL
// -----------------------------------------------------------------------------
//    
TBool CUpnpContentDirectory::IsObjectExistL( TInt aObjectId )
    {
    if ( iContentDirectoryDb->CheckObjectRestrictionL( aObjectId )
            == ENoSuchObject )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::ValidateObjectL
// -----------------------------------------------------------------------------
//   
TBool CUpnpContentDirectory::ValidateObjectL( TInt aObjectId )
    {
    //removes from autodestroy array orphaned objects
    TInt i(0);
    TInt index(0);
    while ( i < iAutoDestroyObjects->Count( ) )
        {
        TInt parentID( iContentDirectoryDb->GetParentIdL(
                        ( *iAutoDestroyObjects )[ i ]->GetObjId( ) ) );
        if ( parentID < 0 )
            {
            index = iAutoDestroyObjects->FindObjIndexByObjId( (
                                             *iAutoDestroyObjects )[ i ]->GetObjId( ) );
            iAutoDestroyObjects->RemoveAndDestroyL( index );
            continue;
            }
        i++;
        }

    //checks if given object exists
    if ( !IsObjectExistL( aObjectId ) )
        {
        index = iAutoDestroyObjects->FindObjIndexByObjId( aObjectId );
        if ( index != KErrNotFound )
            {
            iAutoDestroyObjects->RemoveAndDestroyL( index );
            }
        }

    //cheks if given object exists in autodestroy array
    index = iAutoDestroyObjects->FindObjIndexByObjId( aObjectId );
    if ( index != KErrNotFound )
        {
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferHandler
// -----------------------------------------------------------------------------
//
MUpnpTransferInterface* CUpnpContentDirectory::TransferHandler()
    {
    return iTransferHandler;
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::TransferCompletedL
// -----------------------------------------------------------------------------
//    
void CUpnpContentDirectory::TransferCompletedL( TInt aKey, TInt aResultCode )
    {
    HttpResponseReceivedL( aKey, aResultCode );
    if ( aResultCode == EHttpOk )
        {
        TInt transferid = iTransferController->Transfer( aKey );
        if ( transferid > KErrNotFound )
            {
            TInt
                    len =
                            iTransferController->iFileTransfers[transferid]->TransferTotal( );
            iTransferController->iFileTransfers[transferid]->SetTransferLength( len );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CUpnpContentDirectory::NewTransactionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpContentDirectory::NewTransactionL(
        const TDesC8& aMethod, const TDesC8& aUri, 
        const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans )
    {
    iDlnaFilter->NewTransactionL( aMethod, aUri, aSender, aResultTrans );
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::HttpEventLD
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::HttpEventLD( CUpnpHttpMessage* aMessage )
    {
    if ( aMessage )
        {
        CleanupStack::PushL( aMessage );
        if( aMessage->Type() >= ETransferStart )
            {
            HttpTransferEventReceivedL( *aMessage );
            }
        else
            {
            UnknownHttpEventReceivedL( *aMessage );
            }
        CleanupStack::PopAndDestroy( aMessage );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpContentDirectory::AddressChangeL
// -----------------------------------------------------------------------------
//
void CUpnpContentDirectory::AddressChangeL()
    {
    iHttpServerSession->Stop();
    iHttpServerSession->StartL();        
    }

//End of File
