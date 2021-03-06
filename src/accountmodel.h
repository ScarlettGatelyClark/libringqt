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

#include <functional>

#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QAbstractListModel>

#include "account.h"
#include "typedefs.h"

class ContactRequest;

//Private
class AccountModelPrivate;

///AccountList: List of all daemon accounts
class LIB_EXPORT AccountModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   Q_PROPERTY(Account*       ip2ip                      READ ip2ip                                            )
   Q_PROPERTY(int            size                       READ size   NOTIFY accountListUpdated                 )
   Q_PROPERTY(bool           presenceEnabled            READ isPresenceEnabled                                )
   Q_PROPERTY(bool           presencePublishSupported   READ isPresencePublishSupported                       )
   Q_PROPERTY(bool           presenceSubscribeSupported READ isPresenceSubscribeSupported                     )
   Q_PROPERTY(ProtocolModel* protocolModel              READ protocolModel                                    )
   Q_PROPERTY(bool           isSipSupported             READ isSipSupported   NOTIFY supportedProtocolsChanged)
   Q_PROPERTY(bool           isIP2IPSupported           READ isIP2IPSupported NOTIFY supportedProtocolsChanged)
   Q_PROPERTY(bool           isRingSupported            READ isRingSupported  NOTIFY supportedProtocolsChanged)
   Q_PROPERTY(Account*       selectedAccount            READ selectedAccount                                  )
   Q_PROPERTY(bool           hasAmbiguousAccounts       READ hasAmbiguousAccounts   NOTIFY accountListUpdated )
   Q_PROPERTY(bool           hasMultipleProtocols       READ hasMultipleProtocols   NOTIFY accountListUpdated )
   Q_PROPERTY(bool           hasAvailableAccounts       READ hasAvailableAccounts   NOTIFY hasAvailableAccountsChanged)
   Q_PROPERTY(bool           hasEnabledAccounts         READ hasEnabledAccounts   NOTIFY hasEnabledAccountsChanged)

   Q_PROPERTY(PendingContactRequestModel* incomingContactRequestModel READ incomingContactRequestModel CONSTANT)

   friend class AccountPrivate;

   /// @enum Global saving state to be used when using a single saving mechanism for all accounts at once
   enum class EditState {
      SAVED   = 0, /*!< Everything is ok, nothing has changed                    */
      UNSAVED = 1, /*!< There is changes ready to be saved                       */
      INVALID = 2, /*!< There is changes, but they would create an invalid state */
      COUNT__
   };

   //Singleton
   static AccountModel& instance();
   virtual  ~AccountModel();

   //Getters
   Q_INVOKABLE Account* getById                     ( const QByteArray& id, bool ph = false) const;
   int                  size                        (                                      ) const;
   Account*             getAccountByModelIndex      ( const QModelIndex& item              ) const;
   Q_INVOKABLE static QString getSimilarAliasIndex  ( const QString& alias                 )      ;
   Account*             ip2ip                       (                                      ) const;
   QList<Account*>      getAccountsByProtocol       ( const Account::Protocol protocol     ) const;
   bool                 isPresenceEnabled           (                                      ) const;
   bool                 isPresencePublishSupported  (                                      ) const;
   bool                 isPresenceSubscribeSupported(                                      ) const;
   ProtocolModel*       protocolModel               (                                      ) const;
   bool                 isSipSupported              (                                      ) const;
   bool                 isIP2IPSupported            (                                      ) const;
   bool                 isRingSupported             (                                      ) const;
   bool                 hasAmbiguousAccounts        (                                      ) const;
   bool                 hasMultipleProtocols        (                                      ) const;
   bool                 hasAvailableAccounts        (                                      ) const;
   bool                 hasEnabledAccounts          (                                      ) const;
   EditState            editState                   (                                      ) const;
   Account*             selectedAccount             (                                      ) const;
   QList<Account*>      accountsToMigrate           (                                      ) const;
   QVector<QMap<QString, QString>> getContacts      (const Account* account                ) const;

   QItemSelectionModel* selectionModel              (                                      ) const;
   QItemSelectionModel* userSelectionModel          (                                      ) const;

   PendingContactRequestModel* incomingContactRequestModel (                               ) const;

   //Abstract model accessors
   virtual QVariant              data        ( const QModelIndex& index, int role = Qt::DisplayRole      ) const override;
   virtual int                   rowCount    ( const QModelIndex& parent = {}                            ) const override;
   virtual Qt::ItemFlags         flags       ( const QModelIndex& index                                  ) const override;
   virtual bool                  setData     ( const QModelIndex& index, const QVariant &value, int role )       override;
   virtual QHash<int,QByteArray> roleNames   (                                                           ) const override;
   virtual QMimeData*            mimeData    ( const QModelIndexList &indexes                            ) const override;
   virtual QStringList           mimeTypes   (                                                           ) const override;
   virtual bool                  dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
   virtual Qt::DropActions       supportedDragActions() const override;
   virtual Qt::DropActions       supportedDropActions() const override;

   //Mutators
   Q_INVOKABLE Account* add      ( const QString& alias, const Account::Protocol protocol = Account::Protocol::SIP);
   Q_INVOKABLE Account* add      ( const QString& alias, const QModelIndex&      protocol                         );
   Q_INVOKABLE void     remove   ( Account* account                                                               );
   void                 remove   ( const QModelIndex& index                                                       );
   Q_INVOKABLE void     save     (                                                                                );
   Q_INVOKABLE void     cancel   (                                                                                );
   int                  exportAccounts(const QStringList& accountIDs, const QString& filePath, const QString& password);
   int                  importAccounts(const QString& filePath, const QString& password);

   //Operators
   Account*       operator[] (int               i)      ;
   Account*       operator[] (const QByteArray& i)      ;
   const Account* operator[] (int               i) const;

   //Helpers
   Account* findPlaceHolder(const QByteArray& accountId) const;
   Account* findAccountIf(const std::function<bool(const Account&)>& pred) const;

private:
   //Constructors & Destructors
   explicit AccountModel ();

   //Helpers
   void add(Account* acc);

   AccountModelPrivate* d_ptr;
   Q_DECLARE_PRIVATE(AccountModel)

public Q_SLOTS:
   void update             ();
   void updateAccounts     ();
   void registerAllAccounts();
   bool moveUp             ();
   bool moveDown           ();
   ///Notifies the Ring daemon that the connectivity (network status) has changed
   void slotConnectivityChanged();

Q_SIGNALS:
   ///The account list changed
   void accountListUpdated(                                          );
   ///Emitted when an account enable attribute change
   void accountEnabledChanged( Account* source                       );
   ///Emitted when the default account change
   void defaultAccountChanged( Account* a                            );
   ///Emitted when one account registration state change
   void registrationChanged(Account* a, bool registration            );
   ///Emitted when the network is down
   void badGateway(                                                  );
   ///Emitted when a new voice mail is available
   void voiceMailNotify(Account* account, int count                  );
   ///Propagate Account::presenceEnabledChanged
   void presenceEnabledChanged(bool isPresent                        );
   ///An account has been removed
   void accountRemoved(Account* account                              );
   ///An account has been added
   void accountAdded(Account* account                                );
   ///Emitted when an account using a previously unsupported protocol is added
   void supportedProtocolsChanged(                                   );
   ///Emitted when an account state change
   void accountStateChanged  ( Account* account, const Account::RegistrationState state);
   ///Emitted when an account edit state change
   void accountEditStateChanged(Account* account, const Account::EditState state, const Account::EditState prev);
   ///The global AccountModel edit state changed
   void editStateChanged(const EditState state, const EditState previous) const;
   /// A list of contacts has been obtained.
   void contactsList(Account *account);
   /// A new contact has been added.
   void accountContactAdded(Account* a, const ContactRequest* r);
   /// When one of the account ability to call changed
   void canCallChanged(Account* a, bool status);
   void canVideoCallChanged(Account* a, bool status);
   /// When there is at least one registered account.
   void hasAvailableAccountsChanged();
   /// When there is at least one enabled (checked) account.
   void hasEnabledAccountsChanged();
};
Q_DECLARE_METATYPE(AccountModel*)
