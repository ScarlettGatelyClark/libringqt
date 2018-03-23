/****************************************************************************
 *   Copyright (C) 2015-2016 by Savoir-faire Linux                          *
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

// Qt
#include <QtCore/QString>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QSharedPointer>

// Ring
class ContactMethod;
class Event;
class Account;
#include <media/mimemessage.h>

/*
 * This set of classes help manage a JSON database used to index all known text
 * recording files.
 */

class SerializableEntityManager;

//BEGIN Those classes are serializable to JSon
/**
 * Those classes map 1:1 to the json stored on the disk. References are then
 * extracted, the conversation reconstructed and placed into a TextMessageNode
 * vector.
 */
namespace Serializable {

class Group;

class Peer {
public:
    QString accountId;
    ///The peer URI
    QString uri;
    ///The peer contact UID
    QString personUID;
    ///The ContactMethod hash
    QString sha1;

    ContactMethod* m_pContactMethod;

    void read (const QJsonObject &json);
    void write(QJsonObject       &json) const;
};

class Group {
public:
    Group(Account* a) : m_pAccount(a) {}
    ~Group();

    ///The group ID (necessary to untangle the graph
    int id;
    ///If the conversion add new participants, a new file will be created
    QString nextGroupSha1;
    ///The group type
    MimeMessage::Type type {MimeMessage::Type::CHAT};
    ///This is the group identifier in the file described by `nextGroupSha1`
    int nextGroupId;
    ///The unique identifier of the event associated with this text message group
    QByteArray eventUid;
    ///The timestamp of the first group entry
    time_t begin {0};
    ///The timestamp of the last group entry
    time_t end   {0};

    /// The account that owns this group, ignore the possible cardinality issues
    Account* m_pAccount {nullptr};

    QSharedPointer<Event> event();
    void addMessage(MimeMessage* m);

    /// Prevent the list from being modified directly.
    const QList<MimeMessage*>& messagesRef() const;

    int size() const;

    /// Create an event;
    Event* buildEvent();

    void read (const QJsonObject &json, const QHash<QString,ContactMethod*> sha1s);
    void write(QJsonObject       &json) const;

private:
    ///All messages from this chunk
    QList<MimeMessage*> messages;

    ///Due to complex ownership, give no direct access
    QSharedPointer<Event> m_pEvent;
};

class Peers {
    friend class ::SerializableEntityManager;
public:
    ~Peers();

    ///The sha1(s) of each participants. If there is onlt one, it should match the filename
    QList<QString> sha1s;
    ///Every message groups associated with this ContactMethod (or ContactMethodGroup)
    QList<Group*> groups;
    ///Information about every (non self) peer involved in this group
    QList<Peer*> peers;

    ///This attribute store if the file has changed
    bool hasChanged;

    ///Keep a cache of the peers sha1
    QHash<QString,ContactMethod*> m_hSha1;

    void read (const QJsonObject &json);
    void write(QJsonObject       &json) const;

    QJsonArray toSha1Array() const;

    void addPeer(const ContactMethod* cm);

private:
    Peers() : hasChanged(false) {}
};

}
//END Those classes are serializable to JSon

/**
 * This class ensure that only one Serializable::Peer structure exist for each
 * peer [group].
 */
class SerializableEntityManager
{
public:
    static QSharedPointer<Serializable::Peers> peer(const ContactMethod* cm);
    static QSharedPointer<Serializable::Peers> peers(QList<const ContactMethod*> cms);
    static QSharedPointer<Serializable::Peers> fromSha1(const QByteArray& sha1);
    static QSharedPointer<Serializable::Peers> fromJson(const QJsonObject& obj, const ContactMethod* cm = nullptr);
private:
    static QHash<QByteArray, QWeakPointer<Serializable::Peers>> m_hPeers;
};
