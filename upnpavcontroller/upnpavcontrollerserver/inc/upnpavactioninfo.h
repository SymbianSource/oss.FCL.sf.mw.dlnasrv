/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      callback dispatcher helper class
*
*/






#ifndef C_CUPNPAVACTIONINFO_H
#define C_CUPNPAVACTIONINFO_H


#include <e32base.h>

// FORWARD DECLARATIONS
class MUpnpAVControlPointObserver;
class CUPnPAVControllerImpl;

// CLASS DECLARATION

/**
*  UPnP AV Controller server, callback dispatcher helper class
*  
*
*  @lib - 
*  @since Series 60 3.1
*/
class CUPnPAVActionInfo : public CBase
    {
public:  // Constructors and destructor
        
    /**
     * Two-phased constructor.
     */
    static CUPnPAVActionInfo* NewLC();
    
    /**
     * Two-phased constructor.
     */
    static CUPnPAVActionInfo* NewL();    

    /**
     * Destructor.
     */
    virtual ~CUPnPAVActionInfo();

private: //

    /**
     * Constructs the action info
     */
    CUPnPAVActionInfo( );

    /**
     * Perform the second phase construction
     */
    void ConstructL();

public: // New methods

    /**
     * Sets session id
     * @param aSessionid session id
     */
    void SetSessionId( TInt aSessionId );
    
    /**
     * Returns session id
     * @return session id
     */
    TInt SessionId() const;
    
    /**
     * Sets AV Control Point observer
     * @param aObserver AV Control Point observer
     */
    void SetObserver( MUpnpAVControlPointObserver& aObserver );
    
    /**
     * Returns AV Control Point observer
     * @return AV Control Point observer
     */
    MUpnpAVControlPointObserver& Observer() const;
    
    /**
     * Sets Uuid
     * @param Uuid
     */
    void SetUuidL( const TDesC8& aUuid );
    
    /**
     * Returns Uuid
     * @return Uuid
     */
    const TDesC8& Uuid() const;
    
private:
    
    TInt                            iSessionId;
    
    MUpnpAVControlPointObserver*    iObserver; // Not own
    
    HBufC8*                         iUuid; // Own
        
    };


#endif // C_CUPNPAVACTIONINFO_H