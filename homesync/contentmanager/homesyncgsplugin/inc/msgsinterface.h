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
* Description:  Interface for Mediaservant application
*
*/


#ifndef MSGSINTERFACE_H
#define MSGSINTERFACE_H

#include <e32std.h>
#include <aknview.h>


/**
 *  GS plugin interface
 *
 *  @lib msgsplugin.lib
 */
class CMSGSInterface : public CAknView
    {
    public: // New functions

        /**
        * Wraps ECom object instantiation.
        * @param aUid Specifies the concrete implementation.
        */
        inline static CMSGSInterface* NewL( const TUid aUid );       
        
        
        /**
        * Notifies framework that this instance is being destroyed
        * and resources can be released.
        */
        inline void DestroyPlugin();        
        
	protected:
	    /** iDtor_ID_Key Instance identifier key. When instance of an
	     *               implementation is created by ECOM framework, the
	     *               framework will assign UID for it. The UID is used in
	     *               destructor to notify framework that this instance is
	     *               being destroyed and resources can be released.
         */	
	    TUid iDtor_ID_Key;     
    };

#include "msgs.inl"
    
#endif // MSGSINTERFACE_H

// End of File

