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
* Description:  Produces cm actions
*
*/



#ifndef C_CUPNPCMACTIONFACTORY_H
#define C_CUPNPCMACTIONFACTORY_H


//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpService;
class CUpnpAVControlPoint;

// CLASS DECLARATION

/**
*  Factory class.
*  This class creates Connection Manager actions.
*
*  @since Series 60 2.6
*/
class CUpnpCmActionFactory : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpCmActionFactory* NewL( 
            MUpnpAVControlPointObserver& aAVCPObserver, 
            CUpnpAVControlPoint& aAVCP );
        
        /**
        * Destructor.
        */
        ~CUpnpCmActionFactory();

    public: // New functions
        /**
        * This function creates an action for Connection Manager 
        *   GetProtocolInfo function.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aDevice Destination of action.
        * @return errorcode KErrNone if no errors. 
        */
        TInt CmProtocolInfoActionLC(CUpnpAction*& aAction, CUpnpDevice* aDevice);
        /**
        * This function creates an action for Connection Manager 
        *   GetCurrentConnections function.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aDevice Destination of action.
        * @return errorcode KErrNone if no errors. 
        */
        TInt CmCurrentConnectionsActionLC(CUpnpAction*& aAction, CUpnpDevice* aDevice);

        /**
        * This function creates an action for Connection Manager 
        *   PrepareForConnection function.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aDevice Destination of action.
        * @param aRemoteProtocolInfo.
        * @param aPeerConnectionManager.
        * @param aPeerConnectionID connection ID.
        * @param aDirection connection direction.
        * @return errorcode KErrNone if no errors. 
        */
        TInt CmPrepareConnectionActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aDevice,
            const TDesC8& aRemoteProtocolInfo,
            const TDesC8& aPeerConnectionManager,
            TInt aPeerConnectionID,
            const TDesC8& aDirection);
        /**
        * This function creates an action for Connection Manager 
        *   ConnectionComplete function.
        * @since Series 60 2.6
        * @param aAction created action
        * @param aDevice Action's target device.
        * @param aConnectionId target connection.
        * @return errorcode KErrNone if no errors. 
        */
        TInt CmConnectionCompleteActionLC(
            CUpnpAction*& aAction, 
            CUpnpDevice* aDevice,
            TInt aConnectionId);
        /**
        * This function creates an action for Connection Manager 
        *   GetCurrentConnections function.
        * @since Series 60 2.6
        * @param aAction created action
        * @param aDevice Action's target device.
        * @param aConnectionId target connection.
        * @return errorcode KErrNone if no errors. 
        */
        TInt CmCurrentConnectionInfoActionLC(
            CUpnpAction*& aAction, 
            CUpnpDevice* aDevice,
            TInt aConnectionId);
        
    private:

        /**
        * C++ default constructor.
        */
	    CUpnpCmActionFactory( 
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    private:    // Data
        // Observer that is intrested about AVCP actions.
        MUpnpAVControlPointObserver& iAVControlPointObserver;
        // AVCP main class.
        CUpnpAVControlPoint& iAVCP;
    };

#endif      // C_CUPNPCMACTIONFACTORY_H   
            
// End of File