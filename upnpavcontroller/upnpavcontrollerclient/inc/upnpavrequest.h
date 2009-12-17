/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a data class for client-server comm.
*
*/






#ifndef C_CUPNPAVREQUEST_H
#define C_CUPNPAVREQUEST_H


#include <e32base.h>
//#include <s32strm.h>
#include <s32mem.h>
#include "upnpavbrowsingsession.h"

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION

/**
*  UPnP AV Controller Client/Server communication helper class
*  
*
*  @lib -
*  @since Series 60 3.1
*/

class CUpnpAVRequest : public CBase
    { 
    
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static inline CUpnpAVRequest* NewLC();
    
    /**
     * Two-phased constructor.
     */
    static inline CUpnpAVRequest* NewL();
    
    /**
     * Destructor.
     */
    inline virtual ~CUpnpAVRequest();
    
public:

    /**
     * Externalizes information to stream.
     * Leaves in case of errors.
     * @since Series 60 3.1
     * @param reference to RWriteStream
     * @return none
     */
    inline void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes information from stream.
     * Leaves in case of errors.
     * @since Series 60 3.1
     * @param reference to RReadStream
     * @return none
     */
    inline void InternalizeL( RReadStream& aStream );

    /**
     * Externalizes information to stream and returns the object as a heap
     * desctiptor.
     */    
    inline HBufC8* ToDes8L() const;

private:

    /**
    * Constructor
    */
    inline CUpnpAVRequest();

    /**
     * Perform the second phase construction
     */
    inline void ConstructL();

public: // New methods

    /**
     * Sets Id
     * @param aId Id
     */
    inline void SetIdL( const TDesC8& aId );

    /**
     * Retuns Id
     * @return Id
     */    
    inline const TDesC8& Id();

    /**
     * Sets Uuid
     * @param aUuid Uuid
     */    
    inline void SetUuidL( const TDesC8& aUuid );

    /**
     * Retuns Uuid
     * @return Uuid
     */        
    inline const TDesC8& Uuid();
    
    /**
     * Sets filepath
     * @param aFilePath filepath
     */    
    inline void SetFilePathL( const TDesC& aFilePath );
    
    /**
     * Retuns filepath
     * @return filepath
     */        
    inline const TDesC& FilePath();

    /**
     * Sets URI
     * @param aURI URI
     */    
    inline void SetURIL( const TDesC8& aURI );
    
    /**
     * Retuns URI
     * @return URI
     */        
    inline const TDesC8& URI();
    
    /**
     * Sets if a browse action is required
     * @param aBrowse ETrue is a browse action is required
     */    
    inline void SetRequiresBrowse( TBool aBrowse );
    
    /**
     * Retuns ETrue if a browse action is required
     * @return ETrue/EFalse
     */        
    inline TBool RequiresBrowse();

    /**
     * Sets used memory type
     * @param aMemoryType memory type
     */
    inline void SetMemoryType( MUPnPAVBrowsingSession::TMemoryType
        aMemoryType );

    /**
     * Retuns used memory type
     * @return memory type
     */        
    inline MUPnPAVBrowsingSession::TMemoryType MemoryType();

    /**
     * Sets the state of filesharing after operation
     * @param aFileSharing state of filesharing
     */
    inline void SetFileSharing( MUPnPAVBrowsingSession::TFileSharing
        aFileSharing );

    /**
     * Retuns state of filesharing
     * @return filesharing
     */        
    inline MUPnPAVBrowsingSession::TFileSharing FileSharing();

private:
    
    HBufC8*                                 iId; // Owned
    HBufC8*                                 iUuid; // Owned
    HBufC*                                  iFilePath; // Owned
    HBufC8*                                 iURI; // Owned
    TBool                                   iRequiresBrowse; // Owned
    MUPnPAVBrowsingSession::TMemoryType     iMemoryType; // Owned
    MUPnPAVBrowsingSession::TFileSharing    iFileSharing; // Owned
    };

#include "upnpavrequest.inl"

#endif // C_CUPNPAVREQUEST_H
