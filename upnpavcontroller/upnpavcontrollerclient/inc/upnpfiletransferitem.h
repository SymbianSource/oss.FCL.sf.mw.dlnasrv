/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      a data class containing filetransfer-related data
*
*/






#ifndef C_CUPNPFILETRANSFERITEM_H
#define C_CUPNPFILETRANSFERITEM_H

// EXTERNAL INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION
/**
*  UPnP AV Controller Client/Server communication helper class
*  
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS( CUpnpFileTransferItem ) : public CBase
    { 
    
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static inline CUpnpFileTransferItem* NewLC();
    
    /**
     * Two-phased constructor.
     */
    static inline CUpnpFileTransferItem* NewL();
    
    /**
     * Destructor.
     */
    inline virtual ~CUpnpFileTransferItem();
    
public:

    /**
     * Externalizes device information to stream.
     * Leaves in case of errors.
     * @since Series 60 3.2
     * @param reference to RWriteStream
     */
    inline void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes device information from stream.
     * Leaves in case of errors.
     * @since Series 60 3.2
     * @param reference to RReadStream
     */
    inline void InternalizeL( RReadStream& aStream );

    /**
     * Externalizes information to stream and returns the object as a heap
     * desctiptor.
     */        
    inline HBufC8* ToDes8L() const;

private: //

    /**
     * Constructor
     */
    inline CUpnpFileTransferItem();

    /**
     * Perform the second phase construction
     */
    inline void ConstructL();

public: // New methods

    /**
     * Set Uri
     *
     * @param aUri Uri
     */        
    inline void SetUriL( const TDesC8& aUri );
    
    /**
     * Return Uri
     *
     * @return Uri
     */        
    inline const TDesC8& Uri() const;

    /**
     * Set title
     *
     * @param aTitle title
     */        
    inline void SetTitleL( const TDesC8& aTitle );
    
    /**
     * Return title
     *
     * @return title
     */        
    inline const TDesC8& Title() const;
    
    /**
     * Set protocolinfo
     *
     * @param aProtocolInfo protocolinfo
     */        
    inline void SetProtocolInfoL( const TDesC8& aProtocolInfo );
    
    /**
     * Return protocolinfo
     *
     * @return protocolinfo
     */        
    inline const TDesC8& ProtocolInfo() const;

    /**
     * Set filepath
     *
     * @param aPath filepath
     */        
    inline void SetPathL( const TDesC& aPath );
    
    /**
     * Return filepath
     *
     * @return filepath
     */        
    inline const TDesC& Path() const;

    /**
     * Set key
     *
     * @param aKey identifier key
     */        
    inline void SetKey( TInt aKey );
    
    /**
     * Return key
     *
     * @return key
     */        
    inline TInt Key();
    
private:
    
    /**
     * Stores URI
     *
     * Owned
     */        
    HBufC8* iUri;

    /**
     * Stores title
     *
     * Owned
     */        
    HBufC8* iTitle;
    
    /**
     * Stores protocolInfo
     *
     * Owned
     */        
    HBufC8* iProtocolInfo;
    
    /**
     * Stores path
     *
     * Owned
     */        
    HBufC*  iPath;

    /**
     * Stores key (filetransfer identifier)
     *
     * Owned
     */        
    TInt    iKey;    
    
    };

#include "upnpfiletransferitem.inl"

#endif // C_CUPNPFILETRANSFERITEM_H

// End of file
