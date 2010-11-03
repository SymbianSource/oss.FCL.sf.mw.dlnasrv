/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the CUpnpContainerResolver class.
*
*/

#ifndef C_UPNP_CONTAINER_RESOLVER_H
#define C_UPNP_CONTAINER_RESOLVER_H

#include <e32base.h>
#include <barsc.h> // for RResourceFile
#include <upnpcontainer.h>
#include "upnpcdsliteobjectarray.h"

// FORWARD DECLARTIONS
class CUpnpItem;

// CLASS DECLARATION

/**
*  CUpnpContainerResolver class definition
*
*  @lib upnpsharingalgorithm.lib
*  @since S60 5.2
*/
class CUpnpContainerResolver : public CBase
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @since S60 5.2
         * @param aCdsLiteObjectArray, array of cds objects
         * @return container resolver object
         */
        static CUpnpContainerResolver* NewL(
            CUpnpCdsLiteObjectArray& aCdsLiteObjectArray );

        /**
         * Destructor.
         *
         * @since S60 5.2
         */
        virtual ~CUpnpContainerResolver();

        /**
         * Resolves containers that need to be created.
         * Container list is returned to the caller.
         *
         * @since S60 5.2
         * @param aItem, upnp item to be shared
         * @return RPointerArray, array containing created containers
         */
        RPointerArray<CUpnpContainer>
            ResolveContainerStructureL( CUpnpItem& aItem );

        /**
         * Resolves containers that need to be created for a reference item.
         * Container list is returned to the caller.
         *
         * @since S60 5.2
         * @param aType, reference media type
         * @param aItem, upnp item to be shared
         * @return RPointerArray, array containing created containers
         */
        RPointerArray<CUpnpContainer>
            ResolveReferenceContainersL( TInt aType, CUpnpItem& aItem );

        /**
         * Resolves containers that need to be removed (containers
         * that does not contain items anymore) after an item
         * has been unshared
         *
         * @since S60 5.2
         * @param aContainerId, Container id where item belongs.
         * @return RPointerArray, containers to be removed
         */
        RPointerArray<CUpnpCdsLiteObject>
            ResolveEmptyContainersL( TDesC8& aContainerId );

    private: // Second-phase contstruction

        /**
         * Default c++ constructor
         *
         * @since S60 5.2
         * @param aCdsLiteObjectArray, array of cds objects
         */
        CUpnpContainerResolver(
            CUpnpCdsLiteObjectArray& aCdsLiteObjectArray ):
                iCdsLiteObjectArray( aCdsLiteObjectArray ) {}

        /**
         * Second-phase constructor.
         *
         * @since S60 5.2
         */
        void ConstructL();

        /**
         * Creates a new UpnpContainer according to the given parameters
         *
         * @since S60 5.2
         * @param aTitle, title of created container
         * @param aClass, class of created container
         * @return CUpnpContainer, created container
         */
        CUpnpContainer* CreateContainerL( const TDesC8& aTitle,
                                          const TDesC8& aClass );

        /**
         * Finds non-existing containers from CDS and creates them.
         * If new containers are not created, returned parent id
         * is for item, otherwise for last found container
         *
         * @since S60 5.2
         * @param aContainerNames, container names to be resolved
         * @param aContainers, containers that need to be shared
         * @param aMediaClass, media class type
         * @return HBufC8, parent id of a container / item
         */
        HBufC8* CreateContainersL(
            RPointerArray<HBufC8>& aContainerNames,
            RPointerArray<CUpnpContainer>& aContainers,
            const TDesC8& aMediaClass );


        /**
         * Reads resource text from component resource file.
         *
         * @since S60 5.2
         * @param aResourceId, resource id
         * @return HBufC8, resource text
         */
        HBufC8* ReadResourceTextL( TInt aResourceId );

        /**
         * Finds element value of a certain element
         *
         * @since S60 5.2
         * @param elements, element array
         * @return HBufC8, element value
         */
        HBufC8* GetElementL( const RUPnPElementsArray& elements,
                                   const TDesC8& aElement );

        /**
         * Modifies aDate parameter so that month and day
         * values are decreased by one. This is because
         * month and day count need to be started from
         * zero, not from 1.
         *
         * @since S60 5.2
         * @param aDate, date to be adjusted
         * @return HBufC8, adjusted date
         */
        HBufC8* AdjustDateL( const TDesC8& aDate );

        /**
         * Adds date containers to the aContainerNames
         * list. Year, Month and Day are added separately.
         * Returns KErrNotFound, if date metadata is missing.
         *
         * @since S60 5.2
         * @param aContainerNames, list of containers
         * @param aElements, list of item elements
         * @return TInt, error code
         */
        TInt AddByDateContainersL(
            RPointerArray<HBufC8>& aContainerNames,
            const RUPnPElementsArray& aElements );

        /**
         * Finds an element and modifies element value so
         * that only the first letter of the element is
         * capitalized. Modified element value is appended
         * to the aContainerNames array. Returns KErrNotFound,
         * if date metadata is missing.
         *
         * @since S60 5.2
         * @param aContainerNames, list of containers
         * @param aElements, list of item elements
         * @param aElementType, element type
         */
        void ModifyAndAppendElementL(
                RPointerArray<HBufC8>& aContainerNames,
                const RUPnPElementsArray& aElements,
                const TDesC8& aElementType );

        /**
         * Appends container name to array.
         * Container name is read from resource.
         * @since S60 5.2
         * @param aArray, array of container names
         * @param TInt, resource text id
         */
        void AppendContainerNameToArrayL( RPointerArray<HBufC8>& aArray,
                                          TInt aResourceTextId );
    private: // Data members

        /**
         * Reference to the object array
         */
        CUpnpCdsLiteObjectArray& iCdsLiteObjectArray;

        /**
         * Resource file. Owned
         */
        RResourceFile            iResourceFile;

        /**
         * File server session. Owned
         */
        RFs                      iFs;
    };

#endif // C_UPNP_CONTAINER_RESOLVER_H

// End of file
