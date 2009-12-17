/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Includes SQL clauses for creating and dropping tables/indexes
*
*/






#ifndef __CMSQLMAINTENANCECLAUSES_H__
#define __CMSQLMAINTENANCECLAUSES_H__

///////////////////////////////////////////////////////////////////
// DROP CLAUSES
///////////////////////////////////////////////////////////////////
_LIT8( KCmSqlDropArtists, "DROP TABLE Artists;");
_LIT8( KCmSqlDropGenres, "DROP TABLE Genres;" );
_LIT8( KCmSqlDropAlbums, "DROP TABLE Albums;" );
_LIT8( KCmSqlDropUpnpProfiles, "DROP TABLE UpnpProfiles;" );
_LIT8( KCmSqlDropUpnpClasses, "DROP TABLE Upnpclasses;" );
_LIT8( KCmSqlDropResolutions, "DROP TABLE Resolutions;" );
_LIT8( KCmSqlDropResources, "DROP TABLE Resources;" );
_LIT8( KCmSqlDropItems, "DROP TABLE Items;" );

///////////////////////////////////////////////////////////////////
// CREATE CLAUSES FOR TABLES
///////////////////////////////////////////////////////////////////
_LIT8( KCmSqlCreateArtists, 
    "CREATE TABLE Artists("
        "ArtistId           INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value              TEXT NOT NULL UNIQUE);"
        );

_LIT8( KCmSqlCreateGenres, 
    "CREATE TABLE Genres("
        "GenreId           INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value             TEXT NOT NULL UNIQUE);"
        );
        
_LIT8( KCmSqlCreateAlbums, 
    "CREATE TABLE Albums("
        "AlbumId           INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value             TEXT NOT NULL UNIQUE);"
        );

_LIT8( KCmSqlCreateResolutions, 
    "CREATE TABLE Resolutions("
        "ResolutionId       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value              TEXT NOT NULL UNIQUE,"
        "Width              INTEGER,"
        "Height             INTEGER,"
        "PixelCount         INTEGER);"
        );
            
_LIT8( KCmSqlCreateUpnpclasses, 
    "CREATE TABLE Upnpclasses("
        "UpnpclassId        INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value              TEXT NOT NULL UNIQUE);"
        );            
        
_LIT8( KCmSqlCreateUpnpProfiles, 
    "CREATE TABLE UpnpProfiles("
        "ProfileId          INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "Value              TEXT NOT NULL UNIQUE);"
        );                         
        
_LIT8( KCmSqlCreateResources, 
    "CREATE TABLE Resources("
        "ResourceId       INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "ResItemId        INTEGER NOT NULL REFERENCES Items( Id ),"
        "ResResolution    INTEGER REFERENCES Resolutions( ResolutionId ),"
        "ResUri           TEXT NOT NULL,"
        "Size             INTEGER,"
        "ResDuration      INTEGER,"
        "ResBitrate       INTEGER);"
        );

_LIT8( KCmSqlCreateItems, 
    "CREATE TABLE Items("
        "Id                 INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
        "ItemId             TEXT NOT NULL,"
        "Hash               TEXT NOT NULL,"
        "Uri                TEXT NOT NULL,"
        "Title              TEXT NOT NULL,"
        "AlbumArtUri        TEXT,"
        "DeviceId           INTEGER NOT NULL,"
        "Duration           INTEGER,"
        "Bitrate            INTEGER,"
        "Date               DATETIME,"
        "HarvestDate        DATETIME,"
        "Size               INTEGER,"
        "Mediatype          INTEGER,"
        "SearchId           INTEGER,"
        "UpnpclassId        INTEGER REFERENCES Upnpclasses( UpnpclassId ),"
        "ArtistId           INTEGER REFERENCES Artists( ArtistId ),"
        "AlbumId            INTEGER REFERENCES Albums( AlbumId ),"
        "GenreId            INTEGER REFERENCES Genres( GenreId ),"
        "ProfileId          INTEGER REFERENCES UpnpProfiles( ProfileId ),"
        "ResolutionId       INTEGER REFERENCES Resolutions( ResolutionId ) );"
        );
                
///////////////////////////////////////////////////////////////////
// INDEXES
///////////////////////////////////////////////////////////////////
_LIT8( KCmSqlCreateItemsIdIndex, 
    "CREATE INDEX ItemsId_Index ON Items(Id);"
    );

_LIT8( KCmSqlCreateItemTitleIndex, 
    "CREATE INDEX Item_Title_Index ON Items(Title);"
    );
    
_LIT8( KCmSqlCreateItemDateIndex, 
    "CREATE INDEX Item_Date_Index ON Items(Date);"
    );        

_LIT8( KCmSqlCreateItemHarvestDateIndex, 
    "CREATE INDEX Item_HarvestDate_Index ON Items(HarvestDate);"
    ); 
    
_LIT8( KCmSqlCreateItemArtistIndex, 
    "CREATE INDEX Item_Artist_Index ON Items(ArtistId);"
    );

_LIT8( KCmSqlCreateItemAlbumIndex, 
    "CREATE INDEX Item_Album_Index ON Items(AlbumId);"
    );
    
_LIT8( KCmSqlCreateItemGenreIndex, 
    "CREATE INDEX Item_Genre_Index ON Items(GenreId);"
    );
    
_LIT8( KCmSqlCreateItemUpnpclassIndex, 
    "CREATE INDEX Item_Upnpclass_Index ON Items(UpnpclassId);"
    );

_LIT8( KCmSqlCreateItemUpnpProfileIndex, 
    "CREATE INDEX Item_UpnpProfile_Index ON Items(ProfileId);"
    );                         

_LIT8( KCmSqlCreateItemResolutionIndex, 
    "CREATE INDEX Item_Resolution_Index ON Items(ResolutionId);"
    );

_LIT8( KCmSqlCreateItemMediatypeIndex, 
    "CREATE INDEX Item_Mediatype_Index ON Items(Mediatype);" 
    );

_LIT8( KCmSqlCreateAlbumIdIndex,
    "CREATE INDEX AlbumId_Index ON Albums(AlbumId);" 
    );

_LIT8( KCmSqlCreateArtistIdIndex,
    "CREATE INDEX ArtistId_Index ON Artists(ArtistId);" 
    ); 

_LIT8( KCmSqlCreateGenreIdIndex,
    "CREATE INDEX GenreId_Index ON Genres(GenreId);" 
    );
        
// Dropping indexes
_LIT8( KCmSqlDropItemsIdIndex, 
    "DROP INDEX ItemsId_Index;"
    );

_LIT8( KCmSqlDropItemTitleIndex,
    "DROP INDEX Item_Title_Index;"
    );
    
_LIT8( KCmSqlDropItemDateIndex,
    "DROP INDEX Item_Date_Index;"
    ); 
    
_LIT8( KCmSqlDropItemHarvestDateIndex,
    "DROP INDEX Item_HarvestDate_Index;"
    );             

_LIT8( KCmSqlDropItemArtistIndex, 
    "DROP INDEX Item_Artist_Index;"
    ); 

_LIT8( KCmSqlDropItemAlbumIndex, 
    "DROP INDEX Item_Album_Index;"
    );                

_LIT8( KCmSqlDropItemGenreIndex, 
    "DROP INDEX Item_Genre_Index;"
    ); 
    
_LIT8( KCmSqlDropItemUpnpclassIndex, 
    "DROP INDEX Item_Upnpclass_Index;"
    );
    
_LIT8( KCmSqlDropItemUpnpProfileIndex, 
    "DROP INDEX Item_UpnpProfile_Index;"
    );                 

_LIT8( KCmSqlDropItemResolutionIndex, 
    "DROP INDEX Item_Resolution_Index;"
    );

_LIT8( KCmSqlDropItemMediatypeIndex, 
    "DROP INDEX Item_Mediatype_Index;" 
    );

_LIT8( KCmSqlDropAlbumIdIndex,
    "DROP INDEX AlbumId_Index;" 
    );

_LIT8( KCmSqlDropArtistIdIndex,
    "DROP INDEX ArtistId_Index;" 
    ); 

_LIT8( KCmSqlDropGenreIdIndex,
    "DROP INDEX GenreId_Index;" 
    );
                                                                            
#endif // __CMSQLMAINTENANCECLAUSES_H__


