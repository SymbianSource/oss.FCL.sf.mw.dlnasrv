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
* Description:  MediaServant settings view.
*
*/


#ifndef MSSETTINGSVIEW_H
#define MSSETTINGSVIEW_H

// INCLUDES
#include <GSPluginInterface.h>

// FORWARD DECLARATIONS
class CMSSettingItemList;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;

/**
 *  MediaServant settings view.
 *
 *  @lib msgsplugin.lib
 */
NONSHARABLE_CLASS( CMSSettingsView ) : public CGSPluginInterface
    {

    public:
        /**
         * NewL
         * Two phased constructor.
         * @return CMSSettingsView*, the new CMSSettingsView object
        */
        static CMSSettingsView* NewL();

        /**
         * NewLC
         * Two phased constructor.
         * @return CMSSettingsView*, the new CMSSettingsView object
         */
        static CMSSettingsView* NewLC();
		
        /**
         * Destructor.
         */
        virtual ~CMSSettingsView();

        /**
         * SetNavigationPaneTextL
         * Sets Navigation pane text.
         * @since S60 5.1
         * @param aText, text to be shown
         */
        void SetNavigationPaneTextL( const TDesC& aText );

        /**
         * ClearCurrentNaviPaneText
         * Clears current navi pane text
         * @since S60 5.1
         */
        void ClearCurrentNaviPaneText();
        
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

    private:

        /**
         * Default constructor.
         */
        CMSSettingsView();

        /**
         * Second phase constructor.
         */
        void ConstructL();
    
        /**
         * Sets text by resource ID in title pane.
         *
         * @since S60 5.1
         * @param aResourceId, The ID of outline number to displayed next.
         */
        void SetTitlePaneTextL( const TInt aResourceId );    

    private: // data

        /**
         * Setting list
         */
        CMSSettingItemList*                 iSettingItemList;   // owned

        /**
         * Resource file offset
         */
        TInt                                iResFileOffset;

        /**
         * Navi decorator
         */
        CAknNavigationDecorator*            iNaviDecorator;     // owned 

        /**
         * Navigation pane
         */
        CAknNavigationControlContainer*     iNaviPane;          // not owned
        
        /**
         * Control environment for file session
         */
        CEikonEnv*                          iCoeEnv;            // not owned
        
    };

#endif // MSSETTINGSVIEW_H
