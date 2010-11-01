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
* Description:      Resolver for local items
*
*/




#ifndef UPNPLOCALITEMRESOLVER_H
#define UPNPLOCALITEMRESOLVER_H

//  INCLUDES
#include <e32base.h>
#include "upnpitemresolver.h" // base class
#include "upnpthumbnailcreator.h"

// FORWARD DECLARATIONS
class MUPnPAVController;
class CUpnpItem;
class MUPnPItemResolverObserver;
class MMPXCollectionHelper;
class CUpnpAVDevice;
class CUpnpTranscodeHelper;

// CLASS DECLARATION

/**
* Resolves remote upnp items from a plain item id.
* This resolving is done by executing a upnp metadata browse, or
* possibly several ones of those.
*
* @lib upnpavcontrollerhelper.lib
* @since S60 3.2
*/
class CUPnPLocalItemResolver
    : public CBase
    , public MUPnPItemResolver
    , public MUpnpThumbnailCreatorObserver
    {
public: // construction/destruction

    /**
     * static constructor
     *
     * @since Series 60 3.2
     * @param aFilePath, local item to be resolved
     * @param aAvController, AVController
     * @param aSelector, resource selector
     * @param aOptimisationFlags flags to optimise the algorithm
     * @return LocaltemResolver instance 
     */
    static CUPnPLocalItemResolver* NewL(
        const TDesC& aFilePath,
        MUPnPAVController& aAvController,
        MUPnPResourceSelector& aSelector,
        TInt aOptimisationFlags );

    /**
     * destructor
     * @since Series 60 3.2
     */
    virtual ~CUPnPLocalItemResolver();

private:

    /**
     * default constructor
     *
     * @since Series 60 3.2
     * @param aAvController, AVController
     * @param aSelector, resource selector
     */
    CUPnPLocalItemResolver(
        MUPnPAVController& aAvController,
        MUPnPResourceSelector& aSelector,
        TInt aOptimisationFlags );

    /**
     * 2nd phase constructor
     *
     * @param aFilePath, local item to be resolved
     */
    void ConstructL( const TDesC& aFilePath );


private: 

    /**
     * see UPnPItemResolver
     */
    void ResolveL(
        MUPnPItemResolverObserver& aObserver
        , CUpnpAVDevice* aDevice = NULL);

    /**
     * see UPnPItemResolver
     */
    const CUpnpItem& Item() const;


    /**
     * see UPnPItemResolver
     */
    const CUpnpElement& Resource() const;


private: // private methods

	/**
	 * shares upnpitem
	 */
    void ShareL();
    
    /**
     * Clean up all resources
     */
    void Cleanup();

    /**
     * checks if media "song" file has a album art 
	 *
     * @param aFileName filepath to media
     */
    void SetAlbumArtResourceToItemL( const TDesC& aFileName );
    
private:
    void ThumbnailCreatorReady( TInt aError);
    
    void AddAlbumArtAndShareL();
    
    void AddThumbnailandShareL();

private: // members

    // resource selector
    MUPnPResourceSelector& iSelector;

    // local file path (Owned).
    HBufC* iFilePath;

    // the first level browse result item (Owned).
    CUpnpItem* iSharedItem;

    // The selected resource within the last level item.
    const CUpnpElement* iResource;

    // optimisation flags
    TInt iOptimisationFlags;

    CUpnpThumbnailCreator* iThumbnailCreator;

    MUPnPItemResolverObserver* iObserver;
    
    CUpnpItem* iLocalItem;
    
	// owned
    MMPXCollectionHelper* iCollectionHelper;
	
    CUpnpTranscodeHelper* iTranscodeHelper;
    };


#endif  // UPNPLOCALITEMRESOLVER_H

// End of File
