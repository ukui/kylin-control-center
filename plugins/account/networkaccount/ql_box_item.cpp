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
#include "ql_box_item.h"

ql_box_item::ql_box_item(QWidget *parent) : QWidget(parent)
{
    cd= new QLabel(this);
    ct = new QLabel(this);
    cd->setStyleSheet("QLabel{color:rgba(0,0,0,0.85);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    ct->setStyleSheet("QLabel{color:rgba(0,0,0,0.65);font-size:14px}"
                      "QLabel:hover{color:#FFFFFF;font-size:14px}");
    QHBoxLayout *layout = new QHBoxLayout;
    cd->setObjectName("code");
    ct->setObjectName("country");

    layout->setContentsMargins(16,0,16,0);
    layout->setSpacing(0);
    layout->addWidget(cd,0,Qt::AlignLeft);
    layout->addWidget(ct,0,Qt::AlignRight);
    setLayout(layout);
    //setFixedSize(200,36);
}

void ql_box_item::set_code(QString str) {
    cd->setText(str);
}

void ql_box_item::set_country_code(QString str) {
    ct->setText(str);
}

void ql_box_item::enterEvent(QEvent *e) {
    QString str = "color:#FFFFFF;font-size:14px";
    cd->setStyleSheet(str);
    ct->setStyleSheet(str);
}

void ql_box_item::leaveEvent(QEvent *e) {
    cd->setStyleSheet("color:rgba(0,0,0,0.85);font-size:14px");
    ct->setStyleSheet("color:rgba(0,0,0,0.65);font-size:14px");
}
