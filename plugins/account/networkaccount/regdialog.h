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
#include "areacodelineedit.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include "passwordlineedit.h"
#include "tips.h"
#include "svghandler.h"

class RegDialog : public QWidget
{
    Q_OBJECT
public:
    explicit RegDialog(QWidget *parent = nullptr);
    QLabel* get_pass_tip();
    QLabel* get_user_tip();
    QLineEdit* get_reg_user();
    PasswordLineEdit* get_reg_pass();
    QLineEdit* get_valid_code();
    AreaCodeLineEdit* get_phone_user();
    PasswordLineEdit* get_reg_confirm();
    QPushButton* get_send_code();
    QString     get_user_phone();
    QString     get_user_account();
    QString     get_user_passwd();
    QString     get_user_mcode();
    void        set_staus(bool ok);
    void        set_code(QString codenum);
    void        set_clear();
    Tips*     get_tips();
public slots:
    void change_uppercase();
    void setstyleline();
protected:

signals:
    void code_changed();
private:
    AreaCodeLineEdit *m_phoneLineEdit;
    QLineEdit *m_accountLineEdit;
    PasswordLineEdit *m_passLineEdit;
    QLineEdit *m_mcodeLineEdit;
    PasswordLineEdit *m_confirmLineEdit;
    QPushButton *m_sendMsgBtn;
    QVBoxLayout *m_workLayout;
    QHBoxLayout *m_mcodeLayout;
    QLabel      *m_accountTips;
    QLabel      *m_passTips;
    Tips      *m_errorTips;
    QString     m_errorCode;
    SVGHandler *m_svgHandler;
};

#endif // REGDIALOG_H
