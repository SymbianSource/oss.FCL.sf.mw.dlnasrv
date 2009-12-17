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
* Description:  Gallery notifier class
*
*/



#ifndef __CMFMGLXNOTIFIER_H
#define __CMFMGLXNOTIFIER_H

#include <e32base.h>

/**
 *  CCmFmGlxNotifier class
 *  Part of Fill manager. CCmFmGlxNotifier manages
 *  Gallery notify events
 *
 *  @lib cmfillmanager.lib
 *
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CCmFmGlxNotifier ): public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFS, file session
     * @return  pointer to CCmFmGlxNotifier class
     */
    static CCmFmGlxNotifier* NewL( RFs& aFS );
    
    /**
     * Two-phased constructor.
     *
     * @since S60 5.1
     * @param aFS, file session
     * @return  pointer to CCmFmGlxNotifier class
     */
    static CCmFmGlxNotifier* NewLC( RFs& aFS );

    /**
     * Destructor.
     */
    virtual ~CCmFmGlxNotifier();
    
public: 

    /**
     * Notifying Media gallery about filled files
     *
     * @since S60 5.1
     * @param aFullPath, path of the filled files
     * @return None
     */        
    void NotifyGalleryL( const TDesC& aFullPath );

    /**
     * Notifying Media gallery about filled files
     *
     * @since S60 5.1
     * @param None
     * @return None
     */        
    void NotifyGalleryL();

private:

    /**
     * Performs the first phase of two phase construction.
     */
    CCmFmGlxNotifier( );

    /**
     * Performs the second phase construction.
     * @since S60 5.1
     * @param aFs, file server
     */
    void ConstructL( RFs& aFS );
   
    };

#endif //  __CMFMGLXNOTIFIER_H
