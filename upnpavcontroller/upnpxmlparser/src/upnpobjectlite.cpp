/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Light weight version of UpnpObject
*
*/






#include "upnpobjectlite.h"

const TInt KDelimeter = 46;
const TInt KRound1 = 1;
const TInt KRound2 = 2;
const TInt KRound3 = 3; 
const TInt KRound4 = 4;

const TInt KMask1 = 0xFF000000;
const TInt KMask2 = 0x00FF0000;

_LIT8( KObject, "object" );
_LIT8( KItem, "item" );
_LIT8( KContainer,"container" );

_LIT8( KImageItem, "imageItem" );
_LIT8( KAudioItem, "audioItem" );
_LIT8( KVideoItem, "videoItem" );
_LIT8( KPlaylistItem, "playlistItem" );
_LIT8( KStorageFolder, "storageFolder" );
_LIT8( KPlaylistContainer, "playlistContainer" );

_LIT8( KMusicTrack,"musicTrack" );
_LIT8( KPhoto,"photo" );
_LIT8( KVideo,"video" );
_LIT8( KMusicAlbum,"musicAlbum" );


// --------------------------------------------------------------------------
// CUpnpObjectLite::CUpnpObjectLite
// See upnpobjectlite.h
// --------------------------------------------------------------------------
CUpnpObjectLite::CUpnpObjectLite()
    {
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::~CUpnpObjectLite
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C CUpnpObjectLite::~CUpnpObjectLite()
    {
    delete iObjectId;
    delete iTitle;
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::NewL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
void CUpnpObjectLite::ConstructL()
    {
    
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::NewL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C CUpnpObjectLite* CUpnpObjectLite::NewL()
    {
    CUpnpObjectLite* self = new (ELeave) CUpnpObjectLite();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::SetObjectIdL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpObjectLite::SetObjectIdL( const TDesC8& aObjectId )
    {
    HBufC8* tmp = aObjectId.AllocL();
    delete iObjectId;
    iObjectId = tmp;
    }

    
// --------------------------------------------------------------------------
// CUpnpObjectLite::SetObjectIdL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpObjectLite::SetObjectIdL( HBufC8* aObjectId )
    {
    if( aObjectId )
        {
        delete iObjectId;
        iObjectId = aObjectId;
        }
    else
        {
        User::Leave( KErrArgument );
        }    
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::ObjectId
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C const TDesC8& CUpnpObjectLite::ObjectId() const
    {
    if( iObjectId )
        {
        return *iObjectId;
        }
    else
        {
        return KNullDesC8;
        }    
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::SetTitleL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpObjectLite::SetTitleL( const TDesC& aTitle )
    {
    HBufC* tmp = aTitle.AllocL();
    delete iTitle;
    iTitle = tmp;    
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::SetTitleL
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpObjectLite::SetTitleL( HBufC* aTitle )
    {
    if( aTitle )
        {
        delete iTitle;
        iTitle = aTitle;
        }
    else
        {
        User::Leave( KErrArgument );
        }        
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::Title
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C const TDesC& CUpnpObjectLite::Title() const
    {
    if( iTitle )
        {
        return *iTitle;
        }
    else
        {
        return KNullDesC;
        }        
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::SetObjectClass
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C void CUpnpObjectLite::SetObjectClass( const TDesC8& aObjectClass )
    {
    iObjectClass = 0;
    TInt round = 0;
    
    TLex8 input( aObjectClass );
    while( !input.Eos() )
        {
        round++;
        ParseToDelimeter( input, TChar( KDelimeter ) );
        
        HandleRound( round, input.MarkedToken() );

        if( !input.Eos() )
            {
            input.SkipAndMark( 1 ); // Skip the delimeter
            }
        }
    
    }
    
// --------------------------------------------------------------------------
// CUpnpObjectLite::IsTypeOf
// See upnpobjectlite.h
// --------------------------------------------------------------------------
EXPORT_C TBool CUpnpObjectLite::IsTypeOf( TObjectType aType ) const
    {
    TBool retVal = EFalse;
    switch( aType )
        {
        case EItem:
            {
            if( ( iObjectClass & KMask1 ) == EBaseItem )
                {
                retVal = ETrue;
                }
            }
            break;
        case EContainer:
            {
            if( ( iObjectClass & KMask1 ) == EBaseContainer )
                {
                retVal = ETrue;
                }            
            }
            break;
        case EMusicItem:
            {
            if( ( iObjectClass & KMask2 ) == EBaseMusicItem )
                {
                retVal = ETrue;
                }                                    
            }
            break;
        case EImageItem:
            {
            if( ( iObjectClass & KMask2 ) == EBaseImageItem )
                {
                retVal = ETrue;
                }                                    
            }
            break;
        case EVideoItem:
            {
            if( ( iObjectClass & KMask2 ) == EBaseVideoItem )
                {
                retVal = ETrue;
                }                                                
            }
            break;
        default:
            {
            retVal = EFalse;                                               
            }
            break;
        }
    
    return retVal;
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::ParseToDelimeter
// See upnpobjectlite.h
// --------------------------------------------------------------------------
void CUpnpObjectLite::ParseToDelimeter( TLex8& aLex, TChar aDelimeter ) const
    {
    aLex.Mark();
    while( (aLex.Peek() != aDelimeter) && (!aLex.Eos()) )
        {
        aLex.Inc();
        }
    }

// --------------------------------------------------------------------------
// CUpnpObjectLite::HandleRound
// See upnpobjectlite.h
// --------------------------------------------------------------------------
void CUpnpObjectLite::HandleRound( TInt aRound, TPtrC8 aString )
    {
    switch( aRound )
        {
        case KRound1:
            {
            Round1( aString );
            break;
            }
        case KRound2:
            {
            Round2( aString );
            break;
            }
        case KRound3:
            {
            Round3( aString );
            break;
            }
        case KRound4:
            {
            Round4( aString );
            break;
            }
            
        default:
            {
            
            break;
            }
        }
    }

void CUpnpObjectLite::Round1( TPtrC8 aString )
    {
    if( aString.Compare( KObject ) != 0 )
        {
        // Not an object
        iObjectClass |= EBaseUnknown;
        }
    }
    
void CUpnpObjectLite::Round2( TPtrC8 aString )
    {
    if( aString.Compare( KItem ) == 0 )
        {
        iObjectClass |= EBaseItem;
        }
    else if( aString.Compare( KContainer ) == 0 )
        {
        iObjectClass |= EBaseContainer;
        }
    else
        {
        iObjectClass |= EBaseUnknown;
        }    
    }
    
void CUpnpObjectLite::Round3( TPtrC8 aString )
    {
    if( aString.Compare( KImageItem ) == 0 )
        {
        iObjectClass |= EBaseImageItem;
        }
    else if( aString.Compare( KAudioItem ) == 0 )
        {
        iObjectClass |= EBaseMusicItem;
        }
    else if( aString.Compare( KVideoItem ) == 0 )
        {
        iObjectClass |= EBaseVideoItem;
        }
    else if( aString.Compare( KPlaylistItem ) == 0 )
        {
        iObjectClass |= EBasePlaylistItem;
        }    
    else if( aString.Compare( KStorageFolder ) == 0 )
        {
        iObjectClass |= EStorageFolder;
        }    
    else if( aString.Compare( KPlaylistContainer ) == 0 )
        {
        iObjectClass |= EPlaylistContainer;
        }
    else
        {
        
        }        
    }
    
void CUpnpObjectLite::Round4( TPtrC8 aString )
    {
    if( aString.Compare( KMusicTrack ) == 0 )
        {
        iObjectClass |= EMusicTrack;
        }
    else if( aString.Compare( KPhoto ) == 0 )
        {
        iObjectClass |= EPhoto;
        }
    else if( aString.Compare( KVideo ) == 0 )
        {
        iObjectClass |= EMovie;
        }
    else if( aString.Compare( KMusicAlbum ) == 0 )
        {
        iObjectClass |= EMusicAlbum;
        }    
    else
        {
        
        }            
    }

// end of file


