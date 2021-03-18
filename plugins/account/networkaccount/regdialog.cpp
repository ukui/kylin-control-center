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
#include "regdialog.h"

RegDialog::RegDialog(QWidget *parent) : QWidget(parent)
{
    this->setFixedWidth(338);
    m_phoneLineEdit = new AreaCodeLineEdit(this);
    m_accountLineEdit = new QLineEdit(this);
    m_passLineEdit = new PasswordLineEdit(this);
    m_confirmLineEdit = new PasswordLineEdit(this);
    m_mcodeLineEdit = new QLineEdit(this);
    m_sendMsgBtn = new QPushButton(tr("Get"),this);

    m_workLayout = new QVBoxLayout;
    m_mcodeLayout = new QHBoxLayout;
    m_passTips = new QLabel(this);
    m_accountTips = new QLabel(this);
    m_errorTips = new Tips(this);
    m_svgHandler = new SVGHandler(this);

    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;color:rgba(0,0,0,0.85)}");
    m_phoneLineEdit->setFixedSize(QSize(338,36));
    m_accountLineEdit->setFixedSize(QSize(338,36));
    m_passLineEdit->setFixedSize(QSize(338,36));
    m_mcodeLineEdit->setFixedSize(QSize(192,36));
    m_passTips->setFixedWidth(330);
   // m_sendMsgBtn->setFixedSize(QSize(130,36));

    m_sendMsgBtn->setMaximumSize(136,36);
    m_sendMsgBtn->setMinimumSize(130,36);
    m_sendMsgBtn->resize(130,36);

    m_confirmLineEdit->setFixedSize(QSize(338,36));
    m_phoneLineEdit->setFocusPolicy(Qt::StrongFocus);


    m_accountLineEdit->setMaxLength(30);
    m_phoneLineEdit->setMaxLength(11);
    m_passLineEdit->setPlaceholderText(tr("Your password here"));
    m_passLineEdit->setEchoMode(QLineEdit::Password);
    m_confirmLineEdit->setEchoMode(QLineEdit::Password);
    m_accountLineEdit->setPlaceholderText(tr("Your account here"));
    m_confirmLineEdit->setPlaceholderText(tr("Confirm your password"));
    m_mcodeLineEdit->setPlaceholderText(tr("Your code here"));
    m_mcodeLineEdit->setMaxLength(4);
    m_accountTips->setText(tr("This operation is permanent"));
    m_accountTips->setContentsMargins(12,0,0,0);
    //m_accountTips->setFixedHeight(16);
    m_passTips->setText(tr("At least 6 bit, include letters and digt"));
   // m_passTips->setFixedHeight(16);
    m_passTips->setContentsMargins(12,0,0,0);
    m_accountTips->setStyleSheet("");
    m_passTips->setStyleSheet("");

    m_confirmLineEdit->setTextMargins(12,0,0,0);
    m_accountLineEdit->setTextMargins(12,0,0,0);
    //m_passLineEdit->setTextMargins(12,0,0,0);
    m_mcodeLineEdit->setTextMargins(12,0,0,0);
    //valid_code->setStyleSheet(str);

    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, m_mcodeLineEdit );
    m_mcodeLineEdit->setValidator( validator );

    //reg_pass->setStyleSheet(str);
    //reg_phone->setStyleSheet(str);
   // reg_user->setStyleSheet(str);
   // reg_confirm->setStyleSheet(str);
    //send_msg_btn->setStyleSheet("QPushButton{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
     //                           "QPushButton:hover{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
    //                            "QPushButton:click{background-color:#F4F4F4;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");

    m_workLayout->setMargin(0);
    m_workLayout->setSpacing(8);
    m_workLayout->addWidget(m_phoneLineEdit);
    m_workLayout->addWidget(m_accountLineEdit);
    m_workLayout->addWidget(m_accountTips);
    m_workLayout->addWidget(m_passLineEdit);
    m_workLayout->addWidget(m_passTips);
    m_workLayout->addWidget(m_confirmLineEdit);
    m_mcodeLayout->setMargin(0);
    m_mcodeLayout->setSpacing(16);
    m_mcodeLayout->addWidget(m_mcodeLineEdit);
    m_mcodeLayout->addWidget(m_sendMsgBtn);
    m_mcodeLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_workLayout->addLayout(m_mcodeLayout);
    m_workLayout->addWidget(m_errorTips);
    m_workLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(m_workLayout);

    QRegExp regx_phn("^((13[0-9])|(14[5,7])|(15[0-3,5-9])|(17[0,3,5-8])|(18[0-9])|166|198|199|(147))\\d{8}$");
    QValidator *validator_phn = new QRegExpValidator(regx_phn, m_phoneLineEdit);
    m_phoneLineEdit->setValidator(validator_phn);

    QRegExp regx_acc("^[a-zA-Z0-9_@.-]+$");
    QValidator *validator_acc = new QRegExpValidator(regx_acc, m_accountLineEdit );
    m_accountLineEdit->setValidator(validator_acc);

    m_passTips->hide();
    m_passTips->setAttribute(Qt::WA_DontShowOnScreen);
    m_accountTips->hide();
    m_accountTips->setAttribute(Qt::WA_DontShowOnScreen);

    m_errorTips->hide();
    m_errorTips->setAttribute(Qt::WA_DontShowOnScreen);
    m_sendMsgBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_mcodeLineEdit,SIGNAL(textChanged(QString)),this,SLOT(change_uppercase()));
    connect(this,SIGNAL(code_changed()),this,SLOT(setstyleline()));
    connect(m_passLineEdit,&PasswordLineEdit::verify_text,[this] () {
       m_passTips->setText(tr("Your password is valid!"));
    });
    connect(m_passLineEdit,&PasswordLineEdit::false_text,[this] () {
       m_passTips->setText(tr("At least 6 bit, include letters and digt"));
    });
    adjustSize();
    m_phoneLineEdit->setFocus();
}

void RegDialog::set_staus(const bool &ok) {
    m_confirmLineEdit->setEnabled(ok);
    m_accountLineEdit->setEnabled(ok);
    m_phoneLineEdit->setEnabled(ok);
    m_mcodeLineEdit->setEnabled(ok);
    m_sendMsgBtn->setEnabled(ok);
}

/* 更新设置错误提示 */
void RegDialog::setstyleline() {
    m_errorTips->set_text(m_errorCode);
}

/* 获取错误代码 */
void RegDialog::set_code(const QString &codenum) {
    m_errorCode = codenum;
    emit code_changed();
}

PasswordLineEdit* RegDialog::get_reg_confirm() {
    return m_confirmLineEdit;
}

QLabel* RegDialog::get_pass_tip() {

    return m_passTips;
}

QLabel* RegDialog::get_user_tip() {
    return m_accountTips;
}

QLineEdit* RegDialog::get_reg_user() {
    return m_accountLineEdit;
}

PasswordLineEdit* RegDialog::get_reg_pass() {
    return m_passLineEdit;
}

QLineEdit* RegDialog::get_valid_code() {
    return m_mcodeLineEdit;
}

AreaCodeLineEdit* RegDialog::get_phone_user() {
    return m_phoneLineEdit;
}

QPushButton* RegDialog::get_send_code() {
    return m_sendMsgBtn;
}

/* 小写字母转大写，备用 */
void RegDialog::change_uppercase() {
    QString str = m_mcodeLineEdit->text();
    m_mcodeLineEdit->setText(str.toUpper());
}

QString RegDialog::get_user_mcode() const {
    return m_mcodeLineEdit->text();
}

QString RegDialog::get_user_phone() const {
    return m_phoneLineEdit->text();
}

QString RegDialog::get_user_account() const {
    return m_accountLineEdit->text();
}
QString RegDialog::get_user_passwd() const {
    return m_passLineEdit->text();
}

/* 清空注册框 */
void RegDialog::set_clear() {
    if (!m_errorTips->isHidden()) {
        m_errorTips->hide();
    }
    m_passLineEdit->get_visble()->setChecked(false);
    m_confirmLineEdit->get_visble()->setChecked(false);
    m_passLineEdit->setText("");
    m_accountLineEdit->setText("");
    m_phoneLineEdit->setText("");
    m_mcodeLineEdit->setText("");
    m_confirmLineEdit->setText("");
}

Tips* RegDialog::get_tips() {
    return m_errorTips;
}
