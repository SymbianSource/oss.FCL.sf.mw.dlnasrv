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
* Description:      Base listbox class
*
*/






#ifndef HOMEMEDIABASELISTBOX_H_
#define HOMEMEDIABASELISTBOX_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include <aknlists.h>
#include <eikclbd.h>
#include <AknIconArray.h> 

/**
 * CHomeMediaBaseListbox
 * baseclass for lisboxes 
 */
class CHomeMediaBaseListbox : public CCoeControl
    {
public: // Constructors and destructor
    
    /**
    * Constructor.
    */
    CHomeMediaBaseListbox();

    /**
    * Destructor.
    */
    virtual ~CHomeMediaBaseListbox();

    /**
    * Returns reference to listbox
    * @return reference to CEikListBox
    */
    CEikListBox& ListBox();
    
    /**
    * Add's new column to listbox 
    * 
    * @param aFmt format of the lisbox ex. "%d\t%S\t\t",0,str
    */    
    void AddColumnL( TRefByValue<const TDesC> aFmt,... );

private:

    /**
    * From CoeControl, SizeChanged.
    * Called by framework when the view size is changed.
    */
    void SizeChanged();
    
    /**
    * Handles key events
    * 
    * @param  aKeyEvent
    * @param  aType
    * @return TKeyResponse
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent,
                                TEventCode aType);
    
    /**
    * From CoeControl,CountComponentControls.
    * 
    * @return Number of components in this control
    */
    TInt CountComponentControls() const;
    
    /**
    * From CCoeControl,ComponentControl.
    * @param  aIndex
    * @return pointer to control
    */
    CCoeControl* ComponentControl(TInt aIndex) const;

protected:

    /**
    * EPOC default constructor for performing 2nd stage construction
    * 
    * @param aRect rectangle for this control
    * @param aListBox baseclass for all s60 lisboxes, takes ownership
    */
    void BaseConstructL( const TRect& aRect 
                       , CEikTextListBox* aListBox );

    /**
     * Adds new icon to icon array
     * 
     * @param aArray
     * @param aSkin
     * @param aMbmFile
     * @param aID
     * @param aBitmapId
     * @param aMaskId
     */
    void AppendIconToArrayL(CAknIconArray* aArray,
                            MAknsSkinInstance* aSkin,
                            const TDesC& aMbmFile,
                            const TAknsItemID& aID,
                            TInt aBitmapId,
                            TInt aMaskId);
    
    /**
     * TOverFlowHandler
     * handles over flow in formatlist
     */
    class TOverFlowHandler : public TDes16Overflow 
        {
        /**
         * called when descriptor is overflowed 
         * 
         * @param reference to overflown data
         */
        void Overflow(TDes16&){}
        };

    /**
     * protected because derived classes may need to use iListBox ( owned )
     */
    CEikTextListBox* iListBox;
    
    /**
     * protected because derived classes may need to use 
     * iListBoxItems ( owned )
     */ 
    CDesCArray* iListBoxItems;    

    };

#endif /*HOMEMEDIABASELISTBOX_H_*/
