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
    svg_hd = new ql_svg_handler(this);

    visble->setCursor(Qt::PointingHandCursor);
    visble->setCheckable(true);
    visble->setFlat(true);
    visble->setFixedSize(32,32);
    QPixmap pixmap = svg_hd->loadSvg(":/new/image/invisible.svg");
    visble->setIcon(pixmap);
    connect(visble,&QPushButton::toggled,[this] (bool checked) {
        if(checked) {
            setEchoMode(QLineEdit::Normal);
            QPixmap pixmap = svg_hd->loadSvg(":/new/image/visible.svg");
            visble->setIcon(pixmap);
        } else {
            setEchoMode(QLineEdit::Password);
            QPixmap pixmap = svg_hd->loadSvg(":/new/image/invisible.svg");
            visble->setIcon(pixmap);
        }
    }); //点击后可见或者不可见
    visble->setStyleSheet("QPushButton{width:  16px;height: 16px;qproperty-flat: true;"
    "margin-right: 8px;border: none;border-width: 0;"
    "background: transparent;}");

    layout->addStretch();
    layout->addWidget(visble);
    layout->setMargin(0);
    visble->setFocusPolicy(Qt::NoFocus);
    setLayout(layout);

}

/* 获取密码显示按钮 */
QPushButton* ql_lineedit_pass::get_visble() {
    return visble;
}

/* 密码检测模块 */
bool ql_lineedit_pass::check() {
    bool uper = false;
    bool normal = false;
    bool number = false;
    bool line = false;
    if(this->text() != "") {
        QString str = this->text();
        for(QChar c:str) {
            if(c>='A' && c <= 'Z') {
                uper = true;
                continue;
            }
            if(c>='a' && c <='z') {
                normal = true;
                continue;
            }
            if(c>='0' && c<='9') {
                number = true;
                continue;
            }
        }
        if(text().length() >= 6) {
            line = true;
        }
    } else {
        return false;
    }
    bool ok = uper && number && line == true ?true:normal && number && line;
    return  ok;
}
