/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for meta data gathering for audio files
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <MetaDataUtility.h>
#include <MetaDataFieldContainer.h>

#include "upnpmetadatautility.h"
#include "upnpcommonutils.h"

// ============================ MEMBER FUNCTIONS ============================

// ---------------------------------------------------------------------------
// CUPnPMetaDataUtility::CUPnPMetaDataUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUPnPMetaDataUtility::CUPnPMetaDataUtility()
    {
    }

// ---------------------------------------------------------------------------
// CUPnPMetaDataUtility::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPMetaDataUtility* CUPnPMetaDataUtility::NewL()
    {
    CUPnPMetaDataUtility* self = new( ELeave ) CUPnPMetaDataUtility;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// CUPnPMetaDataUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUPnPMetaDataUtility::ConstructL()
    {
    iMetaDataUtility = CMetaDataUtility::NewL();
    }
    
// ---------------------------------------------------------------------------
// CUPnPMetaDataUtility::~CUPnPMetaDataUtility()
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CUPnPMetaDataUtility::~CUPnPMetaDataUtility()
    {
    if ( iMetaDataUtility )
        {
        TRAP_IGNORE( iMetaDataUtility->ResetL() );
        }
        
    delete iArtist;
    delete iAlbum;
    delete iGenre;
    delete iTitle;
    delete iDate;
    delete iMetaDataUtility;        
    }

// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::LoadMetaDataL
// Finds correct MMF controller according to give file name
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPMetaDataUtility::LoadMetaDataL( const TDesC& aFileName )
    {
    // Cleanup existing information
    Reset();
    
    RArray<TMetaDataFieldId> wantedFields;
    CleanupClosePushL( wantedFields );
    wantedFields.AppendL( EMetaDataSongTitle );
    wantedFields.AppendL( EMetaDataArtist );
    wantedFields.AppendL( EMetaDataAlbum );
    wantedFields.AppendL( EMetaDataGenre );
    wantedFields.AppendL( EMetaDataDate );
    
    // Reset to be sure that no allocations are left over
    iMetaDataUtility->ResetL();
    
    // Open the file
    iMetaDataUtility->OpenFileL( aFileName, wantedFields );
    
    // wantedFields is not needed anymore
    CleanupStack::PopAndDestroy( &wantedFields );
    
    // Go through the meta data fields and pick the required ones
    TInt count = iMetaDataUtility->MetaDataCount();
    
    if ( count > 0 )
        {
        const CMetaDataFieldContainer& container = 
                                        iMetaDataUtility->MetaDataFieldsL();
        TMetaDataFieldId fieldId;
        for ( TInt index = 0; index < count; index++ )
            {
            TPtrC content = container.At( index, fieldId );
            switch ( fieldId )
                {
                case EMetaDataSongTitle:
                    {
                    iTitle = content.AllocL();
                    break;
                    }
                case EMetaDataArtist:
                    {
                    iArtist = content.AllocL();
                    break;
                    }
                case EMetaDataAlbum:
                    {
                    iAlbum = content.AllocL();
                    break;
                    }
                case EMetaDataGenre:
                    {
                    iGenre = content.AllocL();
                    break;
                    }
                case EMetaDataDate:
                    {
                    iDate = content.AllocL();
                    break;
                    }
                default:
                    {
                    // Nothing to do
                    break;
                    }
                }
            }
        }
        
    if( !iDate )
        {
        // Date was not found from metadata, so get it from file properties
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        RFile file;
        User::LeaveIfError( file.Open( fs,aFileName, EFileRead |
            EFileShareReadersOrWriters ) );
        CleanupClosePushL( file );
        
        TTime time;
        User::LeaveIfError( file.Modified( time ) );
        

        iDate = UPnPCommonUtils::TTimeToUPnPDateL( time );
                
        CleanupStack::PopAndDestroy( &file );        
        CleanupStack::PopAndDestroy( &fs );        

        }
    // Cleanup
    iMetaDataUtility->ResetL();
    }

// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::Title
// Returns title for the song
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUPnPMetaDataUtility::Title() const
    {
    if ( iTitle )
        {
        return *iTitle;
        }
    else
        {
        return KNullDesC;
        }
    }   
        
// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::Artist
// Returns artist for the song
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUPnPMetaDataUtility::Artist() const
    {
    if ( iArtist )
        {
        return *iArtist;
        }
    else
        {
        return KNullDesC;
        }
    }       
        
// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::Album
// Returns album for the song
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUPnPMetaDataUtility::Album() const
    {
    if ( iAlbum )
        {
        return *iAlbum;
        }
    else
        {
        return KNullDesC;
        }
    }
        
// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::Genre
// Returns genre for the song
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUPnPMetaDataUtility::Genre() const
    {
    if ( iGenre )
        {
        return *iGenre;
        }
    else
        {
        return KNullDesC;
        }
    }

// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::Date
// Returns date for the file
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CUPnPMetaDataUtility::Date() const
    {
    if ( iDate )
        {
        return *iDate;
        }
    else
        {
        return KNullDesC;
        }
    }

// --------------------------------------------------------------------------
// CUPnPMetaDataUtility::LoadTitleL
// Finds the title(ID3 tag) of the file according to give file name
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
EXPORT_C void CUPnPMetaDataUtility::LoadTitleL( const TDesC& aFileName )
    {
    // Cleanup existing information
    Reset();
    
    RArray<TMetaDataFieldId> wantedFields;
    CleanupClosePushL( wantedFields );
    wantedFields.Append( EMetaDataSongTitle );
    
    // Reset to be sure that no allocations are left over
    iMetaDataUtility->ResetL();
    
    // Open the file
    iMetaDataUtility->OpenFileL( aFileName, wantedFields );
    
    // wantedFields is not needed anymore
    CleanupStack::PopAndDestroy( &wantedFields );
    
    // Go through the meta data fields and pick the required ones
    TInt count = iMetaDataUtility->MetaDataCount();
    
    if ( count > 0 )
        {
        const CMetaDataFieldContainer& container = 
                                        iMetaDataUtility->MetaDataFieldsL();
        TMetaDataFieldId fieldId;
        for ( TInt index = 0; index < count; index++ )
            {
            TPtrC content = container.At( index, fieldId );
            if ( EMetaDataSongTitle == fieldId )
                {
                iTitle = content.AllocL();
                break;
                }
            }
        }
    
    // Cleanup
    iMetaDataUtility->ResetL();
    }

// ---------------------------------------------------------------------------
// CUPnPMetaDataUtility::Reset
// Reset member variables
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CUPnPMetaDataUtility::Reset()
    {
    delete iTitle;
    iTitle = NULL;
    delete iArtist;
    iArtist = NULL;
    delete iAlbum;
    iAlbum = NULL;
    delete iGenre;
    iGenre = NULL;
    delete iDate;
    iDate = NULL;
    }

//  End of File  
