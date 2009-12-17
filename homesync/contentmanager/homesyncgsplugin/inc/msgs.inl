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
* Description:  Inline methods of Mediaservant GS plugin
*
*/


#ifndef MSGS_INL
#define MSGS_INL

// ----------------------------------------------------
// CMSGSInterface::NewL
// Creates General Settings plugin.
// ----------------------------------------------------
//    
CMSGSInterface* CMSGSInterface::NewL( const TUid aUid )
    {
	TAny* ext = REComSession::CreateImplementationL(
		aUid, _FOFF( CMSGSInterface, iDtor_ID_Key ) );

    CMSGSInterface* result = 
        reinterpret_cast< CMSGSInterface* >( ext );
	return result;
    }
    
    
// ----------------------------------------------------
// CMSGSInterface::DestroyPlugin
// Destroy Ecom plugin.
// ----------------------------------------------------
//    
void CMSGSInterface::DestroyPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );    
    }    

#endif // MSGS_INL

// End of File
