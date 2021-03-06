/****************************************************************************
 *   Copyright (C) 2009-2016 by Savoir-faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#pragma once

// Std
#include <memory>

//Qt
#include <QtCore/QObject>
class QTimer;

// Ring
#include "call.h"
#include "libcard/matrixutils.h"
class Account;
class ContactMethod;
class UserActionModel;
class InstantMessagingModel;
class Certificate;

class CallPrivate;
typedef  void (CallPrivate::*function)();

namespace Media {
    class Media;
    class Recording;
}

class CallPrivate final : public QObject
{
    Q_OBJECT

public:
    friend class Call;

    ///@class ConferenceStateChange Possible values from "conferencechanged" signal
    class ConferenceStateChange {
    public:
        constexpr static const char* HOLD           = "HOLD"           ;
        constexpr static const char* ACTIVE         = "ACTIVE_ATTACHED";
        constexpr static const char* DETACHED       = "ACTIVE_DETACHED";
    };

    ///"getConferenceDetails()" fields
    class ConfDetailsMapFields {
    public:
        constexpr static const char* CONF_STATE        = "CONF_STATE"     ;
        constexpr static const char* CONFID            = "CONFID"         ;
    };

    ///If the call is incoming or outgoing
    class CallDirection {
    public:
        constexpr static const char* INCOMING = "0";
        constexpr static const char* OUTGOING = "1";
    };

    /** @enum DaemonState
    * This enum have all the states a call can take for the daemon.
    */
    enum class DaemonState : unsigned int
    {
        RINGING = 0, /*!< Ringing outgoing or incoming call         */
        CONNECTING,  /*!< Call connection progressing               */
        CURRENT,     /*!< Call to which the user can speak and hear */
        BUSY,        /*!< Call is busy                              */
        HOLD,        /*!< Call is on hold                           */
        HUNG_UP,     /*!< Call is over                              */
        FAILURE,     /*!< Call has failed                           */
        OVER,        /*!< Call is over                              */
        INACTIVE,    /*!< The call exist, but is not ready          */
        COUNT__,
    };

    explicit CallPrivate(Call* parent);
    virtual ~CallPrivate();

    //Attributes
    QString                   m_DringId;
    QString                   m_PeerName;
    FlagPack<Call::HoldFlags> m_fHoldFlags;
    QString                   m_FormattedDate;
    Call::State               m_CurrentState       {Call::State::ERROR       };
    Call::Type                m_Type               {Call::Type::CALL         };
    bool                      m_History            {           false         };
    QTimer*                   m_pTimer             {          nullptr        };
    UserActionModel*          m_pUserActionModel   {          nullptr        };
    Certificate*              m_pCertificate       {          nullptr        };
    Call*                     m_pParentCall        {          nullptr        };
    QDateTime*                m_pDateTime          {          nullptr        };
    QDate*                    m_pDateOnly          {          nullptr        };
    int                       m_LastErrorCode      {            200          };
    int                       m_VideoFrameCounter  {             0           };
    int                       m_UnholdCounter      {             0           };

    FlagPack<Call::LiveMediaIssues> m_fCurrentIssues {Call::LiveMediaIssues::OK};

    FlagPack<Media::Media::TypeFlags> m_DefaultMediaFlags {
        Media::Media::TypeFlags::VIDEO | Media::Media::TypeFlags::AUDIO
    };

    /**
     * The event/calendar APIs move the time keeping responsability away from
     * the call object. To make the transition progressive, these fields are
     * kept for now, but should be removed ASAP.
     */
    struct {
        time_t          m_pStartTimeStamp    {            0            };
        time_t          m_pStopTimeStamp     {            0            };
        bool            m_Missed             {          false          };
        Call::Direction m_Direction          {Call::Direction::OUTGOING};
        Account*        m_Account            {          nullptr        };
        ContactMethod*  m_pPeerContactMethod {          nullptr        };

        QSharedPointer<Event> m_pEvent {nullptr};
    } m_LegacyFields;

    //Cache
    HistoryTimeCategoryModel::HistoryConst m_HistoryConst {HistoryTimeCategoryModel::HistoryConst::Never};

    //State machine
    /**
        *  actionPerformedStateMap[orig_state][action]
        *  Map of the states to go to when the action action is
        *  performed on a call in state orig_state.
    **/
    static const TypedStateMachine< TypedStateMachine< Call::State , Call::Action > , Call::State > actionPerformedStateMap;

    /**
        *  actionPerformedFunctionMap[orig_state][action]
        *  Map of the functions to call when the action action is
        *  performed on a call in state orig_state.
    **/
    static const TypedStateMachine< TypedStateMachine< function , Call::Action > , Call::State > actionPerformedFunctionMap;

    /**
        *  stateChangedStateMap[orig_state][daemon_new_state]
        *  Map of the states to go to when the daemon sends the signal
        *  callStateChanged with arg daemon_new_state
        *  on a call in state orig_state.
    **/
    static const TypedStateMachine< TypedStateMachine< Call::State , DaemonState > , Call::State > stateChangedStateMap;

    /**
        *  stateChangedFunctionMap[orig_state][daemon_new_state]
        *  Map of the functions to call when the daemon sends the signal
        *  callStateChanged with arg daemon_new_state
        *  on a call in state orig_state.
    **/
    static const TypedStateMachine< TypedStateMachine< function , DaemonState > , Call::State > stateChangedFunctionMap;

    /**
        * metaStateTransitionValidationMap help validate if a state transition violate the lifecycle logic.
        * it should technically never happen, but this is an easy additional safety to implement
        * and prevent human (developer) errors.
        */
    static const TypedStateMachine< TypedStateMachine< bool , Call::LifeCycleState > , Call::State > metaStateTransitionValidationMap;

    /**
        * Convert the call state into its meta state (life cycle state). The meta state is a flat,
        * forward only progression from creating to archiving of a call.
        */
    static const TypedStateMachine< Call::LifeCycleState , Call::State > metaStateMap;

    Matrix2D<Media::Media::Type, Media::Media::Direction, QList<Media::Media*>* > m_mMedias {{{
        /*                                            IN                                                            OUT                           */
        /* AUDIO */ {{ new QList<Media::Media*>() /*Created lifecycle == progress*/, new QList<Media::Media*>() /*Created lifecycle == progress*/, new QList<Media::Media*>()}},
        /* VIDEO */ {{ new QList<Media::Media*>() /*On demand                    */, new QList<Media::Media*>() /*On demand                    */, new QList<Media::Media*>()}},
        /* TEXT  */ {{ new QList<Media::Media*>() /*On demand                    */, new QList<Media::Media*>() /*On demand                    */, new QList<Media::Media*>()}},
        /* FILE  */ {{ new QList<Media::Media*>() /*Not implemented              */, new QList<Media::Media*>() /*Not implemented              */, new QList<Media::Media*>()}},
    }}};

    Matrix2D<Media::Media::Type, Media::Media::Direction, QList<Media::Recording*>* > m_mRecordings {{{
        /*                           IN                            OUT                */
        /* AUDIO */ {{ new QList<Media::Recording*>(), new QList<Media::Recording*>(), new QList<Media::Recording*>()}},
        /* VIDEO */ {{ new QList<Media::Recording*>(), new QList<Media::Recording*>(), new QList<Media::Recording*>()}},
        /* TEXT  */ {{ new QList<Media::Recording*>(), new QList<Media::Recording*>(), new QList<Media::Recording*>()}},
        /* FILE  */ {{ new QList<Media::Recording*>(), new QList<Media::Recording*>(), new QList<Media::Recording*>()}},
    }}};

    Matrix2D<Media::Media::Type, Media::Media::Direction, bool > m_mIsRecording {{{
        /*              IN     OUT   */
        /* AUDIO */ {{ false, false, false }},
        /* VIDEO */ {{ false, false, false }},
        /* TEXT  */ {{ false, false, false }},
        /* FILE  */ {{ false, false, false }},
    }}};

    static const Matrix1D<Call::LifeCycleState,function> m_mLifeCycleStateChanges;

    static Call* buildHistoryCall  (const QMap<QString,QString>& hc);

    static DaemonState toDaemonCallState   (const QString& stateName);
    static Call::State       confStatetoCallState(const QString& stateName);
    Call::State stateChanged(const QString & newState);
    void performAction(Call::State previousState, Call::Action action);
    void performActionCallback(Call::State previousState, Call::Action action);

    //Automate functions
    // See actionPerformedFunctionMap and stateChangedFunctionMap
    // to know when it is called.
    void nothing           () __attribute__ ((const));
    void error             () __attribute__ ((noreturn));
    void failure           ();
    void accept            ();
    void refuse            ();
    void miss              ();
    void acceptTransf      ();
    void acceptHold        ();
    void hangUp            ();
    void cancel            ();
    void hold              ();
    void call              ();
    void transfer          ();
    void unhold            ();
    void toggleAudioRecord ();
    void toggleVideoRecord ();
    void start             ();
    void startStop         ();
    void stop              ();
    void startWeird        ();
    void warning           ();
    void remove            ();
    void abort             ();
    void sendProfile       ();

    //LifeCycleState change callback
    void initMedia();
    void notifyInit();
    void terminateMedia();

    //Helpers
    void changeCurrentState(Call::State newState);
    void setStartTimeStamp(time_t stamp);
    void setStartTimeStamp(); // this version set stamp to current time
    void initTimer();
    void registerRenderer(Video::Renderer* renderer);
    void removeRenderer(Video::Renderer* renderer);
    void setRecordingPath(const QString& path);
    static MapStringString getCallDetailsCommon(const QString& callId);
    void peerHoldChanged(bool onPeerHold);
    template<typename T>
    T* mediaFactory(Media::Media::Direction dir);
    void updateOutgoingMedia(const MapStringString& details);

    //Static getters
    static Call::State        startStateFromDaemonCallState ( const QString& daemonCallState, const QString& daemonCallType );

    //Constructor
    static Call* buildDialingCall  (const QString & peerName, Account* account = nullptr, Call* parent = nullptr );
    static Call* buildIncomingCall (const QString& callId                                );
    static Call* buildExistingCall (const QString& callId                                );

private:
    Call* q_ptr;

    //Constructor helper
    static Call* buildCall(const QString& callId, Call::Direction callDirection, Call::State startState);

    //Destructor helper (~Call is private, CallPrivate is a friend class)
    static void deleteCall(Call* call);

    // Used as contact until m_pPeerContactMethod is created
    // Owner is PhoneDirectoryModel instance
    TemporaryContactMethod* m_pDialNumber {nullptr};

    // Owner is PhoneDirectoryModel instance
    TemporaryContactMethod* m_pTransferNumber {nullptr};

private Q_SLOTS:
    void updated();
    void videoStopped();
    void slotFrameAcquired();
};
