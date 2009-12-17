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
* Description:  Source file for CUpnpFilePipe class.
*
*/


// INCLUDE FILES
#include "upnpfilepipe.h"

_LIT( KComponentLogfile, "upnpcommand.log");
#include "upnplog.h"

// --------------------------------------------------------------------------
// CUpnpFilePipe::NewL
// Creates a new file pipe with unlimited length.
// --------------------------------------------------------------------------
//
CUpnpFilePipe* CUpnpFilePipe::NewL()
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::NewL" );

    // Create a new file pipe with unlimited length (0).
    return new (ELeave) CUpnpFilePipe( 0 );
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::NewL
// Creates a new file pipe with a set length.
// --------------------------------------------------------------------------
//
CUpnpFilePipe* CUpnpFilePipe::NewL( TInt aMaxSize )
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::NewL" );

    // Check param
    if( aMaxSize <= 0 )
        {
        User::Leave( KErrArgument );
        }

    // Create a new file pipe with the given length.
    return new (ELeave) CUpnpFilePipe( aMaxSize );
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::CUpnpFilePipe
// First phase construction.
// --------------------------------------------------------------------------
//
CUpnpFilePipe::CUpnpFilePipe( TInt aMaxSize )
    : iMaxSize( aMaxSize )
    {
    // No implementation
    }

// --------------------------------------------------------------------------
// Destructor.
// --------------------------------------------------------------------------
//
CUpnpFilePipe::~CUpnpFilePipe()
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::Destructor" );

    // Reset the content of the array
    Reset();

    // Close the array
    iValues.Close();
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::PushL
// Pushes one file into the pipe.
// --------------------------------------------------------------------------
//
void CUpnpFilePipe::PushL( const TDesC& aParam )
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::PushL" );

    // Parameter check
    if( aParam == KNullDesC )
        {
        User::Leave( KErrArgument );
        }

    // If we are using a pipe with a limited size
    if( iMaxSize > 0 )
        {
        // If the pipe is already full (max size reached) remove the newest
        // file from the pipe
        if( iValues.Count() >= iMaxSize )
            {
            RemoveNewestFileFromPipe();
            }
        }

    // Create a new parameter value
    HBufC* temp = aParam.AllocL();
    CleanupStack::PushL( temp );

    // Append the new parameter value to the array
    iValues.AppendL( temp );

    // Just pop, ownership transfered to iValues array
    CleanupStack::Pop( temp ); 
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::FileAt
// --------------------------------------------------------------------------
//
const TDesC& CUpnpFilePipe::FileAt( TInt aIndex )
    {
    __ASSERTD( aIndex >= 0 && aIndex < iValues.Count(),
        __FILE__, __LINE__ );
    TDesC* file = iValues[aIndex];
    return *file;
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::Reset
// Resets the pipe.
// --------------------------------------------------------------------------
//
void CUpnpFilePipe::Reset()
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::Reset" );

    // Simply reset the array
    iValues.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::Count
// Returns the count of the items in the pipe.
// --------------------------------------------------------------------------
//
TInt CUpnpFilePipe::Count()
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::Count" );

    // Simply retun the array's item count
    return iValues.Count();
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::AsArray
// returns the entire pipe as an array
// --------------------------------------------------------------------------
//
RPointerArray<TDesC>& CUpnpFilePipe::AsArray()
    {
    return iValues;
    }

// --------------------------------------------------------------------------
// CUpnpFilePipe::RemoveNewestFileFromPipe
// Removes the newest file from the pipe.
// --------------------------------------------------------------------------
//
void CUpnpFilePipe::RemoveNewestFileFromPipe()
    {
    __LOG( "[UpnpCommand]\t CUpnpFilePipe::RemoveNewestFileFromPipe" );

    // Remove item only if there are items in the pipe
    if( iValues.Count() > 0 )
        {
        // Get the index of the newest item (last item in the array)
        TInt newestIndex = iValues.Count() - 1; // indexing starts from 0

        // Get the pointer to the data, and delete it
        HBufC* temp = static_cast<HBufC*>( iValues[newestIndex] );
        delete temp;
        temp = NULL;

        // Remove the pointer from the array, this will also compress the
        // array
        iValues.Remove( newestIndex );
        }
    }

// End of file
