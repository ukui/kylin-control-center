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

const QByteArray kVinoSchemas    = "org.gnome.Vino";
const QString kVinoViewOnlyKey   = "view-only";
const QString kVinoPromptKey     = "prompt-enabled";
const QString kAuthenticationKey = "authentication-methods";
const QString kVncPwdKey         = "vnc-password";

enum RequestPwd {
    NOPWD,
    NEEDPWD
};


class ShareMain : public QWidget {
    Q_OBJECT
public:
    ShareMain(QWidget *parent = nullptr);
    ~ShareMain();

private:
    QFrame * mViewFrame;
    QFrame * mSecurityFrame;
    QFrame * mSecurityPwdFrame;
    QFrame * mNoticeWFrame;
    QFrame * mNoticeOFrame;
    QFrame * mNoticeNFrame;

    QCheckBox * mViewBox;
    QRadioButton * mAccessBox;
    QRadioButton * mPwdBox;

    QRadioButton * mNoticeWBtn;
    QRadioButton * mNoticeOBtn;
    QRadioButton * mNoticeNBtn;

    QLabel * mShareTitleLabel;
    QLabel * mViewLabel;
    QLabel * mSecurityTitleLabel;
    QLabel * mAccessLabel;
    QLabel * mPwdsLabel;
    QLabel * mNoticeTitleLabel;
    QLabel * mNoticeWLabel;
    QLabel * mNoticeOLabel;
    QLabel * mNoticeNLabel;

    QLineEdit * mPwdLineEdit;

    QVBoxLayout * mVlayout;
    QButtonGroup * mBtnGroup;

    QGSettings * mVinoGsetting;

private:
    void initUI();
    void initConnection();
    void initShareStatus(bool isConnnect, bool isPwd);

private slots:
    void viewBoxSlot(bool status);
    void accessSlot(int index);
    void pwdInputSlot(QString pwd);
};

#endif // SHAREMAIN_H
