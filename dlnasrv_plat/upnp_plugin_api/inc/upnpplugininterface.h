/** @file
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for ECom plugins
*
*/



#ifndef C_UPNPPLUGININTERFACE_H
#define C_UPNPPLUGININTERFACE_H

// INCLUDE FILES
#include <e32base.h>


// FORWARD DECLARATIONS
class CGulIcon;

// CONSTANT DEFINITIONS
const TUid interfaceUid = { 0x200075DD };



/**
 * This event is sent from plugins to the MUPnPPluginObserver
 *
 * @since S60 3.2
 */
enum TExtensionEvent
    {
    // the extension is enabled by default - no need to call explicitly
    EExtensionEnabled = 0x0001,
    // when called, disables the item in the list, cannot execute
    EExtensionDisabled = 0x0002,
    // will call GetIcon again, and redraw
    EExtensionIconChanged = 0x0010,
    // will call GetTitle again, and redraw
    EExtensionTitleChanged = 0x0020,
    // will call GetSubTitle again, and redraw
    EExtensionSecondaryTextChanged = 0x0040,
    // extension which was ExecuteL'd, has been closed.
    EExtensionClosed = 0x0100
    };


/**
 * Implemented by CUPnPPluginLoader
 *
 * @since S60 3.2
 */
class MUPnPPluginObserver
    {
public:

    /**
     * Called by plugins when it needs updating.
     *
     * @since S60 3.2
     * @param aEvent event type sent from plugin
     */
    virtual void ExtensionEvent(const TExtensionEvent& aEvent)=0;
    
    /**
     * Virtual destructor 
     *
     * @since S60 3.2
     */
     virtual ~MUPnPPluginObserver() {}

    };


/**
 * ECom interface class which all home connect plugins have to implement.
 *
 * IMPORTANT!!!
 * 
 * Following methdods and attribute have to be implemented in the implementing
 * class as well so that observer argument can be passed correctly.
 * Example is given from all methods.
 *
 * private:
 *    // attribute
 *    MUPnPPluginObserver* iPluginObserver;
 *    
 *    // constructor
 *    // constructor has to be inherited from the interface constructor
 *    // the following way
 *    CUPnPPluginImplementation(TAny* aEcomArguments)
 *         : CUPnPPluginInterface( aEcomArguments ) {}
 *
 *    // ConstructL
 *    // ConstructL has to set its argument to the attribute
 *    void ConstructL(MUPnPPluginObserver* aPluginObserver) 
 *      {
 *      iPluginObserver = aPluginObserver;
 *      }
 *
 * public:
 *    // NewL
 *    // This has to give its argument to the constructor and pass it to
 *    // ConstructL as well
 *    static CUPnPPluginImplementation* NewL(MUPnPPluginObserver* 
 *                                             aPluginObserver)
 *      {
 *      CUPnPPluginImplementation* self = new(ELeave) 
 *                       CUPnPPluginImplementation( aPluginObserver );
 *      CleanupStack::PushL( self );
 *      self->ConstructL( aPluginObserver );
 *      CleanupStack::Pop( self );
 *      return self;
 *      }
 *
 *
 */
class CUPnPPluginInterface : public CBase
    {
public:
        
    /**
     * Creates instance of ECom plugin implementation which corresponds
     * the parameter aUid.
     *
     * @since S60 3.2
     * @param aUid - Uid of the plugin implementation
     * @param aPluginObserver - Call back to CUPnPPluginLoader
     * @return Instance of CUPnPPluginInterface
     */
    static CUPnPPluginInterface* NewL(TUid aUid,
                                    MUPnPPluginObserver& aPluginObserver);

    /**
     * Informs to ECom that it has been destroyed.
     *
     * @since S60 3.2
     */
    virtual ~CUPnPPluginInterface();

    //-------------------------------------------------------------------
    // ECom interface methods
    //-------------------------------------------------------------------
    /**
     * Returns reference to the icon of the plugin
     *
     * @since S60 3.2
     * @return Reference to the icon of the plugin
     */
    virtual const CGulIcon& GetIcon()=0;

    /**
     * Returns const reference to the icon of the plugin
     *
     * @since S60 3.2
     * @return const reference to the title of the plugin
     */
    virtual const TDesC& GetTitle()=0;

    /**
     * Returns const reference to the scondary text of the plugin
     *
     * @since S60 3.2
     * @return const reference to the secondary text of the plugin
     */
    virtual const TDesC& GetSecondaryText()=0;

    /**
     * Executes the plugin
     *
     * @since S60 3.2
     */
    virtual void ExecuteL()=0;

    /**
     * Checks if the plugin needs upnp configuration (set IAP)
     *
     * @since S60 3.2
     * @return Information whether plugin needs upnp configuration
     */
    virtual TBool RequiresUpnpConfiguration()=0;

protected:
    /**
     * Constructor. Sets plugin observer.
     *
     * @since S60 3.2
     * @param aPluginObserver - reference to CUPnPPluginObserver
     */
    CUPnPPluginInterface(TAny* aEcomArguments);

private:

    /**
     * Uid of the plugin
     */
    TUid iDtor_ID_Key;

    /**
     * Pointer to the plugin observer which handles the events plugin sends
     *
     * Not owned.
     */
    MUPnPPluginObserver& iPluginObserver;
    };


// inline methods

// ----------------------------------------------------------------------------
// CUPnPPluginInterface::NewL
// Constructor
// ----------------------------------------------------------------------------
inline CUPnPPluginInterface* CUPnPPluginInterface::NewL(TUid aUid,
                                  MUPnPPluginObserver& aPluginObserver)
    {
    TAny* arg = reinterpret_cast<TAny*>(&aPluginObserver);
    TAny* interface = REComSession::CreateImplementationL(aUid,
                           _FOFF(CUPnPPluginInterface,iDtor_ID_Key),arg);
    CUPnPPluginInterface* retval =
                   REINTERPRET_CAST(CUPnPPluginInterface*,interface);
    return retval;
    }

// ----------------------------------------------------------------------------
// CUPnPPluginInterface::~CUPnPPluginInterface
// Destructor
// ----------------------------------------------------------------------------
inline CUPnPPluginInterface::~CUPnPPluginInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CUPnPPluginInterface::CUPnPPluginInterface
// Constructor
// ----------------------------------------------------------------------------
inline CUPnPPluginInterface::CUPnPPluginInterface(TAny* aEcomArguments)
    : iPluginObserver(*(reinterpret_cast<MUPnPPluginObserver*>
                                        (aEcomArguments)))
    {
    }


#endif // C_UPNPPLUGININTERFACE_H

// end of file
