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

#include <itembase.h>
#include <time.h>

//Qt
#include <QtCore/QDebug>
class QString;
class QTimer;

//Ring
#include "typedefs.h"
#include "historytimecategorymodel.h"
#include "media/media.h"
#include "itemdataroles.h"
class Account               ;
class UserActionModel       ;
class ContactMethod         ;
class TemporaryContactMethod;
class CollectionInterface   ;
class Certificate           ;
class Person                ;
class Event                 ;
class Individual            ;

namespace Video {
   class Manager;
   class Renderer;
   class ManagerPrivate;
   class SourceModel;
}

namespace Media {
   class Media;
   class Audio;
   class Video;
   class Text;
   class Recording;
}

class Call;

//Private
class CallPrivate;

namespace RingMimes
{
   QMimeData* payload(const Call*, const ContactMethod*, const Person*);
}


/**
 * This class represent a call object from a client perspective. It is
 * fully stateful and has all properties required for a client. This object
 * is created by the CallModel class and its state can be modified by sending
 * Call::Action to the call using the '<<' operator.
 *
 * History calls will have the Call::State::OVER set by default. The LifeCycleState
 * system is designed to ensure that the call never go backward in its expected
 * lifecycle and should be used instead of "if"/"switch" on individual states
 * when possible. This will avoid accidentally forgetting a state.
**/
class  LIB_EXPORT Call : public ItemBase
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   friend class CallModel            ;
   friend class CategorizedHistoryModel;
   friend class CallModelPrivate     ;
   friend class IMConversationManager;
   friend class VideoRendererManager;
   friend class VideoRendererManagerPrivate;
   friend class Media::Media;
   friend class Media::Audio;
   friend class Media::Video;
   friend class Media::Text;
   friend class MediaTypeInference;
   friend class IMConversationManagerPrivate;
   friend class Calendar; // Manage the events
   friend QMimeData* RingMimes::payload(const Call*, const ContactMethod*, const Person*);

   //Enum

   ///Model roles
   enum class Role {
      Name               = static_cast<int>(Ring::Role::UserRole) + 100, /*!< The peer name from SIP or Persons */
      Number             , /*!< The peer URI / phone number (as text)                               */
      Direction          , /*!<                                                                     */
      Date               , /*!< The date when the call started                                      */
      Length             , /*!< The current length of the call                                      */
      FormattedDate      , /*!< An human readable starting date                                     */
      Historystate       , /*!<                                                                     */
      Filter             , /*!<                                                                     */
      FuzzyDate          , /*!<                                                                     */
      IsBookmark         , /*!<                                                                     */
      Security           , /*!<                                                                     */
      Department         , /*!<                                                                     */
      Email              , /*!<                                                                     */
      Organisation       , /*!<                                                                     */
      HasAVRecording     , /*!<                                                                     */
      Object             , /*!<                                                                     */
      Photo              , /*!<                                                                     */
      State              , /*!<                                                                     */
      StartTime          , /*!<                                                                     */
      StopTime           , /*!<                                                                     */
      IsAVRecording      , /*!<                                                                     */
      ContactMethod      , /*!<                                                                     */
      IsPresent          , /*!<                                                                     */
      SupportPresence    , /*!<                                                                     */
      IsTracked          , /*!<                                                                     */
      CategoryIcon       , /*!<                                                                     */
      CategoryName       , /*!<                                                                     */
      CallCount          , /*!< The number of calls made with the same phone number                 */
      TotalSpentTime     , /*!< The total time spent speaking to with this phone number             */
      Missed             , /*!< This call has been missed                                           */
      LifeCycleState     , /*!<                                                                     */
      Certificate        , /*!< The certificate (for encrypted calls)                               */
      HasAudioRecording  , /*!<                                                                     */
      HasVideoRecording  , /*!<                                                                     */
      HumanStateName     , /*!<                                                                     */
      DTMFAnimState      , /*!< GUI related state to hold animation key(s)                          */
      LastDTMFidx        , /*!< The last DTMF (button) sent on this call                            */
      DropPosition       , /*!< GUI related state to keep track of metadata during drag and drop    */
      AudioRecording     , /*!< The Media::AVRecording object or null                               */
      IsConference       , /*!< If the call is a conference (for QML convenience)                   */
      DateOnly           ,
      DateTime           ,
      SecurityLevel      , //TODO REMOVE use the extensions
      SecurityLevelIcon  , //TODO REMOVE use the extensions
      DropState          = static_cast<int>(Ring::Role::DropState), /*!< GUI related state to keep track of metadata during drag and drop */
   };

   ///Possible call states
   enum class State : unsigned int{
      NEW             = 0, /*!< The call has been created, but no dialing number been set                         */
      INCOMING        = 1, /*!< Ringing incoming call                                                             */
      RINGING         = 2, /*!< Ringing outgoing call                                                             */
      CURRENT         = 3, /*!< Call to which the user can speak and hear                                         */
      DIALING         = 4, /*!< Call which numbers are being added by the user                                    */
      HOLD            = 5, /*!< Call is on hold by this side of the communication, @see Call::HoldFlags           */
      FAILURE         = 6, /*!< Call has failed                                                                   */
      BUSY            = 7, /*!< Call is busy                                                                      */
      TRANSFERRED     = 8, /*!< Call is being transferred.  During this state, the user can enter the new number. */
      TRANSF_HOLD     = 9, /*!< Call is on hold for transfer                                                      */
      OVER            = 10,/*!< Call is over and should not be used                                               */
      ERROR           = 11,/*!< This state should never be reached                                                */
      CONFERENCE      = 12,/*!< This call is the current conference                                               */
      CONFERENCE_HOLD = 13,/*!< This call is a conference on hold                                                 */
      INITIALIZATION  = 14,/*!< The call have been placed, but the peer hasn't confirmed yet                      */
      ABORTED         = 15,/*!< The call was dropped before being sent to the daemon                              */
      CONNECTED       = 16,/*!< The peer has been found, attempting negotiation                                   */
      COUNT__,
   };
   Q_ENUMS(State)

   ///DEPRECATED @enum Direction If the user have been called or have called
   enum class Direction : int {
      INCOMING, /*!< Someone has called      */
      OUTGOING, /*!< The user called someone */
   };
   Q_ENUMS(Direction)

   ///Is the call between one or more participants
   enum class Type {
      CALL      , /*!< A simple call                  */
      CONFERENCE, /*!< A composition of other calls   */
      HISTORY   , /*!< A call from a previous session */
   };

   /** @enum Call::Action
   * This enum have all the actions you can make on a call.
   */
   enum class Action : unsigned int
   {
      ACCEPT       = 0, /*!< Accept, create or place call or place transfer */
      REFUSE       = 1, /*!< Red button, refuse or hang up                  */
      TRANSFER     = 2, /*!< Put into or out of transfer mode               */
      HOLD         = 3, /*!< Hold or unhold the call                        */
      RECORD_AUDIO = 4, /*!< Enable or disable audio recording              */
      RECORD_VIDEO = 5, /*!< Enable or disable video recording              */
      RECORD_TEXT  = 6, /*!< Enable or disable text  recording              */
      COUNT__,
   };
   Q_ENUMS(Action)

   /** @enum Call::LifeCycleState
    * This enum help track the call meta state
    * @todo Eventually add a meta state between progress and finished for
    *  calls that are still relevant enough to be in the main UI, such
    *  as BUSY OR FAILURE while also finished
    */
   enum class LifeCycleState {
      CREATION       = 0, /*!< Anything before creating the daemon call   */
      INITIALIZATION = 1, /*!< Anything before the media transfer start   */
      PROGRESS       = 2, /*!< The peers are in communication (or hold)   */
      FINISHED       = 3, /*!< Everything is over, there is no going back */
      COUNT__
   };
   Q_ENUMS(LifeCycleState)

   /// Categories of (live) Quality of Service issues
   enum class LiveMediaIssues : uchar {
      OK                       = 0x0 << 0, /*!< Everything is well                                */
      VIDEO_ACQUISITION_FAILED = 0x1 << 0, /*!< Failed to get video frames for 5 seconds          */
      HIGH_PACKET_LOSS         = 0x1 << 1, /*!< There more than 15 lost packet per report         */
      RUNAWAY_PACKET_LOSS      = 0x1 << 2, /*!< There is more and more packet less in each report */
      HIGH_LATENCY             = 0x1 << 3, /*!< The latency is non-optimal                        */
      RUNAWAY_LATENCY          = 0x1 << 4, /*!< The latency is increasing                         */
      UNHOLD_FAILED            = 0x1 << 5, /*!< The unholding does nothing after 5 seconds        */
   };
   Q_FLAGS(LiveMediaIssues)

   /** @enum Call::HoldFlags
    * Those flags help track the holding state of a call. Call::State::HOLD is
    * only used for outgoing holding.
    */
   enum class HoldFlags {
      NONE = 0x0 << 0, /*!< The call is not on hold        */
      OUT  = 0x1 << 0, /*!< This side put the peer on hold */
      IN   = 0x1 << 1, /*!< The peer put this side on hold */
      COUNT__
   };
   Q_FLAGS(HoldFlags)

   ///TODO should be deprecated when a better factory system is implemented
   class HistoryMapFields {
   public:
      static const QString ACCOUNT_ID      ;
      static const QString CALLID          ;
      static const QString DISPLAY_NAME    ;
      static const QString PEER_NUMBER     ;
      static const QString RECORDING_PATH  ;
      static const QString STATE           ;
      static const QString TIMESTAMP_START ;
      static const QString TIMESTAMP_STOP  ;
      static const QString MISSED          ;
      static const QString DIRECTION       ;
      static const QString CONTACT_USED    ;
      static const QString CONTACT_UID     ;
      static const QString NUMBER_TYPE     ;
      static const QString CERT_PATH       ;
   };

   //TODO should be deprecated when a better factory system is implemented
   ///@class HistoryStateName history map fields state names
   class HistoryStateName {
   public:
      constexpr static const char* MISSED         = "missed"  ;
      constexpr static const char* INCOMING       = "incoming";
      constexpr static const char* OUTGOING       = "outgoing";
   };

   //Read only properties
   Q_PROPERTY( Call::State        state              READ state             NOTIFY stateChanged     )
   Q_PROPERTY( Call::LifeCycleState lifeCycleState   READ lifeCycleState    NOTIFY stateChanged     )
   Q_PROPERTY( QString            historyId          READ historyId                                 )
   Q_PROPERTY( Account*           account            READ account                                   )
   Q_PROPERTY( bool               isHistory          READ isHistory                                 )
   Q_PROPERTY( uint               stopTimeStamp      READ stopTimeStamp                             )
   Q_PROPERTY( uint               startTimeStamp     READ startTimeStamp                            )
   Q_PROPERTY( bool               isSecure           READ isSecure                                  )
   Q_PROPERTY( Video::Renderer*   videoRenderer      READ videoRenderer     NOTIFY videoStarted     )
   Q_PROPERTY( QString            formattedName      READ formattedName                             )
   Q_PROPERTY( QString            length             READ length            NOTIFY lengthChanged    )
   Q_PROPERTY( bool               recordingAV        READ isAVRecording     NOTIFY recordingChanged )
   Q_PROPERTY( UserActionModel*   userActionModel    READ userActionModel   CONSTANT                )
   Q_PROPERTY( QString            toHumanStateName   READ toHumanStateName  NOTIFY stateChanged     )
   Q_PROPERTY( bool               missed             READ isMissed          NOTIFY changed          )
   Q_PROPERTY( Direction          direction          READ direction         CONSTANT                )
   Q_PROPERTY( bool               hasVideo           READ hasVideo                                  )
   Q_PROPERTY( Certificate*       certificate        READ certificate       CONSTANT                )
   Q_PROPERTY( bool               hasParentCall      READ hasParentCall                             )
   Q_PROPERTY( int                lastErrorCode      READ lastErrorCode     NOTIFY errorChanged     )
   Q_PROPERTY( QString            lastErrorMessage   READ lastErrorMessage  NOTIFY errorChanged     )
   Q_PROPERTY( Video::SourceModel* sourceModel       READ sourceModel       NOTIFY mediaAdded       )
   Q_PROPERTY( QSharedPointer<Event> calendarEvent   READ calendarEvent     CONSTANT                )

   //Read/write properties
   Q_PROPERTY( ContactMethod*     peerContactMethod  READ peerContactMethod WRITE setPeerContactMethod NOTIFY dialNumberChanged)
   Q_PROPERTY( QString            peerName           READ peerName          WRITE setPeerName       )
   Q_PROPERTY( QString            transferNumber     READ transferNumber    WRITE setTransferNumber )
   Q_PROPERTY( QString            dialNumber         READ dialNumber        WRITE setDialNumber      NOTIFY dialNumberChanged(QString))
   Q_PROPERTY( Individual*        peer               READ peer              NOTIFY dialNumberChanged)

   //Constructors & Destructors
   static Call* buildHistoryCall  (const QMap<QStringRef,QStringRef>& hc);

   //Static getters
   static const QString      toHumanStateName ( const Call::State );

   //Getters
   Call::State              state            () const;
   const QString            historyId        () const;
   ContactMethod*           peerContactMethod() const;
   const QString            peerName         () const;
   bool                     isAVRecording    () const;
   Account*                 account          () const;
   bool                     isHistory        () const;
   time_t                   stopTimeStamp    () const;
   time_t                   startTimeStamp   () const;
   bool                     isSecure         () const;
   const QString            transferNumber   () const;
   const QString            dialNumber       () const;
   Video::Renderer*         videoRenderer    () const;
   const QString            formattedName    () const;
   QString                  length           () const;
   UserActionModel*         userActionModel  () const;
   QString                  toHumanStateName () const;
   bool                     isMissed         () const;
   Call::Direction          direction        () const;
   bool                     hasVideo         () const;
   Call::LifeCycleState     lifeCycleState   () const;
   Call::Type               type             () const;
   bool                     hasRemote        () const;
   Certificate*             certificate      () const;
   FlagPack<HoldFlags>      holdFlags        () const;
   bool                     hasParentCall    () const;
   int                      lastErrorCode    () const;
   QString                  lastErrorMessage () const;
   QDateTime                dateTime         () const;
   QDate                    date             () const;
   Video::SourceModel*      sourceModel      () const;
   QSharedPointer<Event>    calendarEvent    () const;
   Individual*              peer             () const;

   FlagPack<Call::LiveMediaIssues>   liveMediaIssues  () const;
   FlagPack<Media::Media::TypeFlags> defaultMediaFlags() const;

   Q_INVOKABLE void removeMedia(Media::Media::Type m);
   Q_INVOKABLE void addMedia(Media::Media::Type m);

   Q_INVOKABLE QVariant   roleData         (int  role) const;
   Q_INVOKABLE QVariant   roleData         (Role role) const;
   Q_INVOKABLE QMimeData* mimePayload      (         ) const;

   Q_INVOKABLE bool hasIssue(Call::LiveMediaIssues issue) const;

   template<typename T>
   T* firstMedia(Media::Media::Direction direction) const;
   const QList<Media::Recording*> recordings  (Media::Media::Type type, Media::Media::Direction direction) const;
   const QList<Media::Media*>& media       (Media::Media::Type type, Media::Media::Direction direction) const;
   bool                     hasMedia    (Media::Media::Type type, Media::Media::Direction direction) const;
   bool                     hasRecording(Media::Media::Type type, Media::Media::Direction direction) const;
   bool                     isRecording (Media::Media::Type type, Media::Media::Direction direction) const;
   QList<Media::Media*>     allMedia    (                                                          ) const;

   //Automated function
   Q_INVOKABLE Call::State performAction(Call::Action action);

   //Setters
   void setTransferNumber ( const QString&     number     );
   void setDialNumber     ( const QString&     number     );
   void setDialNumber     ( const ContactMethod* number   );
   void setPeerContactMethod( ContactMethod* cm           );
   void setPeerName       ( const QString&     name       );
   void setAccount        ( Account*           account    );
   void setParentCall     ( Call*               call      );

   //Mutators
   template<typename T>
   T* addOutgoingMedia(bool useExisting = true);
   Q_INVOKABLE void appendText(const QString& str);
   Q_INVOKABLE void backspaceItemText();
   void reset();
   bool joinToParent();

   //syntactic sugar
   Call* operator<<( Call::Action& c);

private:
   Call(const QString& confId, const QString& account);
   virtual ~Call();
   explicit Call(Call::State startState, const QString& peerName = QString(), ContactMethod* number = nullptr, Account* account = nullptr); //TODO MOVE TO PRIVATE

   //Friend API
   const QString dringId() const;

   CallPrivate* d_ptr;
   Q_DECLARE_PRIVATE(Call)

public Q_SLOTS:
   void playDTMF(const QString& str);

Q_SIGNALS:
   ///Emitted when a call change (state or details)
   void changed();
   ///Emitted when the call is over
   void isOver();
   ///Notify that a DTMF have been played
   void dtmfPlayed(const QString& str);
   ///Notify of state change
   void stateChanged(Call::State newState, Call::State previousState);
   ///Notify that the lifeCycleStateChanged
   void lifeCycleStateChanged(Call::LifeCycleState newState, Call::LifeCycleState previousState);
   ///The call start timestamp changed, this usually indicate the call has started
   void startTimeStampChanged(time_t newTimeStamp);
   ///The dial number has changed
   void dialNumberChanged(const QString& number);
   ///Announce a new video renderer
   void videoStarted(Video::Renderer* renderer); //TODO remove, use the media signals
   ///Remove a new video renderer
   void videoStopped(Video::Renderer* renderer); //TODO remove, use the media signals
   ///Notify when a media is added
   void mediaAdded(Media::Media* media);
   ///Notify when a media state change
   void mediaStateChanged(Media::Media* media, const Media::Media::State s, const Media::Media::State m);
   ///The holding combination has changed
   void holdFlagsChanged(const FlagPack<HoldFlags>& current, const FlagPack<HoldFlags>& previous);
   /// When the last error code changed
   void errorChanged();
   /// Notify when the live call is having problems
   void liveMediaIssuesChanaged(const FlagPack<Call::LiveMediaIssues> issues);
   /// Void length changed
   void lengthChanged();
   /// When one of the recording changed
   void recordingChanged();
};

Q_DECLARE_METATYPE(Call*)
Q_DECLARE_METATYPE(Call::State)
Q_DECLARE_METATYPE(Call::Type)
Q_DECLARE_METATYPE(Call::Action)
Q_DECLARE_METATYPE(Call::Direction)
Q_DECLARE_METATYPE(Call::LifeCycleState)

DECLARE_ENUM_FLAGS(Call::HoldFlags)
DECLARE_ENUM_FLAGS(Call::LiveMediaIssues)

LIB_EXPORT Call* operator<<(Call* c, Call::Action a);
LIB_EXPORT QDebug operator<<(QDebug dbg, const Call::State& c       );
LIB_EXPORT QDebug operator<<(QDebug dbg, const Call::Action& c      );

#include <call.hpp>
