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
 * Description:  CUpnpDlnaFilter declaration.
 *
 */

#ifndef UPNPDLNAFILTER_H
#define UPNPDLNAFILTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h> 
#include "upnphttpservertransactioncreator.h"

class MUpnpContentDirectoryDataFinder;
class CUpnpHttpMessage;
class CUpnpDlnaProtocolInfo;
class CUpnpSecurityManager;
class CUpnpHttpFileServeTransaction;
class CUpnpHttpFileReceiveTransaction;
class CUpnpDlnaFilterHeaders;
class TUpnpDlnaCorelation;

// CLASS DECLARATION

/**
 *  CUpnpDlnaFilter
 * 
 */
NONSHARABLE_CLASS( CUpnpDlnaFilter ) : public CBase,
        public MUpnpHttpServerTransactionCreator
    {
public:
    // Public Constructors and destructor

    /**
     * Destructor.
     */
    ~CUpnpDlnaFilter();

    /**
     * Two-phased constructor.
     */
    static CUpnpDlnaFilter* NewL( MUpnpContentDirectoryDataFinder* aFinder,
        CUpnpSecurityManager* aSecurityManager );

    /**
     * Two-phased constructor.
     */
    static CUpnpDlnaFilter* NewLC( MUpnpContentDirectoryDataFinder* aFinder,
        CUpnpSecurityManager* aSecurityManager );

    //from MUpnpHttpServerTransactionCreator
    virtual void NewTransactionL(
        const TDesC8& aMethod, const TDesC8& aUri, 
        const TInetAddr& aSender, CUpnpHttpServerTransaction*& aResultTrans );

    //from MUpnpHttpServerTransactionCreator

    /**
     * Checks correctness of path to, requested in HTTP message, file and 
     *  changes its format to be compliant with Symbian's file system.
     * @since Series60 2.6
     * @param aMsg HTTP message containing file request.
     * @param aPathWithNewMethod reference which is filled with formated path 
     *  to requested file.
     **/
    void FormatPathL( CUpnpHttpFileServeTransaction *aTransaction, TDes &aPath );

    /**
     * Add appropriate directives to http header according to DLNA specification
     * @param aTransaction http transaction
     * @return KErrNone header prepared successfully, otherwise -EHttpNotFound
     *         If DLNA correlations are wrong error from then it returns an error
     */
    TInt PrepareHeaderL( CUpnpHttpFileServeTransaction &aTransaction );

    /**
     * Method checks DLNA transferMode for POST messages.
     * @since Series60 2.6
     * @param aPostMessage - incoming POST message with upload to be saved in file.
     * @return ETrue if POST can be accepted.
     **/
    TInt CheckDLNAPostCorrelationsL(CUpnpHttpFileReceiveTransaction& aTransaction);

    /**
     * Determines download path basing on incomming POST message and media path got
     *  from server. Result is saved in iInFilename. 
     *  If file should not be saved, flag iSaveToFile will be set to EFalse.
     * @since Series60 2.6
     * @param aPostMessage - incoming POST message with upload to be saved in file.
     * @return download path or NULL in case of error
     */
    HBufC* DetermineDownloadPathL(CUpnpHttpFileReceiveTransaction& aTransaction);

    //security manager getter
    CUpnpSecurityManager* SecurityManager();
    
    //file server session getter        
    RFs& FileSession();

    /**
     * Check if resource is accessible 
     * @param aFileName data source file name
     * @param aSender sender
     * @return KErrNone if resource is accessible, otherwise -EHttpNotFound
     */
    TInt AuthorizeRequestL( const TDesC& aFileName, const TInetAddr& aSender );

public:
    // protected
    // Private Constructors

    /**
     * Constructor for performing 1st stage construction
     */
    CUpnpDlnaFilter( MUpnpContentDirectoryDataFinder* aFinder,
        CUpnpSecurityManager* aSecurityManager );

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:

    /**
     * Find protocolInfo by contentUri (Not by importUri) and extract 3rd field,
     * using ContentDirectory.
     * @param aFullContentUri uri to be searched in database
     *       (full value of resource with IP:port prefix).
     * @return a3rdhField 3rdhField from protocolInfo (if it is DLNA compatible) which is
     *       related to founded resource. Ownership is transfered to the caller.
     */
    HBufC8* CUpnpDlnaFilter::ThirdFieldFromCdL( const TDesC8& aContentUri );

    /**
     * Gets name of file with content for given object's id.
     * @param aObjectId id of the ContentDirecotry object holding the content
     * @param aFileName pointer to buffer in which the name of the file will be returned.
     *       Buffer for aFileName has to allocated first.
     */
    void GetMediaFileNameL( TInt aObjectId, TPtr& aFileName );

    /**
     * Find a folder shared from DB (ContentDirectory).
     * @param aUrlPath Name of URL path that needs to be found and converted
     * to aFolderPath
     * @param aFileName Name of shared file (can be null if just folder is looking for).
     * @param aSystemPath Name of shared folder.
     * @return KErrNone or another of the system error codes.
     */
    TInt FindSharedFolderDBL(const TDesC8& aUrlPath, const TDesC8& aFileName,
            HBufC8*& aSystemPath);

    /**
     * Checks if specified URI exists in database and returns object id for
     *  given URI or KErrNotFound if URI is no registered in database.
     * @param aImportUri uri to be searched in databse.
     * @return objId if successful or KErrnone if object wasn't found.
     */
    TInt CheckImportUriL( TDesC8& aImportUri );

    /**
     * Checks DLNA correlations for given message.
     * @since Series60 2.6
     * @param aMessage message for which correlations check will be performed 
     *  exists for given message.
     * @return KErrNone if no HTTPerror problem occurs	 
     **/
    TInt CheckDLNACorrelationsL( CUpnpHttpFileServeTransaction& aTransaction );


    /**
     * Method returns content type of a resource
     * @since Series60 3.2
     * @param aMessage, aMime, aFilename
     * @return KErrNone if mime type retrieved with no errors
     **/
    TInt GetContentTypeL( CUpnpHttpFileServeTransaction &aTransaction, HBufC8*& aMime,
        const TDesC16& aFilename );
 

    CUpnpDlnaProtocolInfo* ProtocolInfoL( const TDesC8& aContentUri );

    void AddHeaderIfNotEmptyL( const TDesC8& aHeaderName, 
        CUpnpHttpFileServeTransaction& aTransaction );
    
    
    HBufC* MakeFileNameUniqueL( const TDesC& aFilename, RFs& aFs );

    HBufC* PreparePostfixToMakeFileUniqueL( const TDesC& aFilename, RFs& aFs );
    
    HBufC* PrepareBaseFileNameL( const TDesC& aFilename, RFs& aFs );
    
    TInt CheckCorelationL( CUpnpHttpFileServeTransaction& aTransaction,
                            TUpnpDlnaCorelation& aDlnaCorelation );
                            
    TInt CheckTransferModeL( CUpnpHttpFileServeTransaction& aTransaction,
                             TUpnpDlnaCorelation& aDlnaCorelation );
                             
    TInt AppendCorelationHeadersL( CUpnpHttpFileServeTransaction& aTransaction,
                             TUpnpDlnaCorelation& aDlnaCorelation, TDesC8& aTransferMode  );
	
protected:	
	
    // Pointer to ContentDirectoryDataFinder implementation.
    // Not owned.
    MUpnpContentDirectoryDataFinder* iCdDataFinder;
    
    CUpnpSecurityManager* iSecurityManager;
    
    // protocol info for dlna corelation
    CUpnpDlnaProtocolInfo* iProtocolInfo;

    RFs iFs;
    
    };

#endif // UPNPDLNAFILTER_H
// End Of File
