/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      Capsulating property container objects
*
*/






#ifndef __CMSQLPROPERTYCOLLECTOR_H
#define __CMSQLPROPERTYCOLLECTOR_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class CCmSqlPropertyContainer;

/**
 *  CCmSqlPropertyCollector class
 *  Capsulating property item objects
 *  @lib cmcommon.lib
 *  @since S60 v3.1
 */
class CCmSqlPropertyCollector : public CBase
    {

    public:

        /* Constructors and destructor. */

        /**
         * Creates new CCmSqlPropertyCollector class.
         * @param None
         * @return  pointer to CCmSqlPropertyCollector class
         */
        IMPORT_C static CCmSqlPropertyCollector* NewL();

        /**
         * Creates new CCmSqlPropertyCollector class and
         * leaves the instance in the cleanup stack.
         * @return  pointer to CCmSqlPropertyCollector class
         */
        IMPORT_C static CCmSqlPropertyCollector* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C virtual ~CCmSqlPropertyCollector();

    public:

        /**
         *
         * @since Series 60 3.1
         * @param CCmSqlPropertyContainer*
         * @return TInt
        */
        IMPORT_C TInt AddPropertyContainerL(
                        CCmSqlPropertyContainer* aContainer );

        /**
         *
         * @since Series 60 3.1
         * @param aIndex
         */
        IMPORT_C void DeletePropertyContainer( TInt aIndex );

        /**
         *
         * @since Series 60 3.1
         * @param aIndex
         * @return CCmSqlPropertyContainer*
         */
        IMPORT_C CCmSqlPropertyContainer* PropertyContainer( TInt aIndex );

        /**
         *
         * @since Series 60 3.1
         * @return TInt
         */
        IMPORT_C TInt PropertyContainerCount() const;

    public:

        /**
         * Externalizes container information to stream.
         * Leaves in case of errors.
         * @since Series 60 3.1
         * @param reference to RWriteStream
         * @return none
         */
        IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

        /**
         * Internalizes container information from stream.
         * Leaves in case of errors.
         * @since Series 60 3.1
         * @param reference to RReadStream
         * @return none
         */
        IMPORT_C void InternalizeL( RReadStream& aStream );

    protected:

        /**
         * Constructor.
         */
        CCmSqlPropertyCollector();

        /**
         * Second-phase constructor.
         */
        void ConstructL();


    private: // data
     
        /**
         * array of metadata containers
         */
        RPointerArray<CCmSqlPropertyContainer> iPropertyContainers;
    };


#endif //  __CMSQLPROPERTYCOLLECTOR_H