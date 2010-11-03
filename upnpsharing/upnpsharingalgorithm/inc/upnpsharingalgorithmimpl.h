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
* Description:  Header file for the CUPnPSharingAlgorithm class.
*
*/

#ifndef C_UPNP_SHARING_ALGORITHM_IMPL_H
#define C_UPNP_SHARING_ALGORITHM_IMPL_H

//  INCLUDES
#include <upnpmediaserverclient.h>
#include "upnpsharingalgorithm.h"

// FORWARD DECLARATIONS
class CUpnpContainer;
class CUpnpItem;
class CUpnpSharingAO;
class CUpnpCdsLiteObjectArray;
class CUpnpCdsLiteObject;
class CUpnpContainerResolver;

// CLASS DECLARATION

/**
*  CUPnPSharingAlgorithm class definition
*
*  @lib upnpsharingalgorithm.lib
*  @since S60 5.2
*/
NONSHARABLE_CLASS( CUpnpSharingAlgorithmImpl )
                    : public CBase,
                      MUpnpSharingAlgorithm
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CUpnpSharingAlgorithmImpl* NewL();

        /**
         * Two-phased constructor.
         */
        static CUpnpSharingAlgorithmImpl* NewLC();

        /**
         * Destructor.
         */
        virtual ~CUpnpSharingAlgorithmImpl();

    public:

        /**
         * Check if item is shared based on item filename
         *
         * @since S60 5.2
         * @param aFileName, file to be checked
         * @return TBool, ETrue if item is shared, EFalse otherwise
         */
        IMPORT_C TBool IsFileSharedL( const TFileName &aFileName );

    public:

    // From base class MUpnpSharingAlgorithm

        /**
         * From MUpnpSharingAlgorithm
         * See base class definition
         */
        void ShareFileL( const TFileName &aFileName );

        /**
         * From MUpnpSharingAlgorithm
         * See base class definition
         */
        void UnshareFileL( const TFileName &aFileName );

        /**
         * From MUpnpSharingAlgorithm
         * See base class definition
         */
        CUpnpCdsLiteObjectArray& ReadSharedContent( );

        /**
        * From MUpnpSharingAlgorithm
        * See base class definition
        */
        void Close();

    private: // Private business logic methods

        /**
         * Checks if file exists
         * Leaves with KErrArgument if file not exist
         *
         * @since S60 5.2
         * @param aFileName, filename with path
         */
        void CheckFileExistenceL( const TFileName& aFileName );

        /**
         * Checks if there are empty containers in the tree and
         * unshares them
         * Leaves with symbian error codes in error cases
         *
         * @since S60 5.2
         * @param aParentId, parent container id where item locates
         * @return TBool, ETrue if container is unshared, EFalse otherwise
         */
        TBool UnshareEmptyContainersL( TDesC8& aParentId );

        /**
         * Shares containers based on item metadata
         * Leaves with Symbian error codes if error
         *
         * @since S60 5.2
         * @param aContainerArray, container structure for item
         */
        void ShareContainersL(
                    RPointerArray<CUpnpContainer>& aContainerArray );

        /**
         * Unshares objects and removes from cdsliteobjectarray
         * Leaves with Symbian error codes if error
         *
         * @since S60 5.2
         * @param aContainerArray, container structure
         */
        void RemoveSharedObjectsL(
                    RPointerArray<CUpnpContainer>& aContainerArray );

        /**
         * Shares reference items with containers
         * Leaves with Symbian error codes if error
         *
         * @since S60 5.2
         * @param aItem, item to be shared
         */
        void ShareReferenceObjectsL( CUpnpItem* aItem );

        /**
         * Shares reference item with container
         * Leaves with Symbian error codes if error
         *
         * @since S60 5.2
         * @param aContainerArray, reference container array
         * @param aOriginalObject, original item
         * @param aItem, reference item to be shared. Item gets data on
         *               sharing based on original item data
         */
        void ShareReferenceObjectL(
                            RPointerArray<CUpnpContainer>& aContainerArray,
                            CUpnpCdsLiteObject& aOriginalObject,
                            CUpnpItem& aItem );

        /**
         * Finds original object's reference objects and ushares them.
         * Leaves with Symbian error codes if error
         *
         * @since S60 5.2
         * @param aOriginalObject, original cds object
         */
        void UnshareReferenceObjectsL( CUpnpCdsLiteObject& aOriginalObject );

        /**
         * Starts server offline mode if not started yet
         *
         * @since S60 5.2
         */
        void StartServer();

        /**
         * Checks if file is protected
         * Leaves with KErrAccessDenied if file is protected
         *
         * @since S60 5.2
         * @param aFileName, filename with path
         */
        void CheckFileProtectionL( const TFileName& aFileName );
        
    private: // Second-phase contstruction

        /**
         * Default c++ constructor
         */
        CUpnpSharingAlgorithmImpl() {};

        /**
         * Second-phase constructor.
         */
        void ConstructL();

    private: // Data members

        /**
         * Asyncronous file sharing handler. Owned
         */
        CUpnpSharingAO*                         iSharingAO;

        /**
         * Shared content array. Owned.
         */
        CUpnpCdsLiteObjectArray*                iSharedContentArray;

        /**
         * Media server session handle. Owned
         */
        RUpnpMediaServerClient                  iMediaServer;

        /**
         * Container structure resolver. Owned
         */
        CUpnpContainerResolver*                 iContainerResolver;

    };

#endif // C_UPNP_SHARING_ALGORITHM_IMPL_H

// End of file
