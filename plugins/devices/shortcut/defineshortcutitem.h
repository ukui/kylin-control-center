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
#ifndef DEFINESHORTCUTITEM_H
#define DEFINESHORTCUTITEM_H

#include <QWidget>
#include <QFrame>

class QLabel;
class QPushButton;
class CustomLineEdit;

class DefineShortcutItem : public QFrame
{
    Q_OBJECT

public:
    explicit DefineShortcutItem(QString name, QString binding);
    ~DefineShortcutItem();

public:
    QWidget * widgetComponent();
    QLabel * labelComponent();
    QPushButton * btnComponent();
    CustomLineEdit * lineeditComponent();

public:
    void setDeleteable(bool deleteable);
    void setUpdateable(bool updateable);

    void setShortcutName(QString newName);
    void setShortcutBinding(QString newBinding);

protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent * e);


private:
    QWidget * pWidget;
    QLabel * pLabel;
    CustomLineEdit * pLineEdit;
    QPushButton * pButton;

private:
    bool _deleteable;
    bool _updateable;

Q_SIGNALS:
    void updateShortcutSignal();

};

#endif // DEFINESHORTCUTITEM_H
