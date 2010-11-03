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
* Description:      
*
*/

//  Include Files  

#include <f32file.h>
#include <mdeproperty.h>
#include <mdeinstanceitem.h>
#include <mdeitem.h>
#include <mdesession.h>
#include <fbs.h>
#include <thumbnaildata.h>
#include <ImageConversion.h>
#include "upnpthumbnailcreator.h"  // CUpnpThumbnailCreator
#include "upnpthumbnailcreatorObserver.h"
#include "upnpfileutilitytypes.h"

_LIT( KComponentLogfile, "upnpthumbnailcreator.txt");
_LIT(KSystemFilePath, "c:\\system\\temp\\");
#include "upnplog.h"

const TInt KSmallWidth = 640;
const TInt KSmallHeight = 480;
const TInt KMediumWidth = 1024;
const TInt KMediumHeight = 768;
const TInt KLargeWidth = 4096;
const TInt KLargeHeight = 4096;
const TInt KThumbnailWidth = 160;
const TInt KThumbnailHeight = 160;
const TInt KSmallIconWidth = 48;
const TInt KSmallIconHeight = 48;
const TInt KLargeIconWidth = 160;
const TInt KLargeIconHeight = 160;
//  Member Functions


// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::NewLC
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
EXPORT_C CUpnpThumbnailCreator* CUpnpThumbnailCreator::NewLC(
        MUpnpThumbnailCreatorObserver& aMUpnpThumbnailCreatorObserver,
        const TDesC& aFilePath,
        TThumbnailDlnaSize aSize)
    {
    CUpnpThumbnailCreator* self = new (ELeave) CUpnpThumbnailCreator(
            aMUpnpThumbnailCreatorObserver, aSize);
    CleanupStack::PushL(self);
    self->ConstructL(aFilePath);
    return self;
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::NewL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
EXPORT_C CUpnpThumbnailCreator* CUpnpThumbnailCreator::NewL(
        MUpnpThumbnailCreatorObserver& aMUpnpThumbnailCreatorObserver,
        const TDesC& aFilePath,
        TThumbnailDlnaSize aSize)
    {
    __LOG1( "CUpnpThumbnailCreator:NewL() aFilePath: &s", &aFilePath);    
    CUpnpThumbnailCreator* self = CUpnpThumbnailCreator::NewLC(
            aMUpnpThumbnailCreatorObserver,aFilePath, aSize);
    CleanupStack::Pop(self);
    return self;
    }



// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::MdeObjectId
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
EXPORT_C TInt CUpnpThumbnailCreator::MdeObjectId() const
    {
    return iObjectid;
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::ThumbnailFilePath
// See upnpthumbnailcreator.h // CI: RM return TDesC &
//---------------------------------------------------------------------------
EXPORT_C const TDesC& CUpnpThumbnailCreator::ThumbnailFilePath() const
    {
    __ASSERT( iThumbnailFilePath, __FILE__, __LINE__ );
    return *iThumbnailFilePath;
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::Thumbnail
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
EXPORT_C CFbsBitmap& CUpnpThumbnailCreator::Thumbnail() const
    {
    __ASSERT( iThumbnail, __FILE__, __LINE__ );
    return *iThumbnail;
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::~CUpnpThumbnailCreator
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
CUpnpThumbnailCreator::~CUpnpThumbnailCreator()
    {
    __LOG( "CUpnpThumbnailCreator:~CUpnpThumbnailCreator()");
    if (IsActive())
        {
        Cancel();
        }
    
    delete iImageFilePath;
    delete iMdEQuery;
    delete iMdESession;
    delete iThumbnail; 
    delete iThmbManager;
    delete iImageEncoder;
	
    if(iThumbnailFilePath != NULL)
        {
        iFsSession.Delete(iThumbnailFilePath->Des());
        delete iThumbnailFilePath;
        }
    iFsSession.Close();
    
    }
 

/********************************************************************
 * MMdESessionObserver  virtual methods
 ********************************************************************/ 

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::HandleSessionOpened
// See mdesession.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::HandleSessionOpened(CMdESession& /*aSession*/, 
                                                            TInt aError)
    {
    __LOG1( "CUpnpThumbnailCreator:HandleSessionOpened() %d", aError);
    CompleteRequest(aError);
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::HandleSessionError
// See mdesession.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::HandleSessionError(CMdESession& /*aSession*/, 
                                                            TInt aError)
    {
     __LOG1( "CUpnpThumbnailCreator:HandleSessionError() %d", aError);
     CompleteRequest(aError);
    }
/********************************************************************
 * MMdEQueryObserver virtual methods
 ********************************************************************/

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::HandleQueryNewResults
// See mdequery.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::HandleQueryNewResults(CMdEQuery& aQuery, 
                                                TInt aFirstNewItemIndex,
                                                TInt aNewItemCount)
    {
    CMdEObjectQuery& query = (CMdEObjectQuery &) aQuery;
    if (aNewItemCount > 0)
        {
        TInt i( aFirstNewItemIndex);
        for ( ; i < (aFirstNewItemIndex+aNewItemCount); i++)
            {
            CMdEObject* object = (CMdEObject*)query.TakeOwnershipOfResult(i);
            if(object->Uri().CompareF(iImageFilePath->Des()) == 0)
                {
                iObjectid = object->Id();
                CompleteRequest(KErrNone);
                break;
                }
            delete object;
            }
        if(i == (aFirstNewItemIndex + aNewItemCount))
            {
            CompleteRequest(KErrNotFound);
            }
        }
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::HandleQueryCompleted
// See mdequery.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::HandleQueryCompleted(CMdEQuery& /*aQuery*/, 
                                                                TInt aError)
    {
     __LOG1( "CUpnpThumbnailCreator:HandleQueryCompleted() %d",aError);
     if(aError != KErrNone)
         {
         CompleteRequest(aError);
         }     
    }

/********************************************************************
 * MThumbnailManagerObserver virtual methods
 ********************************************************************/

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::ThumbnailPreviewReady
// See thumbnailmanagerobserver.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::ThumbnailPreviewReady(MThumbnailData& /*aThumbnail*/,
                                                TThumbnailRequestId /*aId*/ )
    {    
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::ThumbnailReady
// See thumbnailmanagerobserver.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::ThumbnailReady(TInt aError, 
                                            MThumbnailData& aThumbnail, 
                                            TThumbnailRequestId /*aId*/ )
    {
    // This function must not leave.
     __LOG1( "CUpnpThumbnailCreator:ThumbnailReady() %d",aError);
    delete iThumbnail; iThumbnail = NULL;
    if ( aError == KErrNone )
        {
        // Claim ownership of the bitmap instance for later use
        iThumbnail = aThumbnail.DetachBitmap();
        }
    
    CompleteRequest(aError);
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::NotifyCompletion
// See upnpbitmapconverterobserver.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::ThumbnailReady(TInt aError)
    {
    if(iThumbnailFilePath)
        {
        TInt ret = iFsSession.SetAtt(iThumbnailFilePath->Des(),KEntryAttHidden,KEntryAttArchive); 
        __LOG1( "CUpnpThumbnailCreator:NotifyCompletion - thumbnail attribute hidden result %d",ret);
        }
    iMUpnpThumbnailCreatorObserver.ThumbnailCreatorReady(aError);    
    }


// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::CUpnpThumbnailCreator
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
CUpnpThumbnailCreator::CUpnpThumbnailCreator(
        MUpnpThumbnailCreatorObserver& aMUpnpThumbnailCreatorObserver,
        TThumbnailDlnaSize aSize)
        :CActive( CActive::EPriorityStandard ),
        iMUpnpThumbnailCreatorObserver(aMUpnpThumbnailCreatorObserver),
        iThumbnailDlnaSize (aSize)
        
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::ConstructL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------        
void CUpnpThumbnailCreator::ConstructL(const TDesC& aFilePath)
    {
    // second phase constructor, anything that may leave must be constructed here
    __LOG( "CUpnpThumbnailCreator:ConstructL() ");
    iState = EIdle;
    User::LeaveIfError(iFsSession.Connect());
    iImageFilePath = aFilePath.AllocL();
    
    ConvertDlnaSizeToImageSize();
    
    iThmbManager = CThumbnailManager::NewL( *this );
        
    // Set flags: Keep aspect ratio and allow smaller image than requested
    iThmbManager->SetFlagsL(static_cast<CThumbnailManager::TThumbnailFlags>(
    CThumbnailManager::EDefaultFlags | CThumbnailManager::EAllowAnySize )); 

    // Preferred size is 160x120 (or less)
    iThmbManager->SetThumbnailSizeL( ConvertDlnaSizeToImageSize() );
    
    iState = EInitializeMde;
    iMdESession = CMdESession::NewL(*this);
    
    iStatus = KRequestPending;
    SetActive();
    }

TSize CUpnpThumbnailCreator::ConvertDlnaSizeToImageSize()
    {
    TSize thumbnailSize;
    
    switch (iThumbnailDlnaSize)
        {
        case ESmall:
            {
            thumbnailSize = TSize (KSmallWidth, KSmallHeight);
            break;
            }
        case EMedium:
            {
            thumbnailSize = TSize (KMediumWidth, KMediumHeight);
            break;
            }
        case ELarge:
            {
            thumbnailSize = TSize (KLargeWidth, KLargeHeight);
            break;
            }
        case EThumbnail:
            {
            thumbnailSize = TSize (KThumbnailWidth, KThumbnailHeight);
            break;
            }
        case ESmallIcon:
            {
            thumbnailSize = TSize (KSmallIconWidth, KSmallIconHeight);
            break;
            }
        case ELargeIcon:
            {
            thumbnailSize = TSize (KLargeIconWidth, KLargeIconHeight);
            break;
            }
        default:
            {
            __LOG( "CUpnpThumbnailCreator::ConvertDlnaSizeToImageSize - Not Expected!" );
            __PANIC( __FILE__, __LINE__ );
            break;
            }
        }
    
    return thumbnailSize;
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::QueryMdeL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::QueryMdeL()
    {
     __LOG( "CUpnpThumbnailCreator:QueryMdeL() ");
    User::LeaveIfNull(iMdESession); //CI: LeaveIfNull() leaves with KErrNoMemory is this the idea? or can this even happen??
    
    CMdENamespaceDef& namespacedef = iMdESession->GetDefaultNamespaceDefL();
    CMdEObjectDef& objectdef = namespacedef.GetObjectDefL(
                                    MdeConstants::Image::KImageObject);
    iMdEQuery = iMdESession->NewObjectQueryL(namespacedef, objectdef, this);

    iMdEQuery->SetResultMode(EQueryResultModeItem);
    
    iMdEQuery->FindL();
    
    iStatus = KRequestPending;
    SetActive();
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::ParseImageMediaObjectL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------

void CUpnpThumbnailCreator::GetThumbnail()
    {
     __LOG( "CUpnpThumbnailCreator:GetThumbnail() ");
    // object id is available for the image,
    // thumbnail manager can give us the thumbnail.
    iThmbManager->GetThumbnailL(iObjectid);
    
    iStatus = KRequestPending;
    SetActive();
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::PrepareThumbnailFilePathL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::PrepareThumbnailFilePathL()
    {
     __LOG( "CUpnpThumbnailCreator:PrepareThumbnailFilePathL() ");
    TFileName path(KSystemFilePath);    
    //User::LeaveIfError(iFsSession.SessionPath(path));
    TInt ret = iFsSession.MkDir(path);
    //handle when creation of the path fails.
    if(ret == KErrNone || ret == KErrAlreadyExists )
        {
        TParse p;
        p.Set(iImageFilePath->Des(),NULL,NULL);
        TFileName filenameandext(p.NameAndExt());
        path.Append(filenameandext);
        iThumbnailFilePath = path.AllocL();
        }
    else
        {
        User::Leave(ret);
        }
    }

// --------------------------------------------------------------------------
// CUpnpThumbnailCreator::SaveThumbnailL
// See upnpthumbnailcreator.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::EncodeAndSaveThumbnailL()
    {
     __LOG( "CUpnpThumbnailCreator:SaveThumbnailL() ");
     PrepareThumbnailFilePathL();
     
     iImageEncoder = CImageEncoder::FileNewL(iFsSession,*iThumbnailFilePath,
             CImageEncoder::EOptionNone, KImageTypeJPGUid,KNullUid );

     iImageEncoder->Convert( &iStatus, *iThumbnail );
     SetActive();     
    }

void CUpnpThumbnailCreator::CompleteRequest(TInt aError)
    {
    TRequestStatus* stat = &iStatus;
    User::RequestComplete( stat, aError );
    }


// --------------------------------------------------------------------------
// CUpnpBitmapConverter::RunL
// See upnpbitmapconverter.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::RunL()
    {
    __LOG1( "CUpnpThumbnailCreator:RunL() iStatus=%d", iStatus.Int());
    
    if (iStatus.Int() < 0)
        {
        ThumbnailReady(iStatus.Int());
        }
    else
        {
        switch (iState)
            {
            case EIdle:
                {
                break;
                }
            case EInitializeMde:
                {
                iState = EQueryingImage;
                QueryMdeL();
                break;
                }
            case EQueryingImage:
                {
                iState = EFetchingThumbnail;
                GetThumbnail();
                break;
                }
            case EFetchingThumbnail:
                {
                iState = EConvertingThumbnail;
                EncodeAndSaveThumbnailL();
                break;
                }
            case EConvertingThumbnail:
                {
                ThumbnailReady(iStatus.Int());             
                break;
                }
            }
        }
    }

TInt CUpnpThumbnailCreator::RunError( TInt aError )
    {
    iState = EIdle;
    ThumbnailReady(aError);
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CUpnpBitmapConverter::DoCancel
// See upnpbitmapconverter.h
//---------------------------------------------------------------------------
void CUpnpThumbnailCreator::DoCancel()
    {
    __LOG( "CUpnpThumbnailCreator:DoCancel()");
    if (iState == EConvertingThumbnail)
        {
        iImageEncoder->Cancel();
        }
    else
        {
        TRequestStatus* stat = &iStatus;
        User::RequestComplete( stat, KErrCancel );        
        }
    }
