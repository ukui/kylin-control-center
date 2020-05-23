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
#include "passdialog.h"

PassDialog::PassDialog(QWidget *parent) : QWidget(parent)
{
    reg_phone = new QLineEdit(this);
    reg_pass = new ql_lineedit_pass(this);
    reg_pass_confirm = new ql_lineedit_pass(this);
    valid_code = new QLineEdit(this);
    send_msg_btn = new QPushButton(tr("Get the phone binding code"),this);
    passtips = new QLabel(this);

    vlayout = new QVBoxLayout;
    hlayout = new QHBoxLayout;
    tips = new QLabel(this);
    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    reg_phone->setFixedSize(QSize(338,36));
    reg_pass->setFixedSize(QSize(338,36));
    reg_pass_confirm->setFixedSize(QSize(338,36));
    valid_code->setFixedSize(QSize(120,34));
    send_msg_btn->setFixedSize(QSize(198,34));
    reg_phone->setFocusPolicy(Qt::StrongFocus);
    reg_phone->setFocus();


    reg_phone->setPlaceholderText(tr("Your account here"));
    reg_pass->setPlaceholderText(tr("Your new password here"));
    reg_pass->setEchoMode(QLineEdit::Password);
    reg_pass_confirm->setPlaceholderText(tr("Confirm your new password"));
    reg_pass_confirm->setEchoMode(QLineEdit::Password);
    valid_code->setPlaceholderText(tr("Your code here"));
    valid_code->setMaxLength(4);

    reg_phone->setTextMargins(12,0,0,0);
    reg_pass->setTextMargins(12,0,0,0);
    reg_pass_confirm->setTextMargins(12,0,0,0);
    valid_code->setTextMargins(12,0,0,0);
    passtips->setFixedHeight(16);

    //valid_code->setStyleSheet(str);
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, valid_code );
    valid_code->setValidator( validator );

    QRegExp regx_acc("^[a-zA-Z0-9_@.-]+$");
    QValidator *validator_acc = new QRegExpValidator(regx_acc, reg_phone );
    reg_phone->setValidator(validator_acc);

//    QRegExp regx_pas("^[a-zA-Z0-9_-]{4,16}$");
//    QValidator *validator_pas = new QRegExpValidator(regx_pas, reg_pass );
//    reg_phone->setValidator(validator_acc);

    passtips->setText(tr("At least 6 bit, include letters and digt"));
    passtips->setStyleSheet("font-size:14px;");
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
    //reg_pass->setStyleSheet(str);
    //reg_phone->setStyleSheet(str);
    //reg_pass_confirm->setStyleSheet(str);
    //send_msg_btn->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
    //                            "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
    //                            "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");

    vlayout->setMargin(0);
    vlayout->setSpacing(8);
    vlayout->addWidget(reg_phone);
    vlayout->addWidget(reg_pass);
    vlayout->addWidget(passtips);
    passtips->setContentsMargins(12,0,0,0);
    vlayout->addWidget(reg_pass_confirm);
    hlayout->setMargin(0);
    hlayout->setSpacing(16);
    hlayout->addWidget(valid_code);
    hlayout->addWidget(send_msg_btn);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(tips);
    vlayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    send_msg_btn->setFocusPolicy(Qt::NoFocus);
    setLayout(vlayout);

    adjustSize();

    tips->hide();
    passtips->hide();
    passtips->setAttribute(Qt::WA_DontShowOnScreen);

    connect(valid_code,SIGNAL(textChanged(QString)),this,SLOT(change_uppercase()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
}

/* 获取错误代码 */
void PassDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

/* 设置错误代码更新 */
void PassDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                        "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

/* 以下均为类接口函数 */
QLabel* PassDialog::get_passtips() {
    return passtips;
}

void PassDialog::change_uppercase() {
    QString str = valid_code->text();
    valid_code->setText(str.toUpper());
}

QPushButton* PassDialog::get_send_msg_btn() {
    return send_msg_btn;
}

QString PassDialog::get_user_name() {
    return reg_phone->text();
}

QString PassDialog::get_user_mcode() {
    return valid_code->text();
}

QString PassDialog::get_user_confirm() {
    return reg_pass_confirm->text();
}

QString PassDialog::get_user_newpass() {
    return reg_pass->text();
}

ql_lineedit_pass* PassDialog::get_reg_pass() {
    return reg_pass;
}

QLineEdit* PassDialog::get_reg_phone() {
    return reg_phone;
}

ql_lineedit_pass* PassDialog::get_reg_pass_confirm() {
    return reg_pass_confirm;
}

QLineEdit* PassDialog::get_valid_code() {
    return valid_code;
}

/* 清空忘记密码框 */
void PassDialog::set_clear() {
    if(!tips->isHidden()) {
        tips->hide();
    }
    reg_pass->get_visble()->setChecked(false);
    reg_pass_confirm->get_visble()->setChecked(false);
    reg_pass->setText("");
    reg_phone->setText("");
    reg_pass_confirm->setText("");
    valid_code->setText("");
}

QLabel* PassDialog::get_tips() {
    return tips;
}
