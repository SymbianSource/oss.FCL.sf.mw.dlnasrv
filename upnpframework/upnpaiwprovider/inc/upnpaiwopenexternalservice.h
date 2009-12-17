/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the UpnpAiwOpenExternalService.
*
*/


#ifndef _UPNP_AIW_OPEN_EXTERNAL_SERVICE_H
#define _UPNP_AIW_OPEN_EXTERNAL_SERVICE_H

//  INCLUDES
#include <AiwServiceIfBase.h>

// FUNCTION PROTOTYPES
class CUPnPAiwEngine;

// CLASS DECLARATION

/**
*  CUPnPAiwOpenExternalService
*  The class implements the "Open external" service of the
*  UPnP AIW Media Provider.
*  @since Series S60 3.0
*/
class CUPnPAiwOpenExternalService : public CAiwServiceIfBase
    {

    public: // Constructors and destructor

        // Two-phased constructor.
        static CUPnPAiwOpenExternalService* NewL();
        
        // Destructor.
        virtual ~CUPnPAiwOpenExternalService();

    public: // Methods from the CAiwServiceIfMenu

        // Unnecessary method
        void InitialiseL( MAiwNotifyCallback& /*aFrameworkCallback*/,
                          const RCriteriaArray& /*aInterest*/) {}

        /**
        * Executes generic service commands included in criteria.
        * @param aCmdId Command to be executed
        * @param aInParamList Input parameters, can be an empty list
        * @param aOutParamList Output parameters, can be an empty list
        * @param aCmdOptions Options for the command, see KAiwCmdOpt* 
        *        constants.
        * @param aCallback callback for asynchronous command handling, 
        *        parameter
        *        checking, etc.
        * @see enum TServiceCmdOptions in GENERICPARAM.HRH
        * @exception KErrArgument if callback is missing when required.
        * @exception KErrNotSupported if no provider support service
        */
        void HandleServiceCmdL( const TInt& aCmdId,
                                const CAiwGenericParamList& aInParamList,
                                CAiwGenericParamList& aOutParamList,
                                TUint aCmdOptions = 0,
                                const MAiwNotifyCallback* aCallback = NULL);

    private: // construction

        // Constructor
        CUPnPAiwOpenExternalService();

        // ConstructL
        void ConstructL();

    private: // Data members

        // reference to the engine component
        CUPnPAiwEngine* iEngine;                // not owned (singleton)

    };

#endif // _UPNP_AIW_OPEN_EXTERNAL_SERVICE_H

// End of file
