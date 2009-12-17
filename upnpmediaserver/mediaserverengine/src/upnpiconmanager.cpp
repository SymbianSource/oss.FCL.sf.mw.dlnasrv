/** @file
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Media Server icon's manager
*
*/


// INCLUDE FILES

#include "upnpiconmanager.h"
#include "upnpicon.h"
#include "upnpmediaserverglobals.h"
#include "upnpcons.h"
#include "upnpdeviceimplementation.h"
#include "upnpstring.h"
#include "upnpmediaserverdescriptionprovider.h"
#include <upnpdevicedescriptionstore.h>

_LIT8( KIconDir, "/mediaserver1/icon/") ;

_LIT( KPathC, "C:" );
_LIT( KFileSpec,"*" );


// -----------------------------------------------------------------------------
// CUpnpIconManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpIconManager* CUpnpIconManager::NewL( CUpnpDeviceDescriptionStore& aDescriptionStore,
                              CUpnpMediaServerDescriptionProvider& aDescriptionProvider )
    {
    CUpnpIconManager* self = CUpnpIconManager::NewLC( aDescriptionStore, 
                                                      aDescriptionProvider );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpIconManager* CUpnpIconManager::NewLC( CUpnpDeviceDescriptionStore& aDescriptionStore,
                               CUpnpMediaServerDescriptionProvider& aDescriptionProvider )
    {
    CUpnpIconManager* self= new (ELeave) CUpnpIconManager( aDescriptionStore, 
                                                           aDescriptionProvider );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::~CUpnpIconManager
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpIconManager::~CUpnpIconManager()
    {
    iFileSession.Close();
    iIconList.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::AddIconL
// -----------------------------------------------------------------------------
//
void CUpnpIconManager::AddIconL( const TDesC8& aIconDes )
    {
    CUpnpIcon* icon = CUpnpIcon::NewL();
    CleanupStack::PushL( icon );
    
    RDesReadStream stream( aIconDes );
    CleanupClosePushL( stream );
    stream >> *icon;
    
    TPtrC8 iconUrl;
    TPtrC8 iconName8;
    TInt iconNameTemp;
    TInt iconNameInt(0);
    
    if ( iIconList.Count() != 0 )
        {
        for ( TInt i(0); i < iIconList.Count(); i++ )
            {
            iconUrl.Set( iIconList[ i ]->Url() );
            iconName8.Set( iconUrl.Mid( KIconDir().Length() ) );

            TLex8 lexer( iconName8 );
            lexer.Val( iconNameTemp );

            if ( iconNameInt < iconNameTemp )
                iconNameInt = iconNameTemp;
            }
        iconNameInt++;
        }
    
    TBufC<UpnpString::KMaxTUintLength> iconNameBuff;
    iconNameBuff.Des().Num( iconNameInt );
    
    HBufC8* iconUrl8 = HBufC8::NewLC( KIconDir().Length() + iconNameBuff.Length() );
    iconUrl8->Des().Append( KIconDir() );
    iconUrl8->Des().Append( iconNameBuff );

    icon->SetUrlL( *iconUrl8 );

    HBufC* iconPath = GetIconPathLC();

    HBufC16* destinationPath = HBufC16::NewLC( iconPath->Length() + UpnpString::KMaxTUintLength );
    destinationPath->Des().Append( *iconPath );
    destinationPath->Des().Append( iconNameBuff );
 
    CopyIconToPrivateL( icon , *destinationPath );
    iDescriptionStore.AddIconL( *icon );
    iIconList.AppendL( icon );

    iDescriptionProvider.AddUriToFileMapL( *iconUrl8, *destinationPath );
        
    CleanupStack::PopAndDestroy( destinationPath );
    CleanupStack::PopAndDestroy( iconPath );
    CleanupStack::PopAndDestroy( iconUrl8 );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( icon );
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::UpdateIconL
// -----------------------------------------------------------------------------
//    
void CUpnpIconManager::UpdateIconL( const TDesC8& aNewIconDes, const TDesC8& aUrl )
    {
    HBufC16* url = UpnpString::ToUnicodeL( aUrl );
    CleanupStack::PushL( url );

    HBufC16* iconName = GetIconNameLC( aUrl );

    HBufC8* iconUrl = HBufC8::NewLC( KIconDir().Length() + iconName->Length() );
    iconUrl->Des().Append( KIconDir() );
    iconUrl->Des().Append( *iconName );

    CUpnpIcon* icon = CUpnpIcon::NewL();
    CleanupStack::PushL( icon );

    RDesReadStream stream( aNewIconDes );
    CleanupClosePushL( stream );
    stream >> *icon;
    icon->SetUrlL( *iconUrl );
    
    User::LeaveIfError( FindIcon( *iconUrl ) );

    HBufC* iconPath = GetIconPathLC();

    HBufC16* destinationPath = HBufC16::NewLC( iconPath->Length() + 
                                               iconName->Length() );
    destinationPath->Des().Append( *iconPath );
    destinationPath->Des().Append( *iconName );
   
    HBufC* backupName = PrepareIconToRollbackLC( *destinationPath );
    
    TRAPD( err , DoUpdateIconL( icon , *destinationPath ) );
    if ( err )
        {
        TUint att;
        if( ! iFileSession.Att( *backupName , att ) )
            {
            iFileSession.Delete( *destinationPath );
            iFileSession.Rename( *backupName , *destinationPath );         
            }
        else
            {
            RestoreFromOrginalPathL( aUrl );
            }
        DeleteIconFile( *backupName );
        User::Leave( err );
        }
    
    DeleteIconFile( *backupName );
    
    CleanupStack::PopAndDestroy( backupName );
    CleanupStack::PopAndDestroy( destinationPath );
    CleanupStack::PopAndDestroy( iconPath );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( icon );
    CleanupStack::PopAndDestroy( iconUrl );
    CleanupStack::PopAndDestroy( iconName );
    CleanupStack::PopAndDestroy( url );
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::RemoveIconL
// -----------------------------------------------------------------------------
//  
void CUpnpIconManager::RemoveIconL( const TDesC8& aUrl )
    {
    HBufC16* url = UpnpString::ToUnicodeL( aUrl );
    CleanupStack::PushL( url );

    HBufC16* iconName16 = GetIconNameLC( aUrl );    

    HBufC8* iconName8 = UpnpString::FromUnicodeL( *iconName16 );
    CleanupStack::PushL( iconName8 );

    HBufC8* iconUrl = HBufC8::NewLC( KIconDir().Length() 
                                    + iconName8->Length() );  
    iconUrl->Des().Append( KIconDir() );
    iconUrl->Des().Append( *iconName8 );

    HBufC* iconDirectory = GetIconPathLC();
    HBufC* iconFullPath = HBufC::NewLC( iconDirectory->Length() 
                                        + iconName16->Length() );
    iconFullPath->Des().Append( *iconDirectory );
    iconFullPath->Des().Append( *iconName16 );
      
    TInt iconIdx = FindIcon( *iconUrl );
    User::LeaveIfError( iconIdx );
    
    User::LeaveIfError( DeleteIconFile( *iconFullPath ) );
    
    TRAPD( error, iDescriptionStore.RemoveIconL( *iconUrl ) );
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );  
        }
    delete iIconList[ iconIdx ];
    iIconList.Remove( iconIdx );
    
    iDescriptionProvider.RemoveUriToFileMapL( *iconUrl );
    
    CleanupStack::PopAndDestroy( iconFullPath );
    CleanupStack::PopAndDestroy( iconDirectory );
    CleanupStack::PopAndDestroy( iconUrl ); 
    CleanupStack::PopAndDestroy( iconName8 );
    CleanupStack::PopAndDestroy( iconName16 );    
    CleanupStack::PopAndDestroy( url );      
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::RemoveIconsL
// -----------------------------------------------------------------------------
//      
void CUpnpIconManager::RemoveIconsL()
    {
    for ( TInt i = 0; i < iIconList.Count(); i++ )
        {
        iDescriptionProvider.RemoveUriToFileMapL( iIconList[ i ]->Url() );
        }

    TRAPD( error, iDescriptionStore.RemoveIconsL() );
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );  
        }
    iIconList.ResetAndDestroy();

    HBufC* iconPath = GetIconPathLC();
    
    HBufC* fileSpec = HBufC::NewLC( iconPath->Length() + KFileSpec().Length() );
    fileSpec->Des().Append( *iconPath );
    fileSpec->Des().Append( KFileSpec() );

    CDir* dirList;
    User::LeaveIfError( iFileSession.GetDir( *fileSpec , 
                                             KEntryAttMaskSupported, 
                                             ESortByName, dirList) );
    CleanupStack::PushL( dirList );    

    TBuf<UpnpString::KMaxTUintLength> fileName;
    for ( TInt i(0); i < dirList->Count(); i++)
        {
        fileName = (*dirList)[i].iName;

        HBufC* totalPath = HBufC::NewLC( iconPath->Length() + fileName.Length() );
        totalPath->Des().Append( *iconPath );
        totalPath->Des().Append( fileName );

        DeleteIconFile( *totalPath );

        CleanupStack::PopAndDestroy( totalPath );
        }

    CleanupStack::PopAndDestroy( dirList );
    CleanupStack::PopAndDestroy( fileSpec );
    CleanupStack::PopAndDestroy( iconPath );
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::GetIconPathLC
// -----------------------------------------------------------------------------
//
HBufC* CUpnpIconManager::GetIconPathLC()
    {
    TFileName privatePath;
    iFileSession.PrivatePath( privatePath );

    HBufC* iconPath = HBufC::NewLC( KPathC().Length() + privatePath.Length() 
                                  + KIconDest().Length());
    iconPath->Des().Append( KPathC() );
    iconPath->Des().Append( privatePath );
    iconPath->Des().Append( KIconDest() );

    return iconPath;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::GetIconNameLC
// -----------------------------------------------------------------------------
//
HBufC* CUpnpIconManager::GetIconNameLC( const TDesC8& aPath )
    {
    _LIT16( KIcon, "icon" );
    const TInt KSlashesOffset( 1 );
    
    HBufC* iconPath = HBufC::NewLC( aPath.Length() );
    iconPath->Des().Copy( aPath );
    
    iconPath->Des() = iconPath->Mid( 
                   User::LeaveIfError( iconPath->FindF( KIcon() ) ) 
                                     + KIcon().Length()
                                     + KSlashesOffset );
    
    HBufC* iconName = HBufC::NewL( iconPath->Length() );
    iconName->Des().Append( *iconPath );

    CleanupStack::PopAndDestroy( iconPath );
    CleanupStack::PushL( iconName );
    
    TLex iconLex( *iconName );
    TInt iconNum;
    TInt err( iconLex.Val( iconNum ) );
    if ( err )
        {
        User::Leave( KErrNotFound );
        }
        
    return iconName;
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::CopyIconToPrivateL
// -----------------------------------------------------------------------------
//
void CUpnpIconManager::CopyIconToPrivateL( const CUpnpIcon* aIcon , 
                                           const TDesC& aDestinationPath 
                                         )
    {   
    CFileMan* fM = CFileMan::NewL( iFileSession );
    CleanupStack::PushL( fM );

    TInt err( fM->Copy( aIcon->BitmapFilename(), aDestinationPath ) );
    if( err == KErrAccessDenied )
        {
        fM->Attribs( aDestinationPath ,KEntryAttNormal , KEntryAttReadOnly|
                                                         KEntryAttHidden|
                                                         KEntryAttSystem , 0 );
        User::LeaveIfError( fM->Copy( aIcon->BitmapFilename(), aDestinationPath ) );
        }
    else
        {
        User::LeaveIfError( err );
        }
    User::LeaveIfError( fM->Attribs( aDestinationPath , 
                                     KEntryAttNormal , KEntryAttReadOnly|
                                                       KEntryAttHidden|
                                                       KEntryAttSystem , 0 ) );
       
    CleanupStack::PopAndDestroy( fM );    
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::PrepareIconToRollbackLC
// -----------------------------------------------------------------------------
//   
HBufC* CUpnpIconManager::PrepareIconToRollbackLC( const TDesC& aPath )
    {
    _LIT( KBeckupExt, ".beckup" );
    HBufC* beckupName = HBufC::NewLC( aPath.Length() + KBeckupExt().Length() );//on cleanupstack
    beckupName->Des().Append( aPath );
    beckupName->Des().Append( KBeckupExt() );
    
    iFileSession.Delete( *beckupName );
    
    iFileSession.Rename( aPath , *beckupName );
    
    return beckupName;
    }
      
// -----------------------------------------------------------------------------
// CUpnpMediaServer::DoUpdateIconL
// -----------------------------------------------------------------------------
//      
void CUpnpIconManager::DoUpdateIconL( CUpnpIcon* aIcon, const TDesC& aDestinationPath )
    {
    CopyIconToPrivateL( aIcon , aDestinationPath );

    iDescriptionStore.UpdateIconL( *aIcon );
    TInt idx = FindIcon( aIcon->Url() );
    if ( idx > KErrNotFound )
        {
        delete iIconList[ idx ];
        iIconList.Remove( idx );
        iIconList.AppendL( aIcon );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::DeleteIconFile
// -----------------------------------------------------------------------------
//  
TInt CUpnpIconManager::DeleteIconFile( const TDesC& aPath )
    {
    TInt err( iFileSession.Delete( aPath ) );  
    if( err == KErrAccessDenied )
        {
        iFileSession.SetAtt( aPath ,KEntryAttNormal , KEntryAttReadOnly|
                                                     KEntryAttHidden|
                                                     KEntryAttSystem );
        err = iFileSession.Delete( aPath );
        }
    else if ( err == KErrNotFound )
        {
        err = KErrNone;
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::RestoreFromOrginalPathL
// -----------------------------------------------------------------------------
//  
void CUpnpIconManager::RestoreFromOrginalPathL( const TDesC8& aUrl )
    {
    for ( TInt i(0) ; i< iIconList.Count() ; i++ )
        {
        if ( !iIconList[ i ]->Url().CompareF( aUrl ) )
            {
            HBufC* iconPath = GetIconPathLC();
            HBufC* iconName = GetIconNameLC( aUrl );
            HBufC* destinationPath = HBufC16::NewLC( iconPath->Length() +
                                                     UpnpString::KMaxTUintLength );
            destinationPath->Des().Append( *iconPath );
            destinationPath->Des().Append( *iconName );
            
            CopyIconToPrivateL( iIconList[ i ] , *destinationPath );
            
            CleanupStack::PopAndDestroy( destinationPath );
            CleanupStack::PopAndDestroy( iconName );
            CleanupStack::PopAndDestroy( iconPath );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpMediaServer::FindIcon
// -----------------------------------------------------------------------------
// 
TInt CUpnpIconManager::FindIcon( const TDesC8& aUrl )
    {
    for ( TInt i(0) ; i< iIconList.Count() ; i++ )
        {
        if ( !iIconList[i]->Url().CompareF( aUrl ) )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::AddIconsToDescriptionPropoertyMapL
// -----------------------------------------------------------------------------
//
void CUpnpIconManager::AddIconsToDescriptionPropertyMapL()
    {
    HBufC* iconPath = GetIconPathLC();
    for ( TInt i = 0; i < iIconList.Count(); i++ )
        {
        TPtrC8 url = iIconList[ i ]->Url();
        HBufC* iconName = GetIconNameLC( url );
        HBufC* path = HBufC::NewLC( iconPath->Length() + iconName->Length() );
        
        path->Des().Copy( iconPath->Des() );
        path->Des().Append( iconName->Des() );
        
        iDescriptionProvider.AddUriToFileMapL( url, path->Des() );
        
        CleanupStack::PopAndDestroy( path );
        CleanupStack::PopAndDestroy( iconName );
        }
    CleanupStack::PopAndDestroy( iconPath );
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::ConstructL
// Two-phase constructor
// -----------------------------------------------------------------------------
//
void CUpnpIconManager::ConstructL()
    {
    User::LeaveIfError( iFileSession.Connect() );
    
    iIconList = iDescriptionStore.IconListL();

    AddIconsToDescriptionPropertyMapL();
    }

// -----------------------------------------------------------------------------
// CUpnpIconManager::CUpnpIconManager
// Constructor
// -----------------------------------------------------------------------------
//    
CUpnpIconManager::CUpnpIconManager( CUpnpDeviceDescriptionStore& aDescriptionStore,
                                    CUpnpMediaServerDescriptionProvider& aDescriptionProvider )
     : iDescriptionProvider( aDescriptionProvider ), iDescriptionStore( aDescriptionStore )
    {
    }
    
// End of File
