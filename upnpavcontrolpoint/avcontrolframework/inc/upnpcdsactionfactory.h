/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  ?Description
*
*/



#ifndef C_CUPNPCDSACTIONFACTORY_H
#define C_CUPNPCDSACTIONFACTORY_H


//  INCLUDES
#include <e32base.h>
#include "upnpavcontrolpointobserver.h"

// FORWARD DECLARATIONS

class CUpnpService;
class CUpnpAVControlPoint;
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @since Series 60 2.6
*/
class CUpnpCdsActionFactory : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CUpnpCdsActionFactory* NewL( 
            MUpnpAVControlPointObserver& aAVCPObserver,
            CUpnpAVControlPoint& aAVCP );
        
        /**
        * Destructor.
        */
        ~CUpnpCdsActionFactory();

    public: // New functions
        
        /**
        * This function creates an action for content directory browse function.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Destination of action.
        * @param aObjectId object to be browsed
        * @param aBrowseFlag item or children browse
        * @param aFilter 
        * @param aStartingIndex
        * @param aRequestedCount
        * @param aSortCriteria
        * @return errorcode KErrNone if no errors 
        */
        TInt CdsBrowseActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aObjectId,
            const TDesC8& aBrowseFlag,
            const TDesC8& aFilter, 
            TInt aStartingIndex,
            TInt aRequestedCount,
            const TDesC8& aSortCriteria);
        /**
        * This function creates an action for content directory Search function.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Destination of action.
        * @param aObjectId object to be browsed
        * @param aSearchCriteria search parameters
        * @param aFilter 
        * @param aStartingIndex
        * @param aRequestedCount
        * @param aSortCriteria
        * @return errorcode KErrNone if no errors 
        */
        TInt CdsSearchActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aObjectId,
            const TDesC8& aSearchCriteria,
            const TDesC8& aFilter, 
            TInt aStartingIndex,
            TInt aRequestedCount,
            const TDesC8& aSortCriteria);

        /**
        * Creates and places in CU-stack a GetSystemUpdateID action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @return Error code. KErrNone if no errors.
        */
        TInt CdsSystemUpdateIdActionLC(
            CUpnpAction*& aAction, 
            CUpnpDevice* aMediaServer);
        /**
        * Creates and places in CU-stack a GetSearchCapabilities action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @return Error code. KErrNone if no errors.
        */
        TInt CdsSearchCapabilitiesActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer);
        /**
        * Creates and places in CU-stack a GetSortCapabilities action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @return Error code. KErrNone if no errors.
        */
        TInt CdsSortCapabilitiesActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer);
        /**
        * Creates and places in CU-stack a CreateObject action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aId A contatiner id .
        * @param aElements new object as XML string.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsCreateObjectActionLC(
        CUpnpAction*& aAction,
        CUpnpDevice* aMediaServer,
        const TDesC8& aId, 
        const TDesC8& aElements);
        /**
        * Cretes and places in CU-stack a ImportResource action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aSourceUri source of import.
        * @param aDestinationUri destination for import.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsImportResourceActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aSourceUri, 
            const TDesC8& aDestinationUri);
        /**
        * Creates and places in CU-stack a ExportResource action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aSourceUri source of export.
        * @param aDestinationUri destination for export.
        * @return error code. KErrNone if no errors
        */
        TInt CdsExportResourceActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aSourceUri, 
            const TDesC8& aDestinationUri);
        /**
        * Creates and places in CU-stack a CreateRefernce action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aDestinationContainerId ID of container 
        *   in wher reference is created.
        * @param aSourceObjectId object to be refered.
        * @return error code. KErrNone if no errors
        */
        TInt CdsCreateReferenceActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aDestinationContainerId, 
            const TDesC8& aSourceObjectId);
        /**
        * Creates and places in CU-stack a DeleteResource action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aResourceUri URI of target resource.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsDeleteResourceActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aResourceUri);
        /**
        * Creates and places in CU-stack a DestroyObject action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aObjectId URI of target resource.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsDestroyObjectActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aObjectId);
        /**
        * Creates and places in CU-stack a StopTransfer action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aTransferId Id of target transfer.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsStopTransferActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            TInt aTransferId);
        /**
        * Creates and places in CU-stack a GetTransferProgress action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aTransferId Id of target transfer.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsTransferProgressActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            TInt aTransferId);
         /**
        * Creates and places in CU-stack a UpdateObject action.
        * @since Series 60 2.6
        * @param aAction CUpnpAction pointer for created action.
        * @param aMediaServer Action's target device.
        * @param aObjectID object to be updated.
        * @param aCurrentTagValue current metadata.
        * @param aNewTagValue new metadata.
        * @return error code. KErrNone if no errors.
        */
        TInt CdsUpdateObjectActionLC(
            CUpnpAction*& aAction,
            CUpnpDevice* aMediaServer,
            const TDesC8& aObjectID,
            const TDesC8& aCurrentTagValue,
            const TDesC8& aNewTagValue);

    private:

        /**
        * C++ default constructor.
        */
	    CUpnpCdsActionFactory( MUpnpAVControlPointObserver& aAVCPObserver,
	    						CUpnpAVControlPoint& aAVCP );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

    private:    // Data
        
        //observer that is intrested about AVCP actions
        MUpnpAVControlPointObserver& iAVControlPointObserver;	
        // AVCP main class
        CUpnpAVControlPoint& iAVCP;
    };

#endif      // C_CUPNPCDSACTIONFACTORY_H   
            
// End of File