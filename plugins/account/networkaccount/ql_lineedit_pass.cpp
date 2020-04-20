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
#include "ql_lineedit_pass.h"

ql_lineedit_pass::ql_lineedit_pass(QWidget *parent) : QLineEdit(parent)
{
    setEchoMode(QLineEdit::Password);
    visble = new QPushButton(this);
    layout = new QHBoxLayout;

    visble->setCursor(Qt::PointingHandCursor);
    visble->setCheckable(true);
    connect(visble,&QPushButton::toggled,[this] (bool checked) {
        if(checked) {
            setEchoMode(QLineEdit::Normal);
        } else {
            setEchoMode(QLineEdit::Password);
        }
    });
    visble->setStyleSheet("QPushButton{width:  16px;height: 16px;qproperty-flat: true;"
    "margin-right: 4px;border: none;border-width: 0;"
    "border-image: url(:/invisible.png) 0 0 0 0 stretch stretch;"
    "background: transparent;}"
    "QPushButton:checked{border-image: url(:/visible.png) 0 0 0 0 stretch stretch;}");

    layout->addStretch();
    layout->addWidget(visble);
    layout->setMargin(0);
    visble->setFocusPolicy(Qt::NoFocus);
    setLayout(layout);
}
