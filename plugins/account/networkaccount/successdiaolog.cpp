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
    m_bkgWidget = new QSvgWidget(":/new/image/success.svg");
    m_textLabel = new QLabel(this);
    m_backloginBtn = new QPushButton(this);
    m_workLayout = new QVBoxLayout;

    m_bkgWidget->setFixedSize(148,148);

    m_textLabel->setStyleSheet("font-size:24px");

    m_textLabel->adjustSize();

    m_backloginBtn->setFixedSize(338,36);
    m_backloginBtn->setText(tr("Reback sign in"));
    m_backloginBtn->setStyleSheet("QPushButton {background-color: #3D6BE5;border-radius: 4px;color:rgba(255,255,255,0.85);}"
                              "QPushButton:hover {background-color: #415FC4;border-radius: 4px;position:relative;color:rgba(255,255,255,0.85);}"
                              "QPushButton:click {background-color: #415FC4;border-radius: 4px;postion:realative;color:rgba(255,255,255,0.85);}");
    m_backloginBtn->setContentsMargins(0,16,0,0);

    m_workLayout->setContentsMargins(41,100,41,110);
    m_workLayout->setSpacing(0);
    m_workLayout->addWidget(m_bkgWidget,0,Qt::AlignCenter);
    m_workLayout->addWidget(m_textLabel,0,Qt::AlignCenter);
    m_workLayout->addStretch();
    m_workLayout->addWidget(m_backloginBtn,0,Qt::AlignCenter | Qt::AlignBottom);

    setLayout(m_workLayout);
}

/* 成功消息统一接收机制 */
void SuccessDiaolog::set_mode_text(const int &mode) {
    if (mode == 0) {
        m_textLabel->setText(tr("Sign up success!"));
        m_backloginBtn->setText(tr("Confirm"));
    } else if (mode == 1) {
        m_textLabel->setText(tr("Reset success!"));
        m_backloginBtn->setText(tr("Confirm"));
    } else if (mode == 2) {
        m_textLabel->setText(tr("Sign in success!"));
        m_backloginBtn->setText(tr("Confirm"));
    } else if (mode == 3) {
        m_textLabel->setText(tr("Binding phone success!"));
        m_backloginBtn->setText(tr("Confirm"));
    }
}
