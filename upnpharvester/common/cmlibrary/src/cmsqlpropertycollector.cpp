/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating property container objects
*
*/






// INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include "cmsqlpropertycontainer.h"
#include "cmsqlpropertycollector.h"
#include "msdebug.h"

// CONSTANTS
const TInt KArrayGranularity = 16;

// ======== LOCAL FUNCTIONS ========
// --------------------------------------------------------------------------
// NewL
// --------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyCollector* CCmSqlPropertyCollector::NewL()
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::NewL"));
    
    CCmSqlPropertyCollector* self = CCmSqlPropertyCollector::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyCollector* CCmSqlPropertyCollector::NewLC()
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::NewLC"));
    
    CCmSqlPropertyCollector* self = new ( ELeave ) CCmSqlPropertyCollector();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyCollector::~CCmSqlPropertyCollector()
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::\
    ~CCmSqlPropertyCollector"));
    
    iPropertyContainers.ResetAndDestroy();
    iPropertyContainers.Close();
    }

// --------------------------------------------------------------------------
// CCmSqlPropertyCollector::AddPropertyContainerL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlPropertyCollector::AddPropertyContainerL(
    CCmSqlPropertyContainer* aContainer )
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::AddPropertyContainerL"));
    
    iPropertyContainers.AppendL( aContainer );
    TInt index = iPropertyContainers.Count() - 1;
    return index;
    }

// --------------------------------------------------------------------------
// CCmSqlPropertyCollector::DeletePropertyContainer
// --------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyCollector::DeletePropertyContainer( TInt aIndex )
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::\
    DeletePropertyContainer"));
    
    if( iPropertyContainers.Count() > aIndex )
        {
        delete iPropertyContainers[aIndex];
        iPropertyContainers.Remove(aIndex);
        iPropertyContainers.Compress();
        }
    }

// --------------------------------------------------------------------------
// CCCmSqlPropertyCollector::PropertyContainer
// --------------------------------------------------------------------------
//
EXPORT_C CCmSqlPropertyContainer*
                CCmSqlPropertyCollector::PropertyContainer( TInt aIndex )
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::PropertyContainer"));
    
    return iPropertyContainers[aIndex];
    }

// --------------------------------------------------------------------------
// CCmSqlPropertyCollector::PropertyContainerCount
// --------------------------------------------------------------------------
//
EXPORT_C TInt CCmSqlPropertyCollector::PropertyContainerCount() const
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::PropertyContainerCount"));
    
    return iPropertyContainers.Count();
    }

// --------------------------------------------------------------------------
// CCmSqlPropertyCollector::ExternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyCollector::ExternalizeL(
    RWriteStream& aStream ) const
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::ExternalizeL"));
    
    // Let's write the count of fill rules to stream first
    aStream.WriteInt16L( iPropertyContainers.Count() );
    for ( TInt index = 0; index < iPropertyContainers.Count(); index++ )
        {
        CCmSqlPropertyContainer* container = iPropertyContainers[index];
        // Then the object itself
        container->ExternalizeL( aStream );
        }
    }

// --------------------------------------------------------------------------
// CCmSqlPropertyCollector::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CCmSqlPropertyCollector::InternalizeL( RReadStream& aStream )
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::InternalizeL"));
    
    // Then internalize the objects
    if ( iPropertyContainers.Count() > KErrNone )
        {
        iPropertyContainers.ResetAndDestroy();
        }
    // First the count of fill rules
    TInt itemCount = aStream.ReadInt16L();

    // Then internalize them from the stream one by one
    for (TInt index = 0; index < itemCount; index++ )
        {
        CCmSqlPropertyContainer* newContainer =
                                CCmSqlPropertyContainer::NewL();
        CleanupStack::PushL( newContainer );
        newContainer->InternalizeL( aStream );
        AddPropertyContainerL( newContainer );
        CleanupStack::Pop( newContainer );
        newContainer = NULL;
        }
    }

// --------------------------------------------------------------------------
// Default constructor
// --------------------------------------------------------------------------
//
CCmSqlPropertyCollector::CCmSqlPropertyCollector() :
    iPropertyContainers( KArrayGranularity )
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::\
    CCmSqlPropertyCollector"));
    }

// --------------------------------------------------------------------------
// ConstructL
// --------------------------------------------------------------------------
//
void CCmSqlPropertyCollector::ConstructL()
    {
    LOG(_L("[CmLibrary]\t CCmSqlPropertyCollector::ConstructL"));
    }

// End of file

