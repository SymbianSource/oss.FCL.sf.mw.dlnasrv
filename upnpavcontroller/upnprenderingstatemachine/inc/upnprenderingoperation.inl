/*
* Copyright (c) 2007,2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A rendering operation modeled as a class
*
*/


// --------------------------------------------------------------------------
// TUpnpRenderingOperation::TUpnpRenderingOperation
// --------------------------------------------------------------------------
//
inline TUpnpRenderingOperation::TUpnpRenderingOperation( 
    Upnp::TCommand aCmd, TInt aParam, const TAny* aData )
    {
    iCmd=aCmd;
    iParam=aParam;
    iData=aData;
    iUserOriented=ETrue;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::TUpnpRenderingOperation
// --------------------------------------------------------------------------
//
inline TUpnpRenderingOperation::TUpnpRenderingOperation( 
    Upnp::TCommand aCmd, const TAny* aData )
    {
    iCmd=aCmd;
    iParam=0;
    iData=aData;
    iUserOriented=ETrue;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator=
// --------------------------------------------------------------------------
//
inline TUpnpRenderingOperation TUpnpRenderingOperation::operator=( 
    Upnp::TCommand aCmd )
    {
    iCmd=aCmd;
    iParam=0;
    iData=0;
    iUserOriented=ETrue;
    return *this;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator=
// --------------------------------------------------------------------------
//
inline TUpnpRenderingOperation TUpnpRenderingOperation::operator=(
    const TUpnpRenderingOperation& aOther )
    { 
    iCmd=aOther.iCmd; 
    iParam=aOther.iParam;
    iData=aOther.iData;
    iUserOriented=aOther.iUserOriented; 
    return *this;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator==
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::operator==(
    Upnp::TCommand aCmd ) const
    {
    return iCmd==aCmd;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator!=
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::operator!=( 
    Upnp::TCommand aCmd ) const
    {
    return !operator==(aCmd);
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator==
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::operator==(
    const TUpnpRenderingOperation& aOther ) const
    {
    return iCmd==aOther.iCmd && iParam==aOther.iParam &&
        iData==aOther.iData && iUserOriented==aOther.iUserOriented;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::operator!=
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::operator!=(
    const TUpnpRenderingOperation& aOther ) const
    {
    return !operator==(aOther);
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::IsValid
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::IsValid() const
    {
    return iCmd != Upnp::ENone;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::IsUserOriented
// --------------------------------------------------------------------------
//
inline TBool TUpnpRenderingOperation::IsUserOriented() const
    {
    return iUserOriented;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::Command
// --------------------------------------------------------------------------
//
inline const Upnp::TCommand& 
    TUpnpRenderingOperation::Command() const
    {
    return iCmd;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::Param
// --------------------------------------------------------------------------
//
inline TInt& TUpnpRenderingOperation::Param()
    {
    return iParam; 
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::Param
// --------------------------------------------------------------------------
//
inline const TInt& TUpnpRenderingOperation::Param() const
    {
    return iParam;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::Data
// --------------------------------------------------------------------------
//
inline const TAny* TUpnpRenderingOperation::Data() const
    {
    return iData;
    }

// --------------------------------------------------------------------------
// TUpnpRenderingOperation::SetUserOriented
// --------------------------------------------------------------------------
//
inline void TUpnpRenderingOperation::SetUserOriented( TBool aUserOriented )
    {
    iUserOriented = aUserOriented;
    }

// End of File
