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
* Description:      file sharing engine active object class defition
 *
*/

#ifndef __UPNPCONTENTSHARERAO_H__
#define __UPNPCONTENTSHARERAO_H__


// Include Files
#include <e32base.h>
#include <badesca.h>  //for CDesCArray

#include "upnpsharingcallback.h"
#include "upnpcontentserverdefs.h"
#include "upnpsharingrequest.h"

#include "upnpsharingalgorithm.h"


// FORWARD DECLARATIONS
class MUpnpSharingCallback;


// CLASS DECLARATION

/**
*  Active object class for file sharing
*  @since S60 5.2
*/
NONSHARABLE_CLASS( CUpnpContentSharerAo ) : public CActive
{

public: // Constructors and destructor
    
    /**
     * Two-phased constructor.
     * @since S60 5.2
     * @param aEngine Callback to handler
     * @param aSharingAlgorithm Pointer to SharingAlgorithm
     */
    static CUpnpContentSharerAo *NewL( 
        MUpnpSharingCallback* aEngine,
        MUpnpSharingAlgorithm* aSharingAlgorithm );
    
    /**
     * Destructor.
     * @since S60 5.2
     */ 
    virtual ~CUpnpContentSharerAo();
    
protected: //From CActive
    /**
     * See CActive
     */
    virtual void RunL();
    
    /**
     * See CActive
     */
    virtual void DoCancel();
    
    /**
     * See CActive
     */
    TInt RunError(TInt aError);

public: //new functions
    
    /**
     * Starts file sharing process
     * @since S60 5.2
     * @param aSharingRequest List of files to be shared
     */
    void StartSharing( const CUpnpSharingRequest *aSharingRequest );
    
    /**
     * Checks that if sharing is ongoing
     * @since S60 5.2
     * @return ETrue if sharing is ongoing, otherwise EFalse
     */
    TBool SharingOngoing();

    /**
     * Stops sharing process
     * @since S60 5.2
     */
    void StopSharing();
    
    /**
     * Forces to set progress in next file sharing process 
     * @since S60 5.2
     */
    void ForceSetProgress();

private:
    
    /**
     * C++ default constructor.
     * @since S60 5.2
     * @param aEngine Pointer to handler callback
     * @param aSharingAlgorithm Pointer to SharingAlgorithm
     */
    CUpnpContentSharerAo( MUpnpSharingCallback *aEngine, 
        MUpnpSharingAlgorithm* aSharingAlgorithm );
    
    /**
     * By default Symbian 2nd phase constructor is private.
     * @since S60 5.2
     */
    void ConstructL();
    
    /**
     * Writes sharing status to file with upnpselectionwriter
     * @since S60 5.2
     */
    void WriteSharingStatusL();
    
    /**
     * Shares and unshares one file from each array
     * @since S60 5.2
     * @return error code
     */
    TInt DoSingleOperation();
    
    /**
     * Sets progress to engine
     * @since S60 5.2
     */
    void SetProgressL();

    /**
     * Performs complete callback
     * @param aPtr Pointer to CUpnpContentSharerAo instance
     * @return KErrNone
     */
    static TInt CompleteL( TAny* aPtr );
    
private: 
    
    /**
     * Pointer to calling handler
     * not owned
     */
    MUpnpSharingCallback *iSharingEngine;
      
    /**
     * Pointer to request under work. Request contains lists of file to be 
     * shared and unshared.
     * not owned
     */
    const CUpnpSharingRequest *iSharingRequest;
       
    /**
     * Pointer to sharing algorith which performs the actual sharing.
     * Not owned.
     */
    MUpnpSharingAlgorithm* iSharingAlgorithm;
    
    /**
     * Index number of current file in share file array
     */
    TInt iShareIndex;
    
    /**
     * Index number of current file in unshare file array
     */
    TInt iUnshareIndex;
    
    /**
     * Flag if we should cancel current operation
     */
    TBool iCancel;
    
    /**
     * Currently processed progress percent
     */
    TInt iCurrentProgressPercent;
  
    /**
     * Idle for complete callback
     * Owned.
     */
    CIdle* iIdle;

    /**
     * Error code for callback
     */
    TInt iError;
    
  };

#endif  // __UPNPCONTENTSHARINGAO_H__

// End of file
