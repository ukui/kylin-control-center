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
    m_phoneLineEdit = new QLineEdit(this);
    m_passLineEdit = new PasswordLineEdit(this);
    m_confirmLineEdit = new PasswordLineEdit(this);
    m_mcodeLineEdit = new QLineEdit(this);
    m_sendMsgBtn = new QPushButton(tr("Get the phone binding code"),this);
    m_passwordTips = new QLabel(this);

    m_workLayout = new QVBoxLayout;
    m_hboxLayout = new QHBoxLayout;
    m_errorTips = new Tips(this);
    m_svgHandler = new SVGHandler(this);
    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;color:rgba(0,0,0,0.85)}");
    m_phoneLineEdit->setFixedSize(QSize(338,36));
    m_passLineEdit->setFixedSize(QSize(338,36));
    m_confirmLineEdit->setFixedSize(QSize(338,36));
    m_mcodeLineEdit->setFixedSize(QSize(120,36));

    m_sendMsgBtn->setMaximumSize(205,36);
    m_sendMsgBtn->setMinimumSize(198,36);
    m_sendMsgBtn->resize(198,36);

    m_phoneLineEdit->setFocusPolicy(Qt::StrongFocus);
    m_phoneLineEdit->setFocus();


    m_phoneLineEdit->setPlaceholderText(tr("Your account here"));
    m_passLineEdit->setPlaceholderText(tr("Your new password here"));
    m_passLineEdit->setEchoMode(QLineEdit::Password);
    m_confirmLineEdit->setPlaceholderText(tr("Confirm your new password"));
    m_confirmLineEdit->setEchoMode(QLineEdit::Password);
    m_mcodeLineEdit->setPlaceholderText(tr("Your code here"));
    m_mcodeLineEdit->setMaxLength(4);

    m_phoneLineEdit->setTextMargins(12,0,0,0);
    //m_passLineEdit->setTextMargins(12,0,0,0);
    //m_confirmLineEdit->setTextMargins(12,0,0,0);
    m_mcodeLineEdit->setTextMargins(12,0,0,0);
    //m_passwordTips->setFixedHeight(16);

    //valid_code->setStyleSheet(str);
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, m_mcodeLineEdit );
    m_mcodeLineEdit->setValidator( validator );

    QRegExp regx_acc("^[a-zA-Z0-9_@.-]+$");
    QValidator *validator_acc = new QRegExpValidator(regx_acc, m_phoneLineEdit );
    m_phoneLineEdit->setValidator(validator_acc);

//    QRegExp regx_pas("^[a-zA-Z0-9_-]{4,16}$");
//    QValidator *validator_pas = new QRegExpValidator(regx_pas, reg_pass );
//    reg_phone->setValidator(validator_acc);

    m_passwordTips->setText(tr("At least 6 bit, include letters and digt"));
    m_passwordTips->setStyleSheet("");
    //reg_pass->setStyleSheet(str);
    //reg_phone->setStyleSheet(str);
    //reg_pass_confirm->setStyleSheet(str);
    //send_msg_btn->setStyleSheet("QPushButton{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
    //                            "QPushButton:hover{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
    //                            "QPushButton:click{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");

    m_workLayout->setMargin(0);
    m_workLayout->setSpacing(8);
    m_workLayout->addWidget(m_phoneLineEdit);
    m_workLayout->addWidget(m_passLineEdit);
    m_workLayout->addWidget(m_passwordTips);
    m_passwordTips->setContentsMargins(12,0,0,0);
    m_workLayout->addWidget(m_confirmLineEdit);
    m_hboxLayout->setMargin(0);
    m_hboxLayout->setSpacing(16);
    m_hboxLayout->addWidget(m_mcodeLineEdit);
    m_hboxLayout->addWidget(m_sendMsgBtn);
    m_hboxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_workLayout->addLayout(m_hboxLayout);
    m_workLayout->addWidget(m_errorTips);
    m_workLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_sendMsgBtn->setFocusPolicy(Qt::NoFocus);
    setLayout(m_workLayout);

    adjustSize();

    m_passwordTips->setFixedWidth(330);
    m_errorTips->hide();
    m_passwordTips->hide();
    m_passwordTips->setAttribute(Qt::WA_DontShowOnScreen);
    connect(m_passLineEdit,&PasswordLineEdit::verify_text,[this] () {
       m_passwordTips->setText(tr("Your password is valid!"));
    });
    connect(m_passLineEdit,&PasswordLineEdit::false_text,[this] () {
       m_passwordTips->setText(tr("At least 6 bit, include letters and digt"));
    });
    connect(m_mcodeLineEdit,SIGNAL(textChanged(QString)),this,SLOT(change_uppercase()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
}

/* 获取错误代码 */
void PassDialog::set_code(const QString &codenum) {
    m_errorCode = codenum;
    emit code_changed();
}

/* 设置错误代码更新 */
void PassDialog::setstyleline() {
    m_errorTips->set_text(m_errorCode);
}

/* 以下均为类接口函数 */
QLabel* PassDialog::get_passtips() {
    return m_passwordTips;
}

void PassDialog::change_uppercase() {
    QString str = m_mcodeLineEdit->text();
    m_mcodeLineEdit->setText(str.toUpper());
}

QPushButton* PassDialog::get_send_msg_btn() {
    return m_sendMsgBtn;
}

QString PassDialog::get_user_name() const {
    return m_phoneLineEdit->text();
}

QString PassDialog::get_user_mcode() const {
    return m_mcodeLineEdit->text();
}

QString PassDialog::get_user_confirm() const {
    return m_confirmLineEdit->text();
}

QString PassDialog::get_user_newpass() const {
    return m_passLineEdit->text();
}

PasswordLineEdit* PassDialog::get_reg_pass() {
    return m_passLineEdit;
}

QLineEdit* PassDialog::get_reg_phone() {
    return m_phoneLineEdit;
}

PasswordLineEdit* PassDialog::get_reg_pass_confirm() {
    return m_confirmLineEdit;
}

QLineEdit* PassDialog::get_valid_code() {
    return m_mcodeLineEdit;
}

void PassDialog::set_staus(const bool &ok) {
    m_passLineEdit->setEnabled(ok);
    m_confirmLineEdit->setEnabled(ok);
    m_phoneLineEdit->setEnabled(ok);
    m_mcodeLineEdit->setEnabled(ok);
    m_sendMsgBtn->setEnabled(ok);
}

/* 清空忘记密码框 */
void PassDialog::set_clear() {
    if (!m_errorTips->isHidden()) {
        m_errorTips->hide();
    }
    m_passLineEdit->get_visble()->setChecked(false);
    m_confirmLineEdit->get_visble()->setChecked(false);
    m_passLineEdit->setText("");
    m_phoneLineEdit->setText("");
    m_confirmLineEdit->setText("");
    m_mcodeLineEdit->setText("");
}

Tips* PassDialog::get_tips() {
    return m_errorTips;
}
