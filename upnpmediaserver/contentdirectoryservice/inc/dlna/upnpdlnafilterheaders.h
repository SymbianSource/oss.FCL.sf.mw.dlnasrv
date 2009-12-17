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
 * Description:  CUpnpDlnaFilterHeaders declaration.
 *
 */

#ifndef UPNPDLNAFILTERHEADERS_H
#define UPNPDLNAFILTERHEADERS_H

#include <e32std.h>
#include <e32base.h>

class CUpnpHttpMessage;
class TInetAddr;

NONSHARABLE_CLASS( CUpnpDlnaFilterHeaders ) : public CBase
    {
public: 
	
    static CUpnpDlnaFilterHeaders* NewL(); 
    
    /**
     * Standard Destructor
     */
	~CUpnpDlnaFilterHeaders();

    /**
     * Adds header to temporary held CUpnpHttpMessage instance
     */
    void AddHeaderL( const TDesC8& aName, const TDesC8& aValue );
    
    /**
     * Remove header from temporary held CUpnpHttpMessage instance
     */
    void RemoveHeaderL( const TDesC8& aName );
    
    /**
     * Get specified by name header value
     */    
    TDesC8& QueryHeader( const TDesC8& aHeaderName );
 
    /**
     * Retrieves sender IP address from temporary CUpnpHttpMessage
     */    
    TInetAddr& Sender();

private:
    /**
     * Constructor
     */
    CUpnpDlnaFilterHeaders();
    
    /**
     * Second phase constructor 
     */
    void ConstructL();
    
private:
	// Single CUpnpHttpMessage instance to hold temporary headers and 
	// sender IP
    CUpnpHttpMessage* iHeaderContainer;
    }; 


#endif // UPNPDLNAFILTERHEADERS_H
// End of File
