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
* Description:      Class for retrieving and selecting media renderers
*
*/






#ifndef C_UPNPVALUESTATEMACHINE_H
#define C_UPNPVALUESTATEMACHINE_H

// INCLUDES
#include <e32base.h>
#include <mpxplaybackframeworkdefs.h>

// FORWARD DECLARATIONS
class CUPnPMusicPlayer;
class MUPnPAVRenderingSession;



/**
 * Class for handling playback state machine functionality
 *
 * @lib upnpmusicplugins.lib
 * @since S60 v3.1
 */
class CUPnPValueStateMachine : public CBase
    {

public: // construction / destruction

    /**
     * Static 1st phase constructor
     *
     * @since Series 60 3.1
     * @param aParent the parent plugin instance
     * @param aRenderingSession for playback commands
     */
    static CUPnPValueStateMachine* NewL( 
        CUPnPMusicPlayer& aParent,
        MUPnPAVRenderingSession& aRenderingSession );

    /**
     * Destructor
     *
     * @since Series 60 3.1
     */
    virtual ~CUPnPValueStateMachine();

public: // datatypes used in this class
   
    /**
     * current type of operation
     */
    enum TOperationType
        {
        EOperationNone,     // no operation going on
        EOperationValue,    // value get
        EOperationSet,      // value set
        EOperationDurationQuery, // internal duration query
        EOperationPositionQuery, // internal position query
        EOperationVolumeQuery, // internal volume query
        EOperationMute // internal mute
        };

    /**
     * An internal operation class
     */
    class TOperation
        {
        public:
        /** construction */
        TOperation( TOperationType aType )
            : iType( aType ) {}
        TOperation( TOperationType aType, TInt aValue )
            : iType( aType )
            , iValue( aValue ) {}
        TOperation( TMPXPlaybackProperty aProperty, TInt aValue )
            : iType( EOperationSet )
            , iProperty( aProperty )
            , iValue( aValue ) {}
        TOperation( TMPXPlaybackProperty aProperty )
            : iType( EOperationValue )
            , iProperty( aProperty ) {}

        /** setters */
        void operator=( TOperationType aType )
            {
            iType = aType;
            }
        void Set( TMPXPlaybackProperty aProperty, TInt aValue )
            {
            iType = EOperationSet;
            iProperty = aProperty;
            iValue = aValue;
            }
        void Set( TMPXPlaybackProperty aProperty )
            {
            iType = EOperationValue;
            iProperty = aProperty;
            }
        void Reset()
            {
            iType = EOperationNone;
            }
        /** comparision */
        TBool operator==( TOperationType aType )
            {
            return ( iType == aType );
            }
        TBool Compare( TOperationType aType, TMPXPlaybackProperty aProperty )
            {
            return ( iType == aType && iProperty == aProperty );
            }
        TBool None()
            {
            return ( iType == EOperationNone );
            }
        /** identifies the operation type */
        TOperationType iType;
        /** playback property, if op == Value or Set */
        TMPXPlaybackProperty iProperty;
        /** playback property value, if op == Set */
        TInt iValue;
        };

    /**
     * A result of validation query.
     * Indicates what to do with a Value- or a Set- call
     */
    enum TValidationResult
        {
        EHandle, // handle the call in value state machine
        EHandleStatic, // handle the call statically in v.s.m.
        EErrorNotReady, // async error (KErrNotReady)
        EErrorNotSupported, // async error (KErrNotSupported)
        EIgnore // do absolutely nothing
        };

public: // services
    
   /**
    * A method to copy some static renderer-specific values from
    * another instance of a state machine. Note that one instance is
    * track specific, so track specific cached values are NOT copied,
    * (track duration etc.) but renderer specific values are copied
    * (volume etc)
    * @param aOther another instance to copy values from
    */
    void CopyValues( const CUPnPValueStateMachine& aOther );

   /**
    * See CMPXPlaybackPlugin
    */
    void SetL( TMPXPlaybackProperty aProperty, TInt aValue );

   /**
    * See CMPXPlaybackPlugin
    */
    void ValueL( TMPXPlaybackProperty aProperty );
    
   /**
    * See MUPnPRenderiongSessionObserver
    */
    void VolumeResult( TInt aError, TInt aVolumeLevel,
        TBool aActionResponse );
    
   /**
    * See MUPnPRenderiongSessionObserver
    */
    void MuteResult( TInt aError, TBool aMute, TBool aActionResponse );
    
   /**
    * See MUPnPRenderiongSessionObserver
    */
    void PositionInfoResult( TInt aStatus, const TDesC8& aTrackPosition,
        const TDesC8& aTrackLength );

    /**
     * Initiates a duration request, which will be reported to
     * the plugin observer as a change event
     */
    void DurationQueryL();
    
    /**
     * Initiates a position request
     */
    void PositionQueryL();

    /**
     * Initiates a volume request, which will be reported to
     * the plugin observer as a change event
     */
    void VolumeQueryL();

    /**
     * Initiates an internal mute request, which will not be reported.
     * @param aMute if other than zero, mute will be activated
     */
    void MuteRequestL( TInt aMute );

    /**
     * Validates a Value- or Set- call with given property in given
     * player state. There will be a result indicating what to do with
     * the call.
     * @param aProperty the property to be either get or set
     * @param aSetting true if setting the value, false if getting
     * @param aPlayerState state of the player plugin
     * @param aIsReady is remote device to gíve playback information
     * @return result of validation
     */
    static TValidationResult ValidatePropertyInState(
        TMPXPlaybackProperty aProperty,
        TBool aSetting,
        TInt aPlayerState,
        TBool aIsReady );

    /**
     * A static version of Value query - this one is capable of handling
     * some fixed Value queries that do not need network resources
     */
    static void ValueStatic(
         TMPXPlaybackProperty aProperty,
         MMPXPlaybackPluginObserver& aPlaybackObs );

    /**
     * Cancels any ongoing operation
     * (actually it just resets the state so the result will be ignored
     * and not reported)
     */
    void Cancel();

private:

    /**
     * Private default constructor
     *
     * @since Series 60 3.1
     * @param aParent the parent plugin instance
     * @param aRenderingSession for playback commands
     */
    CUPnPValueStateMachine( 
        CUPnPMusicPlayer& aParent,
        MUPnPAVRenderingSession& aRenderingSession );

    /**
     * Checks if operations are in the queue, and executes
     */
    void CheckOperationInQueueL();
    
    /**
     * Check if given property are in queue
     * @param aProperty property to be searched
     * @return Etrue if found
     */
    TBool FoundFromQueue( TMPXPlaybackProperty aProperty );

private: // data
        
    /**
     * Parent plugin instance
     */ 
    CUPnPMusicPlayer&                   iParent;
    
    /**
     * Rendering session
     */ 
    MUPnPAVRenderingSession&            iRendererSession;
    
    /**
     * currently ongoing operation
     */
    TOperation                          iCurrentOperation;

    /**
     * queue of operations pending
     */
    RArray<TOperation>                  iOperationQueue;

    /**
     * current renderer volume level.
     * cached internally to avoid unnecessary message traffic.
     */
    TInt                                iRendererVolume;

    /**
     * current renderer muted state (1 = muted, 0 = normal)
     * cached internally to avoid unnecessary message traffic.
     */
    TInt                                iRendererMuted;

    };


#endif // C_UPNPVALUESTATEMACHINE_H

