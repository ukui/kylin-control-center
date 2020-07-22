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
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListView>
#include <QComboBox>
#include <QMouseEvent>
#include <QLineEdit>
#include <QDebug>
#include <QStackedWidget>
#include <QListWidget>
#include "areacodelineedit.h"
#include "mcodewidget.h"
#include "passwordlineedit.h"
#include "tips.h"
#include "svghandler.h"

class LoginDialog : public QWidget
{
    Q_OBJECT
public:
    explicit        LoginDialog(QWidget *parent = nullptr);

    //Pages
    const int       sizeoff = 10;
    bool            login_account_thr_number();
    bool            login_account_thr_phone();
    void            startaction_1();
    void            startaction_2();
    QLineEdit*      get_user_edit();
    QPushButton*    get_forget_btn();
    PasswordLineEdit*      get_login_pass();
    Tips*         get_tips_pass();
    Tips*         get_tips_code();
    QLineEdit*      get_login_code();
    QString         get_user_name();
    QString         get_user_pass();
    QPushButton*    get_user_mcode();
    QStackedWidget* get_stack_widget();
    void            set_code(QString codenum);
    MCodeWidget*   get_mcode_widget();
    QLineEdit*      get_mcode_lineedit();
    void            set_clear();
    void            set_staus(bool ok);
public slots:
    void            set_window1();
    void            set_window2();
    void            setstyleline();
private:
    //Basic Widgets
    QWidget         *m_normalWidget;
    QWidget         *m_phoneWidget;

    //Suppoter Widgets
    QListWidgetItem *m_itemwidgetList[5];
    QVBoxLayout     *m_phoneLayout;
    QVBoxLayout     *m_normalLayout;
    QHBoxLayout     *m_subHBoxLayout;
    QHBoxLayout     *m_mcodePhoneLayout;
    QVBoxLayout     *m_wayLayout;
    QPushButton     *m_accountLogin_btn;
    QPushButton     *m_phoneLogin_btn;
    QLineEdit       *m_accountLineEdit;
    PasswordLineEdit       *m_passwordLineEdit;
    QLineEdit       *m_phoneLineEdit;
    QLineEdit       *m_mcodeNormalLineEdit;
    QPushButton     *m_sendMsgBtn;
    QPushButton     *m_forgetPasswordBtn;
    QStackedWidget  *m_stackedWidget;
    QListWidget     *m_listWidget;
    Tips          *m_passTips;
    Tips          *m_phoneTips;
    QString         m_szCode;
    MCodeWidget    *m_mcodeWidget;
    QLineEdit       *m_mcodePhoneLineEdit;
    QHBoxLayout     *m_mcodeNormalLayout;
    SVGHandler  *m_svgHandler;
signals:
    void code_changed();
};

#endif // LOGINDIALOG_H
