/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      CUpnpSelectionWriter class implementation
 *
*/






// INCLUDE FILES

#include <upnpstring.h>
#include <centralrepository.h>
#include <s32file.h>

#include "upnpselectionwriter.h"
#include "upnpcontentservercrkeys.h"
#include "upnpcontentserverdefs.h"

_LIT( KComponentLogfile, "contentserver.txt");
#include "upnplog.h"

using namespace UpnpContentServer;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CUpnpSelectionWriter::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CUpnpSelectionWriter* CUpnpSelectionWriter::NewL(
    TUpnpMediaType aMediaType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    CUpnpSelectionWriter* self =
        new(ELeave) CUpnpSelectionWriter();

    CleanupStack::PushL(self);
    self->ConstructL( aMediaType );
    CleanupStack::Pop(self);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return self;
    }


// --------------------------------------------------------------------------
// CUpnpSelectionWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CUpnpSelectionWriter::ConstructL(
    TUpnpMediaType aMediaType )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iMediaType = aMediaType;

    iFileArray = new (ELeave) CDesCArrayFlat(4);
    iOkItems.Reset();

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }


// --------------------------------------------------------------------------
// CUpnpSelectionWriter::CUpnpSelectionWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CUpnpSelectionWriter::CUpnpSelectionWriter()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionWriter::CUpnpSelectionWriter
// C++ default destructor.
// --------------------------------------------------------------------------
//
CUpnpSelectionWriter::~CUpnpSelectionWriter()
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    delete iFileArray;
    iOkItems.Reset();
    delete iRepository;

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionWriter::AppendItemL
// ( other items were commented in header )
// --------------------------------------------------------------------------
//
void CUpnpSelectionWriter::AppendItemL( const TPtrC aNewObject)
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iFileArray->AppendL( aNewObject  );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionWriter::AppendStatusL
// ( other items were commented in header )
// --------------------------------------------------------------------------
//
void CUpnpSelectionWriter::AppendStatusL( TInt aIndex )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );
    iOkItems.AppendL( aIndex  );
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    }

// --------------------------------------------------------------------------
// CUpnpSelectionWriter::SaveSharingState
// Saves sharing state to central repository
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionWriter::SaveSharingStateL( TInt aShareAllSelection )
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    TInt status = KErrNone;
    iRepository = CRepository::NewL( KCrUidUpnpContentserver );
    switch ( iMediaType )
        {
    case EImageAndVideo:
        {
        if (aShareAllSelection == EShareAll ||
            aShareAllSelection == EShareNone ||
            aShareAllSelection == EShareMany )
            {
            status = iRepository->Set( KUPnPAppShareAllVisualFiles,
                                       aShareAllSelection);
            }
        break;
        }
    case EPlaylist:
        {
        if (aShareAllSelection == EShareAll ||
            aShareAllSelection == EShareNone ||
            aShareAllSelection == EShareMany )
            {
            status = iRepository->Set( KUPnPAppShareAllMusicFiles,
                                       aShareAllSelection);
            }
        break;
        }
    default:
        {
        __LOG( "SaveSharingStateL - default");
        break;
        }
        }

    delete iRepository;
    iRepository = NULL;

    // Store the item array
    if ( aShareAllSelection == EShareMany )
        {
        StoreSharedContainerIDsL();
        }

    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return status;
    }

// --------------------------------------------------------------------------
// CUPnPFileSharingEngine::StoreSharedContainerIDsL
// Creates main containers and starts sharing them
// --------------------------------------------------------------------------
//
TInt CUpnpSelectionWriter::StoreSharedContainerIDsL() const
    {
    __LOG8_1( "%s begin.", __PRETTY_FUNCTION__ );

    // Leave if aIdArray = NULL
    if ( !iFileArray )
        {
        User::Leave(KErrArgument);
        }

    RFs fs;
    RFile file;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL(fs);

    TFileName directory;

    // Get private directory path
    fs.SessionPath(directory);

    // Try to create directory
    TInt error = fs.MkDir(directory);
    // it's ok if directory already exists
    if (!(error != KErrNone && error != KErrAlreadyExists))
        {

        // Add file name
        if ( iMediaType == EImageAndVideo )
            {
            directory.Append( KVisualFile );
            }
        else if ( iMediaType == EPlaylist )
            {
            directory.Append( KMusicFile );
            }

        error = file.Replace(fs, directory, EFileWrite );
        if ( !error )
            {
            CleanupClosePushL( file );
            RFileWriteStream writeStream(file);
            CleanupClosePushL( writeStream );

            for (TInt index = 0; index < iFileArray->Count(); index++)
                {
                // check that sharing of the container was succesful
                if ( iOkItems.Find( index ) != KErrNotFound )
                    {
                    // Write identifiers to the file
                    writeStream << iFileArray->MdcaPoint(index);
                    }
                }

            // Clean up
            CleanupStack::PopAndDestroy(&writeStream); // Close writeStream
            CleanupStack::PopAndDestroy(&file); // Close file
            }
        }
    CleanupStack::PopAndDestroy(&fs);
    __LOG8_1( "%s end.", __PRETTY_FUNCTION__ );
    return error;
    }

// End of file
