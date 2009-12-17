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
* Description:      Upnp general settings view.
*
*/






#ifndef UPNPGSPLUGINVIEW_H
#define UPNPGSPLUGINVIEW_H

#include <gsplugininterface.h>
#include "upnpfilesharingengineobserver.h"
class CUPnPAppSettingsList;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CUPnPFileSharingEngine;

/**
 *  MediaServant settings view.
 *
 *  @lib msgsplugin.lib
 */
NONSHARABLE_CLASS( CUPnPGSPluginView ) : 
                                    public CGSPluginInterface,
                                    private MUPnPFileSharingEngineObserver
    {

    public:

        /**
         * Two phased constructor.
         */
        static CUPnPGSPluginView* NewL();

        /**
         * Destructor.
         */
        virtual ~CUPnPGSPluginView();

    protected:
    
    // From base class CAknView

        /**
         * From CAknView
         * See base class definition.
         */
        TUid Id() const;

        /**
         * From CAknView
         * See base class definition
         */
        void HandleCommandL( TInt aCommand );
    
        /**
         * From CAknView
         * See base class definition
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage);

        /**
         * From CAknView
         * See base class definition
         */        
        void DoDeactivate();
                    
    // From base class CGSPluginInterface
    
        /**
         * From CGSPluginInterface
         * See base class definition
         */
        void GetCaptionL( TDes& aCaption ) const;

        /**
         * From CGSPluginInterface
         * See base class definition
         */
        CGulIcon* CreateIconL( const TUid aIconType );   
    
    protected: // From MEikMenuObserver

        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    private:  // From MUPnPFileSharingEngineObserver

        /**
         * Function informs when file sharing was enabled or disabled
         * @param aError, error code
         * @param aEnabled ETrue if sharing was enabled, otherwise EFalse
         */
        void OnSharingStatusChanged( TInt aError, TBool aEnabled );

        /**
         * Function informs when file sharing is ready
         * @param aError, error code
         */
        void OnSharingDone( TInt /*aError*/ ) {}

        /**
         * Informs about status of the sharing progress
         * @since S60 3.1
         * @param aProgressInfo Data type to contain information about \
                  sharing progress
        */                                          
        void OnSharingProgress( 
            const RArray<TUpnpProgressInfo>& /*aProgress*/ ) {}

        /**
         * Callback to inform if connection has been lost
         * @since S60 3.1
         */ 
        void OnSharingConnectionLost();
        
    private:

        /**
         * Default constructor.
         */
        CUPnPGSPluginView();

        /**
         * Second phase constructor.
         */
        void ConstructL();
    
        /**
         * Sets text by resource ID in title pane.
         *
         * @since S60 3.1
         * @param aResourceId, The ID of outline number to displayed next.
         */
        void SetTitlePaneTextL( const TInt aResourceId );    

    private: // data

        /**
         * Setting list
         */
        CUPnPAppSettingsList*               iSettingItemList;   // owned

        /**
         * Resource file offset
         */
        TInt                                iResFileOffset;
        
        /**
         * Control environment for file session
         */
        CEikonEnv*                          iCoeEnv;            // not owned
        
        CUPnPFileSharingEngine*         iFileSharing; //owned
         // iap ids
        TInt                            iSharingState;
        
        /**
         * View's ui to close when plugin is beign closed
         */
        TVwsViewId iViewToClose;

    };

#endif // UPNPGSPLUGINVIEW_H
