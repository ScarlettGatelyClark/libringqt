/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#ifndef NUMBERCATEGORYMODEL_H
#define NUMBERCATEGORYMODEL_H
#include "typedefs.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>

class QPixmap;

class NumberCategoryVisitor;

class LIB_EXPORT NumberCategoryModel : public QAbstractListModel {
   Q_OBJECT
public:
   explicit NumberCategoryModel(QObject* parent = nullptr);

   enum Role {
      INDEX = 100,
   };

   //Abstract model member
   virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
   virtual int rowCount(const QModelIndex& parent = QModelIndex()             ) const;
   virtual Qt::ItemFlags flags(const QModelIndex& index                       ) const;
   virtual bool setData(const QModelIndex& index, const QVariant &value, int role);

   //Mutator
   void addCategory(const QString& name, QPixmap* icon, int index = -1, bool enabled = true);
   void setIcon(int index, QPixmap* icon);
   void save();

   //Singleton
   static NumberCategoryModel* instance();

   //Setter
   void setVisitor(NumberCategoryVisitor* visitor);

private:
   struct InternalTypeRepresentation {
      QString  name   ;
      int      index  ;
      QPixmap* icon   ;
      bool     enabled;
   };
   QVector<InternalTypeRepresentation*>   m_lCategories;
   QHash<int,InternalTypeRepresentation*> m_hByIdx;
   static NumberCategoryModel*            m_spInstance ;
   NumberCategoryVisitor*                 m_pVisitor   ;
};

#endif //NUMBERCATEGORYMODEL_H
