/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UPnP Renderer config container class
*
*/

#ifndef C_UPNPRENDERERCFG_H
#define C_UPNPRENDERERCFG_H

#include <e32base.h>

class CUpnpRendererCfg : public CBase
    {
    
public:

    /**
     * Static NewL
     *
     * @param aConfigFile Reference to config file descriptor
     * @param aModelName Reference to model name 8-bit descriptor
     * @param aSrcMimeType Reference to source mime type 8-bit descriptor
     * 
     * @return instance to CUpnpRendererCfg
     */
    IMPORT_C static CUpnpRendererCfg* NewL( const TDesC& aConfigFile, 
            const TDesC8& aModelName, const TDesC8& aSrcMimeType );
    
    /**
     * Static NewLC
     *
     * @param aConfigFile Reference to config file descriptor
     * @param aModelName Reference to model name 8-bit descriptor
     * @param aSrcMimeType Reference to source mime type 8-bit descriptor
     * 
     * @return instance to CUpnpRendererCfg
     */
    IMPORT_C static CUpnpRendererCfg* NewLC( const TDesC& aConfigFile, 
            const TDesC8& aModelName, const TDesC8& aSrcMimeType );
    
    IMPORT_C ~CUpnpRendererCfg();
    
private:
    
    /**
     * Destructor
     */
    CUpnpRendererCfg();
   
    /**
     * 2nd phrase Constructor
     */
    void ConstructL( const TDesC& aConfigFile, const TDesC8& aModelName,
            const TDesC8& aSrcMimeType );
    
public:
    
    /*
     * Return pipeline descriptor
     * 
     * @return Reference to pipeline descriptor if pipeline set, otherwise 
     *         KNullDesC8
     */
    IMPORT_C const TDesC8& Pipeline() const;

    /*
     * Return protocol info descriptor
     * 
     * @return Reference to protocol info descriptor if pipeline set, 
     *         otherwise KNullDesC8
     */

    IMPORT_C const TDesC8& ProtocolInfo() const;
    
    /*
     * Return protocol info descriptor
     * 
     * @return Size multiplier if set, otherwise KErrNotFound
     */    
    IMPORT_C TReal SizeMultiplier() const;
    
private:
    
    HBufC*  iConfigFile;  //owned
    HBufC8* iModelName;   //owned
    HBufC8* iSrcMimeType; //owned
    
    HBufC8* iPipeline;       //owned
    HBufC8* iProtocolInfo;   //owned
    TReal   iSizeMultiplier;
    
private:
    
    /*
     * CUpnpRendererCfgParser sets directly following private member 
     * variables, if found from renderer config xml:
     * - iPipeline
     * - iProtocolInfo
     * - iSizeMultiplier
     */
    friend class CUpnpRendererCfgParser;
    
    };

#endif // C_UPNPRENDERERCFG_H

