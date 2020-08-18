/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2020 KYLINOS Information Technology Co., Ltd.
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

#ifndef DEFINEGROUPITEM_H
#define DEFINEGROUPITEM_H

#include <QWidget>
#include <QFrame>
#include <QMouseEvent>

class QLabel;
class QPushButton;

class DefineGroupItem : public QFrame
{
    Q_OBJECT

public:
    explicit DefineGroupItem(QString groupName);
    ~DefineGroupItem();

public:
    QWidget * widgetComponent();
    QLabel * labelComponent();
    QPushButton * editBtnComponent();
    QPushButton * delBtnComponent();

public:
    void setDeleteable(bool deleteable);
    void setUpdateable(bool updateable);
    void setEditable(bool editable);

    void setShortcutName(QString newName);
    void setShortcutBinding(QString newBinding);

protected:
    virtual void mousePressEvent(QMouseEvent * e);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);


private:
    QWidget * pWidget;
    QLabel * pLabel;
    QPushButton * pEditBtn;
    QPushButton * pDelBtn;

private:
    bool _deleteable;
    bool _editable;
    bool _updateable;

Q_SIGNALS:
    void updateShortcutSignal();

};

#endif // DEFINEGROUPITEM_H
