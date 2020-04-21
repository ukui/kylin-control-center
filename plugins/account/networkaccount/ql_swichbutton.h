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
#ifndef QL_SWICHBUTTON_H
#define QL_SWICHBUTTON_H

#include <QWidget>
#include <QTimer>
#include <QPainter>

class QL_SwichButton : public QWidget
{
    Q_OBJECT
public:
    explicit        QL_SwichButton(QWidget *parent = nullptr);
    void            set_swichbutton_val(int on);
    int             get_swichbutton_val();
    void            set_id(int id);
    int             get_id();
    int             get_active();
    void            set_active(bool ok);
private:
    int             active = 1;
    int             on = 1;
    QColor          *ql_color;
    QTimer          *timer;
    float           ql_width;
    float           ql_height;
    float           cur_val;
    int             id;
    void            paintEvent(QPaintEvent *event);
    void            mousePressEvent(QMouseEvent *event);
signals:
    void            status(int on,int id);
private slots:
    void startAnimation();
};

#endif // QL_SWICHBUTTON_H
