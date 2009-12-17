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
* Description:  Callback interface to store file processing
*
*/



#ifndef M_CMSMFILERULEPROCESSINGOBSERVER_H
#define M_CMSMFILERULEPROCESSINGOBSERVER_H

#include "cmsmcommon.h"

/**
 *  Callback interface class used to notify CmSmMain -class
 *  about store file processing status
 *
 *  @lib cmstoremanager.lib
 *
 *  @since S60 v5.1
 */
class MCmSmFileProcessingObserver
    {

public:

    /**
     * Used to notify about status changes in fill rule processing
     *
     * @since S60 5.1
     * @param aStatus, status of the file processing
     * @return None
     */
    virtual void FileProcessingStatus( 
        TCmSmFileProcessingStatus aStatus ) = 0;
    };

#endif // M_CMSMFILERULEPROCESSINGOBSERVER_H
