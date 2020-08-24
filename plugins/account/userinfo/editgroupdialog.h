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

#ifndef EDITGROUPDIALOG_H
#define EDITGROUPDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QCheckBox>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QLineEdit>
#include <QListWidget>

class UserInfo;
namespace Ui {
class EditGroupDialog;
}

class EditGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditGroupDialog(QString usergroup, QString groupid, QWidget *parent = nullptr);
    ~EditGroupDialog();

public:
    void limitInput();
    void getUsersList(QString usergroup);
    void refreshCertainBtnStatus();
    QLineEdit * lineNameComponent();
    QLineEdit * lineIdComponent();
    QListWidget * listWidgetComponent();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::EditGroupDialog *ui;

    bool _nameHasModified;
    bool _idHasModified;
    bool _boxModified;
    QString userGroup;
    QString groupId;

    void setupInit();
    void signalsBind();

signals:
    void needRefresh();
};

#endif // EDITGROUPDIALOG_H
