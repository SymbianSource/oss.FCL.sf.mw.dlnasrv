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
* Description:      SQL clause definitions
*
*/






#ifndef __CMSQLCLAUSEDEF_H__
#define __CMSQLCLAUSEDEF_H__

// ---------------------------------------------------------------------------
// Misc definitions
// ---------------------------------------------------------------------------
//

_LIT8( KCmSqlEmpty, "" );
_LIT8( KCmSqlSpace, " " );
_LIT8( KCmSqlLeftBracket, " ( " );
_LIT8( KCmSqlRightBracket, " ) " );
_LIT8( KCmSqlComma, ", " );
_LIT8( KCmSqlDot, "." );
_LIT8( KCmSqlEqual, " = " );
_LIT8( KCmSqlGreater, " >= " );
_LIT8( KCmSqlSmaller, " <= " );
_LIT8( KCmSqlLike, " LIKE " );
_LIT8( KCmSqlSemicolon, ";" );
_LIT8( KCmSqlVariable, "? " );
_LIT8( KCmSqlAppendString, "%S " );
_LIT8( KCmSqlConsistOf, " %%S% " );
_LIT8( KCmSqlBeginsWith, " %S% " );
_LIT8( KCmSqlEndsWith, " %%S " );
_LIT8( KCmSqlAppendInteger, "%d " );
_LIT8( KCmSqlAppendLong, "%ld " );

_LIT8( KCmSqlDummyTrueCriteria, "1 " );
_LIT8( KCmSqlDummyFalseCriteria, "0 " );

_LIT8( KCmSqlSelect, "SELECT " );
_LIT8( KCmSqlFrom, "FROM " );
_LIT8( KCmSqlAs, "AS " );
_LIT8( KCmSqlIn, "IN " );
_LIT8( KCmSqlOr, "OR" );
_LIT8( KCmSqlAnd, "AND " );
_LIT8( KCmSqlAll, "* " );
_LIT8( KCmSqlOrderByDate, "ORDER BY Date " );
_LIT8( KCmSqlOrderByRandom, "ORDER BY Random() " );
_LIT8( KCmSqlLimit, "LIMIT " );
_LIT8( KCmSqlOffset, "OFFSET " );
_LIT8( KCmSqlAsc, "ASC ");
_LIT8( KCmSqlDesc, "DESC " );
_LIT8( KCmSqlSortValue, "sortvalue" ); // Used when sorting object
                                   // query by property value

_LIT8( KCmSqlCollateNoCase, " COLLATE NOCASE " ); // case insensitive sort

_LIT8( KCmSqlBaseSelect, 
       "SELECT Title, Date, Size, Uri, DeviceId, ItemId, Id "
       "FROM Items WHERE (" 
     );
                                            
_LIT8( KCmSqlMusicSelect, "SELECT Title, a.Value, Size, Uri, DeviceId, "
                          "ItemId, Id FROM Items LEFT JOIN Artists a "
                          "ON Items.ArtistId = a.ArtistId LEFT JOIN Albums b "
                          "ON Items.AlbumId = b.AlbumId "
                          "WHERE ("
                          );

_LIT8( KCmSqlSelectArtistById, "Items.ArtistId = " );
_LIT8( KCmSqlSelectAlbumById, "Items.AlbumId = " );
_LIT8( KCmSqlSelectGenreById, "Items.GenreId = " );
_LIT8( KCmSqlSelectResolutionById, "Items.ResolutionId = " );
_LIT8( KCmSqlSelectUpnpclassById, "Items.UpnpclassId = " );
_LIT8( KCmSqlFilterUpnpProfileId, "Items.ProfileId != " );

_LIT8( KCmSqlSelectTitle, "Items.Id = " );
_LIT8( KCmSqlSelectDuration, "Items.Duration " );
_LIT8( KCmSqlSelectBitrate, "Items.Bitrate " );
_LIT8( KCmSqlSelectSize, "Items.Size " );
_LIT8( KCmSqlSelectDeviceId, "Items.DeviceId = ");
_LIT8( KCmSqlSelectDate, "Items.Date ");
_LIT8( KCmSqlArtist, "Artists " );
_LIT8( KCmSqlAlbum, "Albums " );
_LIT8( KCmSqlGenre, "Genres " );
_LIT8( KCmSqlUpnpclass, "Upnpclasses " );
_LIT8( KCmSqlResolutions, "Resolutions " );
_LIT8( KCmSqlUpnpProfiles, "UpnpProfiles " );

_LIT8( KCmSqlArtistId, "ArtistId" );
_LIT8( KCmSqlAlbumId, "AlbumId" );
_LIT8( KCmSqlGenreId, "GenreId" );
_LIT8( KCmSqlProfileId, "ProfileId" );
_LIT8( KCmSqlUpnpclassId, "UpnpclassId" );
_LIT8( KCmSqlResolutionId, "ResolutionId" );

_LIT8( KCmSqlSelectMediatype, " ( Mediatype = %d ) " );
_LIT8( KCmSqlFreeTextSelectAudio, 
    "( Items.Title LIKE \'%%%S%%\' ) OR ( a.Value LIKE \'%%%S%%\' ) OR "
    "( b.Value LIKE \'%%%S%%\' ) " );
    
_LIT8( KCmSqlFreeTextSelectImageOrVideo, "( Items.Title LIKE \'%%%S%%\' ) " );

_LIT8(KCmSqlBeginTransaction, "BEGIN;");
_LIT8(KCmSqlCommit, "COMMIT;");

_LIT8( KCmSqlSelectMaxIndex, "SELECT count(*) FROM %S" );

_LIT8( KCmSqlSelectMaxPropertyIndex, "SELECT MAX( %S ) FROM %S" );

_LIT8( KCmSqlSelectMaxItemIndex, "SELECT MAX( Id ) FROM Items;" );

_LIT8( KCmSqlSelectItemId, "SELECT Id FROM Items WHERE "
    "( LOWER(Title) = ? AND Size = ? AND DeviceId = ? )" 
    );  

_LIT8( KCmSqlFilteredPropertySelect, "SELECT Items.Id, Items.Title, "
    "Items.ArtistId, b.Value, Items.AlbumId, a.Value, Items.GenreId, "
    "c.Value FROM Items LEFT JOIN Artists b "
    "ON Items.ArtistId = b.ArtistId LEFT JOIN Albums a "
    "ON Items.AlbumId = a.AlbumId LEFT JOIN Genres c "
    "ON Items.GenreId = c.GenreId "
    "WHERE Items.Mediatype = 2 "
    );

_LIT8( KCmSqlFilteredPropertySelectVideo, "SELECT Id, Title "
    "FROM Items WHERE Mediatype = 0 "
    );

_LIT8( KCmSqlWhere,
    "WHERE "
    );
        
// ---------------------------------------------------------------------------
// INSERT STATEMENTS
// ---------------------------------------------------------------------------
//
// Inserting item
_LIT8( KCmSqlInsertItem,
    "INSERT INTO Items( " 
    "ItemId, Hash, Uri, Title, AlbumArtUri, DeviceId, Duration, Bitrate, "
    "Date, HarvestDate, Size, Mediatype, UpnpclassId, ArtistId, AlbumId, "
    "GenreId, ResolutionId, ProfileId, SearchId )"
    "VALUES( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ? );"
    );

// Inserting resource
_LIT8( KCmSqlInsertResource,
    "INSERT INTO Resources( " 
    "ResItemId, ResResolution, ResUri, Size, ResDuration, ResBitrate )"
    "VALUES( ?, ?, ?, ?, ?, ? );"
    );
   
// Inserting artist into artist table
_LIT8( KCmSqlInsertArtist,
    "INSERT INTO Artists( " 
    "Value ) "
    "VALUES( ? );"
    );

// Inserting album into album table
_LIT8( KCmSqlInsertAlbum,
    "INSERT INTO Albums( " 
    "Value ) "
    "VALUES( ? );"
    );

// Inserting genre into genre table    
_LIT8( KCmSqlInsertGenre,
    "INSERT INTO Genres( " 
    "Value ) "
    "VALUES( ? );"
    );

// Inserting upnp class into upnp class table
_LIT8( KCmSqlInsertUpnpclass,
    "INSERT INTO Upnpclasses( " 
    "Value ) "
    "VALUES( ? );"
    );
    
// Inserting upnp class into upnp class table
_LIT8( KCmSqlInsertUpnpProfile,
    "INSERT INTO UpnpProfiles( " 
    "Value ) "
    "VALUES( ? );"
    );
        
// Inserting resolution into resolution table
_LIT8( KCmSqlInsertResolution,
    "INSERT INTO Resolutions( " 
    "Value, Width, Height, PixelCount ) "
    "VALUES( ?, ?, ?, ? );"
    );        
            
// ---------------------------------------------------------------------------
// SELECT STATEMENTS
// ---------------------------------------------------------------------------
//
// Selecting items harvested from the defined server
_LIT8( KCmSqlSelectGenericItem, 
    "SELECT Id, ItemId, Hash, SearchId "
    "FROM Items "
    "WHERE DeviceId = ? "
    "ORDER BY Hash;"
    );

_LIT8( KCmSqlSelectPropertyValues,
    "SELECT * FROM %S "
    "ORDER BY Value;"    
    );

_LIT8( KCmSqlValidateTableExistence,
    "SELECT count(*)"
        " FROM Items;"
        );

_LIT8( KCmSqlSelectTitleAndId,  
    "SELECT Id, Title "
    "FROM Items "
    "WHERE MediaType = %d "
    "ORDER BY Title;"
    );

_LIT8( KCmSqlSelectMediaCount,
    "SELECT count(*) FROM Items WHERE Mediatype = ?;"
    );            

_LIT8( KCmSqlSelectLimitHarvestDate,
    "SELECT HarvestDate FROM Items WHERE ( Mediatype = ? ) ORDER BY "
    "HarvestDate ASC LIMIT ?;"
    );
    
// ---------------------------------------------------------------------------
// DELETE STATEMENTS
// ---------------------------------------------------------------------------
//
_LIT8( KCmSqlDeleteItem, 
    "DELETE FROM Items "
    "WHERE Id = ? ;"
    );
    
_LIT8( KCmSqlDeleteResource, 
    "DELETE FROM Resources "
    "WHERE ResItemId = ? ;"
    );
    
_LIT8( KCmSqlDeleteMetadata, 
    "DELETE FROM Items WHERE DeviceId = ?;" 
    );        

_LIT8( KCmSqlDeleteOldestMediaItems, 
    "DELETE FROM Items WHERE HarvestDate <= ? AND MediaType = ?;"
    );
    
_LIT8( KCmSqlDeleteUnusedAlbums, 
    "DELETE FROM Albums WHERE AlbumId "
    "NOT IN ( SELECT AlbumId FROM Items );"
    );
    
_LIT8( KCmSqlDeleteUnusedArtists, 
    "DELETE FROM Artists WHERE ArtistId "
    "NOT IN ( SELECT ArtistId FROM Items );"
    );
    
_LIT8( KCmSqlDeleteUnusedGenres,
    "DELETE FROM Genres WHERE GenreId "
    "NOT IN ( SELECT GenreId FROM Items );"
    );
    
_LIT8( KCmSqlDeleteUnusedUpnpclasses,
    "DELETE FROM Upnpclasses WHERE UpnpclassId "
    "NOT IN ( SELECT UpnpclassId FROM Items );"
    );
    
_LIT8( KCmSqlDeleteUnusedUpnpProfileIds,
    "DELETE FROM UpnpProfiles WHERE ProfileId "
    "NOT IN ( SELECT ProfileId FROM Items );"
    );
                                              
#endif // __CMSQLCLAUSEDEF_H__



