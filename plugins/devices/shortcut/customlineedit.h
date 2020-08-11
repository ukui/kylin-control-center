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
#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
//#include <QMouseEvent>
#include <QFocusEvent>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit CustomLineEdit(QString shortcut, QWidget *parent = 0);
    ~CustomLineEdit();

//    virtual void mousePressEvent(QMouseEvent * e);
    virtual void focusOutEvent(QFocusEvent * evt);
    virtual void focusInEvent(QFocusEvent * evt);
    virtual void keyReleaseEvent(QKeyEvent * evt);

public:
    void setFlagStatus(bool checked);
    void updateOldShow(QString newStr);

private:
    QString _oldshortcut;
    QString _wait;

    bool flag;

Q_SIGNALS:
    void shortcutCodeSignals(QList<int>);

};

#endif // CUSTOMLINEEDIT_H
