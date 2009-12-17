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
* Description:  Callback interface to fill rule processing
*
*/



#ifndef M_CMFMITEMHANDLEROBSERVER_H
#define M_CMFMITEMHANDLEROBSERVER_H

 /**
  *  Call back observer
  *
  *  @lib cmfillmanager.lib
  *
  *  @since S60 5.1
  */      
NONSHARABLE_CLASS( MCmFmItemHandlerObserver )
    {
public:

    /**
     * Call back for observer
     *
     * @since S60 5.1
     * @param aDuplicate, ETrue if file is a duplicate
     * @return  None
     */    
    virtual void DuplicateL( const TBool aDuplicate ) = 0;  
       
    }; 


#endif // M_CMFMITEMHANDLEROBSERVER_H
