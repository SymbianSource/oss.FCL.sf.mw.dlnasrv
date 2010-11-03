/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UPnP Renderer config container class
*
*/

// INCLUDES
#include "upnprenderercfg.h"

_LIT( KComponentLogfile, "upnprenderercfg.txt");
#include "upnplog.h"

#define __FUNC_LOG __LOG8_1( "%s", __PRETTY_FUNCTION__ );

EXPORT_C CUpnpRendererCfg* CUpnpRendererCfg::NewL( const TDesC& aConfigFile, 
        const TDesC8& aModelName, const TDesC8& aSrcMimeType )
    {
    __FUNC_LOG;
    
    CUpnpRendererCfg* self = CUpnpRendererCfg::NewLC( aConfigFile,
            aModelName, aSrcMimeType );
    CleanupStack::Pop( self );    
    return self;
    }

EXPORT_C CUpnpRendererCfg* CUpnpRendererCfg::NewLC( const TDesC& aConfigFile,
        const TDesC8& aModelName, const TDesC8& aSrcMimeType )
    {
    __FUNC_LOG;
    
    CUpnpRendererCfg* self = new( ELeave ) CUpnpRendererCfg();
    CleanupStack::PushL( self );
    self->ConstructL( aConfigFile, aModelName, aSrcMimeType );
    return self;   
    }

EXPORT_C CUpnpRendererCfg::~CUpnpRendererCfg()
    {
    __FUNC_LOG;
    
    delete iConfigFile; 
    delete iModelName;  
    delete iSrcMimeType;     
    delete iPipeline;     
    delete iProtocolInfo;   
    }

CUpnpRendererCfg::CUpnpRendererCfg()
    : iSizeMultiplier( KErrNotFound )
    {    
    __FUNC_LOG;
    }

void CUpnpRendererCfg::ConstructL( const TDesC& aConfigFile, const TDesC8& aModelName, 
        const TDesC8& aSrcMimeType )
    {
    __FUNC_LOG;
    
    iConfigFile = aConfigFile.AllocL();
    iModelName = aModelName.AllocL();
    iSrcMimeType = aSrcMimeType.AllocL();
    }

EXPORT_C const TDesC8& CUpnpRendererCfg::Pipeline() const
    {
    __FUNC_LOG;
    
    if( !iPipeline )
        {
        return KNullDesC8;
        }
    else
        {
        return *iPipeline;
        }
    }

EXPORT_C const TDesC8& CUpnpRendererCfg::ProtocolInfo() const
    {
    __FUNC_LOG;
    
    if( !iProtocolInfo )
        {
        return KNullDesC8;
        }
    else
        {
        return *iProtocolInfo;
        }    
    }

EXPORT_C TReal CUpnpRendererCfg::SizeMultiplier() const
    {
    __FUNC_LOG;
    
    return iSizeMultiplier;
    }

// end of file
