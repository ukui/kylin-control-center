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
#ifndef PASSDIALOG_H
#define PASSDIALOG_H

#include <QObject>
#include <QWidget>
#include "areacodelineedit.h"
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "passwordlineedit.h"
#include "tips.h"
#include "svghandler.h"

class PassDialog : public QWidget
{
    Q_OBJECT
public:
    explicit PassDialog(QWidget *parent = nullptr);
    QPushButton* get_send_msg_btn();
    QString     get_user_name();
    QString     get_user_newpass();
    QString     get_user_confirm();
    QString     get_user_mcode();
    QLineEdit*  get_reg_phone();
    PasswordLineEdit*  get_reg_pass();
    PasswordLineEdit*  get_reg_pass_confirm();
    QLineEdit*  get_valid_code();
    void        set_code(QString codenum);
    void        set_clear();
    Tips*     get_tips();
    QLabel*     get_passtips();
    void        set_staus(bool ok);
public slots:
    void change_uppercase();
    void setstyleline();

signals:
    void code_changed();
private:
    QLineEdit *m_phoneLineEdit;
    PasswordLineEdit *m_passLineEdit;
    PasswordLineEdit *m_confirmLineEdit;
    QLineEdit *m_mcodeLineEdit;
    QPushButton *m_sendMsgBtn;
    QVBoxLayout *m_workLayout;
    QHBoxLayout *m_hboxLayout;
    Tips *m_errorTips;
    QString     m_errorCode;
    SVGHandler *m_svgHandler;
    QLabel *m_passwordTips;
};

#endif // PASSDIALOG_H
