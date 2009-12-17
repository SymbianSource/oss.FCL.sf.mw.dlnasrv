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
* Description:  Declares ConnectionManager class
*
*/
class CUpnpSecurityManager;


#ifndef C_CUPNPCM_H
#define C_CUPNPCM_H

// INCLUDES
#include "upnpserviceimplementation.h"

const TInt KGranularity = 10;
const TInt KBufExpandSize = 128;
_LIT8(KConnectionManagerType, "ConnectionManager:1" );

/**
*  Declares ConnectionManager class
*  This service-type enables modeling of streaming capabilities of A/V devices, 
*  and binding of those capabilities between devices.
*
*  @upnpcm.lib
*  @since Series60 2.6
*/
NONSHARABLE_CLASS( CUpnpCM ): public CUpnpServiceImplementation
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUpnpCM* NewL( CUpnpDevice& aParentService, 
                                       const TDesC8& aMediaTypes,
                                       const TDesC& aDescriptionPath );
        /**
        * Destructor.
        */
        ~CUpnpCM();
                
    public: // Functions from CUpnpServiceImplementation

        /**
        * ActionReceivedLD
        * From CUpnpServiceImplementation,Handles actions received from the 
        * ServiceFramework, see coments in base class
        * @since Series60 2.0
        * @param aAction Pointer to Action element that is going to be handled
        */
        void ActionReceivedLD( CUpnpAction* aAction );

    public: // Functions from CConnectionManager

        /**
        * GetProtocolInfo
        * From CConnectionManager,Handles GetProtocolInfo actions by setting
        * the needed information to aAction
        * @since Series60 2.0
        * @param aAction Instructions
        * @return TUpnpErrocCode, Returns an error code if the operation has
        * failed
        */
        TUpnpErrorCode GetProtocolInfoL( CUpnpAction* aAction );

        /**
        * PrepareForConnection
        * From CConnectionManager,Handles PrepareForConnection actions 
        * according to instructions
        * @since Series60 2.0
        * @param aAction Instructions
        * @return TUpnpErrocCode, Returns an error code if the operation
        * has failed
        */
        TUpnpErrorCode PrepareForConnection( CUpnpAction* aAction );

        /**
        * ConnectionComplete
        * From CConnectionManager,Releases the connection specified in the
        * aAction
        * @since Series60 2.0
        * @param aAction Instructions
        * @return TUpnpErrocCode, Returns an error code if the operation has
        * failed
        */
        TUpnpErrorCode ConnectionComplete( CUpnpAction* aAction );

        /**
        * GetCurrentConnectionIDsL
        * From CConnectionManager,Check the current connections and inserts a 
        * CSV list of them into the aAction
        * @since Series 602.0
        * @param aAction Instructions
        * @return TUpnpErrocCode, Returns an error code if the operation has 
        * failed
        */
        TUpnpErrorCode GetCurrentConnectionIDsL( CUpnpAction* aAction );

        /**
        * GetCurrentConnectionIDsL
        * From CConnectionManager,Checks the ConnectionInfo of the connection 
        * specified in the aAction and sets the data to aAction
        * @since Series60 2.0
        * @param aAction Instructions
        * @return TUpnpErrocCode, Returns an error code if the operation has 
        * failed
        */
        TUpnpErrorCode GetCurrentConnectionInfoL( CUpnpAction* aAction );
        
	private: 	
		/**
        * C++ default constructor.
        */
        CUpnpCM(CUpnpDevice& aDevice);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aParentService Reference to parent service
        */
        void ConstructL( const TDesC8& aMediaTypes, const TDesC& aDescriptionPath );    

    private: 

        HBufC8* CreateProtocolInfosL(const TDesC8& aMediaTypes );
		void GetMediaListFromStringL(CPtrC8Array* aList, const TDesC8& aMediaTypes);

    private:
        CUpnpSecurityManager* iSecurityManager;
		
	};

#endif // C_CUPNPCM_H

// End of File
