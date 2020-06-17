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
#ifndef REGDIALOG_H
#define REGDIALOG_H

#include <QObject>
#include <QWidget>
#include "area_code_lineedit.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include "ql_lineedit_pass.h"
#include "ql_label_info.h"
#include "ql_svg_handler.h"

class RegDialog : public QWidget
{
    Q_OBJECT
public:
    explicit RegDialog(QWidget *parent = nullptr);
    QLabel* get_pass_tip();
    QLabel* get_user_tip();
    QLineEdit* get_reg_user();
    ql_lineedit_pass* get_reg_pass();
    QLineEdit* get_valid_code();
    area_code_lineedit* get_phone_user();
    ql_lineedit_pass* get_reg_confirm();
    QPushButton* get_send_code();
    QString     get_user_phone();
    QString     get_user_account();
    QString     get_user_passwd();
    QString     get_user_mcode();
    void        set_staus(bool ok);
    void        set_code(QString codenum);
    void        set_clear();
    ql_label_info*     get_tips();
public slots:
    void change_uppercase();
    void setstyleline();
protected:

signals:
    void code_changed();
private:
    area_code_lineedit *reg_phone;
    QLineEdit *reg_user;
    ql_lineedit_pass *reg_pass;
    QLineEdit *valid_code;
    ql_lineedit_pass *reg_confirm;
    QPushButton *send_msg_btn;
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    QLabel      *user_tip;
    QLabel      *pass_tip;
    ql_label_info      *tips;
    QString     code;
    ql_svg_handler *svg_hd;
};

#endif // REGDIALOG_H
