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
* Description:  Callback interface to fill rule processing
*
*/



#ifndef M_CMFMFILLRULEOBSERVER_H
#define M_CMFMFILLRULEOBSERVER_H

#include "cmfmcommon.h"

/**
 *  Fill rule observer class
 *  Callback interface class used to notify CmFmMain -class
 *  about fill rule processing status
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MCmFmFillRuleObserver )
    {

public:

    /**
     * Used to notify about status changes in fill rule processing
     *
     * @since S60 5.1
     * @param aStatus, status of processing
     * @return None
     */
    virtual void FillRuleProcessingStatus( 
        TCmFmFillRuleProcessing aStatus ) = 0;

    };


#endif // M_CMFMFILLRULEOBSERVER_H
