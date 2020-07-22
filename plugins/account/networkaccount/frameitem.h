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
#ifndef NETWORK_ITEM_H
#define NETWORK_ITEM_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include "switchbutton.h"
#include <QFrame>

class FrameItem : public QWidget
{
    Q_OBJECT
public:
    explicit        FrameItem(QWidget *parent = nullptr);
    void            set_itemname(QString name);
    QString         get_itemname();
    void            make_itemoff();
    void            make_itemon();
    QHBoxLayout*    get_layout();
    SwitchButton* get_swbtn();
    QWidget*        get_widget();
    void            set_active(bool ok);
private:
    QLabel          *m_itemName;
    QHBoxLayout     *m_workLayout;
    QFrame          *m_itemFrame;
    int             m_bOn;
    SwitchButton  *m_switchBtn;
signals:

};

#endif // NETWORK_ITEM_H
