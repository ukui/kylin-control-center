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
    m_phoneLineEdit = new AreaCodeLineEdit(this);
    m_mCodeLineEdit = new QLineEdit(this);
    m_sendCode_btn = new QPushButton(this);

    m_tips = new Tips(this);
    m_svgHandler = new SVGHandler(this);
    m_vboxLayout = new QVBoxLayout;
    m_subHBoxLayout = new QHBoxLayout;

    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //valid_code->setStyleSheet(str);
    //phone->setStyleSheet(str);
    m_mCodeLineEdit->setPlaceholderText(tr("Your code here"));
    m_sendCode_btn->setText(tr("Get phone code"));
    //send_code->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
    //                         "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
    //                         "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");


    m_phoneLineEdit->setFixedSize(QSize(338,36));
    m_mCodeLineEdit->setFixedSize(120,36);
    m_sendCode_btn->setFixedSize(198,36);
    m_vboxLayout->setMargin(0);
    m_vboxLayout->setSpacing(8);
    m_vboxLayout->setAlignment(Qt::AlignTop);
    m_subHBoxLayout->setMargin(0);
    m_subHBoxLayout->setSpacing(16);

    m_mCodeLineEdit->setTextMargins(12,0,0,0);

    m_vboxLayout->addWidget(m_phoneLineEdit);
    m_subHBoxLayout->addWidget(m_mCodeLineEdit,0,Qt::AlignLeft);
    m_subHBoxLayout->addWidget(m_sendCode_btn,0,Qt::AlignRight);
    m_vboxLayout->addLayout(m_subHBoxLayout);
    m_vboxLayout->addWidget(m_tips);
    m_vboxLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setLayout(m_vboxLayout);

    m_tips->hide();
    m_phoneLineEdit->setFocus();
    adjustSize();
}

void BindPhoneDialog::set_staus(bool ok) {
   m_phoneLineEdit->setEnabled(ok);
   m_mCodeLineEdit->setEnabled(ok);
   m_sendCode_btn->setEnabled(ok);
}

/* 设置错误代码，并发出错误代码更换信号，传给主框，让主框完成错误代码更新 */
void BindPhoneDialog::set_code(QString codenum) {
    m_countryCode = codenum;
    emit code_changed();
}

/* 富文本处理错误提示消息 */
void BindPhoneDialog::setstyleline() {
    m_tips->set_text(m_countryCode);
}

/* 清理绑定手机号码框 */
void BindPhoneDialog::setclear() {
    m_phoneLineEdit->setText("");
    m_mCodeLineEdit->setText("");
}

/* 获取验证码发送按钮 */
QPushButton* BindPhoneDialog::get_send_code() {
    return m_sendCode_btn;
}

/* 获取密码输入提示文本 */
Tips* BindPhoneDialog::get_tips() {
    return m_tips;
}

/* 获取验证码字符串 */
QString BindPhoneDialog::get_code() {
    return m_mCodeLineEdit->text();
}


/* 获取手机号码字符串 */
QString BindPhoneDialog::get_phone() {
    return m_phoneLineEdit->text();
}

/* 获取验证码输入框 */
QLineEdit* BindPhoneDialog::get_code_lineedit() {
    return m_mCodeLineEdit;
}


/* 获取手机号码输入框 */
AreaCodeLineEdit* BindPhoneDialog::get_phone_lineedit() {
    return m_phoneLineEdit;
}
