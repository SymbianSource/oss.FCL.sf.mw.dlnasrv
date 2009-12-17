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
* Description:      Definition of class CUPnPAVErrorHandler.
*
*/






#ifndef UPNPAVERRORHANDLER_H
#define UPNPAVERRORHANDLER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
enum TUPnPErrorCodeType
    {
    EUPnPGeneralError = 0,
    EUPnPConnectionManagerError,
    EUPnPContentDirectoryError,
    EUPnPRenderingControlError,
    EUPnPAVTransportError,
    EUPnPHTTPError
    };

// CLASS DECLARATION
    
/**
* UPnP AV Error Handler class header. Provides static methods to map UPnP
* error codes to corresponding Symbian (e32err.h) error codes.
*
* @since Series 60 3.0
*/
class UPnPAVErrorHandler
    {
    
    public:

        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given UPnP error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @param aErrorType TUPnPErrorCode type of the the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertToSymbianErrorCode( 
                                TInt aUPnPErrorCode,
                                TUPnPErrorCodeType aErrorType );
    
    private:
    
        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given general UPnP error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertGeneralErrorCode( TInt aUPnPErrorCode );
    
        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given Connection Manager error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertCMErrorCode( TInt aUPnPErrorCode );

        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given Content Directory error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertCDSErrorCode( TInt aUPnPErrorCode );

        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given Rendering Control error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertRCErrorCode( TInt aUPnPErrorCode );

        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given AV Transport error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertAVTErrorCode( TInt aUPnPErrorCode );

        /**
         * Returns the Symbian error code (e32err.h) corresponding to the 
         * given HTTP error code.
         *
         * @param aUPnPErrorCode TInt the UPnP error code
         * @return TInt the corresponding Symbian error code
         */
        static TInt ConvertHTTPErrorCode( TInt aUPnPErrorCode );
    };

#endif // UPNPAVERRORHANDLER_H

// End of File
