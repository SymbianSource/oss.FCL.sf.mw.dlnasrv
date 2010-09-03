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
* Description:  Element Factory.
*
*/
 

// INCLUDE FILES
#include <sysutil.h>
#include <uri8.h>
#include <xml/dom/xmlengdomparser.h>

#include "upnpelementfactory.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpstring.h"
#include "upnpprotocolinfo.h"
#include "upnpcommonupnplits.h"
#include "upnpcdutils.h"
#include "upnpprotocolinfolocal.h"

using namespace UpnpDlnaProtocolInfo;

// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// DestroyRPointerArray
// Used by TCleanupItem to destroy array
// -----------------------------------------------------------------------------
//
void DestroyRArray(TAny* aArray)
{
    RArray<RXmlEngDocument>* array = (RArray<RXmlEngDocument>*) aArray;
    for(TInt i = 0; i < array->Count(); i++ )
            (*array)[i].Close();
    (*array).Close();
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpElementFactory::~CUpnpElementFactory()
// C++ default destructor. (virtual destructor)
// -----------------------------------------------------------------------------
//
CUpnpElementFactory::~CUpnpElementFactory()
{
    //Close XML document
    iDocument.Close();
    
    iDOMImpl.Close(); 
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::NewL()
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpElementFactory* CUpnpElementFactory::NewL( const TDesC& aObjectsXmlPath )
{
    CUpnpElementFactory* self = CUpnpElementFactory::NewLC( aObjectsXmlPath );
    CleanupStack::Pop( self );
    return self;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::NewLC()
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpElementFactory* CUpnpElementFactory::NewLC( const TDesC& aObjectsXmlPath )
{
    CUpnpElementFactory* self = new (ELeave) CUpnpElementFactory();
    CleanupStack::PushL( self );
    self->ConstructL( aObjectsXmlPath );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ConstructL()
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ConstructL(const TDesC& aObjectsXmlPath)
{

    iDOMImpl.OpenL();

    RXmlEngDOMParser parser;
    User::LeaveIfError( parser.Open(iDOMImpl) );
    CleanupClosePushL(parser);
    
    if ( aObjectsXmlPath == KNullDesC() )
        {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);

        TFileName path;
        User::LeaveIfError(fs.PrivatePath(path));

        TParse fp;
        fp.Set(KObjectsXmlFileName(),&path, 0);
        path = fp.FullName();

        iDocument = parser.ParseFileL( path );        
        CleanupStack::PopAndDestroy(&fs);
        }
    else
        {
        iDocument = parser.ParseFileL( aObjectsXmlPath );                
        }
        
    

    CleanupStack::PopAndDestroy(&parser);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::CUpnpElementFactory()
// Default constructor
// -----------------------------------------------------------------------------
//
CUpnpElementFactory::CUpnpElementFactory()
{

}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateNewObjectL()
// Function to validate a new object given by Control Point, especially 
// upnp:class element field eg. object.item.musictrack
// IMPORTANT: If objects's description is invalid due to missing elements
// it tries to repair it by adding those missing elements.
// -----------------------------------------------------------------------------
//

TUpnpErrorCode CUpnpElementFactory::ValidateNewObjectL( const TXmlEngElement& aNewElement, TBool aNew, TBool aLocalSharing)
{
    /*
    TInt i;    
    TPtrC8 objectType = KItem();

    RXmlEngNodeList<TXmlEngElement> elements;
    CleanupClosePushL(elements);
    aNewElement.GetChildElements( elements );
    
    // invalid element if the count of the objects is less than 1 (=0)
    if ( elements.Count() != 1 )
    {
        User::Leave( EInvalidArgs );
    }
    CleanupStack::PopAndDestroy(&elements);
    // try to find the new item from the xml
    TXmlEngElement object;
    UpnpDomInterface::GetElementL( aNewElement, object, KItem() );
    if ( object.IsNull() ) 
    {   
        
        // if it's not an item, it's a container
        UpnpDomInterface::GetElementL( aNewElement, object, KContainer() );
        if ( object.IsNull() )
        {
            // not even a container, leave!
            User::Leave ( EInvalidArgs );
        }
        objectType.Set( KContainer() );
    }
                           
    // restriced value
    TPtrC8 restrVal = UpnpDomInterface::GetAttrValueL( object,  KRestricted );
    if ( 
    (UpnpCD::Kfalse().Compare( restrVal ) != 0) &&
    (UpnpCD::KZero().Compare( restrVal ) != 0) &&
    (UpnpCD::Ktrue().Compare( restrVal ) != 0) &&
    (UpnpCD::KOne().Compare( restrVal ) != 0)
    )
    {
        User::Leave( EBadMetadata );
    }
    
    // for convenience later, take now a reference to the children of the object
    RXmlEngNodeList<TXmlEngElement> children;
    CleanupClosePushL(children);
    object.GetChildElements( children );
            
    // an exception: we can't have <res importUri=".. element in 
    // a new object (set by control point)
    while ( children.HasNext() )
    {
        TXmlEngElement child = children.Next();
        
        if ( IsNotEmptyImportUriL( child ) && aNew ) 
        {
            User::Leave ( EBadMetadata );
        }
        
        //---- check res@duration, 7.3.22 MM DIDL-Lite res@duration Format        
        CheckDurationOfResElementL(child);
        CheckSizeOfResElementL(child);
                    
        RXmlEngNodeList<TXmlEngElement> forbiddenChildren;
        CleanupClosePushL(forbiddenChildren);
        child.GetChildElements( forbiddenChildren );

        if ( forbiddenChildren.HasNext() 
            && child.Name() != KVendorDescriptor ) // ignore <desc></desc>
        {
            User::Leave ( EBadMetadata );
        }
        CleanupStack::PopAndDestroy(&forbiddenChildren);
        if ( child.Name() == KDate8() )
        {
            if (!UpnpCdUtils::ValidateDateL(child.Value())) 
             child.Remove();
        }
        
    }

    // if survived here, we have the new element of type item or container
    // let's analyze its type

    // let's inspect each of types
    // at first, collect object type descriptions from objects xml    
    // to a pointer array
    RArray<TXmlEngElement> objectDescrs;
    CleanupClosePushL( objectDescrs );
    // store class descriptor elements in this array
    ClassesL( object, objectDescrs, objectType );
    
    // validate each separately
    
    ValidatePropertiesL(object, objectDescrs);
    if(aNew)
    {
        ValidateBigImageL(object);
    }
    
    // for each type, check that 
    // this new object has all the required fields
        
    for (i=0; i<objectDescrs.Count(); i++)
    {
        // now we have to go through all the fields of the new item
        // first, for convenience take a pointer to <ps>(properties) element
        
        RXmlEngNodeList<TXmlEngElement> propElems;
        CleanupClosePushL(propElems);
        objectDescrs[i].GetChildElements( propElems );
    

        while ( propElems.HasNext() )
        {
            
            TXmlEngElement ps = propElems.Next(); // ps stands for properties
            if ( ps.Name() == KProperties() )
            {

                // take the properties
                RXmlEngNodeList<TXmlEngElement> properties;
                CleanupClosePushL(properties);
                ps.GetChildElements( properties );
        
                const TDesC8& propertyType = UpnpDomInterface::GetAttrValueL( ps, KType() );

                TBool elementFound;
                

                // check each property
                while ( properties.HasNext() )
                {
                    
                    TXmlEngElement el = properties.Next();

                    // Validate element
                    if ( propertyType == KElement() )
                    {
                        elementFound = EFalse;
                        
                        // Check if this element is requiered
                        const TDesC8& required = UpnpDomInterface::GetAttrValueL( el, KRequiredObject() );

                        TPtrC8 elementName = UpnpDomInterface::GetAttrValueL( el, KName() );

                        // If it's required then check it's existence

                        if ( required == UpnpCD::KOne() || elementName == KRes()  ||  elementName == KAlbumArtURI() )
                        {
                        if( !KRes().Compare(elementName) ||  !KAlbumArtURI().Compare(elementName))
                        {
                            elementFound = ETrue;
                        }
                            // get children once more, because this kind of list does not have any reset function
                            object.GetChildElements( children );
                            while ( children.HasNext() )
                            {
                                
                                TXmlEngElement child = children.Next();
                                
                                HBufC8* nameWithNs = NameWithNsLC( child );

                                if ( *nameWithNs == elementName )
                                {
                                    // local sharing
                                    if(aNew && *nameWithNs == KRes)
                                    {
                                        TUriParser8 up;
                                        TPtrC8 rv(child.Text());
                                        if(rv.Length())
                                        {
                                            User::LeaveIfError( up.Parse(child.Text()) );
                                            TPtrC8 path( up.Extract(EUriPath) );
                                            TPtrC8 scheme( up.Extract(EUriScheme) );

                                            if(aLocalSharing)
                                            { // local action
                                                if( scheme == UpnpHTTP::KSchemeFile8())
                                                {
                                                    // The path can be: /c:/....
                                                    // or /c/... - without colon.
                                                    // Both situation are correct but 
                                                    // in further operations we assume 
                                                    // there is not any colon next to the drive letter.
                                                    // Therefore, remove it if second element of path table equals ':'
                                                    if(path[2] == KColon8()[0])
                                                    { // There IS a colon next to the drive letter.
                                                        // Here is an example uri:
                                                        // file:///c:/...
                                                        // As you can see the second colon must be removed
                                                        HBufC8* uri = child.Text().AllocLC();
                                                        TPtr8 uriPtr(uri->Des());
                                                        TPtrC8 tmp(uri->Des());
                                                        TInt second = 2;
                                                        TInt colonPos = 0;
                                                        for(TInt i = 0; i < second; i++)
                                                        {
                                                            colonPos += tmp.Find(KColon8) + 1;
                                                            tmp.Set( uriPtr.Mid(colonPos) );                                                    
                                                        }
                                                        // remove the colon
                                                        uriPtr.Replace(colonPos - 1, KColon8().Length(), KNullString8);
                                                        // set TXmlEngElement value
                                                        child.SetTextL(uriPtr);
                                                        
                                                        // clean up             
                                                        CleanupStack::PopAndDestroy(uri);                                               
                                                    }
                        
                                                    // check other  restrictions
                                                    TUriParser8 up;
                                                    User::LeaveIfError( up.Parse(child.Text()) );
                                                    TPtrC8 path( up.Extract(EUriPath) );

                                                    // sharing from Z: drive is forbidden
                                                    // second character is a drive letter
                                                    if(path[1] == KForbiddenDrivez()[0] || path[1] == KForbiddenDriveZ()[0])
                                                    {
                                                        User::Leave(EArgumentValue);
                                                    }
                                                    
                                                    // cannot share from private directory
                                                    if(!path.Match(KForbiddenPrivatePattern))
                                                    {
                                                        User::Leave(EArgumentValue);
                                                    }
                                                    
                                                }
                                            }
                                            else
                                            { // not local action
                                                // "file" schema is forbidden here
                                                if(scheme == UpnpHTTP::KSchemeFile8())
                                                {
                                                    User::Leave(EArgumentValue);
                                                }
                                            }
                                        }
                                    }


                                    if ( elementFound ) 
                                    {
                                        // multiple values for one element!
                                        const TDesC8& multiple = UpnpDomInterface::GetAttrValueL( el, KMultiple() );
                                        
                                        // if it's not allowed for this element, leave!
                                        if ( !multiple.Length() )
                                        {
                                            User::Leave( EInvalidArgs );
                                        }
                                    }
                                    elementFound = ETrue;
                                    // mark the element required - if not res
                                    if( KRes().Compare(elementName) && KAlbumArtURI().Compare(elementName))
                                    {
                                        child.AddNewAttributeL(KRequiredAtrName,KTrueValue8);
                                    }
                                    
                                    // nested validation 
                                    RXmlEngNodeList<TXmlEngElement> nestEls;
                                    CleanupClosePushL(nestEls);
                                    el.GetChildElements(nestEls);
                                    while(nestEls.HasNext())
                                    {
                                        TXmlEngElement nestPs = nestEls.Next();

                                        // take the properties
                                        RXmlEngNodeList<TXmlEngElement> nestProperties;
                                        nestPs.GetChildElements( nestProperties );
                                        const TDesC8& nestPropertyType = UpnpDomInterface::GetAttrValueL( nestPs, KType() );
                                        // Validate attribute   
                                        if ( nestPropertyType == KAttribute() )
                                        {
                                            while(nestProperties.HasNext())
                                            {   
                                                TXmlEngElement nestEl = nestProperties.Next();
                                                //TBool nestElementFound;

                                                //nestElementFound = EFalse;
                                                
                                                const TDesC8& nestRequired = UpnpDomInterface::GetAttrValueL( nestEl, KRequiredObject() );
                                                TPtrC8 nestElementName = UpnpDomInterface::GetAttrValueL( nestEl, KName() );
                                                if ( nestRequired == UpnpCD::KOne() || nestElementName == KDlnaProfileID)
                                                {
                                                    TPtrC8 nestCurrentValue = UpnpDomInterface::GetAttrValueL( child, nestElementName );
                                                    
                                                    TPtrC8 nameOfAttr;
                                                    TPtrC8 valOfAttr;
                                                    nameOfAttr.Set( nestElementName );
                                                    
                                                    // Start of 'dlna:profileID' attribute case                                                 
                                                    if( nameWithNs->Des() == KAlbumArtURI && nestElementName == KDlnaProfileID) // ---------------- 1 ------------
                                                    {   
                                                        TXmlEngAttr profId = child.AttributeNodeL(KProfileID, KXmlnsDlna);
                                                        if(profId.NotNull())
                                                        {
                                                            if (profId.Value().Compare(KDefaultProfileID))
                                                            {
                                                                User::Leave( EBadMetadata );
                                                            }
                                                            // setting real name of attribute -> localName
                                                            nestElementName.Set(KProfileID); // descriptor  
                                                            nameOfAttr.Set( nestElementName ); // related TString
                                                                                                                                                                                                                                                                                            
                                                            // generating a new value of 'dlna:profileID'
                                                            HBufC8* albumArtURIelemValue = UpnpDomInterface::GetElementValueL(child).AllocLC();
                                                            TPtr8 albumArtURIelemValuePtr( albumArtURIelemValue->Des() );                                                       
                                                            
                                                            albumArtURIelemValuePtr.Trim(); // deletes leading and trailing whitespace characters                                                   
                                                            child.SetValueL(albumArtURIelemValuePtr); // sets new trimmed value to albumArtURI
                                                                                                                                                        
                                                            CUpnpDlnaProtocolInfo* tempProtocolInfo = NULL;
                                                            TInt error = iContentDirectory->GetProtocolInfoL( albumArtURIelemValuePtr, tempProtocolInfo );
                                                            TPtrC8 tempPnParam;
                                                            if( error >=0 )
                                                            {
                                                                tempPnParam.Set( tempProtocolInfo->PnParameter() );
                                                                    
                                                                nestCurrentValue.Set(tempPnParam); // descriptor
                                                                valOfAttr.Set(tempPnParam); // related TString                                                         
                                                            }
                                                            else 
                                                            {
                                                                User::Leave( EBadMetadata );
                                                            }                                                       
                                                            CleanupStack::PopAndDestroy(albumArtURIelemValue);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
                                                            // if albumArtURI doesn't contain profileID -> it creates one and also related namespace
                                                            // if albumArtURI contains profileID -> it modifies profileID's value
                                                            child.SetAttributeL( nameOfAttr, valOfAttr, KXmlnsDlna(), KDlnaPrefix() );
                                                                                                                    
                                                            delete tempProtocolInfo;
                                                            tempProtocolInfo = NULL;    
                                                        }
                                                        
                                                    } // End of 'dlna:profileID' attribute case
                                                    else                    
                                                    {
                                                        if ( !nestCurrentValue.Length() )  
                                                        {
                                                            
                                                            if(nestElementName == KprotocolInfo)
                                                            {
                                                                if( ! aLocalSharing )
                                                                {
                                                                    valOfAttr.Set( KEmptyProtocolInfoVal8() );
                                                                }
                                                                else 
                                                                {
                                                                    User::Leave( EBadMetadata );
                                                                }
                                                            }                                                                                                   //------- 2 -----                                                                                    //------- 2 -----                  
                                                            else
                                                            {
                                                                valOfAttr.Set( KNullDesC8() );
                                                            }
                                
                                                            child.AddNewAttributeL( nameOfAttr, valOfAttr );                                                            
                                                        }
                                                        else 
                                                        {
                                                            if(nestElementName == KprotocolInfo) 
                                                            {
                                                                HBufC8* protInfoValue = NULL;
                                                                TRAPD(err, protInfoValue = ValidateProtocolInfoInResL( nestCurrentValue, aLocalSharing ));
                                                                if(err)
                                                                {
                                                                    User::Leave(EBadMetadata);
                                                                }
                                                                CleanupStack::PushL( protInfoValue );
                                                                valOfAttr.Set( *protInfoValue );
                                                                child.SetAttributeL( nameOfAttr, valOfAttr );
                                                                CleanupStack::PopAndDestroy( protInfoValue );                                                           
                                                            }                                                                                                 //------- 3 -----                                                                                         //------- 3 -----
                                                        }
                                                    }                                                                                                                                                                                                                                                                                                                                                                                       
                                                                                                        

                                                    // if not main tag mark the attr is required
                                                    if( !IsMainObjectTagL(child) && nestElementName != KDlnaProfileID)
                                                    {
                                                       
                                                        HBufC8* attrReq = HBufC8::NewLC(
                                                            nestElementName.Length()
                                                            +KRequiredAtrSuf().Length() );
                                                        TPtr8 attrReqPtr(attrReq->Des());
                                                        attrReqPtr = nestElementName;
                                                        attrReqPtr.Append(KRequiredAtrSuf);
                                                        child.AddNewAttributeL( attrReqPtr, KTrueValue8 );
                                                        CleanupStack::PopAndDestroy(attrReq);
                                                    }
                                                }
                                            }
                                        }
                                    
                                    }
                                    CleanupStack::PopAndDestroy(&nestEls);
                                }
                                CleanupStack::PopAndDestroy( nameWithNs );
                                
                            }
                                
                            // If not found, add it
                            if ( !elementFound )
                            {
                                User::Leave(EBadMetadata);
                            }
                        } 
                    }
                        
                    // Validate attribute   
                    if ( propertyType == KAttribute() )
                    {
                        elementFound = EFalse;
                        
                        const TDesC8& required = UpnpDomInterface::GetAttrValueL( el, KRequiredObject() );
                        if ( required == UpnpCD::KOne() )
                        {

                            const TDesC8& elementName = UpnpDomInterface::GetAttrValueL( el, KName() );

                            const TDesC8& currentValue = UpnpDomInterface::GetAttrValueL( object, elementName );
                            
                            if ( !currentValue.Length() )  
                            {
                                TXmlEngAttr attr = object.AttributeNodeL( elementName );

                                if(attr.NotNull())
                                {
                                    attr.SetValueL(KNullDesC8);
                                }
                                else
                                {
                                    object.AddNewAttributeL( elementName, KNullDesC8 );
                                }                               
                            }
                            // if not main tag mark the attr is required
                            if(!IsMainObjectTagL(object))
                            {
                                HBufC8* attrReq = HBufC8::NewLC(
                                    elementName.Length()+KRequiredAtrSuf().Length() );
                                TPtr8 attrReqPtr(attrReq->Des());
                                attrReqPtr = elementName;
                                attrReqPtr.Append(KRequiredAtrSuf);
                                object.AddNewAttributeL( attrReqPtr, KTrueValue8 );
                                CleanupStack::PopAndDestroy(attrReq);
                            }
                        }
                    }
                }
                CleanupStack::PopAndDestroy(&properties);
            }
        }
        CleanupStack::PopAndDestroy(&propElems);
    }
    CleanupStack::PopAndDestroy(); //objectDescrs.Close();
    CleanupStack::PopAndDestroy(&children);
    
    // now the object 
    // * has the all required fields (if some were missing, they are added)
    // * had a proper xml structure (_not_ using UpnpDomInterface:: functions that do not care)
    // * has for sure a mostly proper object structure 
*/
    return EUndefined;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidatePropertiesL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidatePropertiesL( TXmlEngElement aObj,
                                               RArray<TXmlEngElement>& aClassList)
{
    // validate main object's attributes
    ValidateMainAttributesL(aObj, aClassList);
    
    // validate elements
    RXmlEngNodeList<TXmlEngElement> elements;
    CleanupClosePushL(elements);
    aObj.GetChildElements (elements);
    
    // for each element
    while(elements.HasNext())
    {
        TXmlEngElement el = elements.Next();
        ValidateElementL(el, aClassList);
    }
    CleanupStack::PopAndDestroy(&elements);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateBigImageL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateBigImageL( TXmlEngElement aObj)
{
    RArray<TXmlEngElement> elms;
    CleanupClosePushL(elms);
    
    if( UpnpDomInterface::GetElementListL(aObj, elms, KRes) )
    {
        for(TInt i = 0; i < elms.Count(); i++)
        {
                
            TXmlEngAttr prInfAttr = elms[i].AttributeNodeL(KprotocolInfo());
            if(prInfAttr.NotNull())
            {
                CUpnpProtocolInfoLocal* protInf = CUpnpProtocolInfoLocal::NewL(
                        UpnpDomInterface::GetAttrValueL(elms[i], KprotocolInfo) );
                CleanupStack::PushL(protInf);
                                                                                   
                /* Only support for DLNA pn-params:
                *  1) JPEG_SM
                *  2) MP3 
                *  3) AAC_ISO_320
                *  4) AVC_MP4_BL_CIF15_AAC_520
                */
                if( protInf->PnParameter() != KDLNA_PN_JPEG_SM &&
                    protInf->PnParameter() != KDLNA_PN_MP3 &&
                    protInf->PnParameter() != KDLNA_PN_AAC_ISO_320 &&
                    protInf->PnParameter() != KDLNA_PN_AVC_MP4_BL_CIF15_AAC_520                     
                  )
                {           
                    // set 4th parameter to "*"
                    protInf->SetFourthFieldL(KAsterisk8);
                    TPtrC8 prInfo = protInf->ProtocolInfoL();
                    HBufC8* prInfTmp = prInfo.Alloc();
                    CleanupStack::PushL(prInfTmp);
                    prInfAttr.SetValueL( *prInfTmp );
                    CleanupStack::PopAndDestroy(prInfTmp);
                }
                // clean up
                CleanupStack::PopAndDestroy(protInf);
            }
        }
    }
    
    // clean up
    CleanupStack::PopAndDestroy(&elms);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateElementL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateElementL( TXmlEngElement aElement,
                                            RArray<TXmlEngElement>& aClassList)
{
    if(aElement.Name() == KRes)
    {
        ValidateResElL(aElement, aClassList);
    }
    else
    {
        //  get pattern 
        HBufC8* name = UpnpCdUtils::GetElmNameWithNsL(aElement);
        CleanupStack::PushL(name);
        TXmlEngElement pattern = GetPatternL(*name, aClassList, KElement);
        if(pattern.NotNull())
        {
            ValidateElWithPatternL(aElement, pattern); // leaves on error
        }
        else
        { // attr is not supported - remove
            aElement.Remove();
        }
        // clean up
        CleanupStack::PopAndDestroy(name);
    }
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateMainAttributesL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateMainAttributesL( TXmlEngElement aObj, 
                                                   RArray<TXmlEngElement>& aClassList)
{
    // list of attributes
    RXmlEngNodeList<TXmlEngAttr> attrList;
    CleanupClosePushL(attrList);
    aObj.GetAttributes(attrList);
    
    // for each attr
    while(attrList.HasNext())
    {
        // get attr
        TXmlEngAttr attr = attrList.Next();
        
        //  get pattern 
        TXmlEngElement pattern = GetPatternL(attr.Name(), aClassList, KAttribute);
        if(pattern.NotNull())
        {
            ValidateAttrWithPatternL(attr, pattern); // leaves on error
        }
        else
        { // attr is not supported - remove
            attr.Remove();
        }
    }
    CleanupStack::PopAndDestroy(&attrList);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateElWithPatternL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateResElL( TXmlEngElement aElement, 
                                         RArray<TXmlEngElement>& aClassList)
{
    // list of attributes
    RXmlEngNodeList<TXmlEngAttr> attrList;
    CleanupClosePushL(attrList);
    aElement.GetAttributes(attrList);
    
    // for each attr
    while(attrList.HasNext())
    {
        // get attr
        TXmlEngAttr attr = attrList.Next();
        
        //  get pattern 
        TXmlEngElement pattern = GetPatternForResAttrL(attr.Name(), aClassList);
        if(pattern.NotNull())
        {
            ValidateAttrWithPatternL(attr, pattern); // leaves on error
        }
        else
        { // attr is not supported - remove
            attr.Remove();
        }
    }
    CleanupStack::PopAndDestroy(&attrList);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateElWithPatternL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateElWithPatternL(   TXmlEngElement aElement, 
                                                    TXmlEngElement aPattern )
{
    // is required?
    if(IsRequiredL(aPattern) )
    { // cannot be empty
        if( !aElement.Value().Length() || 
            UpnpCdUtils::IsWhiteString( aElement.Value() ) )
        {
            User::Leave(EBadMetadata);
        }
    }
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateAttrWithPatternL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ValidateAttrWithPatternL( TXmlEngAttr aAttr, 
                                                    TXmlEngElement aPattern )
{
    // is required?
    if(IsRequiredL(aPattern) )
    { // cannot be empty
        if( !aAttr.Value().Length() || 
            UpnpCdUtils::IsWhiteString( aAttr.Value() ) )
        {
            User::Leave(EBadMetadata);
        }
    }
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateAttrWithPatternL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
TBool CUpnpElementFactory::IsRequiredL(TXmlEngElement aPattern) 
{
    TXmlEngAttr reqAttr = aPattern.AttributeNodeL(KRequiredObject());
    TBool ret = EFalse;
    if( reqAttr.NotNull() &&
        reqAttr.Value().Length() && 
        reqAttr.Value() == KTrueValue8() )
    {
        ret = ETrue;
    }
    return ret;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::GetPatternForElL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpElementFactory::GetPatternL( const TDesC8& aPropertyName,
                                                RArray<TXmlEngElement>& aClassList,
                                                const TDesC8& aType )
{
    TXmlEngElement retEl;
    
    // for each class
    for(TInt i = 0; i < aClassList.Count(); i++)
    {
        TXmlEngElement elPattEl;      
        UpnpDomInterface::GetDirectoryElementL(aClassList[i], elPattEl, KProperties, KType, aType);
        if(elPattEl.NotNull())
        {   
            UpnpDomInterface::GetDirectoryElementL(elPattEl, retEl, KObjectProperty, KObjectName, aPropertyName);
            
            // break if found
            if(retEl.NotNull())
            {
                break;
            }
        }
    }
    
    return retEl;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::GetPatternForResAttrL()
// Function leaves on error.
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpElementFactory::GetPatternForResAttrL(const TDesC8& aPropertyName,
                                                    RArray<TXmlEngElement>& aClassList )
{
    TXmlEngElement retEl;
    
    // for each class
    for(TInt i = 0; i < aClassList.Count(); i++)
    {
        TXmlEngElement resAttrPattEl;     
        UpnpDomInterface::GetDirectoryElementL(aClassList[i], resAttrPattEl, KProperties, KType, KResAttr);
        if(resAttrPattEl.NotNull())
        {   
            UpnpDomInterface::GetDirectoryElementL(resAttrPattEl, retEl, KObjectProperty, KObjectName, aPropertyName);
            
            // break if found
            if(retEl.NotNull())
            {
                break;
            }
        }
    }
    
    return retEl;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::IsMainObjectTagL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpElementFactory::IsMainObjectTagL(TXmlEngElement aElement)
{
    TXmlEngElement notNeeded;
    return UpnpDomInterface::GetElementL(aElement, notNeeded, KClassTagName);
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ActiveElementL()
// Gets active element from XML document. An active element
// is a first element different from <DIDL-Lite> element.
// -----------------------------------------------------------------------------
//
TXmlEngElement CUpnpElementFactory::ActiveElementL( const RXmlEngDocument& aDocument )
{
    if( aDocument.IsNull() || aDocument.DocumentElement().IsNull() )
    {
        User::Leave( KErrNotFound );            
    }
        
    if ( aDocument.DocumentElement().Name().CompareF( KDidlLite() ) != 0 )
    {
        return aDocument.DocumentElement();
    }
    else
    {
        TXmlEngElement root = aDocument.DocumentElement();
        RXmlEngNodeList<TXmlEngElement> children;
        CleanupClosePushL(children);
        root.GetChildElements( children );
    //    children.HasNext();
        CleanupStack::PopAndDestroy(&children);
        return children.Next();
    }
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ExtractActiveElementL()
// Extracts active element from XML document. 
// An active element is a first element different from <DIDL-Lite> element.
// IMPORTANT: Caller takes responsibility for returned element. 
// -----------------------------------------------------------------------------
//
RXmlEngDocument CUpnpElementFactory::ExtractActiveElementL( const RXmlEngDocument& aDocument )
{
    TXmlEngElement active;
    RXmlEngDocument ret;
    ret.OpenL(iDOMImpl);
    CleanupClosePushL(ret);

 /*    if( aDocument.IsNull() || aDocument.DocumentElement().IsNull() )
    {
        User::Leave( KErrNotFound );            
    }
        
    if ( aDocument.DocumentElement().Name().CompareF( KDidlLite() ) != 0 )
    {
        active = aDocument.DocumentElement().Unlink().AsElement();
    }
    else
    {
        TXmlEngElement root = aDocument.DocumentElement();
        RXmlEngNodeList<TXmlEngElement> children;
        CleanupClosePushL(children);
        root.GetChildElements( children );
        children.HasNext();
        active = children.Next().Unlink().AsElement();
        CleanupStack::PopAndDestroy(&children);
    }
    ret.SetDocumentElement(active);
   */    
    CleanupStack::Pop(&ret);
    
    return ret;
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ElementsMatchL()
// Checks if two elements are equal and have exactly the same subtrees.
// -----------------------------------------------------------------------------
//
TBool CUpnpElementFactory::ElementsMatchL( TXmlEngElement aFirst, TXmlEngElement aSecond )
{
    // this function checks following things:
    // 1. element names
    // 2. contents of the elements
    // 3. attributes of the elements
    // 4. child elements of the elements
    
    if ( aFirst.Name().Compare(aSecond.Name()) == 0 )
    {
        // aFirst names match. next: check namespace
        if ( aFirst.Prefix().Compare( aSecond.Prefix()) == 0 )
        {
            if (!aFirst.Text().Compare(aSecond.Text()))
            {
                
                // namespacess match, next: check attributes
                RXmlEngNodeList<TXmlEngAttr> elemAttrs;
                CleanupClosePushL(elemAttrs);
                RXmlEngNodeList<TXmlEngAttr> currAttrs;
                CleanupClosePushL(currAttrs);
                
                aFirst.GetAttributes( elemAttrs );
                aSecond.GetAttributes( currAttrs );
                
                if ( elemAttrs.Count() == currAttrs.Count() )
                {
                    // first step in comparing attributes ready (counts match!)
                    // next, check names and values (for each attribute)

                    while( elemAttrs.HasNext() && currAttrs.HasNext() )
                    {
                        TXmlEngAttr elemAttr = elemAttrs.Next();
                        TXmlEngAttr currAttr = currAttrs.Next();
                        
                        // compare names and values
                        if ( elemAttr.Name().Compare( currAttr.Name() ) )
                        {
                            CleanupStack::PopAndDestroy(&currAttrs);
                            CleanupStack::PopAndDestroy(&elemAttrs);
                            return EFalse;
                        }
                        if ( elemAttr.Value().Compare( currAttr.Value() ) )
                        {
                            CleanupStack::PopAndDestroy(&currAttrs);
                            CleanupStack::PopAndDestroy(&elemAttrs);
                            return EFalse;
                        }
                        
                    }
                    
                    // compare child elements (recursive function call)
                    RXmlEngNodeList<TXmlEngElement> fChildren;
                    CleanupClosePushL(fChildren);
                    RXmlEngNodeList<TXmlEngElement> sChildren;
                    CleanupClosePushL(sChildren);
                                        
                    aFirst.GetChildElements( fChildren );
                    aSecond.GetChildElements( sChildren );
                
                    
                    if ( fChildren.Count() == sChildren.Count() )
                    {
                            while ( fChildren.HasNext() && sChildren.HasNext() )
                            {
                            // call this function again for children
                                TBool matching = ElementsMatchL( fChildren.Next(), sChildren.Next() );

                            // if some of the elements do not match, return false!
                                if (matching == EFalse)
                                {
                                    CleanupStack::PopAndDestroy(&sChildren);
                                    CleanupStack::PopAndDestroy(&fChildren);
                                    CleanupStack::PopAndDestroy(&currAttrs);
                                    CleanupStack::PopAndDestroy(&elemAttrs);
                                    return EFalse;
                                }
                            }
                        // only place to return ETrue
                        // all the checks are made; if survived here, elements match!
                        CleanupStack::PopAndDestroy(&sChildren);
                        CleanupStack::PopAndDestroy(&fChildren);
                        CleanupStack::PopAndDestroy(&currAttrs);
                        CleanupStack::PopAndDestroy(&elemAttrs);
                        return ETrue;
                    }
                    CleanupStack::PopAndDestroy(&sChildren);
                    CleanupStack::PopAndDestroy(&fChildren);
                }
                CleanupStack::PopAndDestroy(&currAttrs);
                CleanupStack::PopAndDestroy(&elemAttrs);
            }
        }
    }
    
    return EFalse;
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::CountElementsL()
// Prepares corresponding RArray<TXmlEngElements> and calls 
// CountElementsL( const TDesC8& aName, RArray<TXmlEngElement>& aArray )
// -----------------------------------------------------------------------------
//
TInt CUpnpElementFactory::CountElementsL( const TDesC8& aName, RArray<RXmlEngDocument>& aArray )
{
    TInt count(0);
        
    for(TInt i = 0; i < aArray.Count(); i++)
    {            
        if ( aArray[i].DocumentElement().NotNull() )
        {
            if ( aArray[i].DocumentElement().Name() == aName )
            {
                count++;
            }
        }
            
    }
    return count;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::CountElementsL()
// Counts elements in array that have the same name.
// -----------------------------------------------------------------------------
//
TInt CUpnpElementFactory::CountElementsL( const TDesC8& aName, RArray<TXmlEngElement>& aArray )
{
    TInt count(0);
    
    for (TInt v(0); v<aArray.Count(); v++)
    {
        if ( aArray[v].NotNull() )
        {
            if ( aArray[v].Name() == aName )
            {
                count++;
            }
        }
    }
    
    return count;
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::NameWithNsLC()
// Constructs descriptor with element's name and namespace prefix separated with colon.
// IMPORTANT: As the function name indicates, it leaves pointer to heap descriptor on 
// cleanup stack.
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpElementFactory::NameWithNsLC(const TXmlEngElement& aElement)
{
    const TDesC8& localName = aElement.Name();
    TPtrC8 prefix = aElement.Prefix();

    if ( prefix.Length() > 0)
    {

        HBufC8* nameWithNs = HBufC8::NewLC( 
            localName.Length() + 
            UpnpString::KColon().Length() +
            prefix.Length() );

        nameWithNs->Des().Append( prefix );
        nameWithNs->Des().Append( UpnpString::KColon() );
        nameWithNs->Des().Append( localName );

        return nameWithNs;
    }
    else
    {
        return localName.AllocLC();
    }
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::ClassesL()
// Gets classes descriptions for given object.
// -----------------------------------------------------------------------------
//
void CUpnpElementFactory::ClassesL( const TXmlEngElement& aObject, RArray<TXmlEngElement>& aArray, const TDesC8& aObjType )
{
    TInt i(0);
    
    // if survived here, we have the new element of type item or container
    // let's analyze its type
    TXmlEngElement type;
    UpnpDomInterface::GetElementL ( aObject, type, KClass() );

    if ( type.IsNull() ) 
    {
        // no <class> element! leave
        User::Leave( EBadMetadata );
    }

    // check that does the new element have all the required fields for its type
    // and also check that it does not have whatever fields (only optional allowed)
    RPointerArray<TPtrC8> objectFields;
    
    TPtrC8 content = type.Text();
    if( !content.Length() )
    {
        User::Leave( EBadMetadata );
    }
    //--- removing white spaces ------------------      
    HBufC8* tempBuffer = type.Text().AllocLC(); 
    TPtr8 tmpPtr(tempBuffer->Des());
    UpnpCdUtils::RemoveWhiteSpacesL(tmpPtr);        
    CleanupStack::Check(tempBuffer);
    type.SetTextL(tmpPtr);  
    //--------------------------------------------
    
    UpnpString::CutToPiecesL(tmpPtr, TChar('.'), objectFields );    
    CleanupStack::Check(tempBuffer);
    // let's inspect each of types
    // at first, collect object type descriptions from objects xml    
    // to a pointer array
    
    TInt NumberOfObjectFields = objectFields.Count();
    
    for ( i=0; i < NumberOfObjectFields; i++ ) 
    {
        // seek for such object type
        TXmlEngElement objectDescription;

        UpnpDomInterface::GetDirectoryElementL( 
                iDocument.DocumentElement(), 
                objectDescription, 
                KElement, 
                KType, 
                *objectFields[i] );
        CleanupStack::Check(tempBuffer);
        // if such type found
        if ( objectDescription.NotNull() ) 
        {
            aArray.AppendL( objectDescription );
        }
        /* This case is valid for any numeric value of containerID used in CreateObject() action
        *  We do some checking starting from 3rd string of <upnp:class> element, if the string isn't
        *  recognizable, instead of leaving, we trims the value of <upnp:class> element, e.g:
        *  'object.item.imageItem.abcd' -> is trimmed into -> 'object.item.imageItem'
        *  DLNA 7.3.120.4 requirement
        */
        else if( i >= 2 )
        {            
            for( TInt j = NumberOfObjectFields - 1; j >= i; j-- )
            {                            
                TInt pos = tmpPtr.LocateReverse( TChar('.') );                    
                if( pos > KErrNotFound )
                {
                    tmpPtr.Copy( tmpPtr.Left(pos) );                   
                                                
                    delete objectFields[j];
                    objectFields.Remove(j);   
                }                    
            }                                                
            type.SetTextL(tmpPtr); // changing xml                        
            
            break;
        }
        else 
        {
            objectFields.ResetAndDestroy();
            objectFields.Close();
            User::Leave( EBadMetadata );
        }
    }
    CleanupStack::Check(tempBuffer);
    /*  for each type, check that: 
    *   1. it is referenced to its current parent type (e.g. audioItem -> item)
    *   2. this new object has all the required fields
    *   3. relations between object types and values of 'upnp:class':
    *      <item> -> 'object.item' or <container> -> 'object.container'
    */
       
    // we count the number of elements once again, because size of the array might change
    NumberOfObjectFields = objectFields.Count(); 

    for ( i=0; i < NumberOfObjectFields; i++ )
    {
        const TDesC8& field = *objectFields[i];

        // 1. first, check the parent relation 
        // first type must be "object"!
        if ( i==0 ) 
        {
            /* Leave if:
            *  1. first type isn't the 'object' OR
            *  2. 'upnp:class' contains only 'object' type [which in matter of fact isn't instantiable]
            */
            if ( field != KObject() || 
                 field == KObject() && NumberOfObjectFields == 1
            ) 
            {
                // if not "item", leave!
                objectFields.ResetAndDestroy();
                objectFields.Close();
                User::Leave( EBadMetadata );
            }
        }
        // for later types, check the relation really
        else 
        {   
            // checking relations between object types and values of 'upnp:class'
            if( i == 1 && aObjType.Length() > 0 )
            {
                if(  !field.Compare(KItem) && aObjType.Compare(KItem) ||
                     !field.Compare(KContainer) && aObjType.Compare(KContainer)
                )
                {          
                    objectFields.ResetAndDestroy();
                    objectFields.Close();                              
                    User::Leave( EBadMetadata );
                }
            }            
            //----------------------------------------    
            const TDesC8& parent = *objectFields[i-1];
            
            // if we've survived to this point, this parent string
            // should match with the one in the previous object description xml element
            TXmlEngElement iparent;
            UpnpDomInterface::GetElementL ( aArray[i], iparent, KIParent() );
            CleanupStack::Check(tempBuffer);
            TPtrC8 cont = iparent.Text();
        
            // now check the actual relation. leave if strings do not match!
            if ( cont.Length() && parent != cont )
            {
                objectFields.ResetAndDestroy();
                objectFields.Close();
                User::Leave( EBadMetadata );
            }
        }
    }
    CleanupStack::PopAndDestroy(tempBuffer);
    objectFields.ResetAndDestroy();
    objectFields.Close();
    return;
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::IsNotEmptyImportUriL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpElementFactory::IsNotEmptyImportUriL( const TXmlEngElement& aElement )
{
    TXmlEngAttr importUri;
    
    if ( aElement.Name() == KRes() )
    {
        importUri = aElement.AttributeNodeL( KImportUri8() );
        
        // remove if empty importUri: DLNA Requirement [7.3.134.5]
        TPtrC8 val( importUri.Value() );
        if( UpnpCdUtils::IsWhiteString( val ) )
        {
            importUri.Remove();
        }
    }

    return importUri.NotNull();
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::IsClassElement()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpElementFactory::IsClassElement( const TXmlEngElement& aElement )
{
    if ( aElement.Name() == KClass() )
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
// -----------------------------------------------------------------------------
// CUpnpElementFactory::ValidateProtocolInfoInResL
//  Allocates string, which is object type for this object. 
// -----------------------------------------------------------------------------
HBufC8* CUpnpElementFactory::ValidateProtocolInfoInResL( const TDesC8& aProtocolInfo, TBool aLocalSharing )
{
    HBufC8* result = NULL; 
   // CUpnpDlnaProtocolInfo* protocolInfo = CUpnpDlnaProtocolInfo::NewL( (TDesC8&)aProtocolInfo );
    CUpnpProtocolInfoLocal* protocolInfo = CUpnpProtocolInfoLocal::NewL( (TDesC8&)aProtocolInfo );
    CleanupStack::PushL( protocolInfo ); 
    _LIT8(KProtocolInfoHttpGet, "http-get"); 
    protocolInfo->SetFirstFieldL( (TDesC8&)KProtocolInfoHttpGet() ); 
    protocolInfo->SetSecondFieldL( (TDesC8&)KAsterisk8() ); 
    TPtrC8 third = protocolInfo->ThirdField(); 
    if(( third.Find( KSlash8()) == KErrNotFound) && 
      ( third.Compare( KAsterisk8()) != KErrNone))
    {
        if( aLocalSharing)
        {
        User::Leave( EBadMetadata );
        }
        protocolInfo->SetThirdFieldL( (TDesC8&) KAsterisk8());                                  
    }
   // if( protocolInfo->IsDlnaInformationIncluded() ) // Check it !!!
    {
        protocolInfo->SetOpParameterL( UpnpDlnaProtocolInfo::B_VAL , ETrue );
        protocolInfo->SetOpParameterL( UpnpDlnaProtocolInfo::A_VAL , EFalse ); 
    }
    TPtrC8 prInfo = protocolInfo->ProtocolInfoL();
    result =  prInfo.Alloc();
    CleanupStack::PopAndDestroy( protocolInfo ); 
    return result; 
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::GetContentDirectoryReference
// Function gets pointer to CUpnpContentDirectory and puts it into CUpnpElementFactory object  
// -----------------------------------------------------------------------------
void CUpnpElementFactory::GetContentDirectoryReference(CUpnpContentDirectory* aCD)
{
    iContentDirectory = aCD;
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::CheckDurationOfResElement
// If res@duration attribute exists, the method checks whether it has valid format
// -----------------------------------------------------------------------------

void CUpnpElementFactory::CheckDurationOfResElementL(const TXmlEngElement& aElement)
{
    TXmlEngAttr duration;
    
    // if this is the 'res' element            
    if( aElement.Name() == KRes() )
    {
        // and it has res@duration attribute
        duration = aElement.AttributeNodeL( KDuration8() );                                        
        if( duration.NotNull() )
        {
            TCurrentAction action = iContentDirectory->ExecutedAction();                                         
            TPtrC8 val( duration.Value() );
            
            // if res@duration atrribute value is invalid [has improper format]
            if( !UpnpCdUtils::ValidateDurationValue(val) )
            {            
                // remove it from 'res' element if it is CreateObject()
                if( action == ECreateObjectAction )
                {
                    duration.Remove();
                }                
            }
        }
    }   
}

// -----------------------------------------------------------------------------
// CUpnpElementFactory::CheckSizeOfResElement
// If res@size attribute exists, the method checks whether it has valid format
// size should be unsigned int.
// -----------------------------------------------------------------------------
void CUpnpElementFactory::CheckSizeOfResElementL(
    const TXmlEngElement& aElement )
    {
    TXmlEngAttr sizeAttr;

    // if this is the 'res' element            
    if ( aElement.Name() == KRes() )
        {
        // and it has res@size attribute
        sizeAttr = aElement.AttributeNodeL( KSize() );
        if ( sizeAttr.NotNull() )
            {
            TCurrentAction action = iContentDirectory->ExecutedAction();
            TPtrC8 val( sizeAttr.Value() );

            // if res@size atrribute value is invalid [is not an unsigned long]
            TUint32 unsignedLong = 0;

            TLex8 lexULong(val);
            TInt error = lexULong.Val( unsignedLong, EDecimal );
                         
            TInt remainder = lexULong.Remainder().Length();

            if ( error != KErrNone || remainder )
                {
                // remove it from 'res' element if it is CreateObject()
                if ( action == ECreateObjectAction )
                    {
                    sizeAttr.Remove();
                    }
                }
            }
        }

    }

//  End of File
