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



#ifndef C_CUPNPCDDBFACTORY_H
#define C_CUPNPCDDBFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <d32dbms.h>
#include <s32file.h>
#include <upnperrors.h>

// CLASS DECLARATION
class CUpnpMetadataStorage;



/**
*  Database factory.
*
*
*  @lib ContentDirectoryDb.lib
*  @since Series 60 3.0
*/
class CUpnpCdDbFactory : public CBase
{
 public:  // Constructors and destructor
 
    /**
    * Two-phased constructor.
    */
    static CUpnpCdDbFactory* NewLC();
    
    /**
    * Two-phased constructor.
    */
    static CUpnpCdDbFactory* NewL();

    /**
    * Destructor
    */
    ~CUpnpCdDbFactory();
    
 public: // New functions
 
    /**
    * @since Series60 3.1
    * @param aDatabaseFileName Databse filename
    * @return 
    */
    CUpnpMetadataStorage* CreateMetadataStorageL(const TDesC& aDatabaseFileName);

    /**
    * Creates database file
    * @since Series S60 3.1
    * @param aDbFileName Database filename
    */
    void CreateDatabaseFileL(const TDesC& aDbFileName);

    /**
    * @since Series60 3.2
    * @return Column set for table Object.
    */
    CDbColSet* CreateObjectTableColumnSequenceLC();

    /**
    * @since Series60 3.2
    * @return Column set for table Element.
    */
    CDbColSet* CreateElementTableColumnSequenceLC();

    /**
    * @since Series60 3.2
    * @return Column set for table Attribute.
    */
    CDbColSet* CreateAttributeTableColumnSequenceLC();

    /**
    * @since Series60 3.2
    * @return Column set for table Resource.
    */
    CDbColSet* CreateResourceTableColumnSequenceLC();

    /**
    * @since Series60 3.2
    * @return Column set for table Sequence.
    */
    CDbColSet* CreateSequenceTableColumnSequenceLC();


 private:
    /**
    * C++ default constructor.
    */
    CUpnpCdDbFactory();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();
    
    /**
    * Creates database file. Invoke by CreateDatabaseFileL
    * @since Series S60 3.1
    * @param aDbFileName Database filename
    */
    void DoCreateDatabaseFileL(const TDesC& aDbFileName);

    /**
    * Creates empty database
    * @since Series S60 3.1
    * @param aDbFileName Database filename
    */
    void CreateEmptyDbL(const TDesC& aDbFileName);
    
    /**
    * Create objects table
    * @since Series S60 3.1
    */
    void CreateObjectTableL();
    
    /**
    * Create elements table
    * @since Series S60 3.1
    */
    void CreateElementTableL();
    
    /**
    * Create attributes table
    * @since Series S60 3.1
    */
    void CreateAttributeTableL();
    
    /**
    * Create resources table
    * @since Series S60 3.1
    */
    void CreateResourceTableL();
    
    /**
    * Create sequences table
    * @since Series S60 3.1
    */
    void CreateSequenceTableL();
    
    /**
    * Constructs database
    * @since Series S60 3.1
    * @param aDatabaseFileName
    */
    void ConstructDbL(const TFileName& aDatabaseFileName);  
        
    /**
    * Opens database
    * @since Series S60 3.1
    * @param aDatabaseFileName  Database filename
    */
    void OpenDbL( const TFileName& aDatabaseFileName );
        
    /**
    * 
    * @since Series S60 3.1
    * @param aWeight
    */
    void DbChangedL(TInt aWeight);    
        
    /**
    * Insterts initial data
    * @since Series S60 3.1
    */
    void InsertInitialDataL();

 private:    // Data
 
    // file server session
    RFs         iFs;

    RDbStoreDatabase    iStoreDb;
    
    CFileStore*     iFileStore;  
 
};

#endif // C_CUPNPCDDBFACTORY_H

// End Of File