/************************************************************************************
 *   Copyright (C) 2014-2016 by Savoir-faire Linux                                  *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#pragma once

#include "collectioninterface.h"
#include "collectioneditor.h"

class Call;

template<typename T> class CollectionMediator;

class LIB_EXPORT LocalHistoryCollection : public CollectionInterface
{
public:
   explicit LocalHistoryCollection(CollectionMediator<Call>* mediator);
   virtual ~LocalHistoryCollection();

   virtual bool load () override;
   virtual bool reload() override;
   virtual bool clear () override;

   virtual QString    name     () const override;
   virtual QString    category () const override;
   virtual QVariant   icon     () const override;
   virtual bool       isEnabled() const override;
   virtual QByteArray id       () const override;

   void addCompletionCallback(std::function<void(LocalHistoryCollection*)> cb);

   virtual FlagPack<SupportedFeatures> supportedFeatures() const override;
};

