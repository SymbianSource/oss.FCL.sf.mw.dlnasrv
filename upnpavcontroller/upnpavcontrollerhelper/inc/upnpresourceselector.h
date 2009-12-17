/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Interface for selecting a resource within an item
*
*/






#ifndef UPNPRESOURCESELECTOR_H
#define UPNPRESOURCESELECTOR_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CUpnpItem;
class CUpnpElement;


// CLASS DECLARATION


/**
* An interface for a component that selects a resource from an item.
* In upnp items have multiple resources, and it is not necessarily
* clear which one of them the client wants to display. This interface
* provides a client a clear option to choose whichever resource seems
* best.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class MUPnPResourceSelector
    {
public: // the interface

    /**
     * Selects the resource from given item. The implementing class
     * should return a reference to one of the elements within the
     * given item. If there is some problem in selecting a resource
     * (for instance there are NO resources in the item) the method
     * should leave
     *
     * @since S60 3.2
     * @param aItem the item where to seek resources
     * @return reference to selected resource
     */
    virtual const CUpnpElement& SelectResourceL(
        const CUpnpItem& aItem ) = 0;

    };




/**
 * A helper selector that will select the original or most likely
 * resource from an item, using UPnPItemUtility::ResourceFromItem.
 * Most clients will find this practical.
 * This class could be thought as a DEFAULT implementation for
 * MUPnPResourceSelector.
 */
class TUPnPSelectDefaultResource
    : public MUPnPResourceSelector
    {

public:

    /**
     * see MUPnPResourceSelector
     */
    IMPORT_C const CUpnpElement& SelectResourceL(
        const CUpnpItem& aItem );

    };

/**
 * A helper selector that will select the first resource available.
 * This selector is handy if it is certain that there is only one
 * resource available or if the first resource is always the proper
 * one. (as is when sharing a local item)
 *
 * @since S60 3.2
 */
class TUPnPSelectFirstResource
    : public MUPnPResourceSelector
    {

public:

    /**
     * see MUPnPResourceSelector
     */
    IMPORT_C const CUpnpElement& SelectResourceL(
        const CUpnpItem& aItem );

    };


#endif  // UPNPRESOURCESELECTOR_H

// End of File
