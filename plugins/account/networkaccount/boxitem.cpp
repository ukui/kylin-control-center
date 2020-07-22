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
#include "boxitem.h"

BoxItem::BoxItem(QWidget *parent) : QWidget(parent)
{
    m_countryCode= new QLabel(this);
    m_countryName = new QLabel(this);
    m_countryCode->setStyleSheet("QLabel{color:rgba(0,0,0,0.85);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    m_countryName->setStyleSheet("QLabel{color:rgba(0,0,0,0.65);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    QHBoxLayout *layout = new QHBoxLayout;
    m_countryCode->setObjectName("code");
    m_countryName->setObjectName("country");

    layout->setContentsMargins(16,0,16,0);
    layout->setSpacing(0);
    layout->addWidget(m_countryCode,0,Qt::AlignLeft);
    layout->addWidget(m_countryName,0,Qt::AlignRight);
    setLayout(layout);
    //setFixedSize(200,36);
}

/* 设置国家名字 */
void BoxItem::set_code(QString str) {
    m_countryCode->setText(str);
}

/* 设置国家代码 */
void BoxItem::set_country_code(QString str) {
    m_countryName->setText(str);
}

/* 鼠标进入选框，相对应的国家及代码变白色 */
void BoxItem::enterEvent(QEvent *e) {
    QString str = "color:#FFFFFF;font-size:14px";
    m_countryCode->setStyleSheet(str);
    m_countryName->setStyleSheet(str);
}

/* 鼠标离开选框，相对应的国家及代码变回黑色 */
void BoxItem::leaveEvent(QEvent *e) {
    m_countryCode->setStyleSheet("color:rgba(0,0,0,0.85);font-size:14px");
    m_countryName->setStyleSheet("color:rgba(0,0,0,0.65);font-size:14px");
}
