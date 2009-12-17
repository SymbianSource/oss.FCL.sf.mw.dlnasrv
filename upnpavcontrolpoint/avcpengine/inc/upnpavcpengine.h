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
* Description:  Implementation of CServer2
*
*/


#ifndef C_CUPNPAVCPENGINE_H
#define C_CUPNPAVCPENGINE_H

// INCLUDES
#include <e32base.h>
#include "upnpavcpengine.pan"

// FORWARD DECLARATIONS
class CUpnpAVCPEngineSession;
class CUpnpAVCPManager;

// CLASS DECLARATION
/**
*  Simple imlementatation of server from 
* Symbian Client/Server architecture.
*
*  @since Series 60 3.1
*/
class CUpnpAVCPEngine: public CServer2
    {
	public :
		/**
		* Two-phased constructor.
		*/	
		static CUpnpAVCPEngine* NewLC();
		/**
		* Destructor.
		*/			
		~CUpnpAVCPEngine();
		/**
		* Register new server-side session 
		* it is used for manage life-time of server
		*/
		void IncrementSessions(CUpnpAVCPEngineSession* aObserver);
		/**
		* Unregister new server-side session
		* it is used for manage life-time of server		
		*/	
		void DecrementSessions(CUpnpAVCPEngineSession* aObserver);
		/**
		* Function stars server, creates CleanupStack and
		* executes ThreadFunctionL() in TRAP
		*/
		static TInt ThreadFunction( TAny* aStarted );
		/**
		* Function creates CActiveScheduler, CUpnpAVCPEngine and starts server.
		*/
		static void ThreadFunctionL();
	
	protected:  // From CActive
		/**
		* From CActive notify about leaves in CService2::ServiceL
		*/
		TInt RunError( TInt aError );
	
	private: // New methods
		/**
		* C++ default constructor.
		*/			
		CUpnpAVCPEngine( TInt aPriority );
		/**
		* Symbian 2nd phase constructor.
		*/		
		void ConstructL() ;
		/**
		* Panic client thread
		*
		* @param aMessage message to be panic
		* @param aReason panic code
		*/
		static void PanicClient( const RMessage2& aMessage, TAVCPEnginePanic aReason );
		/**
		* Panic server
		*
		* @param aPanic panic code
		*/
		static void PanicServer( TAVCPEnginePanic aPanic );
	
	private: // From CServer
		/**
		* From CActive notify about leaves in ServiceL
		*/	
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
			
	private: // Data
		// count of session 
		TInt        iSessionCount;
		// AVCP manager
		CUpnpAVCPManager* iAVCPManger;
    };


#endif  // C_CUPNPAVCPENGINE_H