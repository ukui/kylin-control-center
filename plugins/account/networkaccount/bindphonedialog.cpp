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
    valid_code = new QLineEdit(this);
    send_code = new QPushButton(this);

    tips = new QLabel(this);

    layout = new QVBoxLayout;
    sublayout = new QHBoxLayout;

    QString str = ("QLineEdit{background-color:#F4F4F4;border-radius: 4px;border:1px none #3D6BE5;font-size: 14px;color: rgba(0,0,0,0.85);lineedit-password-character: 42;}"
                   "QLineEdit:hover{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}"
                   "QLineEdit:focus{background-color:#F4F4F4;border-radius: 4px;border:1px solid #3D6BE5;font-size: 14px;color:rgba(0,0,0,0.85)}");
    //valid_code->setStyleSheet(str);
    //phone->setStyleSheet(str);
    valid_code->setPlaceholderText(tr("Your code here"));
    send_code->setText(tr("Get phone code"));
    //send_code->setStyleSheet("QPushButton{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(0,0,0,0.85);} "
    //                         "QPushButton:hover{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}"
    //                         "QPushButton:click{background-color:#F4F4F4;font-size:14px;border-radius: 4px;border:4px solid #F4F4F4;color:rgba(61,107,229,0.85);}");


    phone->setFixedSize(QSize(338,36));
    valid_code->setFixedSize(120,36);
    send_code->setFixedSize(198,36);
    layout->setMargin(0);
    layout->setSpacing(8);
    layout->setAlignment(Qt::AlignTop);
    sublayout->setMargin(0);
    sublayout->setSpacing(16);

    valid_code->setTextMargins(12,0,0,0);

    layout->addWidget(phone);
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

/* 设置错误代码，并发出错误代码更换信号，传给主框，让主框完成错误代码更新 */
void BindPhoneDialog::set_code(QString codenum) {
    code = codenum;
    emit code_changed();
}

/* 富文本处理错误提示消息 */
void BindPhoneDialog::setstyleline() {
    tips->setText("<html><head/><body><p><img src=':/new/image/_.png'/><span style=' font-size:14px;color:#F53547'>"
                  "&nbsp;&nbsp;"+code+"</span></p></body></html>");
}

/* 清理绑定手机号码框 */
void BindPhoneDialog::setclear() {
    phone->setText("");
    valid_code->setText("");
}

/* 获取验证码发送按钮 */
QPushButton* BindPhoneDialog::get_send_code() {
    return send_code;
}

/* 获取密码输入提示文本 */
QLabel* BindPhoneDialog::get_tips() {
    return tips;
}

/* 获取验证码字符串 */
QString BindPhoneDialog::get_code() {
    return valid_code->text();
}


/* 获取手机号码字符串 */
QString BindPhoneDialog::get_phone() {
    return phone->text();
}

/* 获取验证码输入框 */
QLineEdit* BindPhoneDialog::get_code_lineedit() {
    return valid_code;
}


/* 获取手机号码输入框 */
area_code_lineedit* BindPhoneDialog::get_phone_lineedit() {
    return phone;
}
