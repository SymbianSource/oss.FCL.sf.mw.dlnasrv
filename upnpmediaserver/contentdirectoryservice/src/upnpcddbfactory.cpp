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



// INCLUDE FILES
#include "upnpcddbfactory.h"
#include "upnplocalstorage.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpmetadatastorage.h"
#define KLogFile _L("ContentDirectoryDb.log")
#include "upnpcustomlog.h"
// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CUpnpCdDbFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpCdDbFactory::CUpnpCdDbFactory()
{
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::ConstructL() 
{
    User::LeaveIfError( iFs.Connect() );
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCdDbFactory* CUpnpCdDbFactory::NewLC()
{
    CUpnpCdDbFactory* self = new ( ELeave ) CUpnpCdDbFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpCdDbFactory* CUpnpCdDbFactory::NewL()
{
    CUpnpCdDbFactory* self = NewLC();
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::~CUpnpCdDbFactory
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpCdDbFactory::~CUpnpCdDbFactory( )
{
    iStoreDb.Close();
    delete iFileStore;
    iFs.Close();
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateDatabaseFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateDatabaseFileL(const TDesC& aDbFileName)
{
    TRAPD(err,DoCreateDatabaseFileL(aDbFileName));
    // close db
    iStoreDb.Close();
    delete iFileStore;
    iFileStore = NULL;
    // remove file if error
    if(err)
    {
        iFs.Delete(aDbFileName);
        User::Leave(err);
    }
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateDatabaseFileL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::DoCreateDatabaseFileL(const TDesC& aDbFileName)
{
    // db
    CreateEmptyDbL(aDbFileName);
    
    // tables
    CreateObjectTableL();
    CreateElementTableL();
    CreateAttributeTableL();
    CreateResourceTableL();
    CreateSequenceTableL();
    
    // insert initial records
    InsertInitialDataL();   
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateObjectTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateObjectTableL()
{
    CDbColSet* colSet = CreateObjectTableColumnSequenceLC();

    // create the table
    User::LeaveIfError(iStoreDb.CreateTable(KObjectTableName, *colSet));
    
    // index
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idKeyCol(KObjIdColName);
    key->AddL(idKeyCol);
    key->MakeUnique();
    User::LeaveIfError( iStoreDb.CreateIndex(KObjIdIndexName, 
    KObjectTableName, *key) );
    
    // index
    CDbKey* key2 = CDbKey::NewLC();
    TDbKeyCol idKeyCol2(KObjRefIdColName);
    key2->AddL(idKeyCol2);
    User::LeaveIfError( iStoreDb.CreateIndex(KRefIdIndexName, 
    	KObjectTableName, *key2) );
    	
    // clean up
    CleanupStack::PopAndDestroy(key2);
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(colSet);
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateElementTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateElementTableL()
{
    CDbColSet* colSet = CreateElementTableColumnSequenceLC();

    // create the table
    User::LeaveIfError(iStoreDb.CreateTable(KElementTableName, *colSet));
    
    // index
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idKeyCol(KElmIdColName);
    key->AddL(idKeyCol);
    key->MakeUnique();
    
    // index
    CDbKey* key2 = CDbKey::NewLC();
    TDbKeyCol idKeyCol2(KElmObjIdColName);
    key2->AddL(idKeyCol2);
    User::LeaveIfError( iStoreDb.CreateIndex(KElmObjIdIndexName, 
    	KElementTableName, *key2) );
    
    // clean up
    CleanupStack::PopAndDestroy(key2);
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(colSet);
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateAttributeTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateAttributeTableL()
{
    CDbColSet* colSet = CreateAttributeTableColumnSequenceLC();

    // create the table
    User::LeaveIfError(iStoreDb.CreateTable(KAttributeTableName, *colSet));
    
    // index
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idKeyCol(KAtrIdColName);
    key->AddL(idKeyCol);
    key->MakeUnique();

    // index
    CDbKey* key2 = CDbKey::NewLC();
    TDbKeyCol idKeyCol2(KAtrIdObjColName);
    key2->AddL(idKeyCol2);
    User::LeaveIfError( iStoreDb.CreateIndex(KAtrElmIdIndexName, 
        KAttributeTableName, *key2) );
    
    // clean up
    CleanupStack::PopAndDestroy(key2);
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(colSet);
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateResourceTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateResourceTableL()
{
    CDbColSet* colSet = CreateResourceTableColumnSequenceLC();

    // create the table
    User::LeaveIfError(iStoreDb.CreateTable(KResourcesTableName, *colSet));
    
    // index
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idKeyCol(KRscIdObjColName);
    key->AddL(idKeyCol);    
    User::LeaveIfError( iStoreDb.CreateIndex(KRscIdObjIndexName, 
    KResourcesTableName, *key) );
    
    // index
    CDbKey* key2 = CDbKey::NewLC();
    TDbKeyCol idKeyCol2(KRscIdColName);
    key2->AddL(idKeyCol2);
    key2->MakeUnique();
    User::LeaveIfError( iStoreDb.CreateIndex(KRscIdIndexName, 
        KResourcesTableName, *key2) );
    
    // clean up
    CleanupStack::PopAndDestroy(key2);    
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(colSet);
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateResourceTableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateSequenceTableL()
{
    CDbColSet* colSet = CreateSequenceTableColumnSequenceLC();

    // create the table
    User::LeaveIfError(iStoreDb.CreateTable(KSequencesTableName, *colSet));
    
    // index
    CDbKey* key = CDbKey::NewLC();
    TDbKeyCol idKeyCol(KSeqColNameColName);
    key->AddL(idKeyCol);
    key->MakeUnique();
    User::LeaveIfError( iStoreDb.CreateIndex(KSeqColNameIndexName, 
    KSequencesTableName, *key) );
    
    // clean up
    CleanupStack::PopAndDestroy(key);
    CleanupStack::PopAndDestroy(colSet);
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateEmptyDbL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::CreateEmptyDbL(const TDesC& aDbFileName)
{
    // Create empty database file.
    iFileStore = CPermanentFileStore::ReplaceL(iFs,
    aDbFileName, EFileRead|EFileWrite);
    iFileStore->SetTypeL(iFileStore->Layout()); 
    TStreamId id = iStoreDb.CreateL(iFileStore);
    iFileStore->SetRootL(id); 
    iFileStore->CommitL();
}
// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::InsertInitialDataL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpCdDbFactory::InsertInitialDataL()
{
    // command table
    const TDesC* sqlCmds[] =    { &KInit1(),
                                  &KInit2(),
                                  &KInit3(),
                                  &KInit4(),
                                  &KInit5(),
                                  &KInit6()
                                };
    // for each command
    TInt count = sizeof(sqlCmds) / sizeof(TDesC*);
    for(TInt i = 0; i < count; i++)
    {
        // execute
        TInt dbErr = iStoreDb.Execute(*sqlCmds[i]);
        User::LeaveIfError(dbErr);  
    }
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateObjectTableColumnSequenceLC
// -----------------------------------------------------------------------------
//
CDbColSet* CUpnpCdDbFactory::CreateObjectTableColumnSequenceLC()
{
    // specify columns
    TDbCol idCol        (   KObjIdColName,          EDbColInt32 );
    TDbCol parentIdCol  (   KObjParentIdColName,    EDbColInt32 );
    TDbCol titleCol     (   KObjTiltleColName,      EDbColText8 , KMaxUpnpStringLen);
    TDbCol classCol     (   KObjClassColName,       EDbColText8 , KMaxUpnpStringLen);
    TDbCol restrictedCol(   KObjRestrictedColName,  EDbColBit   );
    TDbCol searchableCol(   KObjSearchableColName,  EDbColBit   );
    TDbCol refIdCol     (   KObjRefIdColName,       EDbColInt32 );
    TDbCol pathCol      (   KObjPathColName,        EDbColText8 , KMaxUpnpStringLen);

	pathCol.iAttributes			= TDbCol::ENotNull;
    // attributes
    idCol.iAttributes           = TDbCol::ENotNull;
    parentIdCol.iAttributes     = TDbCol::ENotNull;
    titleCol.iAttributes        = TDbCol::ENotNull;
    classCol.iAttributes        = TDbCol::ENotNull;
    restrictedCol.iAttributes   = TDbCol::ENotNull;
	pathCol.iAttributes			= TDbCol::ENotNull;
    
    // create column set
    CDbColSet* colSet = CDbColSet::NewLC();

    // add the columns to column set
    colSet->AddL( idCol             );
    colSet->AddL( parentIdCol       );
    colSet->AddL( titleCol          );
    colSet->AddL( classCol          );
    colSet->AddL( restrictedCol     );
    colSet->AddL( searchableCol     );
    colSet->AddL( refIdCol          );
    colSet->AddL( pathCol           );
    
    return colSet;
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateElementTableColumnSequenceLC
// -----------------------------------------------------------------------------
//
CDbColSet* CUpnpCdDbFactory::CreateElementTableColumnSequenceLC()
{
    // specify columns
    TDbCol idCol     (   KElmIdColName,          EDbColInt32     );
    TDbCol nameCol   (   KElmNameColName,        EDbColText8     , KMaxUpnpStringLen);
    TDbCol valCol    (   KElmValueColName,       EDbColLongText8     , KMaxUpnpLongStringLen);
    TDbCol hasAttrCol(   KElmHasAttrColName,     EDbColBit       );
    TDbCol objIdCol  (   KElmObjIdColName,       EDbColInt32     );
    TDbCol isReqCol  (   KElmIsRequiredColName,  EDbColBit       );

    // attributes
    idCol.iAttributes       = TDbCol::ENotNull;
    nameCol.iAttributes     = TDbCol::ENotNull;
    hasAttrCol.iAttributes  = TDbCol::ENotNull;
    objIdCol.iAttributes    = TDbCol::ENotNull;
    isReqCol.iAttributes    = TDbCol::ENotNull;
    
    // create column set
    CDbColSet* colSet = CDbColSet::NewLC();

    // add the columns to column set
    colSet->AddL( idCol         );
    colSet->AddL( nameCol       );
    colSet->AddL( valCol        );
    colSet->AddL( hasAttrCol    );
    colSet->AddL( objIdCol      );
    colSet->AddL( isReqCol      );
    
    return colSet;
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateAttributeTableColumnSequenceLC
// -----------------------------------------------------------------------------
//
CDbColSet* CUpnpCdDbFactory::CreateAttributeTableColumnSequenceLC()
{
    // specify columns
    TDbCol idCol        (   KAtrIdColName,          EDbColInt32     );
    TDbCol nameCol      (   KAtrNameColName,        EDbColText8     );
    TDbCol valCol       (   KAtrValueColName,       EDbColLongText8 , KMaxUpnpLongStringLen);
    TDbCol elmIdCol     (   KAtrElmIdColName,       EDbColInt32     );
    TDbCol isReqCol     (   KAtrIsRequiredColName,      EDbColBit   );
    TDbCol idObjCol     (   KAtrIdObjColName,          EDbColInt32 );

    // attributes
    idCol.iAttributes       = TDbCol::ENotNull;
    nameCol.iAttributes     = TDbCol::ENotNull;
    elmIdCol.iAttributes    = TDbCol::ENotNull;
    isReqCol.iAttributes    = TDbCol::ENotNull;
    idObjCol.iAttributes    = TDbCol::ENotNull;
    
    // create column set
    CDbColSet* colSet = CDbColSet::NewLC();

    // add the columns to column set
    colSet->AddL( idCol     );
    colSet->AddL( nameCol   );
    colSet->AddL( valCol    );
    colSet->AddL( elmIdCol  );
    colSet->AddL( isReqCol  );
    colSet->AddL( idObjCol  );

    return colSet;    
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateResourceTableColumnSequenceLC
// -----------------------------------------------------------------------------
//
CDbColSet* CUpnpCdDbFactory::CreateResourceTableColumnSequenceLC()
{
    // specify columns
    TDbCol idCol        (   KRscIdColName,          EDbColInt64 );
    TDbCol pathCol      (   KRscPathColName,        EDbColText  , KMaxUpnpStringLen);
    TDbCol readOnlyCol  (   KRscReadonlyColName,    EDbColBit   );
    TDbCol thumbnailCol (   KRscThumbnailColName,   EDbColBit   );
    TDbCol idObjCol     (   KRscIdObjColName,       EDbColInt32 );

    // attributes
    idCol.iAttributes   = TDbCol::ENotNull;
    pathCol.iAttributes = TDbCol::ENotNull;
    readOnlyCol.iAttributes = TDbCol::ENotNull;
    thumbnailCol.iAttributes = TDbCol::ENotNull;
    idObjCol.iAttributes = TDbCol::ENotNull;
    
    // create column set
    CDbColSet* colSet = CDbColSet::NewLC();

    // add the columns to column set
    colSet->AddL( idCol         );
    colSet->AddL( pathCol       );
    colSet->AddL( readOnlyCol       );
    colSet->AddL( thumbnailCol       );
    colSet->AddL( idObjCol  );

    return colSet;    
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateSequenceTableColumnSequenceLC
// -----------------------------------------------------------------------------
//
CDbColSet* CUpnpCdDbFactory::CreateSequenceTableColumnSequenceLC()
{
    // specify columns
    TDbCol colNameCol       ( KSeqColNameColName, EDbColText8   );
    TDbCol nextKeyCol       ( KSeqNextKeyColName, EDbColInt32   );

    // attributes
    colNameCol.iAttributes      = TDbCol::ENotNull;
    nextKeyCol.iAttributes      = TDbCol::ENotNull;
    
    // create column set
    CDbColSet* colSet = CDbColSet::NewLC();

    // add the columns to column set
    colSet->AddL( colNameCol    );
    colSet->AddL( nextKeyCol    );

    return colSet;    
}

// -----------------------------------------------------------------------------
// CUpnpCdDbFactory::CreateMetadataStorageL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CUpnpMetadataStorage* CUpnpCdDbFactory::CreateMetadataStorageL(const TDesC& aDatabaseFileName)
{
    CUpnpMetadataStorage* metadataStorage = NULL;
	TInt err;
    TRAP(err, metadataStorage = CUpnpMetadataStorage::NewL(aDatabaseFileName));
    if(err != KErrNone)
    { // dbfile does not exist or dbfile corrupt, create new db and try again
        CreateDatabaseFileL(aDatabaseFileName);
        LOGCD("CUpnpCdDbFactory::CreateMetadataStorageL","CreateDatabaseFileL",0,"New database created successfully");
        
        metadataStorage = CUpnpMetadataStorage::NewL(aDatabaseFileName);
    }

    LOGCD("CUpnpCdDbFactory::CreateMetadataStorageL","CUpnpMetadataStorage::NewL",err,"Database opened successfully");    
    
    return metadataStorage;
}

//  End of File
