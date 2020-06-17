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
#include "area_code_lineedit.h"
#include "mcode_widget.h"
#include "ql_lineedit_pass.h"
#include "ql_label_info.h"
#include "ql_svg_handler.h"

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
    ql_lineedit_pass*      get_login_pass();
    ql_label_info*         get_tips_pass();
    ql_label_info*         get_tips_code();
    QLineEdit*      get_login_code();
    QString         get_user_name();
    QString         get_user_pass();
    QPushButton*    get_user_mcode();
    QStackedWidget* get_stack_widget();
    void            set_code(QString codenum);
    mcode_widget*   get_mcode_widget();
    QLineEdit*      get_mcode_lineedit();
    void            set_clear();
    void            set_staus(bool ok);
public slots:
    void            set_window1();
    void            set_window2();
    void            setstyleline();
private:
    //Basic Widgets
    QWidget         *widget_number;
    QWidget         *widget_phone;

    //Suppoter Widgets
    QListWidgetItem *items[5];
    QVBoxLayout     *vlayout_p;
    QVBoxLayout     *vlayout;
    QHBoxLayout     *HBox_forget;
    QHBoxLayout     *HBox_forgett;
    QVBoxLayout     *waylayout;
    QPushButton     *account_login_btn;
    QPushButton     *message_login_btn;
    QLineEdit       *account_phone;
    ql_lineedit_pass       *account_pass;
    QLineEdit       *reg_phone;
    QPushButton     *phone_number;
    QLineEdit       *reg_name;
    ql_lineedit_pass       *reg_pass;
    QLineEdit       *valid_code;
    QPushButton     *send_msg_submit;
    QPushButton     *register_submit;
    QPushButton     *back_login_btn;
    QPushButton     *forgot_pass_btn;
    QStackedWidget  *stack_widget;
    QListWidget     *list_widget;
    QListWidget     *list_content;
    area_code_lineedit *acl;
    ql_label_info          *error_pass;
    ql_label_info          *error_code;
    QString         code;
    mcode_widget    *mcode;
    QLineEdit       *mcode_lineedit;
    QHBoxLayout     *mcode_layout;
    ql_svg_handler  *svg_hd;
signals:
    void code_changed();
};

#endif // LOGINDIALOG_H
