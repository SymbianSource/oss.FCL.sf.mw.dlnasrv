/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Implements a list of UPnP Devices
*
*/






#ifndef C_UPNPAVDEVICELIST_H
#define C_UPNPAVDEVICELIST_H

// INCLUDES
#include <e32base.h>
#include <s32mem.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
//None
// FORWARD DECLARATIONS
class CUpnpAVDevice;


// CLASS DECLARATION

/**
 *  UPnP AV Device list class
 *
 *  @lib upnpavcontrollerclient.lib
 *  @since Series 60 3.1
 */
class CUpnpAVDeviceList : public CBase
    {
public:  // Constructors and destructor
    
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpAVDeviceList* NewL();

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CUpnpAVDeviceList* NewLC();
    
    /**
     * Destructor.
     */
    virtual ~CUpnpAVDeviceList();

public: // New functions
    
    /**
     * Operator
     */
    IMPORT_C CUpnpAVDevice* operator[]( TInt aIndex ) const;
        
    /**
     * Serialize the whole list to a heap based descriptor.
     *
     * @return pointer to the descriptor where the list is serialized.
     */
    IMPORT_C HBufC8* ToDes8L() const;       

    /**
     * Appends a new device
     * 
     * @param none
     * @return TUPnPObjectType information of the type
     */
    IMPORT_C void AppendDeviceL( CUpnpAVDevice& aDevice );
    
    /**
     * Removes a device
     * 
     * @param aId id of the object to be removed and destroyed
     */
    IMPORT_C void RemoveAndDestroy( TInt aIndex );

    /**
     * Detaches a device
     * 
     * @param aId id of the object to be removed and destroyed
     */
    IMPORT_C void Remove( TInt aIndex );

    /**
     * Removes all devices
     */        
    IMPORT_C void ResetAndDestroy();

    /**
     * Detaches all devices
     */        
    IMPORT_C void Reset();
    
    /**
     * Returns the count of devices
     *
     * @return TInt count information
     */
    IMPORT_C TInt Count( ) const;  

public:

    /**
     * Externalizes list
     *
     * @param reference to RWriteStream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
    
    /**
     * Internalizes itemlist information from stream.
     * 
     * @param reference to RReadStream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

protected:

    /**
     * C++ default constructor.
     */
    CUpnpAVDeviceList();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );
    
private:
    
    // Device array
    RPointerArray<CUpnpAVDevice>   iDevices;
    };



#endif      // C_UPNPAVDEVICELIST_H   
            
// End of File
