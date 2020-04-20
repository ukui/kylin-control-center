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

#include "bindphonedialog.h"

BindPhoneDialog::BindPhoneDialog(QWidget *parent) : QWidget(parent)
{
    setContentsMargins(0,0,0,0);
    this->setFixedWidth(338);
    phone = new area_code_lineedit(this);
    account = new QLineEdit(this);
    pass = new ql_lineedit_pass(this);
    valid_code = new QLineEdit(this);
    send_code = new QPushButton(this);

    tips = new QLabel(this);

    layout = new QVBoxLayout;
    sublayout = new QHBoxLayout;
    account->setPlaceholderText(tr("Your account here"));
    pass->setPlaceholderText(tr("Your password here"));
    valid_code->setPlaceholderText(tr("Your code here"));
    send_code->setText(tr("Get phone code"));


    phone->setFixedSize(QSize(338,36));
    account->setFixedSize(338,36);
    pass->setFixedSize(338,36);
    valid_code->setFixedSize(120,36);
    send_code->setFixedSize(198,36);
    layout->setMargin(0);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop);
    sublayout->setMargin(0);
    sublayout->setSpacing(16);

    account->setTextMargins(16,0,0,0);
    pass->setTextMargins(16,0,0,0);
    valid_code->setTextMargins(16,0,0,0);

    layout->addWidget(phone);
    layout->addWidget(account);
    layout->addWidget(pass);
    sublayout->addWidget(valid_code,0,Qt::AlignLeft);
    sublayout->addWidget(send_code,0,Qt::AlignRight);
    layout->addLayout(sublayout);
    layout->addWidget(tips);
    layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(layout);

    tips->hide();
    phone->setFocus();
    adjustSize();
}


void BindPhoneDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

void BindPhoneDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                  "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

void BindPhoneDialog::setclear() {
    phone->setText("");
    account->setText("");
    valid_code->setText("");
}

QPushButton* BindPhoneDialog::get_send_code() {
    return send_code;
}

QLabel* BindPhoneDialog::get_tips() {
    return tips;
}

QString BindPhoneDialog::get_code() {
    return valid_code->text();
}

QString BindPhoneDialog::get_pass() {
    return pass->text();
}

QString BindPhoneDialog::get_phone() {
    return phone->text();
}

QString BindPhoneDialog::get_account() {
    return account->text();
}

QLineEdit* BindPhoneDialog::get_code_lineedit() {
    return valid_code;
}

ql_lineedit_pass* BindPhoneDialog::get_pass_lineedit() {
    return pass;
}

area_code_lineedit* BindPhoneDialog::get_phone_lineedit() {
    return phone;
}

QLineEdit *BindPhoneDialog::get_account_lineedit() {
    return account;
}
