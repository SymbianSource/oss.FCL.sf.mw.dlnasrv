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
* Description:  Definition for MediaServant document class
*
*/


#ifndef C_CMSDOCUMENT_H
#define C_CMSDOCUMENT_H


// INCLUDES
#include <AknDoc.h>

/**
 *  CMSDocument application class.
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CMSDocument ) : public CAknDocument
    {

    public:

        /**
         * Two-phased constructor.
         */
        static CMSDocument* NewL( CEikApplication& aApp );

        /**
         * Two-phased constructor.
         */
        static CMSDocument* NewLC( CEikApplication& aApp );
        
        /**
         * Destructor.
         */
        virtual ~CMSDocument();

    private:

        /**
         * Symbian default constructor.
         */
        CMSDocument( CEikApplication& aApp );

        /**
         * Symbian second phase constructor can leave.
         */
        void ConstructL();

// From base class CEikDocument

        /**
         * From CEikDocument
         * See base class definition
         */
        CEikAppUi* CreateAppUiL();
    };

#endif // C_CMSDOCUMENT_H

// End of File

