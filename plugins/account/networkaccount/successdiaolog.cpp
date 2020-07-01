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
#include "successdiaolog.h"

SuccessDiaolog::SuccessDiaolog(QWidget *parent) : QWidget(parent)
{
    bkg = new QSvgWidget(":/new/image/success.svg");
    text_msg = new QLabel(this);
    back_login = new QPushButton(this);
    layout = new QVBoxLayout;

    bkg->setFixedSize(148,148);

    text_msg->setStyleSheet("font-size:24px");

    text_msg->adjustSize();

    back_login->setFixedSize(338,36);
    back_login->setText(tr("Reback sign in"));
    back_login->setStyleSheet("QPushButton {font-size:14px;background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                              "QPushButton:hover {font-size:14px;background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                              "QPushButton:click {font-size:14px;background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    back_login->setContentsMargins(0,16,0,0);

    layout->setContentsMargins(41,100,41,110);
    layout->setSpacing(0);
    layout->addWidget(bkg,0,Qt::AlignCenter);
    layout->addWidget(text_msg,0,Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(back_login,0,Qt::AlignCenter | Qt::AlignBottom);

    setLayout(layout);
}

/* 成功消息统一接收机制 */
void SuccessDiaolog::set_mode_text(int mode) {
    if(mode == 0) {
        text_msg->setText(tr("Sign up success!"));
        back_login->setText(tr("Confirm"));
    } else if(mode == 1) {
        text_msg->setText(tr("Reset success!"));
        back_login->setText(tr("Confirm"));
    } else if(mode == 2) {
        text_msg->setText(tr("Sign in success!"));
        back_login->setText(tr("Confirm"));
    } else if(mode == 3) {
        text_msg->setText(tr("Binding phone success!"));
        back_login->setText(tr("Confirm"));
    }
}
