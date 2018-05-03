/************************************************************************************
 *   Copyright (C) 2018 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
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
#include "callstate.h"

#include <call.h>

namespace Troubleshoot {

class CallStatePrivate
{
    //
};

}

Troubleshoot::CallState::CallState(Dispatcher* parent) :
    Troubleshoot::Base(parent), d_ptr(new CallStatePrivate())
{}

Troubleshoot::CallState::~CallState()
{
    delete d_ptr;
}

QString Troubleshoot::CallState::headerText() const
{
    return {};
}

Troubleshoot::Base::Severity Troubleshoot::CallState::severity() const
{
    return Base::Severity::WARNING;
}

bool Troubleshoot::CallState::setSelection(const QModelIndex& idx, Call* c)
{
    return false;
}

bool Troubleshoot::CallState::setSelection(int idx, Call* c)
{
    return false;
}

bool Troubleshoot::CallState::isAffected(Call* c, time_t elapsedTime, Troubleshoot::Base* self)
{
    return c->lifeCycleState() == Call::LifeCycleState::INITIALIZATION && elapsedTime >= 15;
}

int Troubleshoot::CallState::timeout()
{
    return 10;
}
