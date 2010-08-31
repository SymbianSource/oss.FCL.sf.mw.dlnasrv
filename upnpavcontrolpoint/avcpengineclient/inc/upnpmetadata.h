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
* Description:  Describes UPnP metadata for item/container
*
*/


#ifndef C_CUPNPMETADATA_H
#define C_CUPNPMETADATA_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>
 
#include <xml/dom/xmlengdom.h>
#include <xml/dom/xmlengdocument.h>

//rsfw
#include <rsfwmountman.h>
#include <rsfwmountentry.h>

// FORWARD DECLARATIONS
class RUpnpAVCPEngineMSClient;

enum TMetadataStatus 
{
	EPreparingMetadata, 
	EDownloadingMetadata, 
	EUploadingMetadata		
};
		
// CLASS DECLARATION
/**
*  Describes UPnP metadata for item/container
*
*  @lib AVCPEngineClient.lib
*  @since Series 60 3.1
*/
class CUpnpMetadata : public CActive
	{
	
	public:  // Constructors and destructor	
		/**
		* Two-phased constructor.
		*/
		IMPORT_C static CUpnpMetadata* NewL(RUpnpAVCPEngineMSClient& aSession);        
		/**
		* Destructor.
		*/
		virtual ~CUpnpMetadata();

	public: // New functions            	
		/**
		* Return ID of object
		*
		* @return id
		*/
		IMPORT_C TPtrC8 IdL();
		/**
		* Return title of object
		*
		*  @return title
		*/
		IMPORT_C TPtrC8 TitleL();
		/**
		* Set new title 
		* 
		* @param aTitle new title
		*/
		IMPORT_C void SetTitleL(const TDesC8& aTitle) ;
		/**
		* Return creator of object
		*
		*  @return creator
		*/		
		IMPORT_C TPtrC8 CreatorL();
		/**
		* Set new creator 
		* 
		* @param aCreator new creator
		*/		
		IMPORT_C void SetCreatorL(const TDesC8& aCreator);
		/**
		* Return class of object
		*
		*  @return class
		*/		
		IMPORT_C TPtrC8 ClassL();	
		/**
		* Set new class 
		* 
		* @param aClass new clas
		*/		
		IMPORT_C void SetClassL(const TDesC8& aClass);	
		/**
		* Return atrist of object
		*
		*  @return atrist
		*/		
		IMPORT_C TPtrC8 ArtistL();	
		/**
		* Set new atrist 
		* 
		* @param aArtist new atrist
		*/			
		IMPORT_C void SetArtistL(const TDesC8& aArtist); 
		/**
		* Return genre of object
		*
		*  @return genre
		*/		
		IMPORT_C TPtrC8 GenreL();	
		/**
		* Set new genre 
		* 
		* @param aGenre new genre
		*/		
		IMPORT_C void SetGenreL(const TDesC8& aGenre);
		/**
		* Return album of object
		*
		*  @return album
		*/		
		IMPORT_C TPtrC8 AlbumL();
		/**
		* Set new album 
		* 
		* @param aAlbum new album
		*/		
		IMPORT_C void SetAlbumL(const TDesC8& aAlbum);
		/**
		* Return whole metada as XML
		*
		*  @return document with metadata
		*/			
		IMPORT_C RXmlEngDocument& AsXmlL();		
		/**
		* Fetch metadata for given path
		*
		* @param aPathName path name on MS's drive
		*/
		IMPORT_C void FetchL( const TDesC& aPathName ); 	
		/**
		* Fetch metadata for given path
		*
		* @param aPathName path name on MS's drive
		*/
		IMPORT_C void FetchL(const TDesC& aPathName, TRequestStatus& aStatus );			
		/**
		* Approve changes made in metadata and send them to update in MS
		*/
		IMPORT_C void CommitL(); 
		/**
		* Approve changes made in metadata and send them to update in MS
		*/
		IMPORT_C void CommitL( TRequestStatus& aStatus );		
	protected: // from CActive
        /**
        * From CActive invoke when local request should be cancelled
        */
        void DoCancel();
        /**
        * From CActive invoke when RunL leaves
        */        
        TInt RunError( TInt aError );
        /**
        * From CActive invoke when asynchronous action is completed
        */
        void RunL();  
	private:
		/**
		* C++ default constructor.
		*/
		CUpnpMetadata(RUpnpAVCPEngineMSClient& aSession);
		
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();
	    /**
	    * Get object XML element from inside stored XML 
	    *
	    * @param aObject element for object XML
	    * @return ETrue if found, else EFalse
	    */   
		TBool ObjectL(TXmlEngElement& aObject);
	    /**
	    * Get value of element with given name 
	    * (it has to be child of object element)
	    *
	    * @param aName name of element
	    * @return value of element
	    */   		
		TPtrC8 GetElementL( const TDesC8& aName);
	    /**
	    * Set value of element with given name 
	    * (it is child of object element)
	    *
		* @param aNamespace namespace of element    
	    * @param aName name of element
		* @param aValue value of element    
	    */   				
		void SetElementL( const TDesC8& aNamespace, const TDesC8& aName, const TDesC8& aValue);
		/**
	    * Normalize slashes in path
	    *
	    * @param aData path to normalize
	    */
		void Normalize(TDes8& aData);
		/**
	    * Get metadata synchnronous
	    */		
		void GetMetadataL(); 
		/**
	    * Get metadata asynchnronous
	    */			
		void GetMetadataL(TRequestStatus& aStatus);
		/**
	    * Parse metadata
	    */				
		void ParseMetadataL();
		/**
	    * Prepare information about file, check if drive is upnp, normalize path slashes
	    *
	    * @param aPathName path to check
	    */
		void PrepareDataL(const TDesC& aPathName ); 				
	private:
		// sesion to server AVCPEngine
		RUpnpAVCPEngineMSClient& iSession;
		// UDN of MS
		HBufC8* iUuid;
		// pathname
		HBufC8* iPathName;
		// metadata
		HBufC8* iMetaData;
		// new metadata (used during Commit operation
		HBufC8* iUpdatedMetaData;		
		// pointer to metadata
		TPtr8 iMetaDataPtr;
		// document with parsed metadata
		RXmlEngDocument iDocument;
		// client-interface to RS Fwk
	//	RRsfwControl iRsfwControl;	
		// command id (it's id of command used to fetch metadata)
		TInt iId;
		// package for id
		TPckg<TInt> iIdPckg;
		// size of metadata
		TInt iSize;
		// package for size
		TPckg<TInt> iSizePckg;	
		// request status fo 
		TRequestStatus* iClientStatus;	
		// state of asynchrounus performing
		TMetadataStatus iState;		
		RXmlEngDOMImplementation iDOMImpl;
	};

#endif      // C_CUPNPMETADATA_H
            
// End of File
