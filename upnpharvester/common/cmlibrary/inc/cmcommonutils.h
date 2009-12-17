/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Utilities class description
*
*/






#ifndef CMCOMMONUTILS_H
#define CMCOMMONUTILS_H

#include <e32base.h>

class CEikonEnv;
/**
 * CmCommonUtils class.
 *
 * Class provides common methods
 *
 * @since S60 3.2
 * @lib cmlibrary.lib
 */
class CmCommonUtils : CBase
    {
    public:
    
        /**
         * Loads resource file
         *
         * @since S60 3.2
         * @param aFilePath
         * @return TInt, resource file offset
         */
        IMPORT_C static TInt LoadResourceFileL( const TDesC& aFilePath,
                                                CEikonEnv& eikonEnv ); 

        /**
         * Sets wlan scan interval
         * @since S60 3.2
         *
         * @param aInterval, scan interval         
         */
        IMPORT_C static void SetWlanScanL( const TInt aInterval );                                                
    
    };
                                
#endif // CMCOMMONUTILS_H                                