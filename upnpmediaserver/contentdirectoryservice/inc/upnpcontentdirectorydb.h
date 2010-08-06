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
* Description:  Database for MediaServer
*
*/



#ifndef C_CUPNPCONTENTDIRECTORYDB_H
#define C_CUPNPCONTENTDIRECTORYDB_H

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h>
#include <s32file.h>
#include <flogger.h>
#include <upnperrors.h>
#include <xml/dom/xmlengdom.h>


// CLASS DECLARATION
class CUpnpElementBean;
class CUpnpFilterElement;
class CUpnpAttributeBean;
class CUpnpObjectBean;
class CUpnpDlnaProtocolInfo;
class CUpnpShareMapBean;
class CUpnpResourcesBean;
class CUpnpMetadataStorage;



/**
*  Database for AVContentDirectory.
*  Implements a database which is used to store descriptors of shared content.
*
*  @lib ContentDirectoryDb.lib
*  @since Series 60 3.0
*/
class CUpnpContentDirectoryDb : public CBase
{
 public:  // Constructors and destructor
 
    /**
    * Two-phased constructor.
    */
    static CUpnpContentDirectoryDb* NewLC(CUpnpMetadataStorage* aMetadataStorage);
    static CUpnpContentDirectoryDb* NewL(CUpnpMetadataStorage* aMetadataStorage);

    /**
    * Destructor
    */
    ~CUpnpContentDirectoryDb();
    
 public: // New functions
    
    /**
    * Checks if object with the given id exists, is a container and its 'restricted' attribute has value of 'false'
    * @since Series S60 3.0
    * @param aObjId id of the object to be checked
    * @return returns EUpnpOk if object with the given id exists in database and is a "non restricted conainer"
    */
    TUpnpErrorCode CheckObjectRestrictionL(TInt aObjId);

    /**
    * Inserts object into database 
    * @since Series S60 3.0
    * @param aFragment RDocuemnt of the document to be added to database
    * @param aId id of the object  
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode InsertObjectL( RXmlEngDocument& aFragment, TInt aContainer, TInt* aId);
    
    /**
    * Function returns DIDL-Lite XML fragment with metadata for requested object.
    * IMPORTANT: 1. Caller takes responsibility for returned document and is 
    *               obliged to close it.
    *        2. The root element of the document is <DIDL-Lite> element and
    *               the object's element is its direct, onluy child.
    * @param aObjectID id of requested object.
    * @param aDoc handler to document containing object's metadata.
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode GetObjectL( TInt aObjectID, RXmlEngDocument& aDoc, 
                               const TDesC8& aFilterString);
                    
    /**
    * Gets id of reffered object 
    * @since Series S60 3.0
    * @param aObjectId object id
    * @return reffered object id.
    */
    TInt ReferedObjectIdL( TInt aObjectId );
    
    /**
    * Gets object list
    * @since Series S60 3.1
    * @param aParentId
    * @parem aList
    * @return upnperrorcode descriping situation
    */
    TUpnpErrorCode GetObjectListL(TInt aParentId, RArray<TInt>& aList);
    
    /**
    * Deletes object from database
    * @since Series S60 3.0
    * @param aObjId id of the object to be deleted
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode DeleteObjectL(TInt aObjId,RArray<TInt>& aRefList, TBool aDeleteResource=ETrue);
    
    /**
    * Updates the metadata of the object with specified id
    * @since Series S60 3.0
    * @param aObjId id of the object to be updated
    * @param  aFragment XML DOM tree with the updated object's metadata
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode UpdateObjectL(TInt aObjId, RXmlEngDocument& aFragment);
    

    /**
    * Deletes the res elements with the specified value.
    * @since Series S60 3.0
    * @param aResVal value of the res element which is to be deleted
    * @param aTumbailResVal value of the thumbail res element which is to be deleted
    */
    void DeleteResourceL(const TDesC8& aResVal, RArray<TInt>& aContIds);

    /**
    * Gets object by its attribute value.
    * @since Series S60 3.0
    * @param aDoc on return, it contains the object's data
    * @param aObjId on, return it contains the object's id
    * @param aAttrName attribute's name
    * @param aAttrVal attribute's value
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode GetObjectByAttrL(RXmlEngDocument& aDoc, TInt* aObjId, 
                                    const TDesC8& aAttrName, const TDesC8& aAttrVal);   

    /**
    * Checks the restrictions of the parent object of the given object.
    * @since Series S60 3.0
    * @param aObjId object's id
    * @return returns EUpnpOk if object with the given id exists in database and is a "non restricted conainer"    
    */
    TUpnpErrorCode CheckParentRestrictionL(TInt aObjId);

    /**
    * Gets the object id by its attribute value.
    * @since Series S60 3.0
    * @param aAttrName attribute's name
    * @param aAttrVal attribute's value
    * @return object's id
    */
    TInt GetObjIdByAttrL(const TDesC8& aAttrName, const TDesC8& aAttrVal);

    /**
    * Gets the object id by its attribute value. Uses LIKE operator for comparison
    * @since Series S60 3.0
    * @param aAttrName attribute's name
    * @param aAttrVal attribute's value
    * @return object's id
    */
    TInt GetObjIdByAttrLikeL(const TDesC8& aAttrName, const TDesC8& aAttrVal);


    /**
    * Prepares a new RXmlEngDocument with DIDL-Lite element 
    * @since Series S60 3.0
    * @return Returns just created document
    */
    RXmlEngDocument PrepareDidlDocumentL();
    
    
    /**
    * Find protocolInfo by contentUri (Not by importUri).
    * @param aContentUri uri to be searched in database (Value of resource).
    * @param aProtocolInfo ProtocolInfo related to resorce which has been founded. Ownership is transfered to the caller.
    * @return KErrNone if successful, otherwise another of the system-wide error codes 
    */
    TInt GetProtocolInfoL(const TDesC8& aContentUri, CUpnpDlnaProtocolInfo*& aProtocolInfo); // virtual

    
    /**
    * Gets object list
    * @since Series S60 3.1
    * @param aIds
    * @param aFilter
    * @param aObjs
    * @return upnperrorcode descriping situation
    */
    void GetObjectListL(const RArray<TInt>& aIds, const TDesC8& aFilter, RArray<RXmlEngDocument>& aObjs);
    
    /**
    *
    * @since Series S60 3.1
    * @param aObjId
    * @return boolean whether can be nested
    */
    TBool CanBeNestedL(TInt aObjId);
    
    /**
    *
    * @since Series S60 3.1
    * @param aResBean
    */
    void AddResourceL(CUpnpResourcesBean* aResBean, TInt aObjId);
    
    /**
    * Deletes resource
    * @since Series S60 3.1
    * @param aResId
	* @param aDeleteFile
    */
    void DeleteResourceByResIdL(TInt64 aResId, TBool aDeleteFile = ETrue);
    
    /**
    * Deletes resource
    * @since Series S60 3.1
    * @param aResId
	* @param aDeleteFile
    */
    void DeleteResourceByObjIdL(TInt aObjId, TBool aDeleteFile = ETrue);
    
    /**
    *
    * @since Series S60 3.1
    * @param aOldDir
    */
    void NewMediaDirectoryL(const TDesC& aOldDir);
    
    /**
    * Gets resource
    * @since Series S60 3.1
    * @param aResId
    * @return pointer to CUpnpResourceBean
    */
    CUpnpResourcesBean* GetResourceL(TInt64 aResId);
    
    /**
    * Gets attribute
    * @since Series S60 3.1
    * @param aElmId
    * @param aAttrName
    * @param aAttrVal
    * @return pointer to CUpnpAttributesBean
    */
    CUpnpAttributeBean* GetAttrByNameL(TInt aElmId, const TDesC8& aAttrName, HBufC8*& aAttrVal);
    
    /**
    * Gets attribute
    * @since Series S60 3.1
    * @param aAttrName
    * @param aAttrVal
    * @return pointer to CUpnpAttributeBean
    */
    CUpnpAttributeBean* GetAttrByValueL(const TDesC8& aAttrName, const TDesC8& aAttrVal);
    
    /**
    * Gest protocol info
    * @since Series S60 3,1
    * @param aImportUri
    * @return descriptor pointer
    */
    HBufC8* GetProtocolInfoByImportUriL(const TDesC8& aImportUri);
    
    
    /**
    * Getter
    * @since Series S60 3.1
    * @return TInt
    */
    TInt GetKeyForUriL();
    
    /**
    * Gets first object ID by element value
    * @param aElmName - element name
    * @param aElmValue - element value
    * @return object ID
    */
    TInt GetObjectIdByElementValueLikeL(const TDesC8& aElmName, const TDesC8& aElmValue);

    /**
    * Gets the parent object's id of the object with given id.
    * @since Series S60 3.1
    * @param aObjectId child object id
    * @return parentid
    */
    TInt GetParentIdL(TInt aObjectId);
    /**
    * Gets the next object's id .
    * @since Series S60 3.1
    * @return id
    */
    TInt GetObjectIdL();
    /**
    * Gets the next object's id .
    * @since Series S60 3.1
    * @return id
    */
    HBufC8* GetObjectTitleL(TInt aId);

   /**
    * Checks if database have propper columns in each table.
    * If not it leaves with KErrCorrupt.
    * @since Series S60 3.2
    */
    void CheckDatabaseL();  
    
   /**
    * Handles DB error
    * Tries to recover, if fails tries to recreate
    * @since Series S60 3.2
    * @return TInt - KErrNone if succed
    */
    TInt HandleDbError( TInt aError );

    /**
    * Recreates the database file.
    * @since Series S60 3.1
    */
    TInt RecreateDatabaseFile();
    
    /**
    * Returns DB creation status
    * @since Series S60 3.2
    * @return TBool
    */
    TBool IsDbCreated();

 private:
    
    /**
    * Getter
    * @since Series S60 3.1
    * @param aObjId
    * @return pointer to CUpnpObjectBean
    */
    CUpnpObjectBean* GetObjBeanLC(TInt aObjId);
    
    /**
    * Gets object
    * @since Series S60 3.1
    * @param aIns
    * @param AresVal
    */
    void GetObjectsByResourceL(RArray<TInt>& aIds, const TDesC& aResVal);
       
      
    /**
    * Gets element
    * @since Series S60 3.1
    * @param aElmName
    * @param ElmValue
    * @return TInt
    */
    TInt GetElmIdByNameAndValL(const TDesC& aElmName, const TDesC& aElmValue);
    
    /**
    * Gets attribute
    * @since Series S60 3.1
    * @param aElmId
    * @param aAttrName
    * @param aAttrValue
    */
    void GetAttrValueByNameL(TInt aElmId, const TDesC8& aAttrName, HBufC8*& aAttrVal);

    /**
    * Performs resource deletion of the object being destroyed
    * @since Series S60 3.0
    * @param aObjId aObjId id of the object being destroyed
    * @param aObjId aCd pointer to the ContentDirectory object - it provides a function which deletes
    *                  a file from the private directory
    */
    void DeleteResourcesL(TInt aObjId);

    /**
    * Performs the DeleteResource operation. 
    * Deletes each res element with the value of aResVal and corresponding thumnail res elements
    * @since Series S60 3.0
    * @param aResVal value of the res whis is to be deleted
    * @param aThumbailResVal value of the thumbail res whis is to be deleted
    */
    void DoDeleteResourceL(const TDesC8& aResVal, RArray<TInt>& aContIds);


    /**
    * Sets the childCount attribute in the given object's element.
    * @since Series S60 3.0
    * @param aElement object's element
    * @param aId object's id
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    void SetChildCountL(TXmlEngElement aElement, TInt aId);
    
    /**
    * Adds all (existing in the database) properties to the object's element.
    * @since Series S60 3.0
    * @param aElement object's element
    * @param aObjBean object table description object
    */
    void AddAllPropertiesL(TXmlEngElement aElement, CUpnpObjectBean* aObjBean);

    /**
    * Adds new attribute to the element.
    * @since Series S60 3.0
    * @param aElement object's element
    * @param aAttrName the name of the attribute which is to be added
    * @param aElementId id of the element which the attribute belongs to
    * @param aObjectId id of the object which the elements belongs to
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    void AddAttributeL( TXmlEngElement aElement, const TDesC8& aAttrName, TInt aElementId, TInt aObjectId );

    /**
    * Adds a requested properties to the object's element.
    * @since Series S60 3.0
    * @param aObjectID object's id
    * @param aElement an array containing filter elements of the requested property
    * @param aFilterElements ?description
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode AddRequestedPropertiesL(TInt aObjectID,TXmlEngElement aElement,
                                           const RPointerArray<CUpnpFilterElement>& aFilterElements);

    /**
    * Adds requested element to the object's element.
    * @since Series S60 3.0
    * @param aElement an element wich we want to add a new element to
    * @param aObjectID id of the object
    * @param aReqElement filter object of the element to be added
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    void AddElementL(TXmlEngElement aElement, TInt aObjectID, const CUpnpFilterElement* aReqElement);

    /**
    * Adds required property to the object.
    * @since Series S60 3.0
    * @param aElement TXmlEngElement of the object which is to be added the required property
    * @param aObjId id of the object being processed
    * @param aFilterElement CUpnpFilterElement object of the property to be added
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode AddRequiredPropertyL(TXmlEngElement aElement, TInt aObjId, CUpnpFilterElement* aFilterElement);
    
    /**
    * Parses filter string.
    * @since Series S60 3.0
    * @param aFilter filter string to be parsed
    * @param aList  On return, contains CFilterElements objects of the filter string
    */
    void ParseFilterStringL(const TDesC8& aFilter, RPointerArray<CUpnpFilterElement>& aList);

    /**
    * Performs get object operation.
    * @since Series S60 3.0
    * @param aObjectID id of requested object.
    * @param aDoc handler to document containing object's metadata.
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode DoGetObjectL(TInt aObjectID, RXmlEngDocument& aDoc, TBool aAll);
 
    /**
    * Inserts element into database 
    * @since Series S60 3.0
    * @param aElementel TXmlEngElement of the element to be inserted
    * @param aObjId id of the object this element belongs to
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode InsertElementL(const TXmlEngElement& aElementel, TInt aObjId);
 
    /**
    * Performs object update; a transaction should be started before calling this function
    * @since Series S60 3.0
    * @param aObjId id of the object to be updated
    * @param  aFragment XML DOM tree with the updated object's metadata
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode DoUpdateObjectL(TInt aObjId, TXmlEngElement aFragment);

    /**
    * Inserts updated object into database
    * @since Series S60 3.0
    * @param aObjId id of the object
    * @param  aFragment XML DOM tree with the updated object's metadata
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode InsertUpdatedL(TInt aObjId, TXmlEngElement& aElement);

    /**
    * Sets the id attribute in the given object; adds attribute if does not exist
    * @since Series S60 3.0
    * @param aElement TXmlEngElement of the object
    * @return value of the id
    */
    TInt SetObjectIdL(TXmlEngElement& aElement);

    /**
    * Inserts object into database; a transaction should be started before calling this function 
    * @since Series S60 3.0
    * @param param descr
    * @return EUpnpOk, if successful; Leaves if the object could not be inserted into the database
    *     In case it leaves the transaction must be rolled back
    */
    TUpnpErrorCode DoInsertObjectL( TXmlEngElement& aElement, TInt aContainerId, TInt* aId);


    
    /**
    * Inserts attributes of the given element into the database
    * @since Series S60 3.0
    * @param aElement TXmlEngElement of the element 
    * @param aElmId id of the element
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode InsertAttributesL(const TXmlEngElement aElement, TInt aElmId,TInt aObjId);
    

    /**
    * Checks if the given attribute has a corresponding marker attr in the given array
    * @since Series S60 3.0
    * @param aAttr TXmlEngAttr of the attribute to be chceck
    * @param aMarkers an array containing marker attributes
    * @return ETrue if the attribute is required, otherwise EFalse
    */
    TBool IsAttrRequiredL(const TXmlEngAttr& aAttr, RArray<TXmlEngAttr>& aMarkers);

    /**
    * C++ default constructor.
    */
    CUpnpContentDirectoryDb(CUpnpMetadataStorage* aMetadataStorage);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * If data base file is present it just opens it, 
    * otherwise creates new database storage file.
    * @since Series S60 3.0
    * @param aDatabaseFileName name of storage file
    */
    void ConstructDbL(const TFileName& aDatabaseFileName);
    
    /**
    * Opens existing database.
    * @since Series S60 3.0
    * @param aDatabaseFileName name of storage file
    */
    void OpenDbL( const TFileName& aDatabaseFileName );
    
    /**
    * Adds elements that are children of inquired object to passed DOM TXmlEngElement.
    * @param aObjectID - id of the object, which child elements will be added.
    * @param aObjectElement - element to which the elements will be added.
    */
    void AddElementsL( TInt aObjectID, TXmlEngElement aObjectElement, TBool aAll );
    
    /**
    * Adds attributes to passed DOM TXmlEngElement.
    * @param aElementID - id of the element, which attributes will be added.
    * @param aObjectId id of the object which the elements belongs to
    * @param aElement - element to which the attributes will be added.
    */
    void AddAttributesL( TInt aElementID, TInt aObjectID, TXmlEngElement& aElement, TBool aAll );
    

    

 private:    // Data
    CUpnpMetadataStorage* iMetadataStorage; // not owned
    RXmlEngDOMImplementation iDOMImpl;
 };

#endif //CCONTENTDIRECTORYDB_H

// End Of File
