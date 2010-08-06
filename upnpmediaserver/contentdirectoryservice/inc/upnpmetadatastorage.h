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



#ifndef UPNPMETADATASTORAGE_H
#define UPNPMETADATASTORAGE_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <d32dbms.h>
#include <s32file.h>
#include <upnperrors.h>
#include <xml/dom/xmlengdom.h>

// FORWARD DECLARATIONS
class CUpnpObjectBean;
class CUpnpElementBean;
class CUpnpAttributeBean;
class CUpnpResourcesBean;

// CLASS DECLARATION

/**
*  Metadata storage
*
*  @lib avcontentdirectory.lib
*  @since Series 60 3.0
*/
class CUpnpMetadataStorage : public CBase
{
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    */
    static CUpnpMetadataStorage* NewL(const TDesC& aDatabaseFileName);
    static CUpnpMetadataStorage* NewLC(const TDesC& aDatabaseFileName);
    
    /**
    * Destructor.
    */
    virtual ~CUpnpMetadataStorage();

public: // New functions
    
    /**
    * Deletes object from database
    * @since Series S60 3.0
    * @param aObjId id of the object to be deleted
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
        TBool IsContainerL(TInt aObjId);
        void DeleteResourcesL(RArray<TInt>& aObjList);
        void AddResourceL(CUpnpResourcesBean* aResBean, TInt aObjId);
       	TUpnpErrorCode DoDeleteObjectL(TInt aObjId, TBool aDeleteResource, TBool aDeleteRef=ETrue );
		void InsertIntoAttributeTableL(TInt aElmId, CUpnpAttributeBean* aAtrBn, TInt aObjId, TInt aAtrId);
 		TInt InsertIntoElementTableL(TInt aObjId, CUpnpElementBean* aElmBn, TInt aElmId);
		TInt InsertIntoObjectTableL(TInt aContainer, CUpnpObjectBean* aObjBn);
    /**
    * @since Series S60 3.1
    * @param aObjId
    * @return RDbView
    */
    RDbView GetObjViewL(TInt aObjId);
        
    /**
    *
    * @since Series S60 3.1
    */
    void BeginTransactionL();
    
    /**
    *
    * @since Series S60 3.1
    */
    void CommitTransactionL();
    
    /**
    *
    * @since Series S60 3.1
    */
    void RollbackTransactionL();
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aObjectID
    * @param aAll
    * @return RDbView
    */
    RDbView GetElementsViewL(TInt aObjectID, TBool aAll);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aElementID
    * @param aAll
    * @return RDbView
    */
    RDbView GetAttributesViewL(TInt aElementID, TBool aAll);
       
    /**
    * Getter
    * @since Series S30 3.2
    * @param aObjectID
    * @param aAll
    * @return RDbView
    */
    RDbView GetAttributesViewByObjectIdL( TInt aObjectID, TBool aAll );

    /**
    * Getter
    * @since Series S30 3.1
    * @param aAttrNameID
    * @param aAttrVal
    * @return 
    */
    TInt GetElmIdForAttrL(const TDesC8& aAttrName, const TDesC8& aAttrVal);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aAttrName
    * @param aAttrVal
    * @return 
    */
    TInt GetElmIdForAttrLikeL(const TDesC8& aAttrName, const TDesC8& aAttrVal);
        
    /**
    * 
    * @since Series S30 3.1
    * @param aResVal
    * @param aObjId
    */
    void DeleteResElL(const TDesC& aResVal, TInt ObjId);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aResVal
    * @return RDbView
    */
    RDbView GetViewOfObjectListForResL(const TDesC& aResVal);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aElmName
    * @param aElmValue
    * @return RDbView
    */
    RDbView GetViewOfElmIdByNameAndValL(const TDesC& aElmName,  const TDesC& aElmValue);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aAttrName
    * @param aAttrVal
    * @return RDbView
    */
    RDbView GetAttrViewL(const TDesC8& aAttrName, const TDesC8& aAttrVal);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aElmId
    * @param aAttrName
    * @return RDbView
    */
    RDbView GetAttrViewL(TInt aElmId, const TDesC8& aAttrName);
    
    /**
    * Getter
    * @since Series S30 3.2
    * @param aBjectId
    * @param aAttrName
    * @return RDbView
    */
    RDbView GetAttrViewByObjectIdL(TInt aBjectId, const TDesC8& aAttrName);    
        
    /**
    * 
    * @since Series S30 3.1
    * @param aSqlQuery
    * @return RDbView
    */
    RDbView PrepareViewL(const TDesC& aSqlQuery);
        
    /**
    * Getter
    * @since Series S30 3.1
    * @param aSqlQuery
    * @return RDbView
    */
	RDbView GetEvaluatedViewL(const TDesC& aSqlQuery);
 	HBufC8* GetObjectPathL(TInt aObjId);
 	HBufC8* GetObjectTitleL(TInt aObjId);
	void ExecuteL(const TDesC& aSqlCommand);
 	CUpnpResourcesBean* GetResourceL(TInt64 aResId);
	void DeleteResourceByObjIdL(TInt aObjId, TBool aDeleteFile=ETrue);
	void DeleteResourceByResIdL(TInt64 aResId, TBool aDeleteFile=ETrue);
	void DeleteObjectsL(TInt aObjId);
	void DeleteElementsL(TInt aObjId);
	void DeleteAttributesL(TInt aObjId);
	void DeleteReferencesL(TInt aObjId,RArray<TInt>& aRefList);

    /**
    * Performs object deletion; a transaction should be started before calling this function
    * @since Series S60 3.0
    * @param id of the object to be deleted
    * @param aObjId aCd pointer to the ContentDirectory object - it provides a function which deletes
    * 				a file from the private directory
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
 	TInt DoDeleteObjectL(TInt aObjId,RArray<TInt>& aRefList,
 	                     RArray<TInt>& aResList, TBool aDeleteResource, TBool aLocal, TBool aDeleteRef = ETrue );

    /**
    * Checks the restrictions of the parent object of the given object.
    * @since Series S60 3.0
    * @param aObjId object's id
	* @return returns EUpnpOk if object with the given id exists in database and is a "non restricted conainer"    
	*/
	TUpnpErrorCode CheckParentRestrictionL(TInt aObjId);

    /**
    * Deletes object from database
    * @since Series S60 3.0
    * @param aObjId id of the object to be deleted
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
	TUpnpErrorCode DeleteObjectL(TInt aObjId,RArray<TInt>& aRefList, TBool aDeleteResource,
	                             TBool aLocal = EFalse, TBool aDeleteRef = ETrue );
   	

    /**
    * Gets the descriptor value of the specified column in the rowset.
    * @since Series S60 3.0
    * @param aRowset rowset object
    * @param aColNo the number of the column to be get the value of
    * @return descriptor containing the value of the specified column,
    *       the caller should delete it when it is no longer needed
    */
    HBufC8* GetColDesValFromRowSetL(RDbRowSet aRowset, TDbColNo aColNo);

    /**
    * Gets the value of childCount property for the given object.
    * @since Series S60 3.0
    * @param aObjId object's id
    * @return value of childCount property
    */
    TInt GetChildCountL(TInt aObjId);
        
    /**
    * Gets the parent object's id of the object with given id.
    * @since Series S60 3.0
    * @param aObjId child object id
    * @return perent object id
    */
    TInt GetParentIdL(TInt aObjId);

    /**
    * Gets the id of the object which the element with given id belongs to.
    * @since Series S60 3.0
    * @param aId element's id
    * @return object's id
    */
    TInt GetObjIdForElmIdL(TInt aId);

    /**
    * Gets id of reffered object 
    * @since Series S60 3.0
    * @param aObjectId object id
    * @return reffered object id.
    */
    TInt ReferedObjectIdL( TInt aObjectId );

    /**
    * Gets the next free id in the specified table 
    * @since Series S60 3.0
    * @param aName name of the table
    * @return TInt next free id in the specified table
    */
    TInt GetNextKeyL(const TDesC8& aName);
    TInt GetNextKeyL(const TDesC8& aName, TInt aAmount);
    /**
    * Gets the next free id in the specified table 
    * @since Series S60 3.0
    * @param aName name of the table
    * @return TInt next free id in the specified table
    */
    TInt NextKeyL(const TDesC8& aName);

    /**
    * Insert object's data into 'object' table
    * @since Series S60 3.0
    * @param aElement TXmlEngElement of the object to be added
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    void InsertObjectIntoObjectTableL(const TXmlEngElement& aElement);

    /**
    * Inserts single attribute into the database 
    * @since Series S60 3.0
    * @param aAttr TXmlEngAttr of the attribute to be inserted
    * @param aElmId id of the element this attribute belongs to
    * 2param aIsRequired 
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    void InsertAttributeL(const TXmlEngAttr aAttr, TInt aElmId, TBool aIsRequired, TInt aObjId);

    /**
    * Inserts element into database 
    * @since Series S60 3.0
    * @param aElementel TXmlEngElement of the element to be inserted
    * @param aObjId id of the object this element belongs to
    */
    TInt  InsertElementL(const TXmlEngElement& aElementel, TInt aObjId);

    /**
    * Gets the database view of the named element of the specified object.
    * @since Series S60 3.0
    * @param aObjId object's id
    * @param aElName name of the element which we want to get the view of
    * @return ?description
    */
    RDbView GetElementViewL(TInt aObjId, const TDesC8& aElName);

    /**
    * Adds an attribute from object table to the object's main element.
    * @since Series S60 3.0
    * @param aElement object's element
    * @param aId object's id
    * @param aName name of the attribute which is to be added
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode AddMainTagAttrL(TXmlEngElement aElement, TInt aId, const TDesC8& aName);


    /**
    * Gets the list direct children of object with specified id
    * @since Series S60 3.0
    * @param aParentId id of the parent object
    * @param aList an array to be filled with object
    * @return EUpnpOk, if successful; Otherwise, one of the other Upnp error codes.
    */
    TUpnpErrorCode GetObjectListL(TInt aParentId, RArray<TInt>& aList);
    TUpnpErrorCode GetItemListL(TInt aParentId, RArray<TInt>& aList);
    TUpnpErrorCode GetContainerListL(TInt aParentId, RArray<TInt>& aList);

    /**
    * Checks if object with the given id exists, is a container and its 'restricted' attribute has value of 'false'
    * @since Series S60 3.0
    * @param aObjId id of the object to be checked
    * @return returns EUpnpOk if object with the given id exists in database and is a "non restricted conainer"
    */
    TUpnpErrorCode CheckObjectRestrictionL(TInt aObjId);

    /**
    * Recovers the database.
    * @since Series S60 3.1
    * @return Standard error code
    */
    TInt RecoverDatabase();

    /**
    * Recreates the database file.
    * @since Series S60 3.1
    */
    void RecreateDatabaseFileL();

    /**
    * Checks if all database columns in each table have propper definitions.
    * If not, it leaves with KErrCorrupt.
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
    * Returns DB creation status
    * @since Series S60 3.2
    * @return TBool
    */
    TBool IsDbCreated();
   
    
private:

    /**
    * C++ default constructor.
    */
    CUpnpMetadataStorage();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL(const TDesC& aDatabaseFileName);

private:
    
    void CompactDbL();    
    TInt AddSubObjectsL(TInt aObjId, RArray<TInt>& aObjList, RArray<TInt>& aResList, TBool aLocal);

    /**
    * Compare two column sets (type, maxsize and attributes). Used by CheckDatabaseL() function.
    * Leaves with KErrCorrupt when columns are not agree.
    * @since Series S60 3.2
    * @param aColSet1 First column set to compare
    * @param aColSet2 Second column set to compare
    */
    void CompareDbColSetsL( CDbColSet *aColSet1, CDbColSet *aColSet2 );

    /**
    * Deletes the file
    * @since Series S30 3.1
    * @param aPath
    */
    void DeleteFileL(const TDesC& aPath);
        
    /**
    *
    * @since Series S30 3.1
    * @param aElement
    * @return descriptor pointer
    */
    HBufC8* CreatePathValL(const TXmlEngElement& aElement);
		HBufC8* CreatePathValL(TInt aParentId);
    /**
    * Opens database
    * @since Series S30 3.1
    * @param aDatabaseFileName
    */
    void OpenDbL( const TFileName& aDatabaseFileName );
        
    /**
    *
    * @since Series S30 3.1
    * @param aWeight
    */
    void DbChangedL(TInt aWeight);

private:    

    // For use of iFileStore
    RFs                iFsSession; 
    // For database operations
    RDbStoreDatabase    iDatabase;  
    TBool               iIsOpen;
    // For creating and opening database files
    CFileStore*         iFileStore; 
    // compact counter
    TInt                iCompactCounter;

};

#endif      // UPNPMETADATASTORAGE_H   
        
// End of File
