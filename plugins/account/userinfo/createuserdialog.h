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
#include <QPainterPath>
#include <QKeyEvent>
#include <QFile>
#include <QStyledItemDelegate>

#ifdef ENABLEPQ
extern "C" {

#include <pwquality.h>

}
#define PWCONF "/etc/security/pwquality.conf"
#define RFLAG 0x1
#define CFLAG 0x2
#endif

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
    void setRequireLabel(QString msg);

    void refreshConfirmBtnStatus();

    void nameLegalityCheck(QString username);
    void pwdLegalityCheck(QString pwd);
    bool nameTraverse(QString username);

    void initPwdChecked();

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

private:
    QStringList getHomeUser();

private:
    Ui::CreateUserDialog *ui;

    bool checkCharLegitimacy(QString password);

    bool back;
    bool isCreateUser = false;
    bool enablePwdQuality;

    QProcess * process;

    QString nameTip;
    QString pwdTip;
    QString pwdSureTip;

    QStringList usersStringList;

#ifdef ENABLEPQ
    pwquality_settings_t *settings;
#endif

Q_SIGNALS:
    void newUserWillCreate(QString username, QString pwd, QString pin, int atype);

};

#endif // CREATEUSERDIALOG_H
