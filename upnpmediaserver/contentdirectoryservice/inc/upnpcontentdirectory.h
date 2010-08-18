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
* Description:  Declares ContentDirectory class.
*
*/


#ifndef C_CUPNPCONTENTDIRECTORY_H
#define C_CUPNPCONTENTDIRECTORY_H


// INCLUDES
#include "upnpserviceimplementation.h"
#include "upnpelementfactory.h"

// new xml
#include <xml/dom/xmlengdom.h>

#include "upnpfiletransfer.h"
#include "upnpcontentdirectorydatafinder.h"
#include "upnpthumbnailcreator.h"
#include "upnpcdsettings.h"
#include "upnptransferobserver.h"
#include "upnphttpservertransactioncreator.h"
#include "upnphttpserverobserver.h"

// FORWARD DECLARATIONS

class CUpnpElementFactory;
class TUpnpContainerUpdateId;
class CUpnpFileTransferTimerObserver;
class CUpnpObject;
class CUpnpItem;
class CUpnpContentDirectoryDb;
class CProtocolInfo;
class CUpnpMetadataStorage;
class CUpnpAutoDestroyObjectList;
class MUpnpContentDirectoryEventObserver;
class CUpnpFileTransferEvent;
class MUpnpCdsSender;
class CUpnpStateHandler;
class CUpnpTransferUploader;
class CUpnpMimeMapper;
class CUpnpTransferController;
class MUpnpTransferInterface;
class CUpnpDlnaFilter;
class CUpnpHttpServerSession;
class CUpnpSecurityManager;

// CONSTANTS
static const TInt KMaxImportExportTransfers = 3;
_LIT8( KContentDirectoryType, "ContentDirectory:1" );

// CLASS DECLARATION

/**
*  @brief Main Content Directory Service Class
*
*  @lib ServiceFramework.lib
*  @since Series60 2.6
*/
class CUpnpContentDirectory :             
                public CUpnpServiceImplementation, 
                public MUpnpContentDirectoryDataFinder,
                public MUpnpThumbnailCreatorObserver,
                public MUpnpTransferObserver,
                public MUpnpHttpServerTransactionCreator,
                public MUpnpHttpServerObserver
{
public: // Constructors and destructors
    
    /**
    * Two-phased constructor for creating Content Directory Service (CDS).
    * @since Series60 2.6
    * @param aService Parent service 
    */
    static CUpnpContentDirectory* NewL( TUpnpCdSettings aSettings, 
                                        CUpnpMetadataStorage* aMetadataStorage,
                                        MUpnpTransferInterface* aTransfer );

    /**
    * Destructor function; called automatically from the destructor of a 
    * derived class
    **/
    virtual ~CUpnpContentDirectory();
    IMPORT_C void SetSender(MUpnpCdsSender* aSender);
            

public: //from CUpnpServiceImplementation
    
    CUpnpStateHandler* CUpnpContentDirectory::StateHandler();
    /**
    * Handles auto destroy events.
    * @since Series 60 2.6
    * @param aObjIdn id of evented upnp object
    */
    void AutoDestroyEventL(TInt aObjId, TBool aDeleteObj);

    /**
    * This functions is a entry point for actions. See coments in base class
    * @since Series 60 2.6
    * @param aAction Action received by CDS.
    */
    void ActionReceivedLD( CUpnpAction* aAction );
    
    /**
    * Hnadle unknown http event
    * Export for interneal use only    
    */
    IMPORT_C void UnknownHttpEventReceivedL( CUpnpHttpMessage& aHttpMsg );
    
    /**
    * Handles address change event    
    */
    void AddressChangeL();
    
    /**
    * This function is for informing service from evented statevariables.
    * @since Series 60 2.6
    * @param aVariableName Name of the evented statevariable.
    */
    void StateVariableEvented(const TDesC8& aVariableName);
      
    /**
    * Checks if specified importUri is in database still active.
    * @param aImportUri uri to be searched in databse. 
    * @return objId if successful
    */
    TInt CheckImportUriL(const TDesC8& aImportUri);

    /**
    * Inserts Ip and Port
    * @since Series S60 3.1
    * @param aString
    */
    void InsertRealIpAndPortL(TDes8& aString);
    
    /**
    *
    * @since Series S60 3.1
    * @param aString
    * @return descriptor pointer
    */  
    HBufC8* InsertIpAndPortPlaceholderL(const TDesC8& aString);
    
    /**
    * Find protocolInfo by importUri (Not by importUri).
    * @param aImportUri uri to be searched in database (Value of resource).
    * @return aProtocolInfo related to resorce which has been founded 
    */  
    CUpnpDlnaProtocolInfo* GetProtocolInfoByImportUriL(const TDesC8& aImportUri);
    
    /**
    * Gets infomation about protocol
    * @since Series S60 3.1
    * @param aContentUri
    * @parma aProtocolInfo
    * @return
    */
    TInt GetProtocolInfoL(const TDesC8& aContentUri, CUpnpDlnaProtocolInfo*& aProtocolInfo);
        
    /**
    * Gets title of the object with given id from ContentDirectory service. 
    * @since Series60 2.6
    * @param aObjectId id of the object.
    * @param aValue pointer to buffer in which the tilte of the object will be returned.
    *       Buffer for aValue has to allocated first.
    **/
    void GetTitleForUriL( TInt aObjectId, TPtr& aValue );
        
    /**
    *
    * @since Series S60 3.1
    * @param aUrlPath
    * @param aFileName
    * @param aSystemPath
    */
    TInt FindSharedFolderL( const TDesC& aUrlPath, const TDesC& aFileName, HBufC*& aSystemPath);
        
    /**
    *
    * @since Series S60 3.1
    * @return TPtrC
    */
    const TPtrC MediaRealDir();
        
        
    /**
    * Sets download directory
    * @since Series S60 3.1
    * @param aDownloadDir this directory is going to be set as download directory
    * @return TInt
    */
    void SetDownloadDirectoryL(const TDesC& aDownloadDir);
        
    /**
    * Sets thumbnail directory
    * @since Series S60 3.1
    * @param aDownloadDir this directory is going to be set as thumbnail directory
    * @return TInt
    */
    void SetThumbnailDirectoryL(const TDesC& aDownloadDir);    
    
    TInetAddr GetAddress();
    IMPORT_C void SendActionL(CUpnpAction* aAction, TUpnpErrorCode aError);
    IMPORT_C void SendMessageL( CUpnpHttpMessage* aMessage );
    CUpnpAutoDestroyObjectList* GetAutoDestroyObjectList();
    
    // For internal use only
    IMPORT_C void HttpTransferEventReceivedL(CUpnpHttpMessage& aHttpMsg);
    
    /**
    * Sends transfer failed event to Media Server       
    * @param aSessionId transaction identifier
    * @param aStatusCode HTTP Status code
    */
    void NotifyTransferFailL(TInt aSessionId, TInt aStatusCode);
    
    /**
    * Sends transfer stopped event to Media Server       
    * @param aSessionId transaction identifier
    * @param aType import/export
    */
    void NotifyTransferStoppedL( TInt aSessionId, CUpnpFileTransfer::TTransferType aType);

     // new functions for httptransfer.dll intergration
    void HttpGetStartedSoapNotifyL(TInt aSessionId);    
    void HttpPostStartedL(TInt aSessionId);   
              

    // helper functions
    TCurrentAction ExecutedAction();
    void SetExecutedAction(TCurrentAction aAction);
    
    
    /**
    * This function is a entry point for http messages.
    * received from Symbian HTTP Stack
    * @since Series 60 3.2
    * @param aSessionId identifier of the HTTP request.
    * @param aHttpStatusCode HTTP error code.
    */
    void HttpResponseReceivedL( TInt aSessionId,
                                TInt aStatusCode);   
    
    /**
    * Getter
    * @since Series 60 3.2
    * @return Event Observer
    */
    MUpnpContentDirectoryEventObserver* EventObserver();
    
public://temporary public methods

    void TransferCompleteL(TInt aResId, TInt aObjId);
    
    void TransferProgress( TInt aId, TInt aBytes, TInt aTotalBytes );
    
    void TransferStartL(const TDesC8& aImpUri);

    /** 
    * This fuction removes a TransferID from the TransferIDs Statevariable
    * @since Series 60 2.6
    * @param aTransferID a id of file transfer
    **/
    void RemoveFileTransferFromTransferIDsStateVariableL(TInt aTransferID);
    
    /** 
    * This fuction adds a TransferID to the TransferIDs Statevariable
    * @since Series 60 2.6
    * @param aTransferID a id of file transfer
    **/
    void AddFileTransferToTransferIDsStateVariableL( TInt aTransferID );
    
    /**
    *
    * @since Series S60 3.1
    * @param aFileName
    * @return boolean whether given file is drmfile
    */
    TBool IsDrmFileL(const TDesC& aFileName);
    
    /**
    *
    * @since Series S60 3.1
    * @param aFileName
    * @return boolean whether given file is drmfile
    */
    TBool IsDrmFileL(const TDesC8& aFileName);
    

     /** 
    * This functions is used for generating correct res tags for finished downloads.
    * @param aOnlyThumbnail  this method is used during two action - ImportResource and CreateObject. 
    *                           In CreateObject case res can be created by passed argument so we only thumbail has to be created
    * @since Series 60 2.6
    * @return error code. KErrNone if no errors.
    **/
    TInt CreateResTagL( TInt aSessionId, 
                       TDesC8& aFilePath, 
                        TInt aImportNumber,
                        TBool aOnlyThumbnail = EFalse  );


    /**
    * Notify transfer          
    * @param aSessionId HTTP transaction identifier 
    */    
    void NotifyOutgoingTransferL (TInt aSessionId); 
    
    
    CUpnpContentDirectoryDb*  ContentDirectoryDb();
    
    MUpnpTransferInterface* TransferHandler();
    
public: //from MUpnpTransferObserver
    
    virtual void TransferCompletedL( TInt akey, TInt aResultCode );

public: //from MUpnpHttpServerTransactionCreator

        IMPORT_C virtual void NewTransactionL(
        const TDesC8& aMethod, const TDesC8& aUri, 
        const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans );

private: //form MUpnpHttpServerObserver
    virtual void HttpEventLD( CUpnpHttpMessage* aMessage );
    
private://private methods


    void PrepareAutoDestroyObjectL(TXmlEngElement aObject, TInt aObjId);

    /**
    * Gets ip and port number
    * @since Series S60 3.1
    * @return descriptor pointer
    */
    HBufC8* GetIpAndPortDes8L();

    /**
    * Sets size
    * @since Series S60 3.1
    * @parma aSize
    * @parma aFilePath
    */
    void SetResSizeL(TXmlEngAttr aSize, const TDesC& aFilePath);
        
    /**
    * Adds Import URI to Element
    * @since Series S60 3.1
    * @param aElement
    */
    void AddImportUriToElL(TXmlEngElement aElement);
        
    /**
    *
    * @since Series S60 3.1
    * @return descriptor pointer
    */
    HBufC8* BuildImportUriLC();
    HBufC8* BuildImportUriShorterLC();
        
    /**
    * Checking whether thumbnail is available
    * @since Series S60 3.1
    * @param aFile
    * @return 
    **/
    TBool IsThumbnailAvailableL(const TDesC& aFile);
    
    /**
    * Checking whether thumbnail is available
    * @since Series S60 3.1
    * @param aFile 
    * @return
    */
    TBool IsThumbnailAvailableL(const TDesC8& aFile);
    
    /**
    * Creates thumbnail
    * @since Series S60 3.1
    * @param a SrcFile
    * @param aObjId
    */
    void CreateThumbnailL(const TDesC& aSrcFile, TInt aObjId);
    
    /**
    * Adds new res tag
    * @since Series S60 3.1
    * @param aObjEl 
    */
    void AddNewResTagL(TXmlEngElement aObjEl);
    
    /**
    * Creates a name for thumbnail
    * @since Series S60 3.1
    * @param aFileName
    * @return descriptor pointer
    */
    HBufC* CreateNameForThumbnailL(const TDesC& aFileName);
    
    /**
    *
    * @since Series S60 3.1
    * @param aRes
    */      
    void HandleLocalResourcesL(TXmlEngElement aRes, TInt aObjectId);
    
    /**
    *
    * @since Series S60 3.1
    * @parma aFileName
    * @return descriptor pointer
    */
    HBufC8* BuildContentUriL(const TDesC& aFileExt, TInt aObjectId);  
    
    /**
    *
    * @since Series S60 3.1
    * @param aAction  
    * @param aSenAndDestroy
    * @return
    */
    TInt DoActionReceivedL( CUpnpAction* aAction );
    
    /**
    * 
    * since Series S60 3.1
    * @param aObjs
    * return descriptor pointer
    */
    HBufC8* SerializeObjectsListL(const RArray<RXmlEngDocument>& aObjs);
              
    /**
    *
    * @since Series S60 3.1
    * @param aObj
    * @param aImportUri
    * @param ANewFile
    */
    void DeleteOldResourcesL( RXmlEngDocument& aObj, const TDesC8& aImportUri,
                              const TDesC& aNewFile );
    
    /**
    *
    * @since Series S60 3.1
    * @parma aMessage
    * return descriptor pointer
    */
    HBufC8* GetImportUriLC(CUpnpHttpMessage* aMessage);     
       
    /**
    *
    * @since Series S60 3.1
    * @param aFileName
    * @param aNextKey
    * @return descriptor pointer
    */
    HBufC* BuildThumbnailUrlPathL(const TDesC& aFilename, const TDesC& aNextKey);

    /**
    * This function gets mime type for specified file. 
    * @since Series 60 3.1
    * @param aFilename -  filename, for which mimeType will be taken.
    * @return pointer to variable storing mimeType. 
    */
    HBufC8* GetMimeTypeLC(const TDesC8& aFilename);    

    /**
    * Removes thumbnail creator and corresponding objectId from lists. 
    * @since Series 60 3.1
    * @param aThCreator - thumbnail creator for which the information will be deleted.
    */
    void RemoveThCreatorAndObjectIdLD( CUpnpThumbnailCreator* aThCreator);

    /**
    * Gives object id for specified thumbnail creator. 
    * @since Series 60 3.1
    * @param aThCreator - thumbnail creator for which object id will be searched for. 
    * @return object id for specified thumbnail creator. 
    */
    TInt GetObjectIdFromThCreator( CUpnpThumbnailCreator* aThCreator);

    /**
    * Gives this server address in predefined format. 
    * @since Series 60 3.1
    * @return pointer to address. 
    */
    HBufC8* CreateAddressValueLC();

    /**
    * This function creates ProtocolInfo object for further use. 
    * @since Series 60 3.1
    * @param aMimeType - mime type, for which ProtocolInfo will be created.
    * @param aAddInfo - additional information which can be choosen according to mime type and image size. 
    * @return pointer to CProtocolInfo object, which describes all protocolInfo values for this element.
    */
    CUpnpDlnaProtocolInfo* CreateProtocolInfoFromMimeL(const TDesC8& aMimeType, TInt aAddInfo );

    /**
    * This function retrieves title from object description. 
    * @since Series 60 3.1
    * @param aObjId - objecty id for which title will be retrieved. 
    * @return pointer to HBufC, with specified value. 
    */
    HBufC* GetFileNameL( TInt aObjId );    
    HBufC* GetFileNameWithoutPathL( TInt aObjId );
    
    /**
    * This function add underscore and number to existing filename. 
    * @since Series 60 3.2
    * @param aFileName - file that probably exist. 
    * @return pointer to HBufC, with specified value. 
    */
    HBufC* CreateFileNameL( const TDesC16& aFileName );
   
    /**
    * This function puts on stack upnp:class value for specific mime type.  
    * @since Series 60 3.1
    * @param aMimeType - mime type from which additional information will be retrieved. 
    * @return pointer to HBufC8 object with new value, if the type is unknown there is null returned.
    * Mapping: 
    * image/* - object.item.imageItem
    * video/* - object.item.videoItem 
    * audio/* - object.item.audioItem 
    * text/* - object.item.textItem 
    */ 
    HBufC8* GetItemTypeFromMimeTypeLC(const TDesC8& aMimeType);
        
    /**
    * This function compares upnp:class tag value, with value denoted by aMimeType. 
    * @since Series 60 3.1
    * @param obj - object for which needed changes will be applied.
    * @param aMimeType - mime type from which additional information will be retrieved. 
    */ 
    void CheckItemTypeForObjectL( RXmlEngDocument& obj, const TDesC8&  aMimeType );
        
  
    /**
    * This function returns a XML fragment containing the XML in the parameter
    * @since Series 60 2.6
    * @param aBuffer the XML is in this buffer
    * @return A new CSenDomFragment that is on the cleanup stack
    */
    RXmlEngDocument XmlFragmentL(const TDesC8& aBuffer);
        
    /**
    * This fuctions converts one XML element to a string 
    * @since Series 60 2.6
    * @param aResp XML string
    * @param aSen an Sen element to be converted to string
    */
    void BrowseElementL( HBufC8*& aResp, TXmlEngElement& aSen );

    /**
    * This fuctions is a getter for container updateID. 
    * @since Series 60 2.6
    * @param aID ID of the container which updateID is returned
    * @return a updateId of container
    */
    TInt ContainerUpdateId( const TDesC8& aContainerID );


    /**
    * Function to add data to metadb
    * @param aFragment fragment to add to db
    * @param aContainer container where to add data
    * @param aId parameter returned by AddToMetaDbL method, stands for item id
    * @return upnperrorcode descriping situation
    */
    TUpnpErrorCode AddToMetaDbL( RXmlEngDocument& aFragment, TInt aContainer, TInt* aId,
                                 TBool aIsLocal );

    /*
    *
    * @since Series S60 3.1
    * @param aFragment
    * @param aImportUris
    * @param aRess
    * @param aLocal
    * @param aCreateThumbnailRes
    * @param aNextKey
    * @return upnperrorcode descriping situation
    */
    TUpnpErrorCode PrepareCreateOperationL( RXmlEngDocument& aFragment, 
        RArray<TXmlEngAttr>& aImportUris, RArray<TXmlEngElement>& aRess, TBool& aLocal,
        TBool& aCreateThumbnailRes, TInt& aNextKey );

    /**
    * Related to CreateObject, resultset of create is created here
    * @since Series 60 2.6
    * @param aAction action data structure is needed here
    * @param aElement element added to db
    * @param aId element's id
    **/
    void ConstructCreateResultsetL( CUpnpAction*& aAction, 
                                    RXmlEngDocument aElement, 
                                    TInt aId );

 
    /** 
    * Function for creating a new object
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode CreateObjectL( CUpnpAction*& aAction );
    /** 
    * Function for browsing values of objects
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode BrowseL( CUpnpAction*& aAction );

    /** 
    * Function for destroying objects
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode DestroyObjectL( CUpnpAction*& aAction );

    /** 
    * Function for creating refense items
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode CreateReferenceL( CUpnpAction*& aAction );

    /** 
    * Function to solve CDS's search capabilities
    * @since Series 60 2.6
    * @param aAction received action
    * @retrun upnperrorcode descriping situation
    **/
    TUpnpErrorCode GetSearchCapabilitiesL( CUpnpAction*& aAction );

    /** 
    * Function to solve CDS's sort capabilities
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode GetSortCapabilitiesL( CUpnpAction*& aAction );

    /** 
    * Getter for CDS's SystemUpdateID
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode GetSystemUpdateIdL( CUpnpAction*& aAction );

    /** 
    * Function for importing resources
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode ImportResourceL( CUpnpAction*& aAction );

    /** 
    * Function for exporting resources
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    */
    TUpnpErrorCode ExportResourceL( CUpnpAction*& aAction );

    /** 
    * Function for stopping ongoing file transfer (import / export)
    * @since Series 60 2.6
    * @param aAction received action
    * @retrun upnperrorcode descriping situation
    **/
    TUpnpErrorCode StopTransferResourceL( CUpnpAction*& aAction );

    /** 
    * Function for getting information about ongoing file transfer
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode GetTransferProgressL( CUpnpAction*& aAction );

    /** 
    * This functions removes a "res" tag from a object
    * @since Series 60 2.6
    * @param aAction received action
    * @return upnperrorcode descriping situation
    **/
    TUpnpErrorCode DeleteResourceL( CUpnpAction*& aAction );

    /**
    * This function changes file name and its extension. 
    * @since Series 60 3.1
    * @param aFilePath - the target file. 
    * @param aMimeType - mime type, for which file extension will be choosen. 
    */
    HBufC8* ChangeFileNameWithExtensionL(TDesC8& aFilePath, const TDesC8& aMimeType );
 
 
   
    /**
    * @param aOnlyThumbnail  this method is used during two action - ImportResource and CreateObject. 
    *           In CreateObject case res can be created by passed argument so we only thumbail has to be created
    */
    TInt CreateResTagInternalL( TInt aSessionId, 
                                TDesC8& aFilePath,
                                HBufC8* aImportUri, 
                                TBool aOnlyThumbnail );

    /**
    * calls CreateResTagInternalL but KErrCorrupted is TRAPed in order to invoke recovery on DB
    */
    TInt CreateResTagInternalWithDBRecoveryL( TInt aSessionId,
                                              TDesC8& aFilePath, 
                                              HBufC8* aImportUri, 
                                              TBool aOnlyThumbnail = EFalse );
    
private:    // New functions    

   
    void DoSendActionL(CUpnpAction* aAction, TUpnpErrorCode aError);
    void DoSendMessageL(CUpnpHttpMessage* aMessage);
    TBool IsValidUrlPath(const TDesC& aUrlPath);
    
    /**
    * Gets protocolInfo from mimeType.
    * @param aMimeType mime type for which protocolInfo will be created.
    * @return string with protocolInfo
    */
    HBufC8* ProtocolInfoFromMimeL( const TDesC8& aMimeType, TInt aAddInfo, TBool aOpRangeParam,
                                   TBool aCiParam = EFalse );

    HBufC8* ProtocolInfoFromMimeL(const TDesC8& aMimeType, TXmlEngElement& aResEl);
    
    /**
    * Notify message when the thumbnail creation has been finished. 
    * From MUpnpThumbnailCreatorObserver interface.
    * @param aThCreator this is a thumbnail creator.
    * @param aResult result from thumbnail creation.
    */
    void NotifyThumbnailResultL( CUpnpThumbnailCreator* aThCreator, TInt aResult );

    /**
    * Notify message when the thumbnail creation has been finished. 
    * From MUpnpThumbnailCreatorObserver interface.
    * @param aThCreator this is a thumbnail creator.
    * @param aResult result from thumbnail creation.
    */
    CUpnpThumbnailCreator* CreateThumbnailCreatorL(TInt aObjectId);
    
    /**
    * Cheks if object exists.
    * @param aObjectId
    */
    TBool IsObjectExistL( TInt aObjectId );
    
    /**
    * Cheks if object is valid.
    * if object doesn't exist or has no parent 
    * object is removed from autodestoy table and EFalse is returned
    * @param aObjectId
    */
    TBool ValidateObjectL( TInt aObjectId );
         
    /**
    * Handles DB error
    * Tries to recover, if fails tries to recreate
    * @since Series S60 3.2
    * @return TInt - KErrNone if succeed
    */
    TInt HandleDbError( TInt aError );
    
    /**
    * Checks DB and disk Space
    * Tries to recover DB, if fails tries to recreate
    * @since Series S60 3.2
    * @param aAction received action
    * @return EFalse if succeed
    */
    TBool IsDbOrDiskProblemL( CUpnpAction* aAction );
    
    /**
    * Determines if DB has benn recovered successfully
    * returns ETrue if yes
    * @param a Error - DB is revovered only if aError equal KErrCorrupted
    * @since Series S60 3.2
    * @returns ETrue if succeed 
    */
    TBool DataBaseHasBeenRecoveredL( TInt aError );
    
    /**
    * Returns DB creation status
    * @since Series S60 3.2
    * @return TBool
    */
    TBool IsDbCreated(); 

    /**
    * Updates import URI
    * @since Series S60 3.2
    * @return TPtrC8
    */
    TPtrC8 UpdateImportUriL( const TDesC8& aImportUri );
    
private: // Constructors
    
    /**
    * C++ default constructor.
    */
    CUpnpContentDirectory( CUpnpDevice& aDevice, 
                           MUpnpThumbnailCreatorObserver* aThmbObs,
                           MUpnpTransferInterface* aTransfer
                         );

    /**
    * Second phase of the constructor.
    * Initialises a DOM tree.
    * By default Symbian 2nd phase constructor is private.
    * @param service Parent service
    */
    void ConstructL( TUpnpCdSettings aSettings, 
                     CUpnpMetadataStorage* aMetadataStorage 
                   );
    
    void ConstructHttpL();
    
private:
    
    //Array containing objects ids, which will be updated after Thumbnail creation.
    RArray<TInt> iThObjectIds;
    
    // Array of thumbnail creators in which creators can be searched for.
    RPointerArray<CUpnpThumbnailCreator> iThumbnailCreators;

    // Next free object ID, owned
    TInt iNextFreeID;

    // Pointerarray for pending actions (used in Import and Export), owned
    //RPointerArray<CUpnpAction> iPendingActions;

    // Elementfactory, owned
    CUpnpElementFactory* iElementdb;
    
       
    // Database
    CUpnpContentDirectoryDb* iContentDirectoryDb;
        
    // file session
    RFs iFs;
    
    // Download directory
    HBufC* iDownloadDir;
    
   // Thumbnail directory
    HBufC* iThumbDir;
    // connection try to bitmap and font server. 
    // if there is no such connection there will be no thumbcreated. 
    TInt iBitmapServerConnection;
        
    TBool iEcomUsed;
        
    // Thumbnail creator observer
    MUpnpThumbnailCreatorObserver* iThmbObs;

    // auto-destroy object list    
    CUpnpAutoDestroyObjectList* iAutoDestroyObjects;

    
    // Event observer
    MUpnpContentDirectoryEventObserver* iEventObserver;
    
    // Thumbnail creation flag
    TBool iCreateThumbnails; 
    
    // Max number of browsed object
    TInt iMaxRequestCount;

    TCurrentAction iCurrentAction;
    
    RXmlEngDOMImplementation iDOMImpl;
    
    MUpnpCdsSender* iSender; // not owned
        
    CUpnpMimeMapper* iMimeToExtMap;
        
    HBufC* iObjectsXmlPath;
    
    CUpnpStateHandler* iStateHandler;
         

    CUpnpTransferController* iTransferController;
    
    MUpnpTransferInterface* iTransferHandler;
    
    CUpnpDlnaFilter* iDlnaFilter;
    
    CUpnpHttpServerSession* iHttpServerSession;
    
    CUpnpSecurityManager* iSecurityManager;
    
    };

#endif  // C_CUPNPCONTENTDIRECTORY_H

// End Of File
