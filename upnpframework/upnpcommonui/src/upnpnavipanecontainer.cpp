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
* Description:  Source file for UPnP navi pane container
*
*/



// INCLUDE FILES
// system
#include <AknUtils.h>
#include <upnpstring.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <aknnavilabel.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <aknnotewrappers.h> 
#include <layoutmetadata.cdl.h> 
#include <eiklabel.h>
#include <AknIconUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <data_caging_path_literals.hrh>
#include <layoutmetadata.cdl.h>
#include <AknBidiTextUtils.h>
#include <bautils.h>

// internal
#include "upnpnavipanecontainer.h"
#include <upnpcommonui.rsg>
#include <upnpcommonui.mbg>
#include "upnpperiodic.h"

_LIT( KComponentLogfile, "commonui.txt");
#include "upnplog.h"

_LIT( KAknCommonUIMbmFileName, "\\resource\\apps\\upnpcommonui.mbm" );

// CONSTANTS
const TInt KPeriodicTimerDelay = 1000000; // microseconds
const TInt KPeriodicTimerInterval = 200000; // microseconds
_LIT( KUPnPCommonUiRscFile, "\\resource\\upnpcommonui.rsc" );

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::CUPnPNaviPaneContainer
// --------------------------------------------------------------------------
//
CUPnPNaviPaneContainer::CUPnPNaviPaneContainer()
    {
    }
    
// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CUPnPNaviPaneContainer* CUPnPNaviPaneContainer::NewL( 
                                     const TRect& aRect,
                                     TPoint aPosition,
                                     CAknNavigationControlContainer* aParent )
    {
    CUPnPNaviPaneContainer* self = new ( ELeave ) CUPnPNaviPaneContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aPosition, aParent );
    CleanupStack::Pop();
    return self;
    }    

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::ConstructL
// --------------------------------------------------------------------------
//
void CUPnPNaviPaneContainer::ConstructL( 
                                 const TRect& aRect, 
                                 TPoint aPosition,
                                 CAknNavigationControlContainer* aParent )
    {
    iCoeEnv = CEikonEnv::Static();
    RFs& fileSession = iCoeEnv->FsSession();

    // Load common ui resource file
    TFileName rscFileName( KUPnPCommonUiRscFile );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    rscFileName.Insert( 0, drive );
    
    // Get the exact filename of the resource file
    BaflUtils::NearestLanguageFile( fileSession, rscFileName );
    // Check if the resource file exists or not
    if ( !BaflUtils::FileExists( fileSession, rscFileName ) )
        {
        User::Leave( KErrNotFound );
        }
    iResFileOffset = iCoeEnv->AddResourceFileL( rscFileName );
    
    
    // Create label    
    iFirstDrawDelayWaited = EFalse;

    iPosition.iX = aPosition.iX;
    iPosition.iY = aPosition.iY;
    
    TRect backgroundRect = aRect;
    backgroundRect.iTl.iX = -iPosition.iX;
       
    iSkinContext = CAknsBasicBackgroundControlContext::NewL(
       KAknsIIDQsnBgNavipaneSolid , backgroundRect, EFalse );
       
    SetPosition(iPosition);
    
    SetRect(aRect);
    CreateWindowL( aParent );
        
    iIcon1 = LoadIconL( EFrame1, this );
    iIcon2 = LoadIconL( EFrame2, this );
    iIcon3 = LoadIconL( EFrame3, this );
    iIcon4 = LoadIconL( EFrame4, this );
    iIcon5 = LoadIconL( EFrame5, this );
    iIcon6 = LoadIconL( EFrame6, this );
    iIcon7 = LoadIconL( EFrame7, this );
    iIcon8 = LoadIconL( EFrame8, this );
    iIcon9 = LoadIconL( EFrame9, this );
    
    AknIconUtils::SetSize( iIcon1->Bitmap(), 
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose
                          
    AknIconUtils::SetSize( iIcon2->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose
                          
    AknIconUtils::SetSize( iIcon3->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose
                          
    AknIconUtils::SetSize( iIcon4->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose

    AknIconUtils::SetSize( iIcon5->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose

    AknIconUtils::SetSize( iIcon6->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose

    AknIconUtils::SetSize( iIcon7->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose

    AknIconUtils::SetSize( iIcon8->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose

    AknIconUtils::SetSize( iIcon9->Bitmap(),
                          TSize(aRect.Height(),
                          aRect.Height() ) ); //width = height, on purpose
                          
    iNavipaneIcon = iIcon1; //set first image in animation
    
    iPeriodic = CUPnPPeriodic::NewL( CActive::EPriorityUserInput );
    iPeriodic->Start( KPeriodicTimerDelay,
                      KPeriodicTimerInterval,
                      TCallBack(UpdateAnimationL, this ) );
    
    }
    
// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::~CUPnPNaviPaneContainer
// --------------------------------------------------------------------------
//
CUPnPNaviPaneContainer::~CUPnPNaviPaneContainer()
    {
    if ( iResFileOffset )
        {
        iCoeEnv->DeleteResourceFile( iResFileOffset );
        iResFileOffset = 0;
        }
    
    if ( iPeriodic )
        {
        iPeriodic->Cancel();
        delete iPeriodic;
        }
    DeleteIcon( EFrame1 );
    DeleteIcon( EFrame2 );
    DeleteIcon( EFrame3 );
    DeleteIcon( EFrame4 );
    DeleteIcon( EFrame5 );
    DeleteIcon( EFrame6 );
    DeleteIcon( EFrame7 );
    DeleteIcon( EFrame8 );
    DeleteIcon( EFrame9 );
    delete iSkinContext;
    /*
    delete iLabel;
    */
    }
    
// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CUPnPNaviPaneContainer::CountComponentControls() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CUPnPNaviPaneContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }
    
// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::Draw
// --------------------------------------------------------------------------
//
void CUPnPNaviPaneContainer::Draw( const TRect& aRect ) const
    {
    // Do not draw custom navi pane in landscape orientation
    if (Layout_Meta_Data::IsLandscapeOrientation() == EFalse)
        {
        if (iFirstDrawDelayWaited)
            {        
            CWindowGc& gc = SystemGc();
            gc.Clear();
        
            MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        
            if (iSkinContext)
                {//Draw the skin background
                AknsDrawUtils::Background(
                    skin, iSkinContext, this, gc, aRect);
                }
            else
                {//  clear the area
                gc.SetBrushColor( iEikonEnv->ControlColor(
                                  EColorWindowBackground, *this ) );
                gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                gc.Clear( aRect );
                }
    
    
            const CFont* normalFont = iCoeEnv->AnnotationFont();
            TInt baseline = ((aRect.Height() / 2) +
                         (normalFont->AscentInPixels() / 2));
       
            TRgb color(0,0,0);

            TInt err = AknsUtils::GetCachedColor( skin, color, 
                                                  KAknsIIDQsnTextColors, 
                                                  EAknsCIQsnTextColorsCG2 );

   
            gc.UseFont(normalFont);
            if( KErrNone == err )
                {
                gc.SetPenColor( color );    
                }
            else
                {
                gc.SetPenColor( KRgbBlack );
                }
            gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    
            HBufC* naviPaneText = NULL;
            TRAP( err,  naviPaneText = StringLoader::LoadL(
                                  R_UPNPCOMMONUI_SERVER_SEARCH_MAIN ) );
            
            TSize animationFrameSize =
                  iNavipaneIcon->Bitmap()->SizeInPixels();
          
            const TInt textWidth = (aRect.Width() - 
                                    animationFrameSize.iWidth);
    
            TBuf< 128 > textBuffer;
            
            if( KErrNone == err )
                {
                if (AknBidiTextUtils::ConvertToVisualAndClip( *naviPaneText,
                                                           textBuffer,
                                                          *normalFont,
                                                           textWidth,
                                                           textWidth))
                    {
                    gc.DrawText(textBuffer, aRect, baseline,
                                CGraphicsContext::ELeft);    
                    }
                else //text not clipped, default case
                    {
                    gc.DrawText(textBuffer, aRect, baseline,
                                CGraphicsContext::ELeft);
                    }
                delete naviPaneText;
                }
            TPoint upperLeftCorner((aRect.Width() - 
                                   animationFrameSize.iWidth), 0);
                                   
            gc.BitBltMasked(upperLeftCorner, iNavipaneIcon->Bitmap(), Rect(),
                                         iNavipaneIcon->Mask(), EFalse);
            }
        }
    }

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::LoadIconL
// --------------------------------------------------------------------------
//


CGulIcon* CUPnPNaviPaneContainer::LoadIconL( TAnimationFrames aAnimationState,
                                             TAny* aDlg)
    {
    static_cast<CUPnPNaviPaneContainer*>( aDlg )->
                                            DeleteIcon( aAnimationState );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    TInt id = 0;
    TInt maskId = 0;
    TAknsItemID skinId = KAknsIIDNone;
    
    switch ( aAnimationState )
        {
        
        case EFrame1:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_1;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_1_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame2:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_2;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_2_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame3:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_3;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_3_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame4:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_4;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_4_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame5:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_5;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_5_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame6:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_6;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_6_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame7:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_7;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_7_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame8:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_8;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_8_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }
        case EFrame9:
            {
            id = EMbmUpnpcommonuiQgn_indi_upnp_search_9;
            maskId = EMbmUpnpcommonuiQgn_indi_upnp_search_9_mask;
            skinId = KAknsIIDQsnBgNavipaneSolid;
            break;
            }    
        default:
            {
            break;
            }
        }
    
    TFileName mbmFileName( KAknCommonUIMbmFileName );
    TFileName dllName;
    Dll::FileName( dllName );
    TBuf<2> drive = dllName.Left( 2 ); // Drive letter followed by ':' 
    mbmFileName.Insert( 0, drive );
    
    AknsUtils::CreateIconL( skin,
                             skinId,                             
                             bitmap,
                             mask,
                             mbmFileName,
                             id,
                             maskId );
                             
     CleanupStack::PushL(mask);
     CleanupStack::PushL(bitmap);                         
    
    
   // Get the color of the search text 
    TRgb color(0,0,0);
    TBool useWhite = EFalse;
    TInt err = AknsUtils::GetCachedColor( skin, color, 
                                    KAknsIIDQsnTextColors, 
                                    EAknsCIQsnTextColorsCG2 );


    
     // If text color is closer to white, use white animation
    if( KErrNone == err && 
        color.Difference(KRgbBlack) > color.Difference(KRgbWhite))
        {
        useWhite = ETrue;
        } 
    
    
    
    err = KErrNone;
    if( useWhite )
        {
        TInt imgSize = static_cast<CUPnPNaviPaneContainer*>( 
                                        aDlg )->Rect().Height();
        AknIconUtils::SetSize( bitmap,TSize(imgSize,imgSize) );
        TSize size = bitmap->SizeInPixels();
        
        CleanupStack::PopAndDestroy(bitmap);
        bitmap = new(ELeave) CFbsBitmap();
        CleanupStack::PushL(bitmap);
        err = bitmap->Create(size, EColor16M);
        }    
                            
        
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    icon->SetBitmapsOwnedExternally( EFalse );

    CleanupStack::Pop( bitmap ); // bitmap, mask
    CleanupStack::Pop( mask ); // bitmap, mask
    bitmap = NULL;
    mask = NULL;
    
    return icon;
    }

void CUPnPNaviPaneContainer::DeleteIcon( TAnimationFrames aAnimationState )
    {
    switch ( aAnimationState )
        {
        case EFrame1:
            {
            delete iIcon1;
            iIcon1 = NULL;
            break;
            }
        case EFrame2:
            {
            delete iIcon2;
            iIcon2 = NULL;
            break;
            }
        case EFrame3:
            {
            delete iIcon3;
            iIcon3 = NULL;
            break;
            }
        case EFrame4:
            {
            delete iIcon4;
            iIcon4 = NULL;
            break;
            }
        case EFrame5:
            {
            delete iIcon5;
            iIcon5 = NULL;
            break;
            }
        case EFrame6:
            {
            delete iIcon6;
            iIcon6 = NULL;
            break;
            }
        case EFrame7:
            {
            delete iIcon7;
            iIcon7 = NULL;
            break;
            }
        case EFrame8:
            {
            delete iIcon8;
            iIcon8 = NULL;
            break;
            }
        case EFrame9:
            {
            if (iIcon9)
                {
                delete iIcon9;
                iIcon9 = NULL;
                }
            break;
            }
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::UpdateAnimation
// --------------------------------------------------------------------------
//

TBool CUPnPNaviPaneContainer::UpdateAnimationL( TAny* aDlg )
    {       
    CUPnPNaviPaneContainer* dlg = 
                    static_cast<CUPnPNaviPaneContainer*>( aDlg );
    
    if ( !dlg->iFirstDrawDelayWaited )
        {
        dlg->iFirstDrawDelayWaited = ETrue;
        }
        
    switch ( dlg->iAnimationIndex++ % 9 )
        {
        case 0:
            {
            dlg->iNavipaneIcon = dlg->iIcon1;
            break;
            }
        case 1:
            {
            dlg->iNavipaneIcon = dlg->iIcon2;
            break;
            }
        case 2:
            {
            dlg->iNavipaneIcon = dlg->iIcon3;
            break;
            }
        case 3:
            {
            dlg->iNavipaneIcon = dlg->iIcon4;
            break;
            }
        case 4:
            {
            dlg->iNavipaneIcon = dlg->iIcon5;
            break;
            }
        case 5:
            {
            dlg->iNavipaneIcon = dlg->iIcon6;
            break;
            }
        case 6:
            {
            dlg->iNavipaneIcon = dlg->iIcon7;
            break;
            }
        case 7:
            {
            dlg->iNavipaneIcon = dlg->iIcon8;
            break;
            }
        case 8:
            {
            dlg->iNavipaneIcon = dlg->iIcon9;
            break;
            }
        default:
            {
            // error
            break;
            }
        }

    if (dlg->iAnimationIndex >= 9)
        {
        dlg->iAnimationIndex = 0;
        }   
        
    dlg->DrawAnimation();

    return ETrue;
    }

// --------------------------------------------------------------------------
// CUPnPNaviPaneContainer::DrawAnimation
// --------------------------------------------------------------------------
//

void CUPnPNaviPaneContainer::DrawAnimation()
    {
    TPoint imagePosition;
    imagePosition.iX = iPosition.iX;
    imagePosition.iY = iPosition.iY;
    CWindowGc& gc = SystemGc();
    RDrawableWindow* drawWindow = DrawableWindow();    
    gc.Activate( *drawWindow );    
    Draw( Rect() );
    gc.Deactivate();
    }


// End of file
