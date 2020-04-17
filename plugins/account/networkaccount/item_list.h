/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#ifndef ITEM_LIST_H
#define ITEM_LIST_H

#include <QObject>
#include <QListWidget>
#include <QMouseEvent>
#include <QComboBox>
#include "network_item.h"

#define CURSIZE 6

class item_list : public QListWidget
{
    Q_OBJECT
public:
    explicit        item_list(QListWidget *parent = nullptr,int itemssize = CURSIZE);
    QStringList     get_list();
    network_item*   get_item(int cur);
    void            add_item(QString item_name);
private:
    QListWidgetItem *items[30];
    QStringList     varmap = {tr("Walpaper"),tr("Menu"),tr("Quick Start"),tr("Tab"),tr("User Profile"),tr("Weather"),tr("Media")};
    int             itemsize = 0;
    network_item    *itempack[30];
    QPoint          m_startPoint;
signals:

};

#endif // ITEM_LIST_H
