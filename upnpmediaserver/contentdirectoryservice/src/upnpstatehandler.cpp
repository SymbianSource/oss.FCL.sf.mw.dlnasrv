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
* Description:  State handler
*
*/


//  INCLUDES
#include "upnpstatehandler.h"
#include "upnpstring.h"
#include "upnpcontainerupdateid.h"
#include "upnpcontentdirectory.h"
#include "upnpcontentdirectoryglobals.h"
#include "upnpcommonupnplits.h"


// -----------------------------------------------------------------------------
// CUpnpStateHandler::CUpnpStateHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpStateHandler::CUpnpStateHandler(  )
{
    iContainerUpdateIdsEvented = EFalse;          
}


// -----------------------------------------------------------------------------
// CUpnpStateHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpStateHandler::ConstructL( CUpnpContentDirectory* aCd )
{    
    iContainerUpdateIds.Reset();
    iCurrentContainerUpdateIds.Reset();
    iCd=aCd;
}

// -----------------------------------------------------------------------------
// CUpnpStateHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpStateHandler* CUpnpStateHandler::NewL( CUpnpContentDirectory* aCd )
{
    CUpnpStateHandler* self = new (ELeave) CUpnpStateHandler(  );

    CleanupStack::PushL( self );
    self->ConstructL( aCd );
    CleanupStack::Pop( self );
    return self;
}

// -----------------------------------------------------------------------------
// CUpnpStateHandler::~CUpnpStateHandler
// Destructor
// -----------------------------------------------------------------------------
//
CUpnpStateHandler::~CUpnpStateHandler()
{   
    // clear tables related to ContainerUpdateIds state variable
    for( TInt i( 0 ); i < iContainerUpdateIds.Count(); i++ )
    {
        delete iContainerUpdateIds[i].iContainerId;
    }
    iContainerUpdateIds.Close(); 
    //--------------------------
    for( TInt i( 0 ); i < iCurrentContainerUpdateIds.Count(); i++ )
    {
        delete iCurrentContainerUpdateIds[i].iContainerId;
    }
    iCurrentContainerUpdateIds.Close();
    //-------------------------- 
}

// -----------------------------------------------------------------------------
// CUpnpStateHandler::ContainerUpdateIds
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
TInt CUpnpStateHandler::ContainerUpdateId( const TDesC8& aContainerID ) 
{
    if( aContainerID.Compare( KZero ) == 0 )
    {
        return iSystemUpdateId;
    }

    for(TInt i( 0 ); i < iContainerUpdateIds.Count(); i++ )
    {
        if( aContainerID.Compare(*(iContainerUpdateIds[i].iContainerId))==0)
        {
            return iContainerUpdateIds[i].iUpdateId;
        }
    }
    return 0; //container not found so it is unmodified and updateId is zero.
}

// -----------------------------------------------------------------------------
// CUpnpStateHandler::ContainerUpdateIds
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
RArray<TUpnpContainerUpdateId>& CUpnpStateHandler::ContainerUpdateIds( )
{    
    return iContainerUpdateIds;
}
// -----------------------------------------------------------------------------
// CUpnpStateHandler::IsEnableL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CUpnpStateHandler::IsEnable()
{
    if (iCd->SubcribersAmount() > 0) return ETrue;       
    else return EFalse;
}

// -----------------------------------------------------------------------------
// CUpnpStateHandler::IncreaseSystemUpdateIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUpnpStateHandler::IncreaseSystemUpdateIdL()
{
    if( iSystemUpdateId < KMaxTUint )
    {
        iSystemUpdateId++;
    }
    else
    {
        iSystemUpdateId = 1;
    }
    TBuf8<UpnpString::KMaxIntLength> suid;
    suid.Num( iSystemUpdateId );
    iCd->SetStateVariableL( KSystemUpdateID, suid );
    return iSystemUpdateId;
}
// -----------------------------------------------------------------------------
// CUpnpStateHandler::IncreaseContainerUpdateIdL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpStateHandler::IncreaseContainerUpdateIdL( const TDesC8& aContainerID )
{
    // DLNA.ORG_AnyContainer support - 'any container' 
    //mapped into container with id=1 
    HBufC8* properContainerID;
    
    if( !aContainerID.Compare(KAnyContainerIndicator8) )    
        properContainerID = KOne().AllocLC();    
    else    
        properContainerID = aContainerID.AllocLC();
    
    // iContainerUpdateIds table serving ---------------           
    TInt updateId( KErrNotFound );
    TInt i( 0 );
    for( ; i < iContainerUpdateIds.Count(); i++ )
    {
        if( properContainerID->Des().Compare(
                         *(iContainerUpdateIds[i].iContainerId) ) == 0 )
        {
            updateId = iContainerUpdateIds[i].iUpdateId;
            break;
        }
    }

    if( KErrNotFound == updateId ) //first touch to this con´tainer
    {
        TUpnpContainerUpdateId cui;
        
        cui.iContainerId = properContainerID->AllocL();                                                                           
        //First time of modification, thus ID is 1.
        // No magic (white nor black) used here.
        cui.iUpdateId = 1;
        iContainerUpdateIds.AppendL( cui );
        updateId = cui.iUpdateId;
    }
    else
    {
        if( iContainerUpdateIds[i].iUpdateId == KMaxTInt )
        {
            iContainerUpdateIds[i].iUpdateId = 1;
        }
        else
        {
            iContainerUpdateIds[i].iUpdateId++;
        }
        updateId = iContainerUpdateIds[i].iUpdateId;
    }
    
    // iCurrentContainerUpdateIds table serving ---------------           
    if (IsEnable())     //if any CP subscribed   
    {
    if(iContainerUpdateIdsEvented)
    {
        // clearing iCurrentContainerUpdateIds table after:
        // 1. sending NOTIFY events, and
        // 2. 'ContainerUpdateIds' state variable change
        for( TInt i(0); i < iCurrentContainerUpdateIds.Count(); i++ )
        {
            delete iCurrentContainerUpdateIds[i].iContainerId;
        }
        iCurrentContainerUpdateIds.Reset();
         
       iContainerUpdateIdsEvented = EFalse; 
    }
    
    // finding out whether element should be updated or added                                       
    TBool isEelementFound = EFalse;
    TInt j( 0 );
    
    for( ; j < iCurrentContainerUpdateIds.Count(); j++ )
    {
        if( properContainerID->Des().Compare(
                         *(iCurrentContainerUpdateIds[j].iContainerId) ) == 0 )
        {            
            isEelementFound = ETrue;
            break;
        }
    }
    
    if(isEelementFound)
    {
        iCurrentContainerUpdateIds[j].iUpdateId = updateId;                              
    }
    else
    {
        TUpnpContainerUpdateId newEl;
        
        newEl.iContainerId = properContainerID->AllocL();                                                                                   
        newEl.iUpdateId = updateId;
        
        iCurrentContainerUpdateIds.AppendL(newEl);
    }
                            
    // preparing new value of 'ContainerUpdateIds' state variable ----   
                                
    TInt elCount = iCurrentContainerUpdateIds.Count();
    TInt totalDescLength = 0;
        
    for( TInt k = 0; k < elCount; k++ )
    {
        totalDescLength = totalDescLength + 
                (iCurrentContainerUpdateIds[k].iContainerId)->Des().Length();
    }    
    
    totalDescLength = totalDescLength + elCount*(UpnpString::KMaxIntLength)
            + elCount - 1;// 'elCount-1'-> quantity of commas in the descriptor
    
    HBufC8* variableValue  = HBufC8::NewLC(totalDescLength);                        
    TPtr8 pVar = variableValue->Des();
    pVar.Zero();
        
    for( TInt k = 0; k < elCount; k++ )
    {
        pVar.Append( *(iCurrentContainerUpdateIds[k].iContainerId) );
        pVar.Append(',');
        if ((iCurrentContainerUpdateIds[k].iContainerId)->Compare(KZero) == 0)
            pVar.AppendNum( iSystemUpdateId );
        else        
            pVar.AppendNum( iCurrentContainerUpdateIds[k].iUpdateId );    
        
        if( k != elCount - 1 ) // last comma not appended
            pVar.Append(',');
    }
                    
    //Set new value to be evented
    iCd->SetStateVariableL( KContainerUpdateIDs, *variableValue );
                            
    CleanupStack::PopAndDestroy( variableValue );
    }
    CleanupStack::PopAndDestroy( properContainerID );   
         
 }

// -----------------------------------------------------------------------------
// CUpnpStateHandler::StateVariableEvented
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CUpnpStateHandler::StateVariableEvented(const TDesC8& aVariableName)
{
   /* if( aVariableName.Compare(KContainerUpdateIDs) == 0 
                        && (iCd->SubcribersAmount() > 0 )) */
    if( aVariableName.Compare(KContainerUpdateIDs) == 0)                         
    {        
        iContainerUpdateIdsEvented = ETrue;                                                
        return;
    }
}

//end of file
