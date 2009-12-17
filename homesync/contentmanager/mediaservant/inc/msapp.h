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
* Description:  Definition for cmediaservant class
*
*/


#ifndef C_CMSAPP_H
#define C_CMSAPP_H


// INCLUDES
#include <aknapp.h>

/**
 * CMediaServant application class.
 * Provides factory to create concrete document object.
 *
 * @since S60 5.1
 */
class CMediaServantApp : public CAknApplication
    {

private:

        /**
        * Creates CCDSDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * Returns application's UID (KMediaServantUID3).
        * @return The value of KMediaServantUID3.
        */
        TUid AppDllUid() const;

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

#endif   // C_CMSAPP_H


