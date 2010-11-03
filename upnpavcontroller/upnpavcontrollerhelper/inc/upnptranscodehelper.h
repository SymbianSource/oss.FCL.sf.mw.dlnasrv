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
* Description:  UPnP transcode helper
*
*/

#ifndef C_UPNPTRANSCODEHELPER_H
#define C_UPNPTRANSCODEHELPER_H

// INCLUDES
#include <e32base.h>

#include "upnpgstwrapperobserver.h"

class CUpnpItem;
class CUpnpAVDevice;
class CUpnpGstWrapper;


NONSHARABLE_CLASS(CUpnpTranscodeHelper) : public CBase
                           , public MUpnpGstWrapperObserver
    {
    
    enum TContainerType
        {
        EUnknownContainer,
        EMp4Container,
        ETsContainer
        };
    
public: // construction / destruction
    
    /**
     * Static NewL
     *
     * @param none
     * @return instance to CUpnpTranscodeHelper
     */
     static CUpnpTranscodeHelper* CUpnpTranscodeHelper::NewL();

    /**
     * Destructor
     */
     virtual ~CUpnpTranscodeHelper();
    
private: // construction, private part
    
    /**
     * Constructor
     */
    CUpnpTranscodeHelper();  
    
    /**
     * 2nd phrase Constructor
     */
    void ConstructL();  
     
public:
    
     /**
      * Fetch pre-defined transcoding pipeline and protocol info for renderer
      * 
      * @param aRenderer Reference to renderer
      * @param aUpnpItem Reference to original upnp item  
      * @param aPipeline Reference pointer to descriptor where parsed 
      *        transcoding pipeline will be allocated
      * @param aProtocolInfo Reference pointer to descriptor where parsed 
      *        protocol info will be allocated
      */
     void PreDefinedCfgL( const CUpnpAVDevice& aRenderer, 
             CUpnpItem& aUpnpItem, HBufC8*& aPipeline, 
             HBufC8*& aProtocolInfo );
    
    /**
     * Create transcoding pipeline 
     * 
     * @param aUpnpItem Reference to original upnp item
     * @param aRendererProtocolInfos Reference to descriptor array containing
     *        protocol list from the renderer
     * @return Pointer to descriptor containing transcoding pipeline; 
     *         NULL if match not found
     */
    HBufC8* PipelineL( CUpnpItem& aUpnpItem, 
            const CDesC8Array& aRendererProtocolInfos );    
    
    /**
     * Add a valid protocol info according to transcoding pipeline
     * 
     * @param aUpnpItem Reference to original upnp item
     * @param aPipeline Reference to pipeline descriptor
     * @param aProtocolMatches Reference to descriptor array containing
     *        protocol list from the renderer
     */    
    void AddValidResElementL( CUpnpItem& aUpnpItem, 
            const TDesC8& aPipeline, const CDesC8Array& aProtocolMatches );        
    

    void ReplaceResourceL( CUpnpItem& aUpnpItem, const TDesC8& aProtocolInfo );
    
    /*
     * Start transcoding
     * 
     * @param aPipeline Reference to pipeline descriptor
     */
    void TranscodeL( const TDesC8& aPipeline );
    
private:
    
    HBufC8* GeneratePipelineL( const TDesC8& aOriginalFilePath, 
            TContainerType aDestContainerType );
    
    HBufC8* PathToLinuxSyntaxL( const TDesC& aOriginalPath );
    
private:
    
    void HandleGstEvent( Gst::TEvent aEvent );
    
private:
    
    RFs iFs;
    CUpnpGstWrapper* iGstWrapper;
    
    };

#endif // C_UPNPTRANSCODEHELPER_H
