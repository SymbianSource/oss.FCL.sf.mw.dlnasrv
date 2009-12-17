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
* Description:  Header file for UPnP navi pane container
*
*/



#ifndef UPNPNAVIPANECONTAINER_H
#define UPNPNAVIPANECONTAINER_H


// INCLUDES
#include <coecntrl.h>
#include <AknsBasicBackgroundControlContext.h>

// FORWARD DECLARATIONS
class CUPnPPeriodic;

// CLASS DECLARATION

/**
*  CUPnPNaviPaneContainer
*  Container class for navi pane
*
*  @lib
*  @since 3.0
*/

enum TAnimationFrames
    {
    EFrame1,
    EFrame2,
    EFrame3,
    EFrame4,
    EFrame5,
    EFrame6,
    EFrame7,
    EFrame8,
    EFrame9
    };



class CUPnPNaviPaneContainer : public CCoeControl
    {    
    public:  // Constructors and destructor

        /**
        * Symbian constructor can leave
        */    
        IMPORT_C static CUPnPNaviPaneContainer* NewL(
                                    const TRect& aRect, 
                                    TPoint aPosition, 
                                    CAknNavigationControlContainer* aParent );

        /**
        * Destructor.
        */
        virtual ~CUPnPNaviPaneContainer();
     

    private: // Functions from base classes


        /**
        * From CoeControl
        * @return Number of contained component controls.
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * @param aIndex index of a contained component control.
        */
        
        CCoeControl* ComponentControl(TInt aIndex) const;

        /*
        * From CCoeControl
        * @param aRect drawable area.
        */
        void Draw(const TRect& aRect) const;
        
    private: // Constructors
    
        /**
        * C++ default constructor
        */
        CUPnPNaviPaneContainer();
    
        /**
        * Symbian second phase constructor
        */
        void ConstructL( const TRect& aRect,
                         TPoint aPosition,
                         CAknNavigationControlContainer* aParent = NULL );
        
        /**
        * sets postion and activates and deactivates window
        */       
        void DrawAnimation();
        
        /**
        * loads animation frame
        */
        static CGulIcon* LoadIconL( TAnimationFrames aAnimationState, 
                                    TAny* aDlg );
        
        /**
        * deletes the animation frame which enum is given as parameter
        */
        void DeleteIcon(TAnimationFrames aAnimationState);
          
        /**
        * updates animation, changes the animation frame to next 
        */                       
        static TBool UpdateAnimationL(TAny* aDlg);
        
     private: // data
                   
        CGulIcon*                            iIcon1;   // owned
        CGulIcon*                            iIcon2;   // owned
        CGulIcon*                            iIcon3;   // owned
        CGulIcon*                            iIcon4;   // owned
        CGulIcon*                            iIcon5;   // owned
        CGulIcon*                            iIcon6;   // owned
        CGulIcon*                            iIcon7;   // owned
        CGulIcon*                            iIcon8;   // owned
        CGulIcon*                            iIcon9;   // owned
        CGulIcon*                            iNavipaneIcon;    // owned
        CEikLabel*                           iLabel; //owned       
        CUPnPPeriodic*                       iPeriodic; //owned
        TInt                                 iAnimationIndex;
        TPoint                               iPosition;
        CAknsBasicBackgroundControlContext*  iSkinContext; // owned
        TBool                                iFirstDrawDelayWaited;
        
        TInt                        iResFileOffset;
        CEikonEnv*                  iCoeEnv; //not owned        
    };
#endif      // UPNPNAVIPANECONTAINER_H

// End of File
