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

#ifndef C_UPNPCONTENTDIRECTORYDBGLOBALS_H
#define C_UPNPCONTENTDIRECTORYDBGLOBALS_H
#include <platform_paths.hrh>
#include <bldvariant.hrh>
// CONSTANTS

// integers
const TInt KMaxIntegerLen               = 10;
const TInt KMaxIDResLength              = 18;
const TInt KMaxLongIntegerLen           = 20;
const TInt KMaxBoolLen                  = 1;
const TInt KColNameMaxLenth             = 10;
const TInt KDefaultGranularity          = 5;
const TInt KRespBufGranularity          = 128;
const TInt KMinDiskSpace                = 10000;
const TInt KDownloadContId              = 1;
const TInt KAvailableNestedLevels       = 23;
const TInt KMaxUpnpLongStringLen        = 1024;
const TInt KMaxUpnpStringLen            = 255;
const TInt KCommitEveryNum              = 200;
const TInt KOnlyOnePair                 = 2;
const TInt KExtensionLength             = 10;
const TInt KThumbnailWidth              = 40; 
const TInt KThumbnailHeight             = 30;
const TInt KDefaultThumbnailSize        = 5120; // 5 kb
const TInt KHttpGetStarted              = 800;  
const TInt KHttpPostStarted             = 801;  
const TInt KThumbnailDirLen             = 10;
const TInt KCompactWeight               = 10000; // specify how often 
const TInt KDeleteWeight                = 5;   // Compact function is called.
const TInt KExecuteWeight               = 1;   
const TInt KCharSize                    = 2;                                                  
const TInt KAutoDestroyTimeLimit        = 35000000;
const TInt KMapKeyMaxLength             = 32;
const TInt KRandomMax                   = 8999;
const TInt KRandomMin                   = 1000;
const TInt KRandomRangeLength           = 4;
const TInt KMaxSlashAmount              = 4;
const TInt KCharLen						= 1;   

// files
_LIT(  KObjectsXmlFileName,             "z:objects.xml"                     );
_LIT(  KDefaultMediaDrive,              "C:"                                );
_LIT(  KMediaPath,                      "Media"                             );
_LIT(  KDownloadPath,                   "Download"                          );
_LIT8( KMediaPath8,                     "Media"                             );
_LIT(  KThumbnailPath,                  "Thumbnail"                         );
_LIT(  KSearchThumbnailPathPattern,     "/Thumbnail/"                       );
_LIT(  KMediaThumbnailPath,             "Media/Thumbnail"                   );
_LIT(  KMainDir,                        "\\"                                );
_LIT(  KDiskPathElSep,                  "\\"                                );
_LIT(  KSlash,                          "/"                                 );
_LIT(  KDefaultThumbExtension,          ".jpeg"                             );
_LIT(  KThumbDirectory11,               "Thumbnail\\"                       );
_LIT8( KThumbMimeType,                  "image/jpeg"                        );
_LIT(  KDefaultUploadDir,               "data\\Download\\Media\\"			);
_LIT(  KThumbnailSubDir,                "thumbnail\\"                       );


// common strings
_LIT(  KSemicolon,                      ";"                                 );
_LIT8( KSemicolon8,                     ";"                                 );
_LIT(  KCol,                            ":"                                 );
_LIT(  KCommaStr,                       ","                                 );
_LIT8( KCommaStr8,                      ","                                 );
_LIT(  KFalseString,                    "false"                             );
_LIT8( KFalseString8,                   "false"                             );
_LIT(  KFalseValue,                     "0"                                 );
_LIT8( KFalseValue8,                    "0"                                 );
_LIT(  KAsterisk,                       "*"                                 );
_LIT8( KAsterisk8,                      "*"                                 );
_LIT(  KAtStr,                          "@"                                 );
_LIT8( KAtStr8,                         "@"                                 );
_LIT8( KHash8,                          "#"                                 );
_LIT8( KHashASCII8,                     "%23"                               );
_LIT(  KMinusOne,                       "-1"                                );
_LIT8( KMinusOne8,                      "-1"                                );
_LIT(  KDot,                            "."                                 );
_LIT8( KDot8,                           "."                                 );
_LIT(  KPathSeparator,                  "."                                 );
_LIT8( KPathSeparator8,                 "."                                 );
_LIT(  KQuot,                           "'"                                 );
_LIT(  KUndersc,                        "_"                                 );
 
_LIT(  KTrueString,                     "true"                              );
_LIT8( KTrueString8,                    "true"                              );
_LIT(  KTrueValue,                      "1"                                 );
_LIT8( KTrueValue8,                     "1"                                 );
_LIT(  KNullString,                     ""                                  );
_LIT(  KSpc,                            " "                                 );

// 8bit strings
_LIT8( KNewLine,                        "\r\n"                              );
_LIT8( KLF8,                            "\n"                                );
_LIT8( KTwoSpaces,                      "  "                                );
_LIT8( KNullString8,                    ""                                  );

// anyContainer support
_LIT8( KAnyContainerIndicator8,         "DLNA.ORG_AnyContainer"             );
_LIT(  KAnyContainerIndicator,          "DLNA.ORG_AnyContainer"             );
const TInt KAnyContainerId              = 1;

// forbidden paths
_LIT8( KForbiddenDrivez,                "z"                                 );
_LIT8( KForbiddenDriveZ,                "Z"                                 );
_LIT8( KForbiddenPrivatePattern,        "/?/private*"                       );


// Database structures' names
// object table
_LIT(  KObjectTableName,                "object"                            );
_LIT8( KObjectTableName8,               "object"                            );
_LIT(  KObjIdColName,                   "id"                                );
_LIT(  KObjParentIdColName,             "parentID"                          );
_LIT(  KObjTiltleColName,               "title"                             );
_LIT8( KObjTiltleColName8,              "title"                             );
_LIT(  KObjClassColName,                "class"                             );
_LIT8( KObjClassColName8,               "class"                             );
_LIT(  KObjRestrictedColName,           "restricted"                        );
_LIT(  KObjSearchableColName,           "searchable"                        );
_LIT(  KObjRefIdColName,                "refID"                             );
_LIT(  KObjPathColName,                 "path"                              );
_LIT(  KObjIdIndexName,                 "obj_id_index"                      );
_LIT(  KRefIdIndexName,                 "ref_id_index"                      );
// element table
_LIT(  KElementTableName,               "element"                           );
_LIT8( KElementTableName8,              "element"                           );
_LIT(  KElmIdColName,                   "elm_id"                            );
_LIT(  KElmNameColName,                 "elm_name"                          );
_LIT(  KElmHasAttrColName,              "elm_has_attribute"                 );
_LIT(  KElmObjIdColName,                "elm_obj_id"                        );
_LIT(  KElmValueColName,                "elm_value"                         );
_LIT(  KElmIsRequiredColName,           "elm_is_required"                   );
_LIT(  KElmIdIndexName,                 "elm_id_index"                      );
_LIT(  KElmObjIdIndexName,              "elm_obj_id_index"                  );
// attributes table 
_LIT(  KAttributeTableName,             "attribute"                         );
_LIT8( KAttributeTableName8,            "attribute"                         );
_LIT(  KAtrIdColName,                   "atr_id"                            );
_LIT(  KAtrNameColName,                 "atr_name"                          );
_LIT(  KAtrElmIdColName,                "atr_elm_id"                        );
_LIT(  KAtrObjIdColName,                "atr_elm_id"                        );
_LIT(  KAtrValueColName,                "atr_value"                         );
_LIT(  KAtrIsRequiredColName,           "atr_is_required"                   );
_LIT(  KAtrIdIndexName,                 "atr_id_index"                      );
_LIT(  KAtrElmIdIndexName,              "atr_elm_id_index"                  );
_LIT(  KAtrIdObjColName,                "atr_id_obj"                        );
// resources table
_LIT(  KResourcesTableName,             "resources"                         );
_LIT(  KRscIdColName,                   "rsc_id"                            );
_LIT(  KRscPathColName,                 "rsc_path"                          );
_LIT(  KRscReadonlyColName,             "rsc_readonly"                      );
_LIT(  KRscThumbnailColName,            "rsc_thumbnail"                     );
_LIT(  KRscIdObjIndexName,              "rsc_id_obj_index"                  );
_LIT(  KRscIdIndexName,                 "rsc_id_index"                      );
_LIT(  KRscIdObjColName,                "rsc_id_obj"                        );
// sequences table
_LIT(  KSequencesTableName,             "sequences"                         );
_LIT(  KSeqColNameColName,              "seq_col_name"                      );
_LIT(  KSeqNextKeyColName,              "seq_next_key"                      );
_LIT(  KSeqColNameIndexName,            "seq_col_name_index"                );
// importUri sequence
_LIT(  KImportUriId,                    "uriId"                             );
_LIT8( KImportUriId8,                   "uriId"                             );
// database name
_LIT(  KDatabaseFileName,               "c:AVMediaServerStore"              );

// upnp 
_LIT8( KContainerClass8,                "object.container"                  );
_LIT(  KItemClass,                      "object.item"                       );
_LIT8( KClassTagName,                   "class"                             );
_LIT8( KTitleTagName,                   "title"                             );
_LIT(  KClassTagNameWithNs,             "upnp:class"                        );
_LIT8( KClassTagNameWithNs8,            "upnp:class"                        );
_LIT(  KTitleTagNameWithNs,             "dc:title"                          );
_LIT8( KTitleTagNameWithNs8,            "dc:title"                          );
_LIT(  KResStr,                         "res"                               );
_LIT8( KMimeBegin,                      "&quot;http-get:*:"                 );
_LIT8( KMimeEnd,                        ":*&quot;"                          );
_LIT8( KItemTypeImage,                  "imageItem"                         );
_LIT8( KItemTypeAudio,                  "audioItem"                         );
_LIT8( KItemTypeVideo,                  "videoItem"                         );
_LIT8( KHTTPHeader,                     "http://"                           );
_LIT8( KDrmMatchString,                 "<xmrcc:*/*>*"                      );
_LIT8( KRequiredAtrName,                "el_required"                       );
_LIT8( KRequiredAtrSuf,                 "_req"                              );
_LIT8( KRequiredAtrSufPattern,          "*_req"                             );
_LIT8( KProtInfoReqAtrSufPattern,       "protocolInfo_req"                  );
_LIT8( KIpPortPlaceholder8,             "___.___.___.___:_____"             );
_LIT(  KIpPortPlaceholder,              "___.___.___.___:_____"             );
_LIT8( KTitle,                          "title"                             );
_LIT8( KProInfoBegin,                   "http-get:*:"                       );
_LIT8( KProInfoEnd,                     ":*"                                );
_LIT8( KCompleted,                      "COMPLETED"                         );
_LIT8( KError,                          "ERROR"                             );
_LIT8( KInProgress,                     "IN_PROGRESS"                       );
_LIT8( KStopped,                        "STOPPED"                           );
_LIT8( KBrowseChilds,                   "BrowseDirectChildren"              );
_LIT8( KBrowseFlag,                     "BrowseFlag"                        );
_LIT8( KBrowseMeta,                     "BrowseMetadata"                    );
_LIT8( KContainerID,                    "ContainerID"                       );
_LIT8( KChildCount,                     "childCount"                        );
_LIT(  KChildCount16,                   "childCount"                        );
_LIT8( KContainer,                      "container"                         );
_LIT8( KContainerUpdateIDs,             "ContainerUpdateIDs"                );
_LIT8( KCurrentTagValue,                "CurrentTagValue"                   );
_LIT8( KDcNsPrefix,                     "dc"                                );
_LIT8( KDestinationURI,                 "DestinationURI"                    );
_LIT8( KDidlEnd,                        "</DIDL-Lite>"                      );
_LIT8( KXmlnsNsUri,                     "urn:schemas-upnp-org:"
                                        "metadata-1-0/DIDL-Lite/"           );
_LIT8( KCmlnsNsPref,                    "xmlns"                             );
_LIT8( KDcNsUri,                        "http://purl.org/dc/elements/1.1/"  );
_LIT8( KDcNsPref,                       "dc"                                );
_LIT8( KUpnpNsUri,                      "urn:schemas-upnp-org:"
                                        "metadata-1-0/upnp/"                );
_LIT8( KUpnpNsPref,                     "upnp");
_LIT8( KDidlTag,                        "<DIDL-Lite xmlns=\"urn:schemas-upnp"
                                        "-org:metadata-1-0/DIDL-Lite/\" "
                                        "xmlns:dc=\"http://purl.org/dc/"
                                        "elements/1.1/\" xmlns:upnp=\"urn:"
                                        "schemas-upnp-org:metadata-1-0/upnp"
                                        "/\" xmlns:dlna=\"urn:schemas-dlna-"
                                        "org:metadata-1-0\">");
_LIT8( KElements,                       "Elements"                          );
_LIT8( KFilter,                         "Filter"                            );
_LIT8( KId,                             "Id"                                );
_LIT8( KIdAttrName,                     "id"                                );
_LIT(  KimportUri,                      "importUri"                         );
_LIT8( KImportUri8,                     "importUri"                         );
_LIT8( KItem,                           "item"                              );
_LIT8( KNumberReturned,                 "NumberReturned"                    );
_LIT8( KNewID,                          "NewID"                             );
_LIT8( KNewTagValue,                    "NewTagValue"                       );
_LIT8( KParentID,                       "parentID"                          );
_LIT8( KRefID,                          "refID"                             );
_LIT8( KRequestedCount,                 "RequestedCount"                    );
_LIT8( KResourceURI,                    "ResourceURI"                       );
_LIT8( KRestricted,                     "restricted"                        );
_LIT8( KResult,                         "Result"                            );
_LIT8( KCreator8,                       "dc:creator"                        );
_LIT8( KDate8,                       "date"                        );
_LIT8( KDuration8,                       "duration"                        );
_LIT8( KSearchCaps,                     "SearchCaps"                        );
_LIT8( KSearchCapabilities,             "");

_LIT8( KSearchCapsVariableName,         "SearchCapabilities"                ); 

_LIT8( KSearchClassTag,                 "<upnp:searchClass>"                );
_LIT8( KSearchClassTagEnd,              "</upnp:searchClass>"               );
_LIT8( KSize,                           "size"                              );
_LIT8( KStartingIndex,                  "StartingIndex"                     );
_LIT8( KSortCaps,                       "SortCaps"                          );
_LIT8( KSortCapsVariableName,           "SortCapabilities"                  );
_LIT8( KSortCapabilities,               ""                                  );
_LIT8( KSortCriteria,                   "SortCriteria"                      );
_LIT8( KSourceURI,                      "SourceURI"                         );
_LIT8( KSystemUpdateID,                 "SystemUpdateID"                    );
_LIT8( KTransferID,                     "TransferID"                        );
_LIT8( KTransferIDs,                    "TransferIDs"                       );
_LIT8( KTransferStatus,                 "TransferStatus"                    );
_LIT8( KTransferLength,                 "TransferLength"                    );
_LIT8( KTransferTotal,                  "TransferTotal"                     );
_LIT8( KTotalMatches,                   "TotalMatches"                      );
_LIT8( KUpnpNsPrefix,                   "upnp"                              );
_LIT8( KUpdateID,                       "UpdateID"                          );
_LIT8( KHttpTag,                        "http://"                           );
_LIT(  KHttpTag16,                      "http://"                           );
_LIT8( KprotocolInfo,                   "protocolInfo"                      );
_LIT(  KProtocolInfo,                   "protocolInfo"                      );
_LIT8( KDlnaPrefix,                     "dlna"                              );
_LIT8( KDlnaProfileID,                  "dlna:profileID"                    );
_LIT8( KProfileID,                      "profileID"                         );
_LIT8( KDefaultProfileID,               "JPEG_TN"                           );
_LIT8( KAlbumArtURI,                    "upnp:albumArtURI"                  );
_LIT8( KXmlnsDlna,                      "urn:schemas-dlna-org:metadata-1-0/");
_LIT8( KEmptyProtocolInfoVal8,          "http-get:*:*:*"                    );
_LIT8( KRootNode,                       "<DIDL-Lite xmlns:dc=\"http://purl."
                                        "org/dc/elements/1.1/\" xmlns:upnp="
                                        "\"urn:schemas-upnp-org:metadata-1-"
                                        "0/upnp/\" xmlns=\"urn:schemas-upnp"
                                        "-org:metadata-1-0/DIDL-Lite/\">"   );
_LIT8( KSearchable,                     "searchable"                        );
_LIT8( KUpdateIds,                      "0,0"                               );
_LIT8( KBrowse,                         "Browse"                            );
_LIT8( KCreateReference,                "CreateReference"                   );
_LIT8( KCreateObject,                   "CreateObject"                      );
_LIT8( KDeleteResource,                 "DeleteResource"                    );
_LIT8( KDestroyObject,                  "DestroyObject"                     );
_LIT8( KExportResource,                 "ExportResource"                    );
_LIT8( KGetSearchCapabilities,          "GetSearchCapabilities"             );
_LIT8( KGetSortCapabilities,            "GetSortCapabilities"               );
_LIT8( KGetSystemUpdateID,              "GetSystemUpdateID"                 ); 
_LIT8( KGetTransferProgress,            "GetTransferProgress"               );
_LIT8( KImportResource,                 "ImportResource"                    );
_LIT8( KSearch,                         "Search"                            );
_LIT8( KStopTransferResource,           "StopTransferResource"              );
_LIT8( KMetadata,                       "urn:schemas-upnp-org:metadata"
                                        "-1-0/upnp/"                        );
_LIT8( KXmlNsDc,                        "xmlns:dc=\"http://purl.org/dc"
                                        "/elements/1.1/\""                  );
_LIT8( KType,                           "type"                              ); 
_LIT8( KElement,                        "e"                                 );
_LIT8( KClass,                          "class"                             );
_LIT8( KImportUri,                      "importUri"                         );
_LIT(  KImportUri16,                    "importUri"                         );
_LIT8( KCharR,                          "r"                                 ); 
_LIT8( KIParent,                        "i"                                 );
_LIT8( KProperties,                     "ps"                                );
_LIT8( KName,                           "n"                                 );
_LIT8( KAttribute,                      "Attribute"                         );
_LIT8( KResAttr,                        "res"                               );
_LIT8( KMultiple,                       "m"                                 );
_LIT8( KRes,                            "res"                               );
_LIT(  KRes16,                          "res"                               );
_LIT8( KNestAttr,                       "a"                                 );
_LIT8( KObject,                         "object"                            );
_LIT8( KDidlLite,                       "DIDL-Lite"                         );
_LIT8( KTrue,                           "true"                              );
_LIT8( KObjectProperty,                 "p"                                 );
_LIT8( KObjectName,                     "n"                                 );
_LIT8( KRequiredObject,                 "r"                                 );
_LIT8( KMultiValueObject,               "m"                                 );
_LIT8( KVendorDescriptor,               "desc"                              );
_LIT8( KColon8,                         ":"                                 );
_LIT8( KArtist,                         "artist"                            );
_LIT8( KAlbum,                          "album"                             );
_LIT8( KImageMime,                      "image"                             );
_LIT8( KTextMime,                       "text"                              );
_LIT8( KVideoMime,                      "video"                             );
_LIT8( KAudioMime,                      "audio"                             );
_LIT8( KObjectClassAudioItem,           "object.item.audioItem"             );
_LIT8( KObjectClassVideoItem,           "object.item.videoItem"             );  
_LIT8( KObjectClassTextItem,            "object.item.textItem"              );    
_LIT8( KObjectClassImageItem,           "object.item.imageItem"             );      
_LIT8( KJpegLrg,                        "JPEG_LRG"                          );
_LIT8( KJpegMed,                        "JPEG_MED"                          );
_LIT8( KJpegSm,                         "JPEG_SM"                           );

// SQL Strings
_LIT(  KDeleteFromSqlCmd,               "DELETE FROM %S WHERE %S=%S"        );
_LIT(  KOpenParant,                     "("                                 );
_LIT(  KValuesOpen,                     " VALUES("                          );
_LIT(  KCloseParant,                    ")"                                 );
_LIT(  KInsertInto,                     "INSERT INTO "                      );
_LIT(  KOnlyRequiredFromElement,        " AND elm_is_required=1"            );
_LIT(  KNamedElFromElement,             " AND elm_name='%S'"                );
_LIT(  KSelectFromObjectSqlCmd,         "SELECT %S FROM object WHERE id=%d" );
_LIT(  KOnlyRequiredFromAtr,            " AND atr_is_required=1"            );
_LIT(  KNamedAtrFromAttribute,          " AND atr_name='%S'"                );
_LIT(  KFullPathFromShareMap,           " OR %S='%S'"                       );
_LIT(  KSelectFromSqlCmd,               "SELECT %S FROM %S WHERE %S='%S'"   );
_LIT(  KSelectFromNumSqlCmd,            "SELECT %S FROM %S WHERE %S=%d"     );
_LIT(  KSelectFromNumNotEqSqlCmd,       "SELECT %S FROM %S WHERE %S<>%d"    );
_LIT(  KAndConditionSqlCmd,             " AND %S='%S'"                      );
_LIT(  KAndLikeConditionSqlCmd,         " AND %S LIKE '*%S'"               );
_LIT(  KSelectAttributeByValueSqlCmd,   "SELECT * FROM attribute WHERE atr_value LIKE '*%S*' AND atr_name = '%S' ");
_LIT(  KNewMediaDirSqlCmd,              "UPDATE resources SET rsc_readonly=1 WHERE rsc_path LIKE '%S*' AND rsc_readonly=0");
_LIT(  KInsertElementSqlCmd,            "INSERT INTO element (elm_id,elm_name,elm_value,elm_has_attribute,elm_obj_id,elm_is_required)VALUES(%d,'%S','%S',%d,%d,%d)");
_LIT(  KInsertAttributeSqlCmd,          "INSERT INTO attribute(atr_id,atr_name,atr_value,atr_elm_id,atr_is_required) VALUES(%d,'%S','%S',%d,%d)");
_LIT(  KGetObjectListSqlCmd,            "SELECT id FROM object WHERE parentID=%d");
_LIT(  KGetItemListSqlCmd,              "SELECT id FROM object WHERE parentID=%d AND class LIKE 'object.item*'");
_LIT(  KGetContainerListSqlCmd,         "SELECT id FROM object WHERE parentID=%d AND class LIKE 'object.container*'");
_LIT(  KSelectIdFromElSqlCmd,           "SELECT elm_id, elm_name, elm_value FROM element WHERE elm_obj_id=%d" );
_LIT(  KDeleteResElSqlCmd,              "DELETE FROM element WHERE elm_value='%S' AND elm_obj_id=%d");
_LIT(  KSelectObjectFromObjSqlCmd,      "SELECT id, parentID, title, class, restricted, searchable, refID FROM object WHERE id = %d");
_LIT(  KSelectElmFromElmSqlCmd,         "SELECT elm_name, elm_value, elm_has_attribute, elm_id, elm_is_required FROM element WHERE elm_obj_id = %d");
_LIT(  KSelectAtrFromAttrSqlCmd,        "SELECT atr_name, atr_value FROM attribute WHERE atr_elm_id = %d");
_LIT(  KSelectAtrFromAttrByObjSqlCmd,   "SELECT atr_name, atr_value, atr_elm_id FROM attribute WHERE atr_id_obj = %d");

// initial data
_LIT(  KInit1,                          "INSERT INTO object (id,parentID,title,class,restricted,searchable,path) VALUES(0,-1,'root','object.container',0,1,'-1')");
_LIT(  KInit2,                          "INSERT INTO object (id,parentID,title,class,restricted,searchable,path) VALUES(1,0,'Uploaded','object.container',0,1,'-1.0')");
_LIT(  KInit3,                          "INSERT INTO sequences(seq_col_name,seq_next_key) VALUES('object',2)" );
_LIT(  KInit4,                          "INSERT INTO sequences(seq_col_name,seq_next_key) VALUES('element',0)");
_LIT(  KInit5,                          "INSERT INTO sequences(seq_col_name,seq_next_key) VALUES('attribute',0)");
_LIT(  KInit6,                          "INSERT INTO sequences(seq_col_name,seq_next_key) VALUES('uriId',1)"  );


enum TCurrentAction 
{
    EUndefinedAction = 0,
    ECreateObjectAction = 1,
};

#endif //C_UPNPCONTENTDIRECTORYDBGLOBALS_H
// End of File
