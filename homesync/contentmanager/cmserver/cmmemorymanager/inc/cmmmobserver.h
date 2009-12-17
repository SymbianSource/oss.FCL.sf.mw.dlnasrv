/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Callback interface
*
*/


#ifndef M_CMMMOBSERVER_H
#define M_CMMMOBSERVER_H

/**
 *  Callback interface class used to notify derived class
 *  about status changes in MemoryManager actions
 *  @lib cmmemorymanager.lib
 *  @since S60 v3.0
 */
class MCmMmObserver
    {

public:

    /**
     * Delete completed notification
     *
     * @since S60 3.0
     * @param aErr
     */
    virtual void DeleteCompleteL( TInt aErr ) = 0;

    /**
     * Shrink completed notification
     *
     * @since S60 3.0
     * @param aErr
     */
    virtual void ShrinkCompleteL( TInt aErr ) = 0;
    };

#endif // M_CMMMOBSERVER_H
