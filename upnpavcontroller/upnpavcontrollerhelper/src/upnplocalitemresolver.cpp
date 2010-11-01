/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Resolver for local items
*
*/






// INCLUDE FILES
// dlnasrv / mediaserver api
#include <upnpitem.h>
// mpx
#include <mpxmedia.h>
#include <mpxmediamusicdefs.h>
#include <mpxcollectionhelper.h>
#include <mpxcollectionhelperfactory.h>
// dlnasrv / avcontroller api
#include "upnpavcontroller.h" // avcontroller service
#include "upnpavbrowsingsession.h" // browsing session
#include "upnpavdevice.h" // device (for creating a session)

// dlnasrv / avcontroller helper api
#include "upnpitemresolverfactory.h" // optimisation flags
#include "upnpitemresolverobserver.h" // MUPnPItemResolverObserver
#include "upnpfileutility.h" // IsFileProtected
#include "upnpresourceselector.h" // MUPnPResourceSelector
#include "upnpitemutility.h" // GetResElements
#include "upnpconstantdefs.h" // for upnp-specific stuff

// dlnasrv / internal api
#include "upnppushserver.h" // CUpnpPushServer
#include "upnpmetadatafetcher.h" // CreateItemFromFileLC
#include "upnpstring.h"

// dlnasrv / avcontrollerhelper internal
#include "upnplocalitemresolver.h"

#include "upnptranscodehelper.h"

#include "upnpcdsreselementutility.h"
#include "upnpdlnaprofiler.h"

_LIT( KComponentLogfile, "upnpavcontrollerhelper.txt");
#include "upnplog.h"

// CONSTANTS
static TUint KFirstSharedHash = 8647544;

// METHODS

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver:: NewL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver* CUPnPLocalItemResolver::NewL(
    const TDesC& aFilePath,
    MUPnPAVController& aAvController,
    MUPnPResourceSelector& aSelector,
    TInt aOptimisationFlags    )
    {
    CUPnPLocalItemResolver* self = new (ELeave )CUPnPLocalItemResolver(
								aAvController, aSelector, aOptimisationFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aFilePath );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::CUPnPLocalItemResolver
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver::CUPnPLocalItemResolver(
    MUPnPAVController& /*aAvController*/,
    MUPnPResourceSelector& aSelector,
    TInt aOptimisationFlags )
    : iSelector( aSelector )
    {
    iOptimisationFlags = aOptimisationFlags;
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ConstructL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ConstructL(
    const TDesC& aFilePath )
    {
    __LOG1( "LocalItemResolver:ConstructL() 0x%d", TInt(this) );
    iFilePath = aFilePath.AllocL();
    
#ifdef UPNP_USE_GSTREAMER
    iTranscodeHelper = CUpnpTranscodeHelper::NewL();
#endif
    
    // create mpx collection helper
    iCollectionHelper = CMPXCollectionHelperFactory::NewCollectionHelperL();
    }


// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::~CUPnPLocalItemResolver
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
CUPnPLocalItemResolver::~CUPnPLocalItemResolver()
    {
    __LOG1( "LocalItemResolver destructor 0x%d", TInt(this) );
    delete iFilePath;
    iFilePath = NULL;
    delete iSharedItem;
    iSharedItem = NULL;
    delete iThumbnailCreator;
    iThumbnailCreator = NULL;
    Cleanup();
#ifdef UPNP_USE_GSTREAMER
    delete iTranscodeHelper;
#endif
    iCollectionHelper->Close();
    __LOG( "LocalItemResolver destructor end" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ResolveL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ResolveL(
    MUPnPItemResolverObserver& aObserver, CUpnpAVDevice* aDevice )
    {
    __LOG1( "LocalItemResolver:Resolve() 0x%d", TInt(this) );
    
    _LIT(KExtJpeg, ".jpeg");
    _LIT(KExtJpg,  ".jpg");
    
    iObserver = &aObserver;
    if ( !(iOptimisationFlags & UPnPItemResolverFactory::EOmitDrmCheck ))
        {
        // check DRM
        if ( UPnPFileUtility::IsFileProtectedL( iFilePath->Des() ) )
            {
            User::Leave( KErrNotSupported );
            }
        }
    // create item metadata
	delete iLocalItem;
	iLocalItem = NULL;
    iLocalItem =
		UPnPMetadataFetcher::CreateItemFromFileLC(
			iFilePath->Des());
    CleanupStack::Pop( iLocalItem );

#ifdef UPNP_USE_GSTREAMER
    if( aDevice )
        {
        HBufC8* pipeline = NULL;
        HBufC8* protocolInfo = NULL;
        
        TRAPD( err, iTranscodeHelper->PreDefinedCfgL( *aDevice, 
                *iLocalItem, pipeline, protocolInfo ) );
        
        __ASSERT_ALWAYS( err == KErrNone || err == KErrNotFound, 
                User::Invariant() );
        
        if( err == KErrNone )
            {
            CleanupStack::PushL(pipeline);
            CleanupStack::PushL(protocolInfo);
            
            //config found -> start transc and replace resource
            iTranscodeHelper->TranscodeL( *pipeline );
            iTranscodeHelper->ReplaceResourceL( *iLocalItem, *protocolInfo );
            
            CleanupStack::PopAndDestroy(protocolInfo);
            CleanupStack::PopAndDestroy(pipeline);
            }               
        }
#endif
    
    TParse p;
    p.Set(iFilePath->Des(),NULL,NULL);
    if( p.Ext().CompareF(KExtJpeg) == KErrNone 
        || p.Ext().CompareF(KExtJpg) == KErrNone )
        {
        //TThumbnailDlnaSize size (EThumbnail);
        //iThumbnailCreator = CUpnpThumbnailCreator::NewL(*this,
        //                                                *iFilePath, size);
        // TODO: the above code starts thumbnail creation,
        // but for some reason thumbnail manager returns with KErrBadName.
        // For now, just share the original, without creating a thumbnail res element
        ShareL();
        }
    else if( UPnPItemUtility::BelongsToClass(*iLocalItem,KClassAudio()) )
        {
        // 
        TRAPD( err, SetAlbumArtResourceToItemL( *iFilePath ) );
        __LOG1( "LocalItemResolver:Resolve() music album art err=%d",err );
        if( err )
            {
            // if song did not have album art share normally
            ShareL();
            }
        }
    else
        {
        ShareL();
        }
    __LOG( "LocalItemResolver:Resolve() END" );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ShareL
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ShareL()
    {
    // share
    CUpnpPushServer::ShareL( KFirstSharedHash++, *iLocalItem );

    // Store item & resource
    delete iSharedItem;
    iSharedItem = NULL;    
    iSharedItem = iLocalItem;
	iLocalItem = NULL;
        
    iResource = &iSelector.SelectResourceL( *iSharedItem );
	
    //inform the observer
    iObserver->ResolveComplete( *this, KErrNone );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Item
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
const CUpnpItem& CUPnPLocalItemResolver::Item() const
    {
    __ASSERT( iSharedItem, __FILE__, __LINE__ );
    return *iSharedItem;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Resource
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
const CUpnpElement& CUPnPLocalItemResolver::Resource() const
    {
    __ASSERT( iResource, __FILE__, __LINE__ );
    return *iResource;
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::Cleanup
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::Cleanup()
    {
    __LOG( "CUPnPLocalItemResolver:Cleanup() ");
    // unshare
    TRAP_IGNORE(CUpnpPushServer::UnshareL( (TUint)this ));
    __LOG( "CUPnPLocalItemResolver:Cleanup() end" );
    }

// -----------------------------------------------------------------------------
// CUPnPLocalItemResolver::SetAlbumArtResourceToItemL
// -----------------------------------------------------------------------------
//
void CUPnPLocalItemResolver::SetAlbumArtResourceToItemL( const TDesC& aFileName )
    {
    __LOG1( "CUPnPLocalItemResolver:SetAlbumArtResourceToItemL() fileName = %S",&aFileName);
    RArray<TMPXAttribute> attrs;
    CleanupClosePushL( attrs );
    attrs.AppendL( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) );
    
    CMPXMedia* media( NULL );
    media = iCollectionHelper->GetL( aFileName,
        attrs.Array(), EMPXSong );
    CleanupStack::PopAndDestroy( &attrs );
    CleanupStack::PushL( media );
    
    const TDesC& filePath = media->ValueText( KMPXMediaMusicAlbumArtFileName );
    
    TInt leaveErr(KErrNone);
    if( filePath != KNullDesC && filePath.CompareF(aFileName) != KErrNone )
        {
        __LOG( "CUPnPLocalItemResolver:SetAlbumArtResourceToItemL() \
                start waiting album art");
        TThumbnailDlnaSize size (EThumbnail);
        iThumbnailCreator = CUpnpThumbnailCreator::NewL(*this,
                filePath, size);
        }
    else
        {
        leaveErr = KErrNotFound;
        }
    CleanupStack::PopAndDestroy( media );
    if( leaveErr ) 
        {
        User::Leave(leaveErr);
        }
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::ThumbnailCreatorReady
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::ThumbnailCreatorReady( TInt aError)
    {
    __LOG1( "CUPnPLocalItemResolver:ThumbnailCreatorReady() %d",aError);
    if(aError == KErrNone)
    	{
        if( UPnPItemUtility::BelongsToClass(*iLocalItem,KClassAudio()) )
            {
            TRAPD(err, AddAlbumArtAndShareL())
            aError = err;
            }
        else
            {
            TRAPD(err,AddThumbnailandShareL());    
            aError = err;
            }
    	}
    //inform the observer
    iObserver->ResolveComplete( *this, aError );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::AddAlbumArtAndShareL
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::AddAlbumArtAndShareL()
    {
    __ASSERT( iLocalItem, __FILE__, __LINE__ );    
    __LOG( "CUPnPLocalItemResolver:AddAlbumArtAndShareL()");
    CUpnpElement* albumArtUri = CUpnpElement::NewLC( KElementAlbumArtUri );
    CUpnpAttribute* attr = CUpnpAttribute::NewLC( KAttributeProfileId() );
    CUpnpDlnaProfiler* profiler = CUpnpDlnaProfiler::NewLC();
    
    HBufC8* profile = NULL;
    profile = UpnpString::FromUnicodeL(*(profiler->ProfileForFileL( 
            iThumbnailCreator->ThumbnailFilePath())) ); 
    
    CleanupStack::PopAndDestroy( profiler );                  
    __LOG1( "CUPnPLocalItemResolver:AddAlbumArtAndShareL() \
            profile=%S",profile);
    
    attr->SetValueL(*profile);
    albumArtUri->AddAttributeL(attr);    
    CleanupStack::Pop( attr );                  

    albumArtUri->SetFilePathL( iThumbnailCreator->ThumbnailFilePath() );

    iLocalItem->AddElementL( albumArtUri );
    CleanupStack::Pop( albumArtUri );    
    
    // share
    CUpnpPushServer::ShareL( KFirstSharedHash++, *iLocalItem );	
    
    // Store item & resource
    delete iSharedItem;
    iSharedItem = NULL;
    
    iSharedItem = iLocalItem;
    iResource = &iSelector.SelectResourceL( *iSharedItem );
    }

// --------------------------------------------------------------------------
// CUPnPLocalItemResolver::AddThumbnailandShareL
// See upnplocalitemresolver.h
//---------------------------------------------------------------------------
void CUPnPLocalItemResolver::AddThumbnailandShareL()
    {
    __ASSERT( iLocalItem, __FILE__, __LINE__ );
    UpnpCdsResElementUtility::AddResElementL(*iLocalItem,
                                iThumbnailCreator->ThumbnailFilePath());
    // share
    CUpnpPushServer::ShareL( KFirstSharedHash++, *iLocalItem );

    // Store item & resource
    delete iSharedItem;
    iSharedItem = NULL;
    
    iSharedItem = iLocalItem;
    iResource = &iSelector.SelectResourceL( *iSharedItem );
    }

