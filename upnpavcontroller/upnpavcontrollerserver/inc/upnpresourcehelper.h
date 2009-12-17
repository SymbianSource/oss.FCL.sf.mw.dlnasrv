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
* Description:      Filetransfer helper
*
*/






#ifndef C_UPNPRESOURCEHELPER_H_
#define C_UPNPRESOURCEHELPER_H_

// EXTERNAL INCLUDES 
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;

// CLASS DECLARATION
/**
 * Filetransfer helper
 *
 * @since S60 v3.2
 */
class CUPnPResourceHelper: public CBase
    {

public:

    /**
     * Static 1st phase constructor
     * @param aObserver Observer
     * @param aType timer type
     * @return A new timer instance
     */    
    static CUPnPResourceHelper* NewL();

    /**
     * Destructor
     */    
    virtual ~CUPnPResourceHelper();
    
private:

    /**
     * Private constructor
     */
    CUPnPResourceHelper();
    
    /**
     * 2nd phase construct
     */
    void ConstructL();
    
public: // New functions

    /**
     * Sets resource (uri). Ownership is transferred
     *
     * @param aResource pointer to a resource
     */
    void SetResource( HBufC* aResource );
    
    /**
     * returns resource (uri).
     *
     * @return pointer to a resource
     */
    HBufC* Resource();
    
    /**
     * Sets item id. Ownership is transferred
     *
     * @param aItemId pointer to an item id
     */
    void SetItemId( HBufC8* aItemId );
    
    /**
     * returns item id
     *
     * @return pointer to an item id
     */
    HBufC8* ItemId();
    
    /**
     * Sets item. Ownership is transferred
     *
     * @param aItem pointer to an item
     */
    void SetItem( CUpnpItem* aItem );
    
    /**
     * returns item
     *
     * @return pointer to an item
     */
    CUpnpItem* Item();
            
private:
    
    /**
     * Resource (uri)
     *
     * Own
     */
    HBufC*      iResource;

    /**
     * Item id
     *
     * Own
     */
    HBufC8*     iItemId;

    /**
     * Item
     *
     * Own
     */
    CUpnpItem*  iItem;
        
    /**
     * key
     */
    TInt        iKey;
    
    };

#endif // C_UPNPRESOURCEHELPER_H_

// End of file
