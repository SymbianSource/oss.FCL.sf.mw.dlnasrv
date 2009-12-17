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
* Description:  CUpnpActionResponseHandler handles service specific responses
*
*/



#ifndef C_CUPNPACTIONRESPONSEHANDLER_H
#define C_CUPNPACTIONRESPONSEHANDLER_H


//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS
class CUpnpService;
class CUpnpAVControlPoint;
// CLASS DECLARATION

/**
*  Action response handler class.
*  This class contains functionality to handle UPnP action responses.
*
*  @since Series 60 2.6
*/
class CUpnpActionResponseHandler : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpActionResponseHandler* NewL(
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP );
        
        /**
        * Destructor.
        */
        virtual ~CUpnpActionResponseHandler();

    public: // New functions
        /**
        * The entry point for action response handling.
        * @since Series 60 2.6
        * @param aAction response UPnP action.
        * @return errorcode. KErrNone if no errors. 
        */
        TInt ActionResponseReceived(CUpnpAction* aAction);
        /**
        * Rendering Control response handler.
        * @since Series 60 2.6
        * @param aAction response UPnP action.
        * @return errorcode. KErrNone if no errors. 
        */
        TInt RenderingControlResponse(CUpnpAction* aAction);
        /**
        * AV Transport response handler.
        * @since Series 60 2.6
        * @param aAction response UPnP action.
        * @return errorcode. KErrNone if no errors. 
        */
        TInt AVTransportResponse(CUpnpAction* aAction);
        /**
        * Connection Manager response handler.
        * @since Series 60 2.6
        * @param aAction response UPnP action.
        * @return errorcode. KErrNone if no errors. 
        */
        TInt ConnectionManagerResponse(CUpnpAction* aAction);
        /**
        * Content Directory response handler.
        * @since Series 60 2.6
        * @param aAction response UPnP action.
        * @return errorcode. KErrNone if no errors. 
        */
        TInt ContentDirectoryResponse(CUpnpAction* aAction);

    private:

        /**
        * C++ default constructor.
        */
	    CUpnpActionResponseHandler( MUpnpAVControlPointObserver& aAVCPObserver,
	    							CUpnpAVControlPoint& aAVCP );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();
        
        //observer that is intrested about AVCP actions.
        MUpnpAVControlPointObserver& iAVControlPointObserver;
        // AVCP main class.
        CUpnpAVControlPoint& iAVCP;
    };

#endif      // C_CUPNPACTIONRESPONSEHANDLER_H 
            
// End of File