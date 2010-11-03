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
* Description:  Implementation of UPnP transcode helper
*
*/
#include <bautils.h> //hack

#include <upnpitem.h>
#include <upnpstring.h>
#include <upnpdlnaprotocolinfo.h>

#include <upnpgstwrapper.h>
#include <upnprenderercfg.h>
#include <upnprenderercfgparser.h>

#include "upnpitemutility.h"
#include "upnpavdevice.h"

#include "upnptranscodehelper.h"

_LIT( KComponentLogfile, "upnptranscodehelper.txt");
#include "upnplog.h"

#define __FUNC_LOG __LOG8_1( "%s", __PRETTY_FUNCTION__ );

_LIT( KRendererCfgFile, "C:\\data\\UpnpCfgs.xml" );

_LIT( KTranscodedFlag, "?transcoded=true" );

_LIT8( KSymbianDirDelimiter, "\\" );
_LIT8( KLinuxDirDelimiter, "/" );
_LIT8( KProtocolInfo, "protocolInfo" );
_LIT8( KRes, "res" );

// These are for Transport Stream muxer checking (from pipeline)
_LIT8( KMpegTsMux, "mpegtsmux" );
_LIT8( KFFMuxMpegTs, "ffmux_mpegts" );

// These are for Transport Stream support checking
_LIT8( KTsProtocolInfoIdentifier, "_TS_" );
_LIT8( KAllMpegMimeTypes, "video/mpeg:*" );

// This is for MP4 support checking 
_LIT8( KMp4MimeType, "video/mp4" );

// The pipeline is currently hardcoded (except input file)
_LIT8( KDefaultPipelineFmt, 
"mpegtsmux output-buf-size=0x7FCCC name=mux ! appsink max-buffers=10 name=sinkbuffer filesrc \
location=%S ! qtdemux name=demux ! queue max-size-bytes=0xAAAA ! mux. demux. ! \
queue max-size-bytes=0xAAAA ! mux.");

const TInt KLocationFmtTagLen = 2;

CUpnpTranscodeHelper* CUpnpTranscodeHelper::NewL()
    {
    __FUNC_LOG;
    
    CUpnpTranscodeHelper* self = new ( ELeave ) CUpnpTranscodeHelper();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CUpnpTranscodeHelper::~CUpnpTranscodeHelper()
    {
    __FUNC_LOG;
    
    iGstWrapper->RemoveObserver( *this );
    iGstWrapper->Close();
    iFs.Close();
    }

void CUpnpTranscodeHelper::PreDefinedCfgL( const CUpnpAVDevice& aRenderer, 
        CUpnpItem& aUpnpItem, HBufC8*& aPipeline,
        HBufC8*& aProtocolInfo )
    {
    __FUNC_LOG;
    
    //fetch mimetype and filename from original resource
    RUPnPElementsArray resources;
    CleanupClosePushL( resources );
    UPnPItemUtility::GetResElements( aUpnpItem, resources );
    TInt resourcesCount = resources.Count();
    if ( resourcesCount == 0 )
        {
        User::Leave( KErrNotFound );
        }         

    const CUpnpAttribute& originalProtocolInfoDesc = 
             UPnPItemUtility::FindAttributeByNameL( 
             *resources[0], KProtocolInfo ); 
    
    CUpnpDlnaProtocolInfo* originalProtocolInfo = 
            CUpnpDlnaProtocolInfo::NewL( originalProtocolInfoDesc.Value() );
    CleanupStack::PushL( originalProtocolInfo );            
    
    //create config for renderer
    CUpnpRendererCfg* config = CUpnpRendererCfg::NewLC( KRendererCfgFile, 
            aRenderer.ModelName(), originalProtocolInfo->ThirdField() );    
    
    CUpnpRendererCfgParser* parser = CUpnpRendererCfgParser::NewLC();   
        
    parser->ParseRendererCfgL( *config );
    
    HBufC8* srcFileName8 = PathToLinuxSyntaxL( resources[0]->FilePath() );
    CleanupStack::PushL( srcFileName8 );
    
    HBufC8* pipeline = HBufC8::NewLC( config->Pipeline().Length() + 
            srcFileName8->Length() - KLocationFmtTagLen );    
    pipeline->Des().AppendFormat( config->Pipeline(), srcFileName8 );
      
    HBufC8* protocolInfo = config->ProtocolInfo().AllocL();

    RFile file;
    User::LeaveIfError( file.Open(iFs,resources[0]->FilePath(),EFileRead) );
    CleanupClosePushL(file);
    TInt size(KErrNotFound);
    User::LeaveIfError( file.Size(size) );
    iGstWrapper->SetTranscodedFileSize( config->SizeMultiplier()*size );
    CleanupStack::PopAndDestroy(&file);
    
    CleanupStack::Pop( pipeline );
    CleanupStack::PopAndDestroy( srcFileName8 );
    CleanupStack::PopAndDestroy( parser );
    CleanupStack::PopAndDestroy( config );
    CleanupStack::PopAndDestroy( originalProtocolInfo );
    CleanupStack::PopAndDestroy( &resources );
    
    aPipeline = pipeline;
    aProtocolInfo = protocolInfo;
    }

HBufC8* CUpnpTranscodeHelper::PipelineL( CUpnpItem& aUpnpItem, 
        const CDesC8Array& aRendererProtocolInfos )
    {
    __FUNC_LOG;
    
    HBufC8* pipeline = NULL;
    TBool protocolMatch = EFalse;
    TContainerType srcContainerType = EUnknownContainer;
    TContainerType destContainerType = EUnknownContainer;

    //Get resources from the current upnp item
    RUPnPElementsArray resources;
    CleanupClosePushL( resources );
    UPnPItemUtility::GetResElements( aUpnpItem, resources );
    TInt resourcesCount = resources.Count();
    if ( resourcesCount == 0 )
        {
        User::Leave( KErrNotFound );
        }    

    //Check if suitable source container is found from 1st resource
    const CUpnpAttribute& originalProtocolInfo = 
            UPnPItemUtility::FindAttributeByNameL( 
            *resources[0], KProtocolInfo ); 
            
    if( originalProtocolInfo.Value().Find( KMp4MimeType ) )
        {
        srcContainerType = EMp4Container; 
        }
    else
        {
        User::Leave( KErrNotSupported );
        }        

    //Loop through all protocol infos from renderer and try to find suitable        
    TInt rendererProtocolInfoCount = aRendererProtocolInfos.Count(); 
    for( TInt i = 0; i < rendererProtocolInfoCount && !protocolMatch; i++ )
        {
        for( TInt j = 0; j < resourcesCount && !protocolMatch; j++ )
            {
            const TDesC8& protocolInfo = 
            UPnPItemUtility::FindAttributeByNameL( 
                    *resources[j], KProtocolInfo ).Value();        

            const TDesC8& rendererProtocolInfo = 
            aRendererProtocolInfos.MdcaPoint(i);

            if( rendererProtocolInfo.Find( protocolInfo ) != KErrNotFound )
                {                                
                protocolMatch = ETrue;
                //breaks both 'for' loops here    
                }

            //Check if protocol info is supported 
            if( destContainerType == EUnknownContainer &&
                ( rendererProtocolInfo.Find( 
                      KTsProtocolInfoIdentifier ) != KErrNotFound || 
                  rendererProtocolInfo.Find(
                      KAllMpegMimeTypes ) != KErrNotFound ) ) 
                {
                destContainerType = ETsContainer; 
                }
            //TODO: add here other supported containers (muxers) as well

            }        
        }

    if( !protocolMatch )
        {               
        HBufC8* filePath = PathToLinuxSyntaxL( resources[0]->FilePath() );     
        CleanupStack::PushL( filePath );            
        pipeline = GeneratePipelineL( *filePath, destContainerType );        
        CleanupStack::PopAndDestroy( filePath );                
        }

    CleanupStack::PopAndDestroy( &resources );

    return pipeline;
    }

void CUpnpTranscodeHelper::ReplaceResourceL( CUpnpItem& aUpnpItem, 
                                             const TDesC8& aProtocolInfo )
    {
    RUPnPElementsArray& elements = const_cast<RUPnPElementsArray&>(
                    aUpnpItem.GetElements());

    for( TInt resIndex(0); resIndex < elements.Count(); ++resIndex )
        {
        if( elements[resIndex]->Name() == KRes )
            {
            HBufC* tmp = HBufC::NewLC( 
                    elements[resIndex]->FilePath().Length() +
                    KTranscodedFlag().Length() );
            tmp->Des().Append( elements[resIndex]->FilePath() );
            tmp->Des().Append( KTranscodedFlag() );
            elements.Remove( resIndex );
            aUpnpItem.AddResourceL( *tmp, aProtocolInfo );
            CleanupStack::PopAndDestroy( tmp );
            break;
            }
        }
    }

void CUpnpTranscodeHelper::TranscodeL( const TDesC8& aPipeline )
    {
    __FUNC_LOG;
    
    //iGstWrapper->StartL( aPipeline ); 
    iGstWrapper->SetPipelineL( aPipeline );
    
    //should we wait here until we get pipeline playing state notification
    //or error ;)
    }


void CUpnpTranscodeHelper::AddValidResElementL( 
        CUpnpItem& aUpnpItem, const TDesC8& aPipeline, 
        const CDesC8Array& aRendererProtocolInfos )
    {
    __FUNC_LOG;
    
    TBool suitableFound = EFalse;
    const TDesC8* protocolInfoIdentifier = NULL;

    //TODO: Add also other supported containers (muxers)
    if( aPipeline.Find( KMpegTsMux ) != KErrNotFound || 
        aPipeline.Find( KFFMuxMpegTs ) != KErrNotFound )        
        {
        protocolInfoIdentifier = &KTsProtocolInfoIdentifier(); 
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    TInt rendererProtocolInfoCount = aRendererProtocolInfos.Count();

    if( rendererProtocolInfoCount == 0 )
        {
        User::Leave(KErrNotFound);
        }

    //Get resources from the current upnp item
    RUPnPElementsArray resources;
    CleanupClosePushL( resources );
    UPnPItemUtility::GetResElements( aUpnpItem, resources );
    TInt resourcesCount = resources.Count();
    if ( resourcesCount == 0 )
        {
        User::Leave( KErrNotFound );
        }  
    
    for( TInt i = 0; i < rendererProtocolInfoCount && !suitableFound; i++ )
        {           
        TPtrC8 rendererProtocolInfo = aRendererProtocolInfos.MdcaPoint(i);
        if( rendererProtocolInfo.Find( *protocolInfoIdentifier ) )
            {
            //TODO: Add only suitable ones -- now adds all protocol infos
            //      from renderer, which contains _TS_ sub-string
            const TDesC& originalFilePath = resources[0]->FilePath();
            HBufC* transcodedFilePath = HBufC::NewL( 
                    originalFilePath.Length() + KTranscodedFlag().Length() );
            CleanupStack::PushL( transcodedFilePath );
            
            
            transcodedFilePath->Des().Append( originalFilePath );
            transcodedFilePath->Des().Append( KTranscodedFlag );
        
            aUpnpItem.AddResourceL( *transcodedFilePath, 
                    rendererProtocolInfo );
            
            CleanupStack::PopAndDestroy( transcodedFilePath );
            
            suitableFound = ETrue;
            }                
        }

    CleanupStack::PopAndDestroy( &resources );
    
    if( !suitableFound )
        {
        User::Leave( KErrNotFound );
        }    
    else
        {
        // delete orig.
        RUPnPElementsArray& elements = const_cast<RUPnPElementsArray&>(
                aUpnpItem.GetElements());

        for( TInt resIndex(0); resIndex < elements.Count(); ++resIndex )
            {
            if( elements[resIndex]->Name() == KRes )
                {
                elements.Remove(resIndex);
                break;
                }
            }
        }
    }

CUpnpTranscodeHelper::CUpnpTranscodeHelper()
    {
    __FUNC_LOG;    
    }

void CUpnpTranscodeHelper::ConstructL()
    {
    __FUNC_LOG;    
    
    iGstWrapper = CUpnpGstWrapper::GetInstanceL();    
    iGstWrapper->SetObserverL( *this );
    User::LeaveIfError( iFs.Connect() );
    }

HBufC8* CUpnpTranscodeHelper::GeneratePipelineL( 
        const TDesC8& aOriginalFilePath, 
        TContainerType aDestContainerType )
    {    
    __FUNC_LOG;
    
    if( aDestContainerType == EUnknownContainer )
        {
        User::Leave( KErrNotSupported );
        }    

    HBufC8* pipeline = NULL;     
    
    switch( aDestContainerType )
        {
        case ETsContainer:
            {
            //TODO ??
            pipeline = HBufC8::NewL( KDefaultPipelineFmt().Length() - 
                    KLocationFmtTagLen + aOriginalFilePath.Length() );            
            pipeline->Des().AppendFormat( KDefaultPipelineFmt, 
                    &aOriginalFilePath );
            break;
            }
        default:
            {
            __ASSERT_ALWAYS( 0, User::Invariant() );
            }
        }
    
    return pipeline; 
    }

HBufC8* CUpnpTranscodeHelper::PathToLinuxSyntaxL( 
        const TDesC& aOriginalPath )
    {
    __FUNC_LOG;
    
    HBufC8* newFilePath = UpnpString::FromUnicodeL( aOriginalPath );
     
    TInt index = 0;
    while( index >= 0 )
        {
        index = newFilePath->Find( KSymbianDirDelimiter );
        if( index >= 0 )
            {
            newFilePath->Des().Replace( index, 
                    KSymbianDirDelimiter().Length(), KLinuxDirDelimiter );
            }
        }    
    
    return newFilePath;
    }

void CUpnpTranscodeHelper::HandleGstEvent( Gst::TEvent aEvent )
    {
    __FUNC_LOG;
    
    if( aEvent == Gst::EError )
        {
        __LOG1( "CUpnpTranscodeHelper::HandleGstEvent ErrorMsg: %S",
                iGstWrapper->ErrorMsg() );
        __ASSERT( EFalse, __FILE__, __LINE__ );
        }
    }


// end of file
