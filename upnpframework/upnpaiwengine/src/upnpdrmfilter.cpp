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


// INCLUDE FILES
// System
#include <e32base.h>
#include <f32file.h>

// upnpframework / avcontroller helper api
#include "upnpfileutility.h"

// aiwengine internal
#include <upnpaiwengineresources.rsg>   // Upnp Fw / UpnpAiwEngine
#include "upnpdrmfilter.h"

// logging
_LIT( KComponentLogfile,                "upnpaiwengine.log" );
#include "upnplog.h"

// Constants
// The number of filtering file Every loop. 
const TInt KStepFileNumber = 20; 

// ---------------------------------------------------------------------------
// Two-Phased Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUpnpDrmFilter* CUpnpDrmFilter::NewL(
          RPointerArray<TDesC>& aFiles,
          RPointerArray<TDesC>& aDRMProtectedFiles )
    {    
    __LOG( "CUpnpDrmFilter::NewL Begin" );
    
    CUpnpDrmFilter* self =
        new( ELeave )CUpnpDrmFilter( aFiles, aDRMProtectedFiles );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );   
    return self;    
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CUpnpDrmFilter::CUpnpDrmFilter( RPointerArray<TDesC>& aFiles,
                                RPointerArray<TDesC>& aDRMProtectedFiles ):
    CActive( CActive::EPriorityStandard ),
    iFiles( aFiles ),
    iDRMProtectedFiles( aDRMProtectedFiles )
    {  
    __LOG( "CUpnpDrmFilter::CUpnpDrmFilter Begin" );
    
    iState = EIdle;
    CActiveScheduler::Add( this );
    
    __LOG( "CUpnpDrmFilter::CUpnpDrmFilter End" );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUpnpDrmFilter::~CUpnpDrmFilter()
    {
    __LOG( "CUpnpDrmFilter::~CUpnpDrmFilter Begin" );
    
    iRemovedIndexArray.Close(); 
    
    __LOG( "CUpnpDrmFilter::~CUpnpDrmFilter End" );
    }

// ---------------------------------------------------------------------------
// Start the delete operation
// ---------------------------------------------------------------------------
//
EXPORT_C void CUpnpDrmFilter::FilterDrmL()
    {   
    __LOG( "CUpnpDrmFilter::FilterDrmL Begin" );
    
    iFilteredCount = 0;
    iTotalNum = iFiles.Count();
    iState = EFilter;    
    
    // Start filter.
    TRequestStatus* status = &iStatus;
    *status = KRequestPending;
    User::RequestComplete( status, KErrNone );    
    SetActive();  
    ShowWaitNoteL();
    
    if (iFilteredCount > 0 )
        {
        CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog();
        dlg->ExecuteDlgLD( R_AIW_DRM_SKIP_INFO_NOTE );
        }
    
    __LOG( "CUpnpDrmFilter::FilterDrmL End" );
    }

// ---------------------------------------------------------------------------
// 2nd Phase Constructor
// ---------------------------------------------------------------------------
//
void CUpnpDrmFilter::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Filters the drm file name from the file name array
// which file will be copied to the home network.
// ---------------------------------------------------------------------------
//
void CUpnpDrmFilter::Filter()
    {  
    __LOG( "CUpnpDrmFilter::Filter Begin" );
    
    if ( iTotalNum <= KStepFileNumber + iStartPoint )
        {
        for ( TInt index = 0; index < iTotalNum - iStartPoint; index++ )
            {
            TBool isProtected = ETrue;
            TRAPD( drmError,
                   isProtected = UPnPFileUtility::IsFileProtectedL(
                       *iFiles[index+iStartPoint] ) );
            
            iCurrentPoint++;
            
            __LOG1( "[UpnpAiwEngine]\t CUpnpDrmFilter::\
                   Filter complete , iCurrentPoint :%d" , iCurrentPoint );        
            
            if ( drmError || isProtected )
                {                
                __LOG1( "[UpnpAiwEngine]\t CUpnpDrmFilter::\
                       Filter , drmError :%d" , drmError );        
                
                // Add the file to aDRMProtectedFiles array.
                iDRMProtectedFiles.AppendL( iFiles[index + iStartPoint] );
                // save the drm file index to array. These files will be removed
                // from the aFiles array soon.
                iRemovedIndexArray.AppendL( index + iStartPoint );
                }            
            if ( KErrNone != drmError )
                {
                iAbnormalFileCount ++;
                }
            }
        iState = EFilterComplete; 
        iStartPoint = 0;        
        }
    else
        {
        for ( TInt index = 0; index < KStepFileNumber; index++ )
            {
            TBool isProtected = ETrue;
            TRAPD( drmError,
                   isProtected = UPnPFileUtility::IsFileProtectedL(
                       *iFiles[index+iStartPoint] ) );
            
            iCurrentPoint++;
            
            __LOG1( "[UpnpAiwEngine]\t CUpnpDrmFilter::\
                   Filter , iCurrentPoint :%d" , iCurrentPoint );        
            if ( drmError || isProtected )
                {                
                __LOG1( "[UpnpAiwEngine]\t CUpnpDrmFilter::\
                        Filter , drmError :%d" , drmError );        
                 
                // Add the file to aDRMProtectedFiles array.
                iDRMProtectedFiles.AppendL( iFiles[index + iStartPoint] );
                // save the drm file index to array. These files will be removed
                // from the aFiles array soon.
                iRemovedIndexArray.AppendL( index + iStartPoint );
                } 
            if ( KErrNone != drmError )
                {
                iAbnormalFileCount ++;
                }            
            }  
        iStartPoint += KStepFileNumber;
        }    
    
    //Set a new request to filter the next 20 files
    TRequestStatus* status = &iStatus;
    *status = KRequestPending;
    User::RequestComplete( status, KErrNone );
    SetActive(); 
    
    __LOG( "CUpnpDrmFilter::Filter End" );
    }

// ----------------------------------------------------------------------------
// Handles request completion event
// ----------------------------------------------------------------------------
//
void CUpnpDrmFilter::RunL()
    {
    __LOG( "CUpnpDrmFilter::RunL End" );
    if ( KErrNone == iStatus.Int() )
        {
        DoTaskStep();
        }
    else 
        {
        __LOG( "CUpnpDrmFilter::RunL iStatus != KErrNone" );
        }
    __LOG( "CUpnpDrmFilter::RunL End" );
    }

//----------------------------------------------------------------------------
// Handles exception
//----------------------------------------------------------------------------
//
void CUpnpDrmFilter::RunError()
    {   
    __LOG( "CUpnpDrmFilter::RunError Begin" ); 
    
    Complete(); 
    
    __LOG( "CUpnpDrmFilter::RunError End" ); 
    }

// ---------------------------------------------------------------------------
// Cancel the filter operation
// ---------------------------------------------------------------------------
//
void CUpnpDrmFilter::DoCancel()
    {
    __LOG( "CUpnpDrmFilter::DoCancel Begin" ); 
    
    Complete();
    
    __LOG( "CUpnpDrmFilter::DoCancel End" ); 
    }

// ---------------------------------------------------------------------------
// filter files
// ---------------------------------------------------------------------------
//
void CUpnpDrmFilter::DoTaskStep()
    {    
    __LOG( "CUpnpDrmFilter::DoTaskStep Begin" ); 
    
    switch( iState )
        {
        case EFilter:
            {
            Filter();
            break;
            }
        case EFilterComplete:
            {
            Complete();
            break;
            }            
        default:
            {
            break;
            }
        }
    
    __LOG( "CUpnpDrmFilter::DoTaskStep End" ); 
    }

// ---------------------------------------------------------------------------
// End state for a delete operation
// ---------------------------------------------------------------------------
//
void CUpnpDrmFilter::Complete()
    {
    __LOG( "CUpnpDrmFilter::Complete Begin" ); 
    
    if ( iTotalNum > iCurrentPoint )
        {
        
        for ( TInt index = iCurrentPoint; index < iTotalNum ; index++ )
            {
            //remove the drm files from iFiles array.
            iFiles.Remove( index ); 
            }
        } 
        
        iFilteredCount = iRemovedIndexArray.Count() - iAbnormalFileCount;
        iAbnormalFileCount = 0;
        
        
    for ( TInt index = iRemovedIndexArray.Count() - 1; index >= 0 ; index-- )
        {
        //remove the drm files from iFiles array.
        iFiles.Remove( iRemovedIndexArray[index] ); 
        }

    // Remove the empty slots in the array
    iFiles.Compress();
    //close the iRemovedIndexArray
    iRemovedIndexArray.Close();
    
    //filter complete, start to copy file
    __LOG1( "CUpnpDrmFilter::Complete() iFiles.Count = %d ",\
            iFiles.Count());
    iCurrentPoint = 0;    
    iState = EFilterStop;
    
    __LOG( "CUpnpDrmFilter::Complete End" ); 
    }

// --------------------------------------------------------------------------
// Completes one cycle of the process.
// ---------------------------------------------------------------------------
void CUpnpDrmFilter::StepL()
    {
    
    }
// ---------------------------------------------------------------------------
// Return when the progress finish.
// ---------------------------------------------------------------------------
TBool CUpnpDrmFilter::IsProcessDone(void) const
    {
    __LOG( "CUpnpDrmFilter::IsProcessDone" ); 
    
    TBool ret(EFalse);
    if ( iState == EFilterStop )
        {
        ret = ETrue;
        }
    else
        {        
        __LOG( "CUpnpDrmFilter::IsProcessDone iState != EFilterComplete" );
        }
    return ret;
    }

void CUpnpDrmFilter::ShowWaitNoteL()
    {
    __LOG( "CUpnpDrmFilter::ShowWaitNoteL Begin" ); 
    
    CAknWaitNoteWrapper* waitNoteWrapper = CAknWaitNoteWrapper::NewL();
    CleanupStack::PushL(reinterpret_cast<CBase*>(waitNoteWrapper));
    waitNoteWrapper->ExecuteL(
                    R_AIW_DRM_FILTERING_WAIT_NOTE,      // TInt aResId,
                    *this,  // MAknBackgroundProcess& aBackgroundProcess
                    ETrue );
    CleanupStack::PopAndDestroy( waitNoteWrapper );
    
    __LOG( "CUpnpDrmFilter::ShowWaitNoteL End" ); 
    }

// END OF FILE
