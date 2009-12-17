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
* Description:  CMediaservantPlugin class implementation
*
*/


// INCLUDE FILES
#include <w32std.h>
#include <fbs.h>
#include <StringLoader.h>
#include <mediaservant.mbg>
#include <mediaservant.rsg>
#include <bautils.h>
#include <eikenv.h>
#include <apgcli.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <pathinfo.h>

#include "msengine.h"
#include "mediaservantplugin.h"
#include "msconstants.h"
#include "mediaservantuid.h"

#include "msdebug.h"

// CONSTANTS
_LIT( KMediaservantRscFile, "\\resource\\apps\\mediaservant.rsc" );

// ---------------------------------------------------------------------------
// CMediaservantPlugin::NewL
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------------------------
CMediaservantPlugin* CMediaservantPlugin::NewL(
    MUPnPPluginObserver* aPluginObserver)
    {
    CMediaservantPlugin* self = CMediaservantPlugin::NewLC (aPluginObserver);
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::NewLC
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------------------------
CMediaservantPlugin* CMediaservantPlugin::NewLC(
    MUPnPPluginObserver* aPluginObserver)
    {
    CMediaservantPlugin* self = new(ELeave)
                              CMediaservantPlugin(aPluginObserver);
    CleanupStack::PushL( self );
    self->ConstructL( aPluginObserver );

    return self;
    }
    
// ---------------------------------------------------------------------------
// CMediaservantPlugin::ConstructL
// Second phase constructor. Creates icon and sets title and secondary text
// ---------------------------------------------------------------------------
//
void CMediaservantPlugin::ConstructL(MUPnPPluginObserver*
                                                aPluginObserver)
    {
    LOG(_L("[MediaServant]\t CMediaservantPlugin::ConstructL start"));

    // Set observer
    iPluginObserver = aPluginObserver;

    // set icon
    LOG(_L("[MediaServant]\t CMediaservantPlugin::ConstructL create icon"));
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    AknsUtils::CreateIconLC(skin, KAknsIIDDefault,
                            bitmap, mask, KMSMifFileName,
                            EMbmMediaservantQgn_mserv_app_menu_icon,
                            EMbmMediaservantQgn_mserv_app_menu_icon_mask);

    iIcon = CGulIcon::NewL(bitmap,mask);
    iIcon->SetBitmapsOwnedExternally(EFalse);

    CleanupStack::Pop( mask );
    CleanupStack::Pop( bitmap );
    bitmap = NULL;
    mask = NULL;



    // Load resource file
    LOG(_L("[MediaServant]\t CMediaservantPlugin::ConstructL load resource"));
    TFileName rscFileName( KMediaservantRscFile );
 
    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();

    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
        
    // this works for c and z drives
    TInt err(KErrNone);
    TRAP( err, iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName ))
    if ( err != KErrNone )
        {
        // not found? let's try mmc drive
        rscFileName.Copy( PathInfo::MemoryCardRootPath() );
        rscFileName.Delete(2, 2); // remove //
        rscFileName.Append( KMediaservantRscFile );
        iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );
        }

    LOG(_L("[MediaServant]\t CMediaServantPlugin::ConstructL \
    after resourceloader"));

    // Set item text
    iTitle = StringLoader::LoadL( R_MS_PLUGIN_LISTBOX_PRIMARY_TEXT );

    iMSEngine = CMSEngine::NewL();

    // load secondary text
    LoadPluginTextL();

    // Create property watcher for progress info
    iMSPropertyWatcher = CMSPropertyWatcher::NewL();
    // view will observe changes in property
    iMSPropertyWatcher->SetObserver(this);
    // start listening property
    iMSPropertyWatcher->StartL();

    LOG(_L("[MediaServant]\t CMediaservantPlugin::ConstructL end"));
    }

// ---------------------------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------------------------
CMediaservantPlugin::CMediaservantPlugin(TAny* aEcomArguments)
   : CUPnPPluginInterface( aEcomArguments )
    {
    iRequiresUpnpConf = ETrue;
    iTitle = NULL;
    iSecondaryText = NULL;
    }


// ---------------------------------------------------------------------------
// CMediaservantPlugin::~CMediaservantPlugin
// ---------------------------------------------------------------------------
CMediaservantPlugin::~CMediaservantPlugin()
    {
    LOG(_L("[MediaServant]\t CMediaservantPlugin::~CMediaservantPlugin"));

    delete iMSEngine;

    // Un-Load resource file
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }

    if ( iMSPropertyWatcher )
        {
        iMSPropertyWatcher->Stop();
        delete iMSPropertyWatcher;
        }
    delete iIcon;
    delete iTitle;
    delete iSecondaryText;
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::GetIcon
// Returns reference to the icon of the plugin
// ---------------------------------------------------------------------------
const CGulIcon& CMediaservantPlugin::GetIcon()
    {
    return *iIcon;
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::GetTitle
// Returns const reference to the icon of the plugin
// ---------------------------------------------------------------------------
const TDesC& CMediaservantPlugin::GetTitle()
    {
    return *iTitle;
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::GetSecondaryText
// Returns const reference to the scondary text of the plugin
// ---------------------------------------------------------------------------
const TDesC& CMediaservantPlugin::GetSecondaryText()
    {
    return *iSecondaryText;
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::ExecuteL
// Executes the plugin
// ---------------------------------------------------------------------------
void CMediaservantPlugin::ExecuteL()
    {
    LOG(_L("[MediaServant]\t CMediaservantPlugin::ExecuteL"));

    RApaLsSession apaLsSession;
    User::LeaveIfError(apaLsSession.Connect());
    CleanupClosePushL(apaLsSession);

    TApaAppInfo applicationInfo;
    TInt error = apaLsSession.GetAppInfo(
                                    applicationInfo,
                                    TUid::Uid( KMediaServantUID3 ) );

    if(error == KErrNone)
        {
        CApaCommandLine* commandLine = CApaCommandLine::NewLC();
        commandLine->SetExecutableNameL( applicationInfo.iFullName );
        commandLine->SetCommandL( EApaCommandRun );
        apaLsSession.StartApp( *commandLine );

        CleanupStack::PopAndDestroy( commandLine );
        }
    else
        {
        LOG(_L("[MediaServant]\t CMediaservantPlugin::ExecuteL \
        application launch failed"));
        }

    CleanupStack::PopAndDestroy( &apaLsSession );
    }

// ---------------------------------------------------------------------------
// CMediaservantPlugin::RequiresUpnpConfiguration
// Checks if the plugin needs upnp configuration (set IAP)
// ---------------------------------------------------------------------------
TBool CMediaservantPlugin::RequiresUpnpConfiguration()
    {
    return iRequiresUpnpConf;
    }



// --------------------------------------------------------------------------
// CMediaservantPlugin::PropertyChangedL
// --------------------------------------------------------------------------
//
void CMediaservantPlugin::PropertyChangedL( TCmService aService,
                                    TInt aTotalItems,
                                    TInt aProcessedItems,
                                    TInt /*aItemsToBeProcessed*/ )
    {

    LOG(_L("[MediaServant]\t CMediaservantPlugin::PropertyChangedL"));

    switch ( aService )
        {
        case ECmServiceHarvest:
            {
            delete iSecondaryText;
            iSecondaryText = NULL;
            iSecondaryText = StringLoader::LoadL( R_MS_PLUGIN_HARVESTING );
            iPluginObserver->ExtensionEvent(
                                EExtensionSecondaryTextChanged );
            break;
            }
        case ECmServiceStore:
        // fall through
        case ECmServiceFill:
            {
                {
                TInt value( 0 );
                // calculate procentual value
                if ( aTotalItems > 0 && aProcessedItems >= 0 )
                    {
                	value = (TReal(aProcessedItems)/TReal(aTotalItems))*100;
                    } 
                delete iSecondaryText;
                iSecondaryText = NULL;
                iSecondaryText = StringLoader::LoadL(
                                   R_MS_PLUGIN_SYNCHRONIZING, value );
                iPluginObserver->ExtensionEvent( 
                    EExtensionSecondaryTextChanged );
                }
            break;
            }
        case ECmServiceNone:
            {
            LoadPluginTextL();
            iPluginObserver->ExtensionEvent(EExtensionSecondaryTextChanged);
            break;
            }
        default:
            {
            LOG(_L("[MediaServant]\t CMediaservantPlugin::PropertyChangedL \
            service not found"));
            }

        }
    }

// --------------------------------------------------------------------------
// CMediaservantPlugin::LoadPluginTextL
// --------------------------------------------------------------------------
//
void CMediaservantPlugin::LoadPluginTextL()
    {
    // Need to check content manager service state
    //to determine secondary text
    LOG(_L("[MediaServant]\t CMediaservantPlugin:LoadPluginTextL \
    open msengine"));

    TCmServiceState state = iMSEngine->ServiceState(
                                        ECmServiceContentManager );
    if ( iSecondaryText )
        {
        delete iSecondaryText;
        iSecondaryText = NULL;
        }

    switch ( state )
        {
        case ECmServiceStateAutomatic:
            {
            iSecondaryText =  StringLoader::LoadL(
                                            R_MS_PLUGIN_TEXT_AUTO_SYNC );
            break;
            }
        case ECmServiceStateDisabled:
            {
            iSecondaryText =  StringLoader::LoadL(
                                            R_MS_PLUGIN_TEXT_MANUAL_SYNC );
            break;
            }
        default:
            {
            // don't show sync state
            LOG(_L("[MediaServant]\t CMediaservantPlugin:LoadPluginTextL \
            sync state not shown"));
            break;
            }
        }
    }
// end of file
