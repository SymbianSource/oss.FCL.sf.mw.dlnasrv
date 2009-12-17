/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for the CUpnpFilePipe class.
*
*/


#ifndef UPNP_FILE_PIPE_H
#define UPNP_FILE_PIPE_H

// INCLUDES
#include <e32base.h>

/**
* This class works as a pipe-like storage class.
*
* @since S60 3.2
*/
class CUpnpFilePipe : public CBase
    {
    public: // Instantiation methods

        /**
         * Creates a new file pipe with unlimited length.
         *
         * @since S60 3.2
         * @return instance of this class.
         */
        static CUpnpFilePipe* NewL();

        /**
         * Creates a new file pipe with a set length. In this case, when the
         * pipe if full and client tries to push more files into the pipe, 
         * the newest file in the pipe (the last file pushed into the pipe)
         * will be replaced.
         *
         * @since S60 3.2
         * @param aMaxSize (TInt) maximum length of the pipe.
         * @return instance of this class.
         */
        static CUpnpFilePipe* NewL( TInt aMaxSize );

        /**
         * Destructor.
         */
        virtual ~CUpnpFilePipe();

    public: // Business logic methods

        /**
         * Pushes one file into the pipe.
         *
         * Leaves if the given parameter is not valid.
         *
         * @since S60 3.2
         * @param aParamvalue (const TDesC&) filename
         */
        void PushL( const TDesC& aParameter );

        /**
         * Returns a file from the queue at given index
         *
         * @param aIndex the index at queue
         * @return reference to the file name
         */
        const TDesC& FileAt( TInt aIndex );

        /**
         * Empties the pipe.
         *
         * @since S60 3.2
         */
        void Reset();

        /**
         * Returns the count of the items in the pipe.
         *
         * @since S60 3.2
         * @return TInt the item count
         */
        TInt Count();

        /**
         * returns the entire pipe as an array
         */
        RPointerArray<TDesC>& AsArray();

    private: // Private business logic methods

        /**
         * Removes the newest file from the pipe.
         *
         * @since S60 3.2
         */
        void RemoveNewestFileFromPipe();

    private: // Private construction methods

        /**
         * Perform the first phase of two phase construction.
         *
         * @since S60 3.2
         * @param aMaxSize (TInt) maximum length of the pipe.
         */
        CUpnpFilePipe( TInt aMaxSize );

    private: // Data members

        /**
         * The array storing the file pipe files (filenames). Owned.
         */
        RPointerArray<TDesC>  iValues;

        /**
         * The maximum size of the pipe. 0 if unlimited.
         */
        TInt                    iMaxSize;

    };

#endif // UPNP_FILE_PIPE_H

// End of File