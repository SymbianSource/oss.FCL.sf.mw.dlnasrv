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
* Description:  Produces avt actions
*
*/



#ifndef C_CUPNPAVTACTIONFACTORY_H
#define C_CUPNPAVTACTIONFACTORY_H

//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpService;
class CUpnpAVControlPoint;

// CLASS DECLARATION

/**
*  AV Transport factory class.
*  This class creates AV Transport actions.
*
*  @since Series 60 2.6
*/
class CUpnpAvtActionFactory : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpAvtActionFactory* NewL( MUpnpAVControlPointObserver& aAVCPObserver,
	    									CUpnpAVControlPoint& aAVCP );
        /**
        * Destructor.
        */
        ~CUpnpAvtActionFactory();

    public: // New functions
        /**
        * Cretes and places in CU-stack a SetTransportURI action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aCurrentUri Resource URI.
        * @param aCurrentMetaData Resource metadata.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtSetTransportUriActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aCurrentUri,
            const TDesC8& aCurrentMetaData );
        /**
        * Cretes and places in CU-stack a SetNextTransportURI action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aNextUri Resource URI.
        * @param aNextMetaData Resource metadata.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtSetNextTransportUriActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aNextUri,
            const TDesC8& aNextMetaData );
        /**
        * Cretes and places in CU-stack a Seek action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aUnit 
        * @param aTarget 
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtSeekActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aUnit,
            const TDesC8& aTarget);
        /**
        * Cretes and places in CU-stack a GetMediaInfo action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtMediaInfoActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a GetTransportInfo action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtTransportInfoActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a GetPositionInfo action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtPositionInfoActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a GetDeviceCapabilities action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtDeviceCapabilitiesActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a GetTransportSettings action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtTransportSettingsActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a Stop action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtStopActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a Play action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aSpeed Play speed.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtPlayActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aSpeed);
        /**
        * Cretes and places in CU-stack a Pause action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtPauseActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a Record action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtRecordActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a Next action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtNextActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a Previous action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtPreviousActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);
        /**
        * Cretes and places in CU-stack a SetPlayMode action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aPlayMode Mode of play.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtSetPlayModeActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aPlayMode);
        /**
        * Cretes and places in CU-stack a SetRecordQualityMode action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @param aRecordMode Mode of recording.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtSetRecordModeActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId,
            const TDesC8& aRecordMode);
        /**
        * Cretes and places in CU-stack a GetTransportIDs action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaRenderer Action's target device.
        * @param aInstanceId Media Renderer instance number.
        * @return Error code. KErrNone if no errors.
        */
        TInt AvtTransportsActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaRenderer,
            TInt aInstanceId);

    private:
        
    /**
    * C++ default constructor.
        */
        CUpnpAvtActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
        						CUpnpAVControlPoint& aAVCP );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        //observer that is intrested about AVCP actions
        MUpnpAVControlPointObserver& iAVControlPointObserver;
        // AVCP main class
        CUpnpAVControlPoint& iAVCP;
    };
    
#endif      // C_CUPNPAVTACTIONFACTORY_H
    
    // End of File