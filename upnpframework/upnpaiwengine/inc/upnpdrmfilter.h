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
* Description:  Header file for the CUpnpDrmFilter class.
*
*/


#ifndef C_UPNPDRMFILTER_H
#define C_UPNPDRMFILTER_H

#include <e32base.h>
#include <badesca.h>
#include <e32cmn.h>
#include <AknWaitNoteWrapper.h> 


/**
 *  Class for filtering Drm files out from a list of files. 
 *  @since S60 3.2
 *  @lib upnpaiwengine.dll
 */
class CUpnpDrmFilter  : public CActive,
                        public MAknBackgroundProcess
    {
    
public:

    /** state for filtering DRM */
    enum TDrmFilterState
        {
        EIdle = 1,
        EFilter,
        EFilterComplete,
        EFilterStop
        };
     
    /**
     * Two-Phased Constructor
     * @param aFiles (RPointerArray<TDesC>&) Reference to an array of
     *        TDesC objects containing the list of file names. 
     * @param aDRMProtectedFiles (RPointerArray<TDesC>&) Reference to an
     *        empty array of TDesC objects, the method will append DRM
     *        protected files on this list.
     * @return new instance of CUpnpDrmFilter
     */
    IMPORT_C static CUpnpDrmFilter* NewL( RPointerArray<TDesC>& aFiles,
            RPointerArray<TDesC>& aDRMProtectedFiles );

    /**
     * Constructor
     * @param aFiles (RPointerArray<TDesC>&) Reference to an array of
     *        TDesC objects containing the list of file names. 
     * @param aDRMProtectedFiles (RPointerArray<TDesC>&) Reference to an
     *        empty array of TDesC objects, the method will append DRM
     *        protected files on this list.
     */
     CUpnpDrmFilter( RPointerArray<TDesC>& aFiles,
                     RPointerArray<TDesC>& aDRMProtectedFiles );
    /**
     * Virtual Destructor
     */
    virtual ~CUpnpDrmFilter();
    
    /**
     * Start filter music files.
     */
    IMPORT_C void FilterDrmL();
        
private:
      
    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /** 
     * Filter 20 files every time 
     */
    void Filter();
    
    /**
     *  From CActive
     *  Handles an active object's request completion event.
     */
    void RunL();
    
    /**
     *  From CActive
     *  Handles an active error.
     */    
    void RunError();

    /**
     * From CActive
     * Implements cancellation of an outstanding request.
     */
   void DoCancel();

    /**
     * Performs incremental filter operation.
     */
    void DoTaskStep();

    /**
     * End filter operation.
     */
    void Complete();    

    /**
     * Completes one cycle of the process.
     * From MProgressDialogCallback.
     */
    void StepL(); 

    /**
     * Return true when the process is done. 
     * From MProgressDialogCallback.
     */
    TBool IsProcessDone() const;    
 
    /**
     * Show the wait note when filtering files.
     */
    void ShowWaitNoteL();
     

private: // data 

    /**
     * Reference to an array of TDesC objects containing 
     * the list of file names.
     */
    RPointerArray<TDesC>&     iFiles;               //Not own
    
   /**
    * Reference to an empty array of TDesC objects used to append DRM
    * protected files on this list.
    */
    RPointerArray<TDesC>&     iDRMProtectedFiles;   //Not own   

    
    /**
     * The state of current operation.
     */
    TDrmFilterState           iState;
    
    /**
     * The next start point to filter DRM file from iFiles array
     */
    TInt                      iStartPoint; 
    
    /**
     * The latest point has been filter 
     */
    TInt                      iCurrentPoint;

    /**
     * The total number of file need to filter.
     */
    TInt                      iTotalNum; 
    
    /**
     * Used to append the DRM file index in iFile array.
     */
    RArray<TInt>              iRemovedIndexArray;  
    
    /**
     * The total number of filtered files.
     */
    TInt                      iFilteredCount;
    
    /**
     * The total number of abnormal files except DRM files.
     */
    TInt                      iAbnormalFileCount;
    
    };

#endif // C_UPNPDRMFILTER_H
