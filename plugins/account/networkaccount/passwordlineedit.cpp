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
#include "passwordlineedit.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent) : QLineEdit(parent)
{
    setEchoMode(QLineEdit::Password);
    m_changeStatusBtn = new VisbleButton(this);
    m_workLayout = new QHBoxLayout;
    m_svgHandler = new SVGHandler(this);

    m_changeStatusBtn->setCursor(Qt::PointingHandCursor);
    m_changeStatusBtn->setFixedSize(32,32);
    QPixmap pixmap = m_svgHandler->loadSvgColor(":/new/image/invisible.svg","gray",16);
    m_changeStatusBtn->setPixmap(pixmap);
    connect(m_changeStatusBtn,&VisbleButton::toggled,[this] (bool checked) {
        if (checked) {
            setEchoMode(QLineEdit::Normal);
            QPixmap pixmap = m_svgHandler->loadSvgColor(":/new/image/visible.svg","gray",16);
            m_changeStatusBtn->setPixmap(pixmap);
        } else {
            setEchoMode(QLineEdit::Password);
            QPixmap pixmap = m_svgHandler->loadSvgColor(":/new/image/invisible.svg","gray",16);
            m_changeStatusBtn->setPixmap(pixmap);
        }
    }); //点击后可见或者不可见
    m_changeStatusBtn->setStyleSheet("QLabel{width:  16px;height: 16px;"
    "margin-right: 8px;border: none;border-width: 0;}");

    m_workLayout->addStretch();
    m_workLayout->addWidget(m_changeStatusBtn);
    m_workLayout->setMargin(0);
    connect(this,&PasswordLineEdit::textChanged,[this] (const QString &text) {
        bool uper = false;
        bool normal = false;
        bool number = false;
        bool line = false;
        for(QChar c:text) {
            if (c>='A' && c <= 'Z') {
                uper = true;
                continue;
            }
            if (c>='a' && c <='z') {
                normal = true;
                continue;
            }
            if (c>='0' && c<='9') {
                number = true;
                continue;
            }
        }
        if (text.length() >= 6) {
            line = true;
        }
        bool ok = uper && number && line == true ?true:normal && number && line;
        if (ok) {
            emit verify_text();
        } else {
            emit false_text();
        }
    });
    m_changeStatusBtn->setFocusPolicy(Qt::NoFocus);
    setLayout(m_workLayout);
    setMaxLength(30);
    setTextMargins(12,0,28,0);

}

/* 获取密码显示按钮 */
VisbleButton* PasswordLineEdit::get_visble() {
    return m_changeStatusBtn;
}

/* 密码检测模块 */
bool PasswordLineEdit::check() const {
    bool uper = false;
    bool normal = false;
    bool number = false;
    bool line = false;
    if (this->text() != "") {
        QString str = this->text();
        for(QChar c:str) {
            if (c>='A' && c <= 'Z') {
                uper = true;
                continue;
            }
            if (c>='a' && c <='z') {
                normal = true;
                continue;
            }
            if (c>='0' && c<='9') {
                number = true;
                continue;
            }
        }
        if (text().length() >= 6) {
            line = true;
        }
    } else {
        return false;
    }
    bool ok = uper && number && line == true ?true:normal && number && line;
    return  ok;
}
