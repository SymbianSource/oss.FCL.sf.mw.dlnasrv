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
* Description:  Watches states of control point
*
*/



#ifndef C_CUPNPSTATEUPDATEHANDLER_H
#define C_CUPNPSTATEUPDATEHANDLER_H


//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpService;
class CUpnpAVControlPoint;

// CLASS DECLARATION

/**
*  UPnP event handler.
*  This class contain functions to handle UPnP events.
*
*  @since Series 60 2.6
*/
class CUpnpStateUpdateHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpStateUpdateHandler* NewL( 
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP );
        
        /**
        * Destructor.
        */
        virtual ~CUpnpStateUpdateHandler();

    public: // New functions
        
        /**
        * MAin function for event handling
        * @since Series 60 2.6
        * @param aService Service that has update events.
        * @return KErrNone 
        */
        TInt StateUpdated( CUpnpService*& aService );
        /**
        * Handler for Content Directory events.
        * @since Series 60 2.6
        * @param aDevice Device that has update events
        * @param aService Service that has update events
        * @return KErrNone, KErrNotsupported, KErrGeneral
        */
        TInt ContentDirectoryStateUpdated( 
            CUpnpDevice& aDevice,
            CUpnpService*& aService );
        /**
        * Handler for Connection Manager events.
        * @since Series 60 2.6
        * @param aDevice Device that has update events
        * @param aService Service that has update events
        * @return KErrNone, KErrNotsupported, KErrGeneral
        */
        TInt ConnectionManagerStateUpdated( 
            CUpnpDevice& aDevice, 
            CUpnpService*& aService);
        /**
        * Handler for Rendering Control events.
        * @since Series 60 2.6
        * @param aDevice Device that has update events
        * @param aService Service that has update events
        * @return KErrNone, KErrNotsupported, KErrGeneral
        */
        TInt RenderingControlStateUpdated( CUpnpDevice& aDevice, CUpnpService*& aService );
        /**
        * Handler for AV Transport events.
        * @since Series 60 2.6
        * @param aDevice Device that has update events
        * @param aService Service that has update events
        * @return KErrNone, KErrNotsupported, KErrGeneral
        */
        TInt AVTransportStateUpdated( CUpnpDevice& aDevice, CUpnpService*& aService );

    private:

        /**
        * C++ default constructor.
        */
	    CUpnpStateUpdateHandler( MUpnpAVControlPointObserver& aAVCPObserver, 
	    							CUpnpAVControlPoint& aAVCP );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    private:    // Data
       
        //observer that is intrested about AVCP actions
          MUpnpAVControlPointObserver& iAVControlPointObserver;
          //AVCP main class
          CUpnpAVControlPoint& iAVCP;
    };

#endif      // C_CUPNPSTATEUPDATEHANDLER_H   
            
// End of File