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
* Description:  CUpnpPathElement
*
*/


// INCLUDE FILES
#include    "upnppathelement.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpPathElement::CUpnpPathElement
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpPathElement::CUpnpPathElement()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpPathElement::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpPathElement::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpPathElement::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpPathElement* CUpnpPathElement::NewL()
    {
    CUpnpPathElement* self = new( ELeave ) CUpnpPathElement;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CUpnpPathElement::~CUpnpPathElement
// Destructor
// -----------------------------------------------------------------------------
//    
CUpnpPathElement::~CUpnpPathElement()
    {
    delete iParentId;
    delete iId;
    delete iName;  
    delete iOriginalName;  
    delete iImportURI;
    }


// -----------------------------------------------------------------------------
// CUpnpPathElement::SetParentIdL
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::SetParentIdL(const TDesC8& aParentId) 
    {
    
        delete  iParentId;
        iParentId = NULL;  
	    
        iParentId = aParentId.AllocL();
        
    }

// -----------------------------------------------------------------------------
// CUpnpPathElement::SetIdL
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::SetIdL(const TDesC8& aId) 
    {

        delete  iId;
        iId = NULL;  

        iId = aId.AllocL();    
    }
    
// -----------------------------------------------------------------------------
// CUpnpPathElement::SetNameL
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::SetNameL(const TDesC8& aName) 
    {
        delete  iName;
        iName = NULL;  
        
        iName = aName.AllocL();  
    }

// -----------------------------------------------------------------------------
// CUpnpPathElement::SetImportURIL
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::SetImportURIL(const TDesC8& aImportURI) 
    {
        delete  iImportURI;
        iImportURI = NULL;  
        
        iImportURI = aImportURI.AllocL();  
    }
// -----------------------------------------------------------------------------
// CUpnpPathElement::SetOriginalNameL
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::SetOriginalNameL(const TDesC8& aOriginalName) 
    {
        delete  iOriginalName;
        iOriginalName = NULL;  
        
        iOriginalName = aOriginalName.AllocL();  
    }
// -----------------------------------------------------------------------------
// CUpnpPathElement::DeleteOriginalName
// -----------------------------------------------------------------------------
//    
void CUpnpPathElement::DeleteOriginalName() 
    {
        delete  iOriginalName;
        iOriginalName = NULL;          
    }    
// -----------------------------------------------------------------------------
// CUpnpPathElement::OriginalName
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpPathElement::OriginalName() const
    {
    if (iOriginalName) 
	    {        
	    return *iOriginalName;
	    }
    else 
	    {
        return KNullDesC8;
	    }  
    }    
// -----------------------------------------------------------------------------
// CUpnpPathElement::ParentId
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpPathElement::ParentId() const 
	{
    if (iParentId) 
	    {        
	    return *iParentId;
	    }
    else 
	    {
        return KNullDesC8;
	    }
   
	}

// -----------------------------------------------------------------------------
// CUpnpPathElement::Id
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpPathElement::Id() const
	{
    if (iId) 
		{       
		return *iId;
		}
    else 
		{
		return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CUpnpPathElement::Name
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpPathElement::Name() const
	{
    if (iName) 
		{  
		return *iName;
		}
    else 
		{
		return KNullDesC8;
		}
	}

// -----------------------------------------------------------------------------
// CUpnpPathElement::ImportURI
// -----------------------------------------------------------------------------
//    
const TDesC8& CUpnpPathElement::ImportURI() const
	{
    if (iImportURI) 
		{  
		return *iImportURI;
		}
    else 
		{
		return KNullDesC8;
		}
	}
// -----------------------------------------------------------------------------
// CUpnpPathElement::MatchName
// -----------------------------------------------------------------------------
//    
TBool CUpnpPathElement::MatchName(const CUpnpPathElement& aElement1, const CUpnpPathElement& aElement2) 
	{
    return   (aElement1.Name().Compare(aElement2.Name()) == 0) && 
             (aElement1.ParentId().Compare(aElement2.ParentId()) == 0); 
	}

// -----------------------------------------------------------------------------
// CUpnpPathElement::MatchAll
// -----------------------------------------------------------------------------
//    
TBool CUpnpPathElement::MatchAll(const CUpnpPathElement& aElement1, const CUpnpPathElement& aElement2) 
	{
    return   (aElement1.Name().Compare(aElement2.Name()) == 0) && 
             (aElement1.ParentId().Compare(aElement2.ParentId()) == 0) && 
             (aElement1.Id().Compare(aElement2.Id()) == 0); 
	}

//  End of File  
