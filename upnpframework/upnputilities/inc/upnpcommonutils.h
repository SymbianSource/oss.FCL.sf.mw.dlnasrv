/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP framework's common utility functions
*
*/


#ifndef UPNPCOMMONUTILS_H
#define UPNPCOMMONUTILS_H

//  INCLUDES
#include <e32base.h>
#include <upnpobject.h>
#include <f32file.h>

// DATA TYPES
enum TUPnPItemType
    {
    ETypeAudio = 0,
    ETypeVideo,
    ETypeImage,
    ETypePlaylist,
    ETypeOther
    };

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpContainer;
class CUpnpContainerList;
class CUpnpAttribute;
class TInetAddr;

// CONSTANTS
// upload directory
_LIT( KUpnpUploadDirectory, "Download\\Media\\" );

// CLASS DECLARATION

/**
* UPnP Framework's common utility functions
*
* @lib upnputilities.lib
* @since S60 3.0
*/
class UPnPCommonUtils
    {

public: // Business logic methods

    /**
     * Returns status whether memory card is inserted or not.
     *
     * @since S60 3.0
     * @return TBool indicating the status of memory card
     */
    IMPORT_C static TBool MemoryCardStateL();

    /**
     * Resolves the MIME type of a file by it's content, using the 
     * platform MIME recognizer. Leaves in error cases.
     *
     * @since S60 3.0
     * @param TDesC8& file name (with full path info)
     * @return HBufC8* mime type
     */
    IMPORT_C static HBufC8* ResolveMimeTypeL(
                                const TDesC& aFileName );

    /**
     * Resolves the MIME type of a file by it's extension. Leaves in error
     * cases.
     *
     * @since S60 3.0
     * @param TDesC& const, file name (with full path info)
     * @return HBufC8* mime type
     */
    IMPORT_C static HBufC8* GetMimeTypeByExtensionL( 
                                const TDesC& aFileName );

    /**
     * Resolves the type of a file. Leaves in error cases.
     *
     * @since S60 3.0
     * @param TDesC8& file name (with full path info)
     * @return TUPnPItemType file type
     */
    IMPORT_C static TUPnPItemType ResolveFileTypeL( 
                                const TDesC& aFileName );

    /**
     * Replaces IP address and port number placeholders of the RES
     * element's URI string of the given item.
     *
     * @since S60 3.1
     * @param aItem (CUpnpItem&) the item which RES element's URI is to be
     *        updated
     * @param aAddress (const TInetAddr&) the address and port number of
     *        the updated URI
     */
    IMPORT_C static void ReplacePlaceHolderInURIL( 
                                CUpnpItem& aItem,
                                const TInetAddr& aAddress );

    /**
     * Replaces the illegal filename characters of the given filename
     * string and returns a new fixed filename string. The following
     * characters are replaced with an underscore: >, <, ", \, /, *, |, 
     * : and ?. 8bit version.
     *
     * @since S60 3.1
     * @param aFileName (const TDesC8&) filename to be checked
     * @return HBufC8* fixed filename string
     **/
    IMPORT_C static HBufC8* ReplaceIllegalFilenameCharactersL( 
                                const TDesC8& aFileName );
                    
    /**
     * Replaces the illegal filename characters of the given filename
     * string and returns a new fixed filename string. The following
     * characters are replaced with an underscore: >, <, ", \, /, *, |, 
     * : and ?. 16bit version.
     *
     * @since S60 3.1
     * @param aFileName (const TDesC&) filename to be checked
     * @return HBufC* fixed filename string
     **/
    IMPORT_C static HBufC* ReplaceIllegalFilenameCharactersL( 
                                const TDesC& aFileName );

    /**
     * Checks if the given CUpnpObject is a playlist container. Leaves if
     * the given object is not correctly formed. 
     *
     * @since S60 3.1
     * @param aContainer (const CUpnpObject&) filename to be checked
     * @return TBool
     **/
    IMPORT_C static TBool IsPlaylistContainerL( 
                                const CUpnpObject& aContainer );

    /** 
     * Return container with given title from list of containers
     *
     * @since S60 3.1
     * @param aList List containig containers to search
     * @param aTitle Title to search for
     * @return CUpnpContainer* The pointer to container, NULL if
     * not found
     **/
    IMPORT_C static CUpnpContainer* FindContainerByTitle(
        const CUpnpContainerList& aList, const TDesC8& aTitle );

    /**
     * Convert TInt to TDesC8
     * @since S60 3.1
     * @param aObject TInt to convert
     */
    IMPORT_C static HBufC8* IntToDesC8LC( const TInt& aObject );

    /**
     * Convert TDesC8 to TInt
     * @since S60 3.1
     * @param aObject Descriptor to convert
     */
    IMPORT_C static TInt DesC8ToInt( const TDesC8& aObject );
    
    /**
     * Converts TTime to UPnP date string
     *
     * @since S60 3.1
     * @param aTime time
     * @return UPnP date as a heap descriptor
     */
    IMPORT_C static HBufC* TTimeToUPnPDateL( TTime aTime );

    
    /**
     * Rename the downloaded file to %s_upnpfwtemp(%s is original filename)
     * used in play remote item on local device
     * @since S60 3.1
     *
     * @param aFilePath original file path
     * @return HBufC pointer to the new filepath
     */
    IMPORT_C static HBufC* UPnPCommonUtils::RenameFileL( const TDesC& aFilePath );
    
    /**
     * Delete upnp genereated temporary files from download locations
     * both from phone memory and memory card
     * Leave in case of fatal errors
     * @since S60 3.1
     *
     */
    IMPORT_C static void DeleteTempFilesL();


     /**
     * Checks if device supports images 
     *
     * @since S60 3.1
     * @param TDesC8& protocolinfo
     * @return TBool
     */
    IMPORT_C static TBool IsImageSupported(
                                const TDesC8& aProtocolInfo );
                                
                                
     /**
     * Checks if device supports audio 
     *
     * @since S60 3.1
     * @param TDesC8& protocolinfo
     * @return TBool
     */
    IMPORT_C static TBool IsAudioSupported(
                                const TDesC8& aProtocolInfo );
                                
                                
     /**
     * Checks if device supports video 
     *
     * @since S60 3.1
     * @param TDesC8& protocolinfo
     * @return TBool
     */
    IMPORT_C static TBool IsVideoSupported(
                                const TDesC8& aProtocolInfo );
                                
     /**
     * Return the file extension according a mime type
     *
     * @since S60 3.2
     * @param const TDesC8& mime type
     * @return file extension
     */
    IMPORT_C static HBufC* FileExtensionByMimeTypeL(
        const TDesC8& aMimeType );
    
    /**
     * Removes the tab character of the given text
     * string and returns a new fixed file title string.
     *
     * @since S60 3.2.3
     * @param aText (const TDesC8&) listboxe item text to be checked
     * @return HBufC8* fixed text string
     **/
    IMPORT_C static HBufC8* FixListboxItemTextL( 
                                const TDesC8& aText );    
    };


#endif  // UPNPCOMMONUTILS_H

// End of File
