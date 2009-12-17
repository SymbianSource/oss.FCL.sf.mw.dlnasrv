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
* Description:  State handler
*
*/


#ifndef C_CUPNPSTATEHANDLER_H
#define C_CUPNPSTATEHANDLER_H

//  INCLUDES
#include "e32base.h"

class TUpnpContainerUpdateId;
class CUpnpContentDirectory;

class CUpnpStateHandler :
        public CBase              
    {
    public: // Constructor and destructor

        /**
        * Two-phased constructor.
        */
        CUpnpStateHandler (  );

        /**
        * Destructor.
        */
        ~CUpnpStateHandler ();  
                                                                
    public: // New functions
        static CUpnpStateHandler* NewL(CUpnpContentDirectory* aCd);         
        void  ConstructL(CUpnpContentDirectory* aCd);        
        /**
        * This fuction increases a value of SystemUpdateID. 
        * @since Series 60 2.6
        * @return a new value of SystemUpdateID 
        */
        TInt IncreaseSystemUpdateIdL();
        
        /**
        * This fuctions increases a container update ID.
        * @since Series 60 2.6
        * @param aContainerID a ID of the container whichs updateID is increased
        */
        void IncreaseContainerUpdateIdL( const TDesC8& aContainerID );
        
        
        RArray<TUpnpContainerUpdateId>& ContainerUpdateIds( );
        TInt ContainerUpdateId( const TDesC8& aContainerID );
        /**
        * This function is for informing service from evented statevariables.
        * @since Series 60 2.6
        * @param aVariableName Name of the evented statevariable.
        */
        void StateVariableEvented(const TDesC8& aVariableName);
        TBool IsEnable();
    private:        
        //array for containerUpdateId elements
        RArray<TUpnpContainerUpdateId>  iContainerUpdateIds;
        // This must be changed everytime when content changes.
        TUint iSystemUpdateId;
        // iContainerUpdateIds subarray. 
        //It contains info about some containers' changes,
        // collected before sending next NOTIFY event.             
        RArray<TUpnpContainerUpdateId>  iCurrentContainerUpdateIds;
        // this tells if statevariable is evented after last modification
        TBool iContainerUpdateIdsEvented;
        CUpnpContentDirectory* iCd;
    };
    

#endif      // C_CUPNPSTATEHANDLER_H
        
// End of File