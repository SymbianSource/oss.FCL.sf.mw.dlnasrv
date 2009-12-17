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
* Description:  MUpnpResolverObserver
*
*/


#ifndef C_CUPNPRESOLVEROBSERVER_H
#define C_CUPNPRESOLVEROBSERVER_H

// FORWARD DECLARATIONS
class CUpnpPathElement;

// CLASS DECLARATION
/**
*  This observer class has to be implemented to receive notification about result of path resolving.
*
*  @see CUpnpPathResolver
*  @see ResolveHandler
*  @since Series 60 3.1
*/
class MUpnpResolverObserver
	{
	public: // New functions
		/**
		* Method invoke when resolving process has finished and result are ready.
		* @since Series 60 3.1
		* @param aErrCode status of resolving, if ok, value equal KErrNone
		* @param aPathElement object describing searched element
		*/
		virtual void ResolvedIdL(TInt aErrCode, CUpnpPathElement* aPathElement) = 0;
	};

#endif // C_CUPNPRESOLVEROBSERVER_H
