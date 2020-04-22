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
#ifndef MCODE_WIDGET_H
#define MCODE_WIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QTime>
#include <QPaintEvent>
#include <QPainter>

class mcode_widget : public QLabel
{
    Q_OBJECT
public:
    mcode_widget(QWidget *parent = 0);
    QChar *get_verificate_code();
    void set_change(int ok);
protected:
    void mousePressEvent(QMouseEvent *ev);
    void paintEvent(QPaintEvent *event);
private:
    int noice_point_number;
    const int letter_number = 4;
    void produceVerificationCode() const;
    //产生一个随机的字符
    QChar produceRandomLetter() const;
    //产生随机的颜色
    void produceRandomColor() const;
     QChar *verificationCode;
     QColor *colorArray;
     QList<QColor> colorList;
     bool ok = true;
     void reflushVerification();
};

#endif // MCODE_WIDGET_H
