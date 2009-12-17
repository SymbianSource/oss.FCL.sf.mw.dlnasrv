/** @file
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 *  Description : Authorization query dialog
 *
 */

#ifndef UPNPAUTHORISATIONNOTE_H
#define UPNPAUTHORISATIONNOTE_H

// INCLUDES
#include <e32base.h>	// For CActive, link against: euser.lib
#include "upnpnotifytimer.h"

// FORWARD DECLARATIONS
class CAknGlobalNote;
// CONSTANTS
#define KResourcePath KDC_RESOURCE_FILES_DIR
_LIT( KResourceFileName, "upnpnotifier.rsc" );
const TInt KAuthorizationNoteTimeout = 15;

// CLASS DECLARATION

/**
 * Observer of CUpnpAuthorizationNote results
 */
class MUpnpAuthorizationNoteObserver
    {
public:

    /**
     * A callback function.
     * @param aAuthorized The result of authorization.
     */
    virtual void NoteResponseL( TBool aAuthorized ) = 0;
    };

/**
 * Active wrapper of CAknGlobalNote YES/NO query dialog
 */
class CUpnpAuthorizationNote : public CActive,
        public MUpnpNotifyTimerObserver
    {
public:
    /**
     * Destructor.
     */
    ~CUpnpAuthorizationNote();

    /**
     * A two-phased constructor functions.
     */
    static CUpnpAuthorizationNote* NewL();
    static CUpnpAuthorizationNote* NewLC();

    /** 
     * Creates CAknGlobalNote dialog and performs
     * ShowNoteL() on it. The dialog is destroyed after user 
     * confirmation or 15 second timeout.
     *
     * @param aObserver ShowNoteL results observer
     */
    void ShowNoteL( MUpnpAuthorizationNoteObserver* aObserver );
private:
    // from MUpnpNotifyTimerObserver
    virtual void TimerEventL( CUpnpNotifyTimer* aTimer );
    // from CActive
    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );
    // other
    CUpnpAuthorizationNote();
    void ConstructL();
    void CancelNoteL();
    void GetResourceFilePathLC( RBuf& aPath );
    void ReadAuthorisationStringL( const TDesC& aFilename );
private:
    CUpnpNotifyTimer* iTimer;
    MUpnpAuthorizationNoteObserver* iObserver;
    CAknGlobalNote* iNote;
    TInt iNoteId;
    HBufC* iAuthorizationString;
    };

#endif // UPNPAUTHORISATIONNOTE_H
// End of File
