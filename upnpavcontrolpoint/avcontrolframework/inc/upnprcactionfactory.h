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
* Description:  Produces rc actions
*
*/



#ifndef C_CUPNPRCACTIONFACTORY_H
#define C_CUPNPRCACTIONFACTORY_H


//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpService;
class CUpnpAVControlPoint;

// CLASS DECLARATION

/**
*  RC action factory class.
*  This class provides a Rendering Control action creation services.
*
*  @since Series 60 2.6
*/
class CUpnpRcActionFactory : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpRcActionFactory* NewL( MUpnpAVControlPointObserver& aAVCPObserver,
	    									CUpnpAVControlPoint& aAVCP );
        
        /**
        * Destructor.
        */
        ~CUpnpRcActionFactory();

    public: // New functions

        /**
        * Creates a GetVolume action.
        * @since Series 60 2.6
        * @param aAction Pointer for returning created action.
        * @param aDevice Target of the action.
        * @param aInstanceID rendering instance.
        * @param aChannel audio channel.
        * @return error code. KErrNone if no errors
        */
        TInt RcGetVolumeActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aDevice,
            TInt aInstanceID,
            const TDesC8& aChannel);
        /**
        * Creates a SetVolume action.
        * @since Series 60 2.6
        * @param aAction Pointer for returning created action.
        * @param aDevice Target of the action.
        * @param aInstanceID rendering instance.
        * @param aChannel audio channel.
        * @param aVolume volume.
        * @return error code. KErrNone if no errors
        */
        TInt RcSetVolumeActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aDevice,
            TInt aInstanceID,
            const TDesC8& aChannel,
            TInt aVolume);
         /**
        * Creates a GetMuteAction.
        * @since Series 60 2.6
        * @param aAction Pointer for returning created action.
        * @param aDevice Target of the action.
        * @param aInstanceID rendering instance.
        * @param aChannel audio channel.
        * @return error code. KErrNone if no errors
        */
        TInt RcGetMuteActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aDevice,
            TInt aInstanceID,
            const TDesC8& aChannel);
        /**
        * ?Creates a SetMute action.
        * @since Series 60 2.6
        * @param aAction Pointer for returning created action.
        * @param aDevice Target of the action.
        * @param aInstanceID rendering instance.
        * @param aChannel audio channel.
        * @param aMute 
        * @return error code. KErrNone if no errors
        */
        TInt RcSetMuteActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aDevice,
            TInt aInstanceID,
            const TDesC8& aChannel,
            const TDesC8& aMute);


    private:

        /**
        * C++ default constructor.
        */
	    CUpnpRcActionFactory( MUpnpAVControlPointObserver& aAVCPObserver, 
	    						CUpnpAVControlPoint& aAVCP );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    private:    // Data
        //observer that is intrested about AVCP actions.
        MUpnpAVControlPointObserver& iAVControlPointObserver;
        // AVCP main class.
        CUpnpAVControlPoint& iAVCP;

    };

#endif      // C_CUPNPRCACTIONFACTORY_H  
            
// End of File