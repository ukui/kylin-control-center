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
#ifndef LEFTWIDGETITEM_H
#define LEFTWIDGETITEM_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QSvgRenderer>
#include <QPixmap>

class LeftWidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit LeftWidgetItem(QWidget *parent = 0);
    ~LeftWidgetItem();

public:
    void setLabelPixmap(QString filename, QString icoName, QString color);
    void isSetLabelPixmapWhite(bool selected);
    void setLabelText(QString text);
    void setLabelTextIsWhite(bool selected);

    void setSelected(bool selected);

    QString text();

private:
    // load svg picture
    const QPixmap loadSvg(const QString &fileName, QString color);
    // chang svg picture's color
    QPixmap drawSymbolicColoredPixmap(const QPixmap &source, QString cgcolor);

private:
    QLabel * iconLabel;
    QLabel * textLabel;

    QWidget * widget;
    QString icoName;

};

#endif // LEFTWIDGETITEM_H
