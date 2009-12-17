/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Main application window
*
*/






#ifndef HOMEMEDIACONTAINER_H
#define HOMEMEDIACONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include "homemediabaselistbox.h"

//forward declaration
class CUPnPPluginInterface;
class CUPnPFileSharingEngine;

/**
 *  CHomeMediaContainer
 *  Ui control for appview, inherited from CHomeMediaBaseListbox
 *  Shows listbox on screen containing items: Browse Home , Sharing 
 *  , ++ possible plugin items
 */
class CHomeMediaContainer : public CHomeMediaBaseListbox
    {

public:

    // Constructors and destructor
    /**
     * Two-phased constructor.
     * @param aRect Control's rectangle
     * @param aFileSharing File sharing engine
     * @param aPluginArray Array of extention plugins
     * @return pointer to newly created CHomeMediaContainer
     */
    static CHomeMediaContainer* NewL(const TRect& aRect,
        CUPnPFileSharingEngine& aFileSharing,
        const RPointerArray<CUPnPPluginInterface>& aPluginArray );

    /**
     * Two-phased constructor.
     * @param aRect Control's rectangle
     * @param aFileSharing File sharing engine
     * @param aPluginArray Array of extention plugins
     * @return pointer to newly created CHomeMediaContainer
     */
    static CHomeMediaContainer* NewLC(const TRect& aRect,
        CUPnPFileSharingEngine& aFileSharing,
        const RPointerArray<CUPnPPluginInterface>& aPluginArray );

    /**
     * Destructor.
     */
    ~CHomeMediaContainer();

public:
    /**
     * Update container if any changes has happed.
     */
    void UpdateL();
    
private:

    /**
     * Constructor for performing 1st stage construction
     * @param aFileSharing File sharing engine
     * @param aPluginArray Array of extention plugins
     */
    CHomeMediaContainer( 
        CUPnPFileSharingEngine& aFileSharing,
        const RPointerArray<CUPnPPluginInterface>& aPluginArray );

    /**
     * EPOC default constructor for performing 2nd stage construction
     * 
     * @param aRect rectangle for this control
     */
    void ConstructL(const TRect& aRect);

    /**
     * Load plugins icons and texts, update listbox.
     */
    void UpdatePluginsL();
    
    /**
    * Get help context.
    */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     * File sharing engine ( Not owned )
     */
    CUPnPFileSharingEngine& iFileSharing;

    /**
     * Array of extention plugins ( Not owned )
     */
    const RPointerArray<CUPnPPluginInterface>& iPluginArray;

    };

#endif // HOMEMEDIACONTAINER_H
