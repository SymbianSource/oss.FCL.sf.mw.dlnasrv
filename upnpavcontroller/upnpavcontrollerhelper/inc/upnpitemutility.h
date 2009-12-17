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
* Description:      Utility for parsing upnp items
*
*/






#ifndef UPNPITEMUTILITY_H
#define UPNPITEMUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <upnpobject.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpObject;
class CUpnpElement;
class CUpnpAttribute;

// CLASS DECLARATION

/**
* Utility for working with classes CUpnpObject and CUpnpItem.
* The class provides helpers to locating elements and attributes
* within the class, locating the resource and the URI, decoding
* some special elements like dc:date, res@duration etc.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class UPnPItemUtility
    {

public: // Business logic methods

    /**
     * Tests if given object is of given class type.
     * Note that in upnp the object classes are organised in open
     * hierarchies, and this method will return true even if the class
     * is unknown, if aObject's class is derived from aClass.
     * see upnpconstantdefs.h for defined class definitions.
     * example:
     * TBool isPlaylist = BelongsToClass( item, KClassPlaylist );
     *
     * @since S60 3.2
     * @param aObject the object whose class is to be tested
     * @param aClass the class (in string form) against which to test
     * @return reference to the res-element
     */
    IMPORT_C static TBool BelongsToClass(
        const CUpnpObject& aObject,
        const TDesC8& aClass );

    /**
     * Extracts all RES elements from an item.
     * An item can include multiple resources, for instance
     * an item may include the same image scaled to multiple
     * resolutions, or a music track may include the track and
     * its CR cover art as thumbnail.
     * example:
     * RUPnPElementsArray array;
     * GetResElements( array );
     *
     * @since S60 3.2
     * @param aItem UPnP item where to extract the res element.
     * @return reference to the res-element
     */
    IMPORT_C static void GetResElements(
        const CUpnpObject& aObject,
        RUPnPElementsArray& aResElementsArray );

    /**
     * Finds the original resource of given item.
     * Note that if item has multiple resource elements,
     * this API will select the original and recommended one to use.
     * Also note that if an item includes a thumbnail or another
     * supporting resource, this can not be used to retrieve it.
     * Example:
     * const TDesC& uri = ResourceFromItemL( item ).Value();
     *
     * @since S60 3.2
     * @param aItem UPnP item where to extract the res element.
     * @return reference to the res-element
     */
    IMPORT_C static const CUpnpElement& ResourceFromItemL(
        const CUpnpItem& aItem );

    /**
     * Finds an element within an object.
     * see upnpconstantdefs.h for element name definitions.
     * example:
     * CUpnpElement* date = FindElementByName( item, KElementDate );
     *
     * @since S60 3.2
     * @param aObject the object where to look for elements
     * @param aName element name
     * @return a pointer to the element, or NULL if not found
     */
    IMPORT_C static  const CUpnpElement* FindElementByName(
        const CUpnpObject& aObject, const TDesC8& aName );

    /**
     * Finds an element within an object.
     * Leaves if not found.
     * see upnpconstantdefs.h for element name definitions.
     * example:
     * TDesC& genre = FindElementByNameL( item, KElementDate ).Value();
     *
     * @since S60 3.2
     * @param aObject the object where to look for elements
     * @param aName element name
     * @return a reference to the element found.
     */
    IMPORT_C static  const CUpnpElement& FindElementByNameL(
        const CUpnpObject& aObject, const TDesC8& aName );

    /**
     * Finds an attribute within an upnp element
     * see upnpconstantdefs.h for element name definitions.
     * example:
     * CUpnpAttribute* s = FindAttributeByName( resElem, KAttributeSize );
     *
     * @since S60 3.2
     * @param aElement the element where to look for attributes
     * @param aName attribute name
     * @return a pointer to the attribute found
     */
    IMPORT_C static const CUpnpAttribute* FindAttributeByName(
        const CUpnpElement& aElement, const TDesC8& aName );

    /**
     * Finds an attribute within an upnp element.
     * Leaves if not found
     * see upnpconstantdefs.h for attribute name definitions.
     * example:
     * TDesC& s = FindAttributeByNameL( resElem, KAttributeSize ).Value();
     *
     * @since S60 3.2
     * @param aElement the element where to look for attributes
     * @param aName attribute name
     * @return a reference to the attribute found
     */
    IMPORT_C static const CUpnpAttribute& FindAttributeByNameL(
        const CUpnpElement& aElement, const TDesC8& aName );

    /**
     * Converts given upnp date string into TTime.
     * example:
     * TTime time = UpnpDateAsTTime( FindElementByNameL(
     *      item, KElementDate ).Value() );
     *
     * @since S60 3.2
     * @param aUpnpDate value from item's dc:date element
     * @param aTime out variable to receive corresponding TTime
     * @return KErrNone if conversion was succesful, otherwise < 0
     *         for error values see TTime::Parse()
     */
    IMPORT_C static TInt UPnPDateAsTTime(
        const TDesC8& aUpnpDate, TTime& aTime );

    /**
     * Converts given upnp duration or position string to a value in
     * milliseconds.
     * Duration was received in res element duration attribute, or
     * in PositionInfo result from renderer.
     * example:
     * TInt duration = UpnpDurationAsMilliseconds( FindAttributeByNameL(
     *      ResourceFromItemL( item ), KAttributeDuration ).Value() );
     *
     * @since S60 3.2
     * @param aDuration duration or position as string
     * @param aMilliseconds out variable to receive the result
     * @return KErrNone if conversion was succesful, otherwise < 0
     */
    IMPORT_C static TInt UPnPDurationAsMilliseconds(
        const TDesC8& aDuration, TInt& aMilliseconds );

private:

    static void UPnPDateAsTTimeL( const TDesC8& aUpnpDate,
        TTime& aTime );


    };



#endif  // UPNPITEMUTILITY_H

// End of File
