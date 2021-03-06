/*
 * ActionUpdatable.cpp -
 *
 * Copyright (c) 2018 gi0e5b06 (on github.com)
 *
 * This file is part of LMMS - https://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include "ActionUpdatable.h"

#include <QWidget>

void ActionUpdatable::updateActions(const bool            _active,
                                    QHash<QString, bool>& _table) const
{
    Q_UNUSED(_active)
    Q_UNUSED(_table)
    // qInfo("ActionUpdatable::updateActions active=%d", _active);
}

void ActionUpdatable::actionTriggered(QString _name)
{
    qInfo("ActionUpdatable::actionTriggered() name=%s", qPrintable(_name));
}

void ActionUpdatable::requireActionUpdate()
{
    // qInfo("ActionUpdatable::requireActionUpdate");
    QWidget* p = dynamic_cast<QWidget*>(this);
    if(p == NULL)
    {
        qWarning("ActionUpdatable: not a qwidget");
    }
    else
        while(p)
        {
            p                  = p->parentWidget();
            ActionUpdatable* a = dynamic_cast<ActionUpdatable*>(p);
            if(a)
            {
                a->requireActionUpdate();
                p = NULL;
                break;
            }
        }
}
