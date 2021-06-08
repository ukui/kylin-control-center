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
#ifndef SHAREMAIN_H
#define SHAREMAIN_H

#include <QWidget>
#include <QFrame>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QButtonGroup>
#include <QByteArray>
#include <QDebug>
#include <QGSettings/QGSettings>
#include <QPushButton>
#include "SwitchButton/switchbutton.h"
#include "inputpwddialog.h"
#include "Label/titlelabel.h"

const QByteArray kVinoSchemas = "org.gnome.Vino";
const QString kVinoViewOnlyKey = "view-only";
const QString kVinoPromptKey = "prompt-enabled";
const QString kAuthenticationKey = "authentication-methods";
const QString kVncPwdKey = "vnc-password";

const QByteArray kUkccVnoSchmas = "org.ukui.control-center.vino";
const QString kUkccPromptKey = "remote";

enum RequestPwd {
    NOPWD,
    NEEDPWD
};

class ShareMain : public QWidget
{
    Q_OBJECT
public:
    ShareMain(QWidget *parent = nullptr);
    ~ShareMain();

private:
    QFrame *mEnableFrame;
    QFrame *mViewFrame;
    QFrame *mSecurityFrame;
    QFrame *mSecurityPwdFrame;
    QFrame *mNoticeWFrame;
    QFrame *mNoticeOFrame;
    QFrame *mNoticeNFrame;

    SwitchButton *mEnableBtn;  // 允许其他人查看桌面
    SwitchButton *mViewBtn;    // 允许连接控制屏幕
    SwitchButton *mAccessBtn;  // 为本机确认每次访问
    SwitchButton *mPwdBtn;    // 要求用户输入密码

    QRadioButton *mNoticeWBtn;
    QRadioButton *mNoticeOBtn;
    QRadioButton *mNoticeNBtn;

    TitleLabel *mShareTitleLabel;
    TitleLabel *mSecurityTitleLabel;
    QLabel *mEnableLabel;
    QLabel *mViewLabel;
    QLabel *mAccessLabel;
    QLabel *mPwdsLabel;
    QLabel *mNoticeTitleLabel;
    QLabel *mNoticeWLabel;
    QLabel *mNoticeOLabel;
    QLabel *mNoticeNLabel;

    QPushButton *mPwdinputBtn;

    QVBoxLayout *mVlayout;

    QGSettings *mVinoGsetting;

    QString secpwd;

private:
    void initUI();
    void initConnection();
    void initShareStatus(bool isConnnect, bool isPwd);
    void initEnableStatus();
    void setFrameVisible(bool visible);
    void setVinoService(bool status);

private slots:
    void enableSlot(bool status);
    void viewBoxSlot(bool status);
    void accessSlot(bool status);
    void pwdEnableSlot(bool status);
    void pwdInputSlot();
};

#endif // SHAREMAIN_H
