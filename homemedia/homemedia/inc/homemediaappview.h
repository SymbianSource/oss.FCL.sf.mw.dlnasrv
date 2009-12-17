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
* Description:      Main application view
*
*/






#ifndef __HOMEMEDIAAPPVIEW_H__
#define __HOMEMEDIAAPPVIEW_H__

// INCLUDES
#include <aknview.h> 
#include <AknWaitNoteWrapper.h> //MAknBackgroundProcess
#include "upnpfilesharingengineobserver.h"
#include "upnppluginloaderobserver.h"

//forward declaration
class CHomeMediaContainer;
class CUPnPFileSharingEngine;
class CUPnPPluginInterface;
class CUPnPPluginLoader;
class MUPnPAVController;
class CUPnPSettingsEngine;
class CAknNavigationDecorator;
class CGSPluginInterface;
class CIdle;
class CAknInputBlock;
class CUPnPAppFileSharingDialog;
class CUPnPPeriodic;

/**
 * CHomeMediaAppView
 * View class for CHomeMediaAppUi, handles menu commands and creates
 * container which shows main view items.
 */
class CHomeMediaAppView : public CAknView, 
                          private MUPnPFileSharingEngineObserver,
                          private MAknBackgroundProcess,
                          private MUPnPPluginLoaderObserver
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * @return a pointer to the created instance of CHomeMediaAppView.
     */
    static CHomeMediaAppView* NewL();

    /**
     * NewLC.
     * Two-phased constructor.
     * @return A pointer to the created instance of CHomeMediaAppView.
     */
    static CHomeMediaAppView* NewLC();

    /**
     * ~CHomeMediaAppView
     * Virtual Destructor.
     */
    virtual ~CHomeMediaAppView();

    /**
     * Get the pointer of fileSharingDialog ,which is constructed in the function of 
     * ProcessCommandL
     */
    void RefreshVisibilitySettingItemL() ;

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CHomeMediaAppView object.
     */
    void ConstructL();
    
    /**    
     * CHomeMediaAppView.
     * C++ default constructor.
     */
    CHomeMediaAppView();
    
public: // Functions from base classes
    
    /**
    * Return Uid
    */
    TUid Id() const;

    /**
    * Handle Size changes
    */
    void HandleClientRectChange();

private: // From AknView

    /**
    * Activates view
    */
    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
        const TDesC8& aCustomMessage);

    /**
    * Deactivates view
    */
    void DoDeactivate();
    
private: // Functions from base classes

    /**
    * Handle Commands
    */
    void HandleCommandL(TInt aCommand);
    
    /**
    * Add some comments here
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );    
    
private:
    /**
     * From MAknBackgroundProcess
     */
    void StepL();

    /**
     * From MAknBackgroundProcess
     */
    TBool IsProcessDone() const;

    /**
     * From MAknBackgroundProcess
     */
    TInt CycleError( TInt aError );

private: //From MUPnPFileSharingEngineObserver

    /**
     * Function informs when file sharing was enabled or disabled
     * @param aEngine File sharing engine
     * @param aError, error code
     * @param aPhase ETrue if current phase is sharing activation,
     * otherwise EFalse
     */
    void HandleSharingStatus( 
        CUPnPFileSharingEngine& aEngine, 
        TInt aError, 
        TBool aPhase );

    /**
     * Callback to inform if connection has been lost
     * @param aEngine File sharing engine
     */ 
    void HandleSharingConnectionLost( 
        CUPnPFileSharingEngine& aEngine );

private: //From MUPnPPluginLoaderObserver

    /**
     * Called by UPnPPluginLoader when plugin(s) has been updated
     */
    void PluginsUpdated();

private: //business logic methods
   
    /**    
     * Browse home
     */
    void ExecuteBrowseL();
    
    /**    
     * Browse home
     */
    void DoExecuteBrowseL();
    
    /**    
     * Start wizard
     * @return key pressed in the wizard
     */
    TInt StartWizardL();
    
    /**
     * @return IAP id
     */     
    TInt IAP() const;
    
    /**
     * @return validity of current IAP
     */     
    TBool CheckAccessPointValid();
           
    /**
     * Show the error note
     * 
     * @param aMain resource id for loc
     */     
    void ShowErrorNoteL( TInt aMain ) const;
    
    /**
     * Update title pane
     */
    void UpdateTitlePaneL();
    
    /**
     * update navi pane
     * 
     * @param aShowStatus ETrue if shown
     */
    void UpdateNaviPaneL( TBool aShowStatus );

    /**
     * Load external plugins
     */
    void LoadPluginL();
    
    /**
     * Launch the settings view
     */
    void StartSettingsViewL();
    
    /**
     * If get the correct pointer ,refresh the correspond item in view or share dialog
     */
    void RefreshSharelistItemL() ;
    
    /**
     * Launch the setup wizard
     *
     * @param aPtr CHomeMediaAppView* pointer
     * @return EFalse
     */
    static TBool InitializeWizardL( TAny* aPtr );

    /**
     * Launch a timer
     *
     * @param aPtr CHomeMediaAppView* pointer
     * @return KErrNone
     */
    static TInt RefreshSharingL( TAny* aPtr );
    
private:
    /**
     * The internal phases of wait note when starting browsing
     */
    enum TBrowsingState
        {
        EPhaseNotActive,
        EPhaseProcessing,
        EPhaseCompleted
        };

    /**
     * View's window
     * Own
     */
    CHomeMediaContainer*                    iContainer;
    
    /**
     * File sharing engine 
     * Own
     */
    CUPnPFileSharingEngine*                 iFileSharing;

    /**
     * Array of plugins
     * Not own
     */
    RPointerArray<CUPnPPluginInterface>     iPluginArray;

    /**
     * Plugin loader
     * Own
     */
    CUPnPPluginLoader*                      iPluginLoader;

    /**
     * Current browse state
     */
    TBrowsingState                          iBrowseState;
    
    /**
     * AV contoller
     * Own
     */
    MUPnPAVController*                      iAvControl;
    
    /**
     * Settings engine
     * Own
     */
    CUPnPSettingsEngine*                    iSettingsEngine;
    
    /**
     * App's navigation decorator
     * Own
     */
    CAknNavigationDecorator*                iNaviDecorator;
    
    /**
     * Settings view
     * Not own
     */
    CGSPluginInterface*                     iSettingsView;
    
    /**
     * Idle loop
     * Own
     */
    CIdle*                                  iIdle;
    
    /**
     * block UI in case of first start
     * Own
     */
    CAknInputBlock*                         iInputBlocker;

    /**
     * record the pointer of CUPnPAppFileSharingDialog
     * Own
     */
    CUPnPAppFileSharingDialog* ifileSharingDialog;

    CUPnPPeriodic* iRefreshTimer;   
    };

#endif // __HOMEMEDIAAPPVIEW_H__
// End of File
