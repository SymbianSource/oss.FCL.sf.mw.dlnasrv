/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      UPnP framework custom periodic timer
*
*/






#ifndef __UPNPPERIODIC_H__
#define __UPNPPERIODIC_H__

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
/**
*  Periodic timer active object.
*
*
*  @lib  upnputilities.lib
*  @since 3.2.3
*/

class CUPnPPeriodic : public CPeriodic
    {
    
  public:  // Constructors and destructor    
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CUPnPPeriodic* NewL( TInt aPriority );

        /**
        * Destructor.
        */
        virtual ~CUPnPPeriodic();

    private: // From CPeriodic base class CActive.

        /**
        * From CActive, called if CUPnPPeriodic function RunL() leaves.
        * @param aError Standard Symbian OS error code
        * @return Error code KErrNone
        */
        TInt RunError( TInt aError ); 

    private: // Private methods.
        
        /**
        * C++ constructor.
        */
        CUPnPPeriodic( TInt aPriority );

        /**
        * Symbian OS 2nd phase constructor that can leave.
        */
        void ConstructL();
    
    } ;

#endif // __UPNPPERIODIC_H__

// End of File
