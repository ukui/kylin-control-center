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
#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QList>
#include <QPixmap>
#include <QRadioButton>

class QLabel;

class ThemeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeWidget(QSize iSize, QString name, QStringList iStringList , QWidget *parent = 0);
    explicit ThemeWidget(QSize iSize, QString name, const QList<QPixmap> &listMap , QWidget *parent = 0);
    ~ThemeWidget();

public:
    void setSelectedStatus(bool status);
    void setValue(QString value);
    QString getValue();

public:
    QRadioButton * selectedbtn;
    QLabel * placeHolderLabel;

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual void mousePressEvent(QMouseEvent * event);

private:
    QString pValue;
    QList<QPixmap> listMap;
    bool isCursor;

Q_SIGNALS:
    void clicked();

};

#endif // THEMEWIDGET_H
