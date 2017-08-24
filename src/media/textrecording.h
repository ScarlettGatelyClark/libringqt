/****************************************************************************
 *   Copyright (C) 2015-2016 by Savoir-faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#include "media/recording.h"
#include "media/media.h"
#include "itemdataroles.h"

//Qt
class QJsonObject;
class QAbstractItemModel;

//Ring
class IMConversationManagerPrivate;
class LocalTextRecordingEditor;
class ContactMethod;
class PeerTimelineModel;
class InstantMessagingModel;
class PeerTimelineModelPrivate;

namespace Media {

class TextRecordingPrivate;
class Text;

class LIB_EXPORT TextRecording : public Recording
{
   Q_OBJECT

   //InstantMessagingModel is a view on top of TextRecording data
   friend class ::InstantMessagingModel;
   friend class ::IMConversationManagerPrivate;
   friend class ::LocalTextRecordingEditor;
   friend class Text;
   friend class Video;
   friend class ::ContactMethod;
   friend class ::PeerTimelineModel;
   friend class ::PeerTimelineModelPrivate;

public:

    Q_PROPERTY(QAbstractItemModel* instantMessagingModel           READ instantMessagingModel           CONSTANT)
    Q_PROPERTY(QAbstractItemModel* instantTextMessagingModel       READ instantTextMessagingModel       CONSTANT)
    Q_PROPERTY(QAbstractItemModel* unreadInstantTextMessagingModel READ unreadInstantTextMessagingModel CONSTANT)
    Q_PROPERTY(bool                isEmpty                         READ isEmpty           NOTIFY messageInserted)
    Q_PROPERTY(int                 size                            READ size              NOTIFY messageInserted)

   enum class Role {
      Direction            = static_cast<int>(Ring::Role::UserRole) + 1,
      AuthorDisplayname    ,
      AuthorUri            ,
      AuthorPresenceStatus ,
      Timestamp            ,
      IsRead               ,
      FormattedDate        ,
      IsStatus             ,
      HTML                 ,
      HasText              ,
      ContactMethod        ,
      DeliveryStatus       ,
      FormattedHtml        ,
      LinkList             ,
      Id                   ,
   };

    ///Possible messages states
    ///Order is important and reflected on order in Daemon
    enum class MessageStatus : unsigned int{
        UNKNOWN = 0,
        SENDING,
        SENT,
        READ,
        FAILURE,
        COUNT__,
    };
    Q_ENUMS(Status)

   //Constructor
   explicit TextRecording(const Recording::Status status);
   virtual ~TextRecording();
   static TextRecording* fromJson(const QList<QJsonObject>& items, const ContactMethod* cm = nullptr, CollectionInterface* backend = nullptr);

   //Getter
   QAbstractItemModel* instantMessagingModel    (                         ) const;
   QAbstractItemModel* instantTextMessagingModel(                         ) const;
   QAbstractItemModel* unreadInstantTextMessagingModel(                   ) const;
   bool                isEmpty                  (                         ) const;
   int                 count                    (                         ) const;
   int                 size                     (                         ) const;
   bool                hasMimeType              ( const QString& mimeType ) const;
   QStringList         mimeTypes                (                         ) const;
   QVariant            roleData                 ( int row, int role       ) const;
   virtual QVariant    roleData                 ( int role                ) const override;
   QVector<ContactMethod*> peers                (                         ) const;

   //Helper
   void setAllRead();

Q_SIGNALS:
   void messageInserted(const QMap<QString,QString>& message, ContactMethod* cm, Media::Media::Direction direction);
   void unreadCountChange(int count);

private:
   TextRecordingPrivate* d_ptr;
   Q_DECLARE_PRIVATE(TextRecording)
};

}

Q_DECLARE_METATYPE(Media::TextRecording*)
Q_DECLARE_METATYPE(Media::TextRecording::MessageStatus)
