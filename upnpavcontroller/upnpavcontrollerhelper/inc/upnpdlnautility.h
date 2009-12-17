/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Utility for working with dlna profiles
*
*/






#ifndef UPNPDLNAUTILITY_H
#define UPNPDLNAUTILITY_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
    
/**
* Utility for working with dlna profiles and mime types
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class UPnPDlnaUtility
    {

public: // datatypes 

    enum TDlnaMode
        {
        EDMSMode = 0,
        EDMPMode,
        EDMUMode,
        EDMDMode
        };

public: // Business logic methods

         /**
         * Returns Dlna profiles for which playback on
         * device might be supported.         
         * The client must release the memory allocated for the items 
         * in the array.          
         * @since Series 60 3.2
         * @param aMode, dlna mode
         * @return Dlna profile string array
         */
        IMPORT_C static CDesC8Array& GetSupportedProfilesL( 
            const TDlnaMode aMode );
    
         /**
         * Returns ETrue if aMime is such a mime type for which playback on 
         * device might be supported.
         * @since Series 60 3.2
         * @param aMime mime type
         * @return TBool Can the mime type possibly be playable on device.
         */
        IMPORT_C static TBool IsSupportedMimeType( const TDesC8& aMime );
        
         /**
         * Returns ETrue if aProfile is such a DLNA profile for which 
         * playback on device might be supported.
         * @since Series 60 3.2
         * @param aProfile mime type
         * @return TBool Can the DLNA profile possibly be playable on device.
         */
        IMPORT_C static TBool IsSupportedDlnaProfile( const TDesC8& aProfile );

    };

#endif  // UPNPDLNAUTILITY_H

// End of File
