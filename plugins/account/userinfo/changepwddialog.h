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
#ifndef CHANGEPWDDIALOG_H
#define CHANGEPWDDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QThread>

#include <QTimer>

#include "pwdcheckthread.h"

#ifdef ENABLEPQ
extern "C" {

#include <pwquality.h>

}

#define PWCONF "/etc/security/pwquality.conf"
#define RFLAG 0x1
#define CFLAG 0x2

#endif

namespace Ui {
class ChangePwdDialog;
}

class ChangePwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwdDialog(bool _isCurrentUser, QString _username, QWidget *parent = 0);
    ~ChangePwdDialog();

public:
    void initPwdChecked();
    void setupComponent();
    void setupConnect();

    void refreshConfirmBtnStatus();
    void refreshCancelBtnStatus();

    void setFace(QString iconfile);
    void setUsername(QString realname);
    void setPwdType(QString type);
    void setAccountType(QString text);
    void haveCurrentPwdEdit(bool have);

    bool isCurrentUser;

protected:
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

private:
    Ui::ChangePwdDialog *ui;

    bool checkCharLegitimacy(QString password);
    bool checkOtherPasswd(QString name, QString pwd);

    QString currentUserName;
    QString pwdTip;
    QString pwdSureTip;
    QString curPwdTip;

    bool enablePwdQuality;
    bool pwdChecking;

#ifdef ENABLEPQ
    pwquality_settings_t *settings;
#endif

private:
    PwdCheckThread * pcThread;

    QTimer * timerForCheckPwd;


private Q_SLOTS:
    void pwdLegalityCheck();

Q_SIGNALS:
    void passwd_send(QString oldpwd, QString pwd);
    void passwd_send2(QString pwd);
    void pwdCheckOver();
};

#endif // CHANGEPWDDIALOG_H
