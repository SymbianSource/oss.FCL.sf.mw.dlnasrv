/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     UpnpObjectLite for fast browsing 
*
*/






#ifndef C_UPNPOBJECTLITE_H_
#define C_UPNPOBJECTLITE_H_

// INCLUDES
#include <e32base.h>

/**
 * CUpnpObjectLite for fast browsing. Contains a minimum set of properties
 * and memory consumption is kept low
 *
 * @since Series 60 3.1
 * @lib upnpxmlparser.lib
 */
NONSHARABLE_CLASS( CUpnpObjectLite ) : public CBase
    {

public:

    /**
     * Enumeration to determine object class
     */
    enum TObjectType
        {    
        EItem       = 0,
        EContainer,
        EMusicItem,
        EImageItem,
        EVideoItem,
        };

protected:

    /**
     * Enumeration to determine base type
     */
    enum TObjectTypeBase
        {
        EBaseUnknown        = 0x00000000,
        EBaseItem           = 0x01000000,
        EBaseContainer      = 0x02000000
        };
    
    /**
     * Enumeration to determine sophisticated item type
     */
    enum TItemTypeBase
        {
        EBaseMusicItem      = 0x00010000,
        EBaseImageItem      = 0x00020000,
        EBaseVideoItem      = 0x00040000,
        EBasePlaylistItem   = 0x00080000,
        
        EMusicTrack         = 0x00000100,
        EPhoto              = 0x00000200,
        EMovie              = 0x00000400
        };
        
    /**
     * Enumeration to determine sophisticated container type
     */
    enum TContainerTypeBase
        {
        EStorageFolder      = 0x00010000,
        EPlaylistContainer  = 0x00020000,
        //EAlbum              = 0x00040000,
        //EGenre              = 0x00080000,
        
        EMusicAlbum         = 0x00000100             
        };        
    
public:

    /**
     * 2-phased constructor.
     */
    IMPORT_C static CUpnpObjectLite* NewL();
    
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpObjectLite();

public:

    /**
     * Setter for object id
     *
     * @param aObjectId object id
     */
    IMPORT_C void SetObjectIdL( const TDesC8& aObjectId );
    
    /**
     * Setter for object id
     *
     * @param aObjectId object id (ownership of aObjectId is transferred)
     */
    IMPORT_C void SetObjectIdL( HBufC8* aObjectId );
    
    /**
     * Getter for object id
     *
     * @return object id
     */
    IMPORT_C const TDesC8& ObjectId() const;    

    /**
     * Setter for title
     *
     * @param aTitle title
     */
    IMPORT_C void SetTitleL( const TDesC& aTitle );
    
    /**
     * Setter for title
     *
     * @param aTitle title (ownership of aTitle is transferred)
     */
    IMPORT_C void SetTitleL( HBufC* aTitle );
    
    /**
     * Getter for title
     *
     * @return title
     */
    IMPORT_C const TDesC& Title() const;
    
    /**
     * Setter for object id. Does not store objectid as a string, but as a
     * bitfield to save memory
     *
     * @param aTitle title (ownership of aTitle is transferred)
     */
    IMPORT_C void SetObjectClass( const TDesC8& aObjectClass );
    
    /**
     * Can be used to chech type of the object
     *
     * @return ETrue if the type (part of object class) matches
     */
    IMPORT_C TBool IsTypeOf( TObjectType aType ) const;
    
protected:

    /**
     * Constructor.
     */
    CUpnpObjectLite();

    /**
     * 2nd phase constructor.
     */
    void ConstructL();

protected:

    void ParseToDelimeter( TLex8& aLex, TChar aDelimeter ) const;
    
    void HandleRound( TInt aRound, TPtrC8 aString );
    
    void Round1( TPtrC8 aString );
    
    void Round2( TPtrC8 aString );
    
    void Round3( TPtrC8 aString );
    
    void Round4( TPtrC8 aString );

protected:

    HBufC8* iObjectId;
        
    HBufC* iTitle;
    
    TInt    iObjectClass;    
        

    };

#endif // C_UPNPOBJECTLITE_H_
