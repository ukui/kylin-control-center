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
#ifndef CREATEUSERDIALOG_H
#define CREATEUSERDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QPainter>

#include <QFile>
#include <QStyledItemDelegate>

namespace Ui {
class CreateUserDialog;
}

class CreateUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateUserDialog(QStringList userlist, QWidget *parent = 0);
    ~CreateUserDialog();

public:
    void setupComonpent();
    void setupConnect();

    void refreshConfirmBtnStatus();

    void nameLegalityCheck(QString username);
    void pwdLegalityCheck(QString pwd);

protected:
    void paintEvent(QPaintEvent *);


private:
    Ui::CreateUserDialog *ui;

    QProcess * process;
    bool back;
    QStringList usersStringList;

    QString nameTip;
    QString pwdTip;
    QString pwdSureTip;

private:
    QStyledItemDelegate* itemDelege;
    bool isCreateUser = false;

Q_SIGNALS:
    void newUserWillCreate(QString username, QString pwd, QString pin, int atype);

};

#endif // CREATEUSERDIALOG_H
