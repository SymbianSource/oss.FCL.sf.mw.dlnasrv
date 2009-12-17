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
* Description:  Header file for the UpnpAiwPlayOnExternalBaseService.
*
*/


#ifndef _UPNP_AIW_PLAY_ON_EXTERNAL_BASE_SERVICE_H
#define _UPNP_AIW_PLAY_ON_EXTERNAL_BASE_SERVICE_H

//  INCLUDES
#include <AiwServiceIfBase.h>
#include "upnpaiwengineobserver.h"

// FUNCTION PROTOTYPES
class CUPnPAiwEngine;
class CUpnpDrmFilter;

// CLASS DECLARATION

/**
*  CUPnPAiwPlayOnExternalBaseService
*  The class implements the "Play on external" base service of the
*  UPnP AIW Media Provider.
*  @since Series S60 3.0
*/
class CUPnPAiwPlayOnExternalBaseService : public CAiwServiceIfBase,
                                          public MUPnPAiwEngineObserver
    {

    public: // Constructors and destructor

        // Two-phased constructor.
        static CUPnPAiwPlayOnExternalBaseService* NewL();

        // Destructor.
        virtual ~CUPnPAiwPlayOnExternalBaseService();

    public: // Methods from the CAiwServiceIfBase

        // Unnecessary method
        void InitialiseL( MAiwNotifyCallback& /*aFrameworkCallback*/,
                          const RCriteriaArray& /*aInterest*/ ) {}

        /**
        * Executes generic service commands included in criteria.
        * @param aCmdId Command to be executed
        * @param aInParamList Input parameters, can be an empty list
        * @param aOutParamList Output parameters, can be an empty list
        * @param aCmdOptions Options for the command, see KAiwCmdOpt* 
        *        constants.
        * @param aCallback callback for asynchronous command handling, 
        *        parameter checking, etc.
        * @see enum TServiceCmdOptions in GENERICPARAM.HRH
        * @exception KErrArgument if callback is missing when required.
        * @exception KErrNotSupported if no provider support service
        */
        void HandleServiceCmdL( const TInt& aCmdId,
                                const CAiwGenericParamList& aInParamList,
                                CAiwGenericParamList& aOutParamList,
                                TUint aCmdOptions = 0,
                                const MAiwNotifyCallback* aCallback = NULL);

    public:  // MUPnPAiwEngineObserver

        /**
         * Indicates that the play operation is complete.
         *
         * @since Series 60 3.0
         * @param aStatus status information
         * @return None
         */
        void PlayCompleteL( TInt aStatus );

        /**
         * Indicates that the connection with the target UPnP
         * device has been lost.
         *
         * @since Series 60 3.0
         * @param None
         * @return None
         */
        void ConnectionLostL();

    private: // private methods

       /**
       * Deletes the contents of the iFileNames pointer array
       *
       * @since Series 60 3.0
       * @param None
       * @returns None
       */
       void EmptyFileNameArray();


    private: // construction

        // Constructor
        CUPnPAiwPlayOnExternalBaseService();

        // ConstructL
        void ConstructL();

    private: // Data members

        // reference to the engine component
        CUPnPAiwEngine* iEngine;                    // not owned (singleton)

        // iActiveDrmFilter does the filtering DRM file work
        CUpnpDrmFilter* iActiveDrmFilter;           // owned
        
        // reference to the client (call back reference)
        MAiwNotifyCallback* iClient;                // not owned

        // The list of file names received as a parameter
        RPointerArray<TDesC16> iFileNames;          // owned

        // Filename of the file that is played
        HBufC16* iFileName;                         // owned
    };

#endif // _UPNP_AIW_PLAY_ON_EXTERNAL_BASE_SERVICE_H

// End of file
