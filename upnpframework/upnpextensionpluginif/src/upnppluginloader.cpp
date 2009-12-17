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
* Description:  Implements CUPnPPluginLoader class
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>

// upnpframework / home media extension api
#include <upnpplugininterface.h>

// component internal
#include "upnppluginloaderobserver.h"
#include "upnppluginloader.h"

// log facility
_LIT( KComponentLogfile, "upnpextensionpluginif.txt");
#include "upnplog.h"

#ifdef __UPNP_CONSOLE_MT__
static const TUid KTestPluginId = { 0x20009C9D };
#endif // __UPNP_CONSOLE_MT__

//---------------------------------------------------------------------------
// CUPnPPluginLoader::NewL
// Construction method.
//---------------------------------------------------------------------------
EXPORT_C CUPnPPluginLoader* CUPnPPluginLoader::NewL(
    MUPnPPluginLoaderObserver& aLoaderObserver)
    {
    __LOG("CUPnPPluginLoader::NewL");

    CUPnPPluginLoader* self = 
        new( ELeave ) CUPnPPluginLoader(aLoaderObserver);
    return self;
    }

//---------------------------------------------------------------------------
// CUPnPPluginLoader::CUPnPPluginLoader
// Default constructor. Sets the iLoaderObserver
//---------------------------------------------------------------------------
CUPnPPluginLoader::CUPnPPluginLoader(MUPnPPluginLoaderObserver&
                                                        aLoaderObserver)
    {
    __LOG( "CUPnPPluginLoader::CUPnPPluginLoader" );

    iLoaderObserver = &aLoaderObserver;
    }

//---------------------------------------------------------------------------
// CUPnPPluginLoader::~CUPnPPluginLoader
// Destroys all the plugins
//---------------------------------------------------------------------------
EXPORT_C CUPnPPluginLoader::~CUPnPPluginLoader()
    {
    __LOG( "CUPnPPluginLoader::~CUPnPPluginLoader" );

    for (TInt i=0;i<iPluginArray.Count();i++) 
        {
        delete iPluginArray[i];
        }

    iPluginArray.Close();
    REComSession::FinalClose();
    }

//---------------------------------------------------------------------------
// CUPnPPluginLoader::CreatePluginsL
// Creates all plugins which implement CUPnPPluginInterface
//---------------------------------------------------------------------------
EXPORT_C const RPointerArray<CUPnPPluginInterface>& 
    CUPnPPluginLoader::CreatePluginsL()
    {
    __LOG( "CUPnPPluginLoader::CreatePluginsL" );

    CImplementationInformation* implInfo = NULL;
    RImplInfoPtrArray implArray;

    REComSession::ListImplementationsL(interfaceUid,implArray);
    for(TInt i=0;i<implArray.Count();i++)
        {
        implInfo = implArray[i];
       
        if( implInfo->VendorId() == VID_DEFAULT
#ifdef __UPNP_CONSOLE_MT__
            // In case of module testing load only our test plugin
            && implInfo-> ImplementationUid().iUid == KTestPluginId.iUid
#endif // __UPNP_CONSOLE_MT__
        )
            {
           CUPnPPluginInterface* interface = NULL;           
           interface = CUPnPPluginInterface::NewL
                      ( implInfo->ImplementationUid(), *this );
           
           CleanupStack::PushL( interface );  
           // interface ownership is transfered and 
           // iPluginArray will handle the destroying of interface.
           iPluginArray.AppendL( interface );           
           CleanupStack::Pop( interface );
            }
        else
            {
            __LOG( "CUPnPPluginLoader::CreatePluginsL - \
Invalid plugin vendor id" );
            }
        }
    implArray.ResetAndDestroy();
    implArray.Close();
    return iPluginArray;
    }

//---------------------------------------------------------------------------
// CUPnPPluginLoader::DeletePlugin
// Deletes plugin from iPluginArray
//---------------------------------------------------------------------------
EXPORT_C void CUPnPPluginLoader::DeletePlugin( TInt aPluginIndex )
    {
    __LOG( "CUPnPPluginLoader::DeletePlugin" );
    __ASSERTD( aPluginIndex >= 0, __FILE__, __LINE__ );
    __ASSERTD( aPluginIndex < iPluginArray.Count(), __FILE__, __LINE__ );

    if ( aPluginIndex < iPluginArray.Count() )
        {
        delete iPluginArray[aPluginIndex];
        iPluginArray.Remove(aPluginIndex);
        }
    }

//---------------------------------------------------------------------------
// CUPnPPluginLoader::ExtensionEvent
// From base class MUPnPPluginObserver
// Handles the events which come from plugins
//---------------------------------------------------------------------------
void CUPnPPluginLoader::ExtensionEvent(const TExtensionEvent& aEvent) 
    {
    __LOG( "CUPnPPluginLoader::ExtensionEvent" );

    switch ( aEvent )
        {
        case EExtensionEnabled:
            {
            // the extension is enabled by default - no need to call 
            // explicitly
            // NOT IMPLEMENTED
            break;
            }
        case EExtensionDisabled:
            {
            // when called, disables the item in the list, cannot execute
            // NOT IMPLEMENTED
            break;
            }
        case EExtensionIconChanged:
            {
            // will call GetI1con again, and redraw
            iLoaderObserver->PluginsUpdated();
            break;
            }
        case EExtensionTitleChanged:
            {
            // will call GetTitle again, and redraw
            iLoaderObserver->PluginsUpdated();
            break;
            }
        case EExtensionSecondaryTextChanged:
            {
            // will call GetSubTitle again, and redraw
            iLoaderObserver->PluginsUpdated();
            break;
            }
        case EExtensionClosed:
            {
            // extension which was ExecuteL'd, has been closed.
            // NOT IMPLEMENTED
            break;
            }
        default:
            {
            break;
            }
        }
    }

// end of file
