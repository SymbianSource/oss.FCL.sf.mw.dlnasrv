/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for fetching and updating MS metadata
*
*/



// INCLUDE FILES
#include "upnpmetadata.h"
#include "upnpavcpenginemsclient.h"
#include <upnpdominterface.h>
#include <xml/dom/xmlengdomparser.h>
#include <XmlEngSerializationOptions.h>
#include <Uri16.h>


_LIT(KUPnP16,    "upnp");

_LIT8(KId,      "id");
_LIT8(KUPnP,    "upnp");
_LIT8(KDc,      "dc");
_LIT8(KArtist,  "artist");
_LIT8(KAlbum,   "album");
_LIT8(KGenre,   "genre");
_LIT8(KClass,   "class");
_LIT8(KCreator, "creator");
_LIT8(KTitle,   "title");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpMetadata::CUpnpMetadata
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpMetadata::CUpnpMetadata(RUpnpAVCPEngineMSClient& aSession): 
	CActive(EPriorityStandard), 
	iSession(aSession), 
	iMetaDataPtr(NULL,0),	
	iIdPckg(iId), 
	iSizePckg(iSize)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CUpnpMetadata::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::ConstructL()
    {     		
//    User::LeaveIfError( iRsfwControl.Connect() );
    iDOMImpl.OpenL();
    }

// -----------------------------------------------------------------------------
// CUpnpMetadata::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpMetadata* CUpnpMetadata::NewL(RUpnpAVCPEngineMSClient& aSession)
    {
    CUpnpMetadata* self = new( ELeave ) CUpnpMetadata(aSession);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpMetadata::~CUpnpMetadata
// Destructor
// -----------------------------------------------------------------------------
//    
CUpnpMetadata::~CUpnpMetadata()
    {
	delete iUuid;
	delete iPathName;
	delete iMetaData;
	delete iUpdatedMetaData;
	iDocument.Close();    
	iDOMImpl.Close();
	//iRsfwControl.Close();   
    }


// -----------------------------------------------------------------------------
// CUpnpMetadata::FetchL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpMetadata::FetchL(const TDesC& aPathName ) 
	{
	
	PrepareDataL(aPathName);
	GetMetadataL();
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::FetchL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CUpnpMetadata::FetchL(const TDesC& aPathName, TRequestStatus& aStatus ) 
	{
	PrepareDataL(aPathName);
	GetMetadataL(aStatus);
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::CommitL
// -----------------------------------------------------------------------------
//   		
EXPORT_C void CUpnpMetadata::CommitL() 
	{	        
    //Dump XML tree into buffer
    RBuf8 xmlBuf;
    CleanupClosePushL(xmlBuf);
    TXmlEngSerializationOptions options(TXmlEngSerializationOptions::KOptionIndent);
      
    iDocument.SaveL( xmlBuf, iDocument.DocumentElement(), options);
    
    iSession.SetMetadataL(*iUuid, *iPathName, *iMetaData, xmlBuf);    	    
    
	CleanupStack::PopAndDestroy(&xmlBuf); 
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::CommitL
// -----------------------------------------------------------------------------
//   		
EXPORT_C void CUpnpMetadata::CommitL(TRequestStatus& aStatus) 
	{	 
	
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
							
    //Dump XML tree into buffer
    RBuf8 xmlBuf;
    CleanupClosePushL(xmlBuf);
    TXmlEngSerializationOptions options(TXmlEngSerializationOptions::KOptionIndent);
      
    iDocument.SaveL( xmlBuf, iDocument.DocumentElement(), options);
    delete iUpdatedMetaData;
    iUpdatedMetaData = NULL;
    
    iUpdatedMetaData = xmlBuf.AllocL();
    CleanupStack::PopAndDestroy(&xmlBuf);
    
    iSession.SetMetadata(iStatus, *iUuid, *iPathName, *iMetaData, *iUpdatedMetaData);    	    
    SetActive();
	iState = EUploadingMetadata;    	
	}	
		
// -----------------------------------------------------------------------------
// CUpnpMetadata::PrepareData
// -----------------------------------------------------------------------------
//   
void CUpnpMetadata::PrepareDataL(const TDesC& aPathName ) 
	{

	TParse parseFilename;
	parseFilename.Set( aPathName, NULL, NULL );	
	

	TChar drive = parseFilename.Drive()[0];
	drive.UpperCase();

	CRsfwMountMan* mountman = NULL;
			mountman = CRsfwMountMan::NewL(0, NULL);
			User::LeaveIfNull(mountman);
			CleanupStack::PushL(mountman);
	
			const CRsfwMountEntry* entry = 
				mountman->MountEntryL(drive);

			if (!entry)
			  {
			  CleanupStack::PopAndDestroy(mountman);
			  User::Leave(KErrNotFound);
			  }
			  
	HBufC* uri = entry->Item(EMountEntryItemUri)->Alloc();
	
	if (!uri)
		User::Leave(KErrNoMemory);
	
	CleanupStack::PushL(uri);	
	// parse URI 
	TUriParser uriParser;
	User::LeaveIfError( uriParser.Parse(*uri) );	
	
	if ( uriParser.Extract( EUriScheme ).Compare(KUPnP16) == 0) 
		{
		delete iUuid;
		iUuid = NULL;
		
		delete iPathName;
		iPathName = NULL;	
			
		// filename normalize		
		iPathName = HBufC8::NewL( parseFilename.FullName().Length());
		iPathName->Des().Append(parseFilename.Path());
		TPtr8 path = iPathName->Des();
		Normalize( path );		
		iPathName->Des().Append(parseFilename.NameAndExt());				
		
		// uuid 
		iUuid = HBufC8::NewL(uriParser.Extract( EUriHost ).Length());			
		iUuid->Des().Copy(uriParser.Extract( EUriHost ));						
		}
	else 
		{
		CleanupStack::PopAndDestroy(uri);
		CleanupStack::PopAndDestroy(mountman);
		User::Leave( KErrNotFound );
		}	
	CleanupStack::PopAndDestroy(uri);
	CleanupStack::PopAndDestroy(mountman);
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::Normalize
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::Normalize(TDes8& aData)
	{
   	for (TInt i = 0; i < aData.Length(); i++)
		{
		if (aData[i] == '\\')
    		{
        	aData[i] = '/';
     		}
    	}
 	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::ParseMetadataL
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::ParseMetadataL()
	{
	iDocument.Close();
	// Create a parser
	RXmlEngDOMParser parser;
	User::LeaveIfError( parser.Open(iDOMImpl) );
	CleanupClosePushL(parser);
	// Handler to DOM document
	iDocument = parser.ParseL( *iMetaData );     	
	CleanupStack::PopAndDestroy(&parser);
 	} 		

// -----------------------------------------------------------------------------
// CUpnpMetadata::GetMetadataL
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::GetMetadataL()
	{
	iSession.PrepareMetadataL(*iUuid, *iPathName, iIdPckg, iSizePckg);
	if (iSize > 0) 
		{
		delete iMetaData;
		iMetaData = NULL;		
		iMetaData = HBufC8::NewL(iSize);
		iMetaDataPtr.Set(iMetaData->Des());
		iSession.DownloadMetadataL(iId, iMetaDataPtr);
		ParseMetadataL();
		}
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::GetMetadataL
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::GetMetadataL(TRequestStatus& aStatus)
	{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	iSession.PrepareMetadata(iStatus, *iUuid, *iPathName, iIdPckg, iSizePckg);
	SetActive();
	iState = EPreparingMetadata;	
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::DoCancel
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::DoCancel()
	{
	if (iClientStatus) 
		{	
		User::RequestComplete(iClientStatus, KErrCancel);			
		}
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::RunError
// -----------------------------------------------------------------------------
//
TInt CUpnpMetadata::RunError( TInt aError ) 
	{
	if (iClientStatus) 
		{
		User::RequestComplete(iClientStatus, aError);			
		}
	return KErrNone;
	}
        		
// -----------------------------------------------------------------------------
// CUpnpMetadata::RunL
// -----------------------------------------------------------------------------
//
void CUpnpMetadata::RunL() 
	{
	if (iStatus.Int() == KErrNone) 
		{
		switch (iState) 
			{
			case EPreparingMetadata:
			if (iSize > 0) 
				{
				delete iMetaData;
				iMetaData = NULL;
				iMetaData = HBufC8::NewL(iSize);
				iMetaDataPtr.Set(iMetaData->Des());
				iSession.DownloadMetadata(iStatus, iId, iMetaDataPtr);
				SetActive();
				iState = EDownloadingMetadata;				
				}
				break;
			case EDownloadingMetadata:
				ParseMetadataL();
				User::RequestComplete(iClientStatus, iStatus.Int());
				break;
			case EUploadingMetadata:
				delete iUpdatedMetaData;
				iUpdatedMetaData = NULL;
				User::RequestComplete(iClientStatus, iStatus.Int());
				break;							
			}		
		} 
	else 
		{
		User::RequestComplete(iClientStatus, iStatus.Int());		
		}
	}


// -----------------------------------------------------------------------------
// CUpnpMetadata::ObjectL
// -----------------------------------------------------------------------------
//   
TBool CUpnpMetadata::ObjectL(TXmlEngElement& aObject) 
	{
	TXmlEngElement root = iDocument.DocumentElement();
	
    RXmlEngNodeList<TXmlEngElement> argElems;
    CleanupClosePushL(argElems);
    root.GetChildElements( argElems ); 
                
    if ( argElems.Count() == 1 ) 
    	{
    	aObject = argElems.Next();
    	
    	} 
    	
    CleanupStack::PopAndDestroy(&argElems);
    return !aObject.IsNull();	             	
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::GetElementL
// -----------------------------------------------------------------------------
//   
TPtrC8 CUpnpMetadata::GetElementL( const TDesC8& aName) 	 
	{
	TXmlEngElement result;
	TXmlEngElement object;
	if (ObjectL(object) &&  UpnpDomInterface::GetElementL(  object, result, aName) ) 
		{
		return result.Value();	
		}
                                
	return KNullDesC8();	
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::SetElementL
// -----------------------------------------------------------------------------
//   	
void CUpnpMetadata::SetElementL( const TDesC8& aNamespace, const TDesC8& aName, const TDesC8& aValue) 	 
	{
	TXmlEngElement result;
	TXmlEngElement object;
	if (ObjectL(object))
		{
		if (!UpnpDomInterface::GetElementL(  object, result, aName) ) 
			{			
			result = object.AddNewElementUsePrefixL(aName, aNamespace);
			}
		result.SetTextL( aValue );
		}
	}	
		
// -----------------------------------------------------------------------------
// CUpnpMetadata::Id
// -----------------------------------------------------------------------------
//   	
EXPORT_C  TPtrC8 CUpnpMetadata::IdL() 
	{ 	
	TXmlEngElement object;
	if (ObjectL(object)) 
		{
		return UpnpDomInterface::GetAttrValueL( object, KId );
		} 
	else 
		{
		return KNullDesC8();		
		}
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::Title
// -----------------------------------------------------------------------------
//   	
EXPORT_C TPtrC8 CUpnpMetadata::TitleL() 
	{
	return GetElementL(KTitle);
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::SetTitleL
// -----------------------------------------------------------------------------
//   	
EXPORT_C void CUpnpMetadata::SetTitleL(const TDesC8& aTitle) 
	{
	SetElementL(KDc, KTitle, aTitle);
	}	

// -----------------------------------------------------------------------------
// CUpnpMetadata::Creator
// -----------------------------------------------------------------------------
//   		
EXPORT_C TPtrC8 CUpnpMetadata::CreatorL() 
	{
	return GetElementL(KCreator);
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::SetCreator
// -----------------------------------------------------------------------------
//   	
EXPORT_C void CUpnpMetadata::SetCreatorL(const TDesC8& aCreator)
	{
	SetElementL(KDc,KCreator, aCreator);	
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::Class
// -----------------------------------------------------------------------------
//   	
EXPORT_C TPtrC8 CUpnpMetadata::ClassL() 
	{
	return GetElementL(KClass);
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::SetClass
// -----------------------------------------------------------------------------
//   		
EXPORT_C void CUpnpMetadata::SetClassL(const TDesC8& aClass)
	{
	SetElementL(KUPnP, KClass, aClass);	
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::Artist
// -----------------------------------------------------------------------------
//   		
EXPORT_C TPtrC8 CUpnpMetadata::ArtistL() 
	{
	return GetElementL(KArtist);
	}
		
// -----------------------------------------------------------------------------
// CUpnpMetadata::SetArtist
// -----------------------------------------------------------------------------
//   			
EXPORT_C void CUpnpMetadata::SetArtistL(const TDesC8& aArtist)
	{
	SetElementL(KUPnP, KArtist, aArtist);		
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::Genre
// -----------------------------------------------------------------------------
//   		
EXPORT_C TPtrC8 CUpnpMetadata::GenreL() 
	{
	return GetElementL(KGenre);
	}
	
// -----------------------------------------------------------------------------
// CUpnpMetadata::SetGenre
// -----------------------------------------------------------------------------
//   			
EXPORT_C void CUpnpMetadata::SetGenreL(const TDesC8& aGenre)
	{
	SetElementL(KUPnP, KGenre, aGenre);		
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::Album
// -----------------------------------------------------------------------------
//   			
EXPORT_C TPtrC8 CUpnpMetadata::AlbumL() 
	{
	return GetElementL(KAlbum);
	}		

// -----------------------------------------------------------------------------
// CUpnpMetadata::SetAlbum
// -----------------------------------------------------------------------------
//   	
EXPORT_C void CUpnpMetadata::SetAlbumL(const TDesC8& aAlbum)
	{
	SetElementL(KUPnP, KAlbum, aAlbum);	
	}

// -----------------------------------------------------------------------------
// CUpnpMetadata::AsXmlL
// -----------------------------------------------------------------------------
//   		
EXPORT_C RXmlEngDocument& CUpnpMetadata::AsXmlL() 
	{
	return iDocument;
	}

	
	
//  End of File
