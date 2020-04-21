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
#ifndef HOVERWIDGET_H
#define HOVERWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

class ResHoverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResHoverWidget(QString mname, QWidget *parent = 0);
    ~ResHoverWidget();

public:
    QString _name;

protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
    virtual void paintEvent(QPaintEvent * event);

    virtual void mousePressEvent(QMouseEvent * event);

Q_SIGNALS:
    void enterWidget(QString name);
    void leaveWidget(QString name);

    void widgetClicked(QString name);
};

#endif // HOVERWIDGET_H
